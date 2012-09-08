//*************************************************************************************************
// 内容:	地形系统
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-31
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _TERRAIN_H__
#define _TERRAIN_H__
#include <sdSingleton.h>
#include "sdEntity.h"
#include "sdHeightMap.h"
#include "sdNormalMap.h"
#include "sdLayerMap.h"
#include "sdQuadNode.h"
#include "sdTerrainPick.h"

// 渲染系统
#include "sdRenderSystem.h"
#include "sdDynamicTexture.h"
#include "sdTextureAtlas.h"

// 地形系统,所有对外接口均由此类暴露出去
//
// 几何部分:
//	1.Tile管理: 四叉树管理,从中间节点到叶子节点均预生成Mesh,Node内部没有LOD
//			(可以考虑利用缓存,动态生成Mesh来改进效率)
//  2.Tile边缘接缝:下拉裙边的方式解决LOD边缘接缝,下拉长度是节点到下一级QuadNode的最大误差
//  3.Tile尺寸: 64*64格, 65*65点, 加上裙边67*67点 
//  3.Tile渲染时进行顶点压缩,以减少数据传输
//			XY存入UINT16高8字节和低8字节
//			Z变换为定点小数存入
//  4.Tile渲染时使用TriangleStrip来排列顶点,使用Index, 
//		依据GPU顶点缓存默认大小为24来优化排列,网格按照Cache大小分成数栏
//		
// 纹理部分:
//	1.
class sdTerrain : public Base::sdTSingleton<sdTerrain>, public sdEntity
{
	friend class sdQuadMesh;
	friend class sdQuadNode;
public:
	sdTerrain();
	virtual ~sdTerrain();

	// 
	bool	CreateScene(uint uiTerrainSize, uint uiBlendTextureSize);
	void	DestroyScene();
	//bool	LoadScene();
	bool	SaveScene(const std::string& szSceneFullPath);
	bool	HasScene() { return m_bInitialized;};

	// 地形裁剪(世界坐标)
	//(wz封装了一层TerrainTileEntity,貌似没啥必要,这里直接返回Mesh)
	void	Cull(const NiCamera& kCamera, std::vector<NiMesh*>& kMeshVec);

	// 地形拾取(世界坐标)
	bool	Pick(const Base::Math::sdRay& kRay, Base::Math::sdVector3& kIntersect, float fLimit = FLT_MAX);	

	// 地形高度获取(世界坐标)
	float	GetHeight(float fX, float fY);

	// 地形编辑(函数均用于地形编辑器)
	// @{
	// 地形高度获取与设置(高度图坐标,直接从高度图取出和设置,没有进行变换)
	float	GetRawHeight(uint uiX, uint uiY);
	void	SetRawHeight(uint uiX, uint uiY, float fHeight);

	// 更新几何体(世界坐标)
	void	UpdateGeometry(float fCenterX, float fCenterY, float fRadius);
	// @}


	// 图层编辑
	// @{
	bool	AppendLayer(sdLayerMap* pkLayerMap);
	//bool	Insert(sdLayerMap* pkLayerMap, uint uiIndex);

	//bool	RemoveLayer();
	//bool	RemoveLayer(uint uiIndex);

	//bool	SwapLayer(uint uiLIndex, uint uiRIndex);


	// 设置Layer指定像素点权值(用于编辑)
	void	SetBlendMapData(uint uiX, uint uiY, uint uiLayer, float fWeight);

	// 更新指定范围的混合贴图(用于编辑)
	//	@kRect	m_fX,m_fY为矩形起始位置, m_fZ,m_fW为矩形尺寸(世界坐标系)
	void	UpdateBlendMap();
	void	UpdateBlendMap(const Base::Math::sdVector4& kRect);

	// 对贴图进行重新打包
	void	RepackMaps();
	// @}


	// 常用属性访问
	// @{
	bool	IsVisible() const { return m_bIsVisible;}

	bool	GetEnableEditHeight() const { return m_bEnableEditHeight;}
	bool	GetEnableEditMaterial() const { return m_bEnableEditMaterial;}

	uint	GetTileSize() const { return m_uiTileSize;}
	uint	GetTerrainSize() const { return m_uiTerrainSize;}
	uint	GetMeshLevel() const { return m_uiMeshLevel;}
	bool	GetEnableLOD() const { return m_bEnableLOD;}

	float	GetScale() const { return m_fScale;}
	const NiPoint3& GetOrigin() const { return m_kOrigin;}

	const RenderSystem::sdTerrainParams& GetTerrainParams() const { return m_kRenderParams;};
	// @}

protected:
	// 获取高度图(内部使用,编辑器使用)
	sdHeightMap*	GetHeightMap() { return m_pkHeightMap;};

	// @{
	// 获取指定像素点的权重列表,wz原始版本提取方式
	uint	GetWeights(uint uiX, uint uiY, uchar* pucWeights);
	// @}

	// 数据加载与保存
	// @{
	
	// @}

protected:
	bool	m_bInitialized;			// 是否初始化
	bool	m_bIsVisible;			// 是否可见(一般用于编辑器)	
	bool	m_bEnableEditHeight;	// 是否允许编辑地形高度
	bool	m_bEnableEditMaterial;	// 是否允许编辑地形材质

	// 地形世界偏移与缩放
	NiPoint3	m_kOrigin;	// 地形整体偏移(默认为(0,0,0))
	float		m_fScale;	// 地形整体缩放(默认1.0f)

	// 地表高度图
	sdHeightMapPtr	m_pkHeightMap;	
	sdTerrainPickPtr m_pkPick;

	// 地表法线图
	sdNormalMapPtr	m_pkNormalMap;

	// 地表图层集合和图层混合信息集合
	typedef std::vector<sdLayerMapPtr> LayerMapVec;
	typedef std::vector<sdLayerMapPtr>::iterator LayerMapVecItr;
	LayerMapVec		m_kLayerMapVec;								// 仅用于编辑器
	bool			m_bDirtyLayerMapVec;				

	typedef std::vector<uchar*> LayerAlphaVec;
	typedef std::vector<uchar*>::iterator LayerAlphaVecItr;
	LayerAlphaVec	m_kLayerAlphaVec;							// 仅用于编辑器

	// 地表混合贴图与查找表
	RenderSystem::sdDynamicTexturePtr	m_pkDynamicBlendMap;	// 仅用于编辑器
	NiTexturePtr	m_spBlendMap;

	RenderSystem::sdDynamicTexturePtr	m_pkDynamicTileMap;		// 仅用于编辑器
	NiTexturePtr	m_spTileMap;	

	// 地表漫反射贴图集和查找表
	RenderSystem::sdTextureAtlasPtr m_pkDiffuseAtlas;
	NiTexturePtr	m_spDiffuseAtlasMap;
	NiTexturePtr	m_spDiffuseAtlasTable;

	// 地表法线贴图集和查找表
	RenderSystem::sdTextureAtlasPtr m_pkNormalAtlas;
	NiTexturePtr	m_spNormalAtlasMap;
	NiTexturePtr	m_spNormalAtlasTable;

	// 地形四叉树
	sdQuadNodePtr	m_pkQuadRoot;

	// 地形绘制参数(用于提供给渲染系统使用,内部不应该被分配内存和保存资源)
	RenderSystem::sdTerrainParams	m_kRenderParams;

	uint	m_uiTerrainSize;	// Terrain的尺寸(256/512/1024/2048,单位Grid)
	float	m_fMeterPerGrid;	// 每单位大小(0.125/0.25/0.5/1/2/4/8,目前是1m)
	uint	m_uiTileSize;		// Terrain的Tile的尺寸(目前是64Unit)

	uint	m_uiBlendTexSize;	// BlendMap尺寸(256/512/1024/2048,单位Pixel)
	float	m_fMeterPerPixel;	// 每像素大小(0.125/0.25/0.5/1/2/4/8,目前是0.5f)
	uint	m_uiTexTileSize;	// BlendMap的TexTile尺寸(目前是4Pixel)
	uint	m_uiTileMapSize;	// TileMap的尺寸

	uint	m_uiMeshLevel;		// 四叉树中,只有小于一定层级的才会构建Mesh
	bool	m_bEnableLOD;		// 四叉树是否允许LOD显示
	bool	m_bEnableNormal;	// 材质是否允许法线贴图

	// 根据当前视锥体与分辨率计算出来的Error与Distance之间的转换率,用于控制LOD的切换
	// (多级切换,延迟切换,一般是一个数组,这里有待参考CryEngine)
	//float	m_fError2Distance;	

protected:
	// Tile进行权重混合时的过滤器(用于编辑器,材质编辑)
	// @{
	struct stFilterMap
	{
		uint	uiIdx[4];
		float	fWeight[4];
	};
	typedef std::vector<stFilterMap> FilterMapVec;;
	typedef std::vector<stFilterMap>::iterator FilterMapVecItr;
	FilterMapVec m_kTileFilterMapVec;
	// @}

protected:
	static uint	ms_uiMaxLayerCount;
};
#endif
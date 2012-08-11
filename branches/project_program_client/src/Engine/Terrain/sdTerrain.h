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
#include "sdQuadNode.h"

// 渲染系统
#include "sdRenderSystem.h"
#include "sdDynamicTexture.h"

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
	//void	SaveScene();
	bool	HasScene() { return m_bInitialized;};

	// 地形裁剪
	//(wz封装了一层TerrainTileEntity,貌似没啥必要,这里直接返回Mesh)
	void	Cull(const NiCamera& kCamera, std::vector<NiMesh*>& kMeshVec);


	//
	bool	IsVisible() const { return m_bIsVisible;};

	uint	GetTileSize() const { return m_uiTileSize;};
	uint	GetMeshLevel() const { return m_uiMeshLevel;};
	bool	GetEnableLOD() const { return m_bEnableLOD;};

	const RenderSystem::sdTerrainParams& GetTerrainParams() const { return m_kRenderParams;};

protected:
	sdHeightMap*	GetHeightMap() { return m_pkHeightMap;};

protected:
	bool	m_bInitialized;			// 是否初始化
	bool	m_bIsVisible;			// 是否可见(一般用于编辑器)	
	bool	m_bEnableEditHeight;	// 是否允许编辑地形高度
	bool	m_bEnableEditMaterial;	// 是否允许编辑地形材质

	// 地表高度图
	sdHeightMapPtr	m_pkHeightMap;	

	// 地表法线图
	sdNormalMapPtr	m_pkNormalMap;

	// 地表图层集合和图层混合信息集合
	typedef std::vector<sdLayerMapPtr> LayerMapVec;
	typedef std::vector<sdLayerMapPtr>::iterator LayerMapVecItr;
	LayerMapVec		m_kLayerMapVec;								// 仅用于编辑器

	typedef std::vector<uchar*> LayerAlphaVec;
	typedef std::vector<uchar*>::iterator LayerAlphaVecItr;
	LayerAlphaVec	m_kLayerAlphaVec;							// 仅用于编辑器

	// 地表混合贴图与查找表
	RenderSystem::sdDynamicTexturePtr	m_spDynamicBlendMap;	// 仅用于编辑器
	NiTexturePtr	m_spBlendMap;

	RenderSystem::sdDynamicTexturePtr	m_spDynamicTileMap;		// 仅用于编辑器
	NiTexturePtr	m_spTileMap;	



	// 地表漫反射贴图集和查找表
	NiTexturePtr	m_spDiffuseAtlasMap;
	NiTexturePtr	m_spDiffuseAtlasTable;

	// 地表法线贴图集和查找表
	NiTexturePtr	m_spNormalAtlasMap;
	NiTexturePtr	m_spNormalAtlasTable;

	// 地形四叉树
	sdQuadNodePtr	m_pkQuadRoot;

	// 地形绘制参数(用于提供给渲染系统使用,内部不应该被分配内存和保存资源)
	RenderSystem::sdTerrainParams	m_kRenderParams;

	uint	m_uiTerrainSize;	// Terrain的尺寸(256/512/1024/2048,单位Unit)
	uint	m_uiMeterPerUnit;	// 每单位大小(目前是1m)
	uint	m_uiBlendTexSize;	// BlendMap尺寸(256/512/1024/2048,单位Pixel)
	uint	m_uiTexTileSize;	// BlendMap的TexTile尺寸(目前是4像素)
	uint	m_uiTileSize;		// MeshTile的尺寸
	uint	m_uiMeshLevel;		// 四叉树中,只有小于一定层级的才会构建Mesh
	bool	m_bEnableLOD;		// 四叉树是否允许LOD显示
	bool	m_bEnableNormal;	// 材质是否允许法线贴图

	// 根据当前视锥体与分辨率计算出来的Error与Distance之间的转换率,用于控制LOD的切换
	// (多级切换,延迟切换,一般是一个数组,这里有待参考CryEngine)
	//float	m_fError2Distance;	
};
#endif
//*************************************************************************************************
// 内容:	贴图图集
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-14
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_TEXTURE_ATLAS_H__
#define _SD_ENGINE_TEXTURE_ATLAS_H__
#include "sdTypes.h"
#include "sdTexture.h"
#include "sdSurfaceAtlas.h"

namespace RenderSystem
{
	//
	// 从多张Texture来生成一张大的贴图图集,要求:
	//	1.每页贴图都是D3D内存纹理
	//	2.每页贴图与贴图集有相同的格式
	//	3.每页贴图为贴图集指定尺寸(尺寸必须是pow(2,n),且大小相等)
	//
	// 关于生成的图集:
	//	1.从右下角开始
	//	2.从Mipmap0开始,从大到小
	//	3.
	// 
	// 关于生成的图集的目录纹理:
	//	1.每一列代表一张纹理信息
	//	2.从上到下是各层LOD信息
	class sdTextureAtlas : public sdTexture
	{
	public:
		sdTextureAtlas(D3DFORMAT eFormat);
		~sdTextureAtlas();

		// 
		bool	Update();

		// 图集所属贴图操作
		// @{
		// 向结尾添加和删除纹理(失败返回-1,即0xffffffff)
		uint	AddTexture(NiTexture* spTexture, float fUVRepeat = 1.0f, float fMipmapBias = 0.0f,  uint uiNumLevel = 0);
		bool	RemoveTexture();

		// 向任意位置添加和删除纹理
		uint	InsertTexture(uint uiIndex, NiTexture* spTexture, float fUVRepeat = 1.0f, float fMipmapBias = 0.0f,  uint uiNumLevel = 0);
		bool	RemoveTexture(uint uiIndex);
		// @}

		// 属性访问

		
	protected:
		//
		bool	UpdateAtlasAndTable();
		bool	UpdateRenderTexture();

		// 创建与销毁最终贴图集的D3D纹理
		bool	CreateAtlasTexture();
		void	DestroyAtlasTexture();

		// 创建与销毁贴图集查找表的D3D纹理
		bool	CreateAtlasTableTexture();
		void	DestroyAtlasTableTexture();

		// 设备回调处理
		virtual bool OnDeviceLost();
		virtual bool OnDeviceReset(bool bBefore);

		// 根据已经分配的像素计算
		static void GetCoordinate(uint uiNumAllAllocatedPixels, uint& uiX, uint& uiY);
	
	protected:
		struct stTextureInfo;

		// 贴图集输入Texture的一层Mipmap,对应SurfaceAtlas中的一个Tile
		struct stLevelInfo
		{
			uint	uiSize;						// 当前层级尺寸
			IDirect3DSurface9*	spD3DSurface;	// 当前层级数据

			// SurfaceAtlas相关
			uint	uiTileIndex;	// 用于索引SurfaceAtlas
			uint	uiLeft;			// 计算得到的在SurfaceAtlas的新位置
			uint	uiTop;	

			// 父纹理
			stTextureInfo*	pkTexInfo;

			stLevelInfo() : uiSize(0), spD3DSurface(NULL), uiTileIndex(-1), uiLeft(0), uiTop(0), pkTexInfo(NULL) {}
		};
		typedef std::vector<stLevelInfo> LevelInfoVec;
		typedef std::vector<stLevelInfo>::iterator LevelInfoVecItr;

		// 贴图集的一张输入Texture,可能包含多个Mipmap级数
		struct stTextureInfo
		{
			// 纹理
			NiTexture*			spTexture;
			IDirect3DTexture9*	spD3DTexture;
			LevelInfoVec		kLevelInfoVec;	// 纹理Mipmap信息
			uint				uiTotalPixels;	// 纹理使用的Mipmap总大小
				
			// 属性
			float		fUVRepeat;
			float		fMipmapBias;

			stTextureInfo(): spTexture(NULL), spD3DTexture(NULL), uiTotalPixels(0), fUVRepeat(1.0f), fMipmapBias(0.0f) {}
		};
		typedef std::vector<stTextureInfo*> TextureInfoVec;
		typedef std::vector<stTextureInfo*>::iterator TextureInfoVecItr;

		// LevelInfo的排序算子
		static bool LevelGreater(stLevelInfo* lhs, stLevelInfo* rhs);

		// 贴图集目录
		struct stTableInfo
		{
			uint	uiWidth;		// AtlasTableTexture尺寸
			uint	uiHeight;
			uint	uiTextureCount;	// Atlas的纹理页数(相同纹理不同参数算2个)
			uint	uiLevelCount;	// Atlas最大Mipmap级数

			// 用于Shader计算
			float	vTexId2U[2];
			float	vLevel2V[2];

			stTableInfo() : uiWidth(0), uiHeight(0), uiTextureCount(0), uiLevelCount(0){ vTexId2U[0] = vTexId2U[1] = 0; vLevel2V[0] = vLevel2V[1] = 0;}
		};

	protected:
		// 图集所属贴图信息
		TextureInfoVec	m_kTextureInfoVec;
		uint	m_uiTotalPixels;		// 当前Atlas输入Page总像素数目
		bool	m_bAtlasDirty;			// 当前Atlas输入Page是否被更新了
		bool	m_bAtlasTextureDirty;	// 当前输出AtlasTexture是否需要更新

		// 图集目录信息
		stTableInfo	m_kTableInfo;
		bool	m_bAtlasTableDirty;			// 当前AtlasTable是否被更新了
		bool	m_bAtlasTableTextureDirty;	// 当前输出AtlasTableTexture是否需要更新
		float	m_fLodBias;

		// 图集拼接工具
		sdSurfaceAtlasPtr	m_pkSurfaceAtlas;

		// 最终图集
		D3DFORMAT	m_eFormat;				// 图集纹理格式
		uint		m_uiAtlasSize;			// 图集纹理尺寸
		NiTexturePtr		m_spAtlasTexture;	
		IDirect3DTexture9*	m_spD3DAtlasTexture;

		// 最终图集目录
		NiTexturePtr		m_spAtlasTable;		
		IDirect3DTexture9*	m_spD3DAtlasTable;

		// 渲染设备
		NiDX9Renderer*		m_spRenderer;
		LPDIRECT3DDEVICE9	m_spD3DDevice;

		// 当前允许的最大纹理大小
		static uint ms_uiMaxTextureSize;
	};
	NiSmartPointer(sdTextureAtlas);
}
#endif
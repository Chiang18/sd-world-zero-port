//*************************************************************************************************
// 内容:	贴图图集生成工具
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-14
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_SURFACE_ATLAS_H__
#define _SD_ENGINE_SURFACE_ATLAS_H__
#include "sdTypes.h"

namespace RenderSystem
{
	// 用于将所有输入的Tile拼接并输出到一张大的Surface上面,要求
	//	1.所有输入必须是指定格式
	//	2.输出必须是指定尺寸,指定格式
	class sdSurfaceAtlas : public NiRefObject
	{
	public:
		sdSurfaceAtlas(uint uiWidth, uint uiHeight, D3DFORMAT eFormat);
		~sdSurfaceAtlas();

		// Tile分配与销毁
		uint	AllocTile(uint uiLeft, uint uiTop, uint uiWidth, uint uiHeight);
		bool	DeallocTile(uint uiIndex);
		void	DeallocAllTiles();

		// 绑定/解绑定TileSurface到Tile
		bool	BindSourceSurfaceToTile(uint uiIndex, IDirect3DSurface9* pSurface, uint uiLeft = 0, uint uiTop = 0);
		void	UnbindSourceSurfaceFromTile(uint uiIndex);

		// 绑定/解绑定TargetSurface
		bool	BindTargetSurface(IDirect3DSurface9* pSurface);
		void	UnbindTargetSurface(); 

		// 更新Tile到TargetSurface
		bool	UpdateTargetSurface();

		// 
		uint	GetWidth() const { return m_uiWidth;}
		uint	GetHeight() const { return m_uiHeight;}

	protected:
		// 描述一块Tile的数据来源
		struct stAtlasTileSurface
		{
			IDirect3DSurface9*	pSurface;
			uint	uiLeft;
			uint	uiTop;

			stAtlasTileSurface(): pSurface(NULL), uiLeft(0), uiTop(0) {}
			stAtlasTileSurface(IDirect3DSurface9* surface, uint left, uint top): pSurface(pSurface), uiLeft(left), uiTop(top) {}

			void Reset()
			{
				pSurface = NULL;
				uiLeft = 0;
				uiTop = 0;
			}

			bool operator==(const stAtlasTileSurface& rhs) const
			{
				return (pSurface == rhs.pSurface) && (uiLeft == rhs.uiLeft) && (uiTop == rhs.uiTop);
			}
		};

		// 描述当前表面上的一块Tile
		struct stAtlasTile : public NiMemObject
		{
			// 当前Tile的目标区域
			uint uiLeft;
			uint uiTop;
			uint uiWidth;
			uint uiHeight;

			// 当前Tile的源Surface
			stAtlasTileSurface kSource;

			// 标记Surface已经被修改过
			bool bDirty;

			stAtlasTile(uint left, uint top, uint w, uint h): uiLeft(left), uiTop(top), uiWidth(w), uiHeight(h) {}
		};
		typedef std::vector<stAtlasTile> AtlasTileVec;
		typedef std::vector<stAtlasTile>::iterator AtlasTileVecItr;

 
	protected:
		uint	m_uiWidth;
		uint	m_uiHeight;

		// 当前输入和目标Surface格式
		D3DFORMAT	m_eFormat;	
		bool		m_bIsDDSFormat;	

		// 输入Tile
		AtlasTileVec m_kAtlasTileVec;

		// 输出Surface
		IDirect3DSurface9*	m_pkTargetSurface;
	};
	NiSmartPointer(sdSurfaceAtlas);
}
#endif
//*************************************************************************************************
// ����:	��ͼͼ�����ɹ���
//---------------------------------------------------------
// ����:		
// ����:		2012-08-14
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_SURFACE_ATLAS_H__
#define _SD_ENGINE_SURFACE_ATLAS_H__
#include "sdTypes.h"

namespace RenderSystem
{
	// ���ڽ����������Tileƴ�Ӳ������һ�Ŵ��Surface����,Ҫ��
	//	1.�������������ָ����ʽ
	//	2.���������ָ���ߴ�,ָ����ʽ
	class sdSurfaceAtlas : public NiRefObject
	{
	public:
		sdSurfaceAtlas(uint uiWidth, uint uiHeight, D3DFORMAT eFormat);
		~sdSurfaceAtlas();

		// Tile����������
		uint	AllocTile(uint uiLeft, uint uiTop, uint uiWidth, uint uiHeight);
		bool	DeallocTile(uint uiIndex);
		void	DeallocAllTiles();

		// ��/���TileSurface��Tile
		bool	BindSourceSurfaceToTile(uint uiIndex, IDirect3DSurface9* pSurface, uint uiLeft = 0, uint uiTop = 0);
		void	UnbindSourceSurfaceFromTile(uint uiIndex);

		// ��/���TargetSurface
		bool	BindTargetSurface(IDirect3DSurface9* pSurface);
		void	UnbindTargetSurface(); 

		// ����Tile��TargetSurface
		bool	UpdateTargetSurface();

		// 
		uint	GetWidth() const { return m_uiWidth;}
		uint	GetHeight() const { return m_uiHeight;}

	protected:
		// ����һ��Tile��������Դ
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

		// ������ǰ�����ϵ�һ��Tile
		struct stAtlasTile : public NiMemObject
		{
			// ��ǰTile��Ŀ������
			uint uiLeft;
			uint uiTop;
			uint uiWidth;
			uint uiHeight;

			// ��ǰTile��ԴSurface
			stAtlasTileSurface kSource;

			// ���Surface�Ѿ����޸Ĺ�
			bool bDirty;

			stAtlasTile(uint left, uint top, uint w, uint h): uiLeft(left), uiTop(top), uiWidth(w), uiHeight(h) {}
		};
		typedef std::vector<stAtlasTile> AtlasTileVec;
		typedef std::vector<stAtlasTile>::iterator AtlasTileVecItr;

 
	protected:
		uint	m_uiWidth;
		uint	m_uiHeight;

		// ��ǰ�����Ŀ��Surface��ʽ
		D3DFORMAT	m_eFormat;	
		bool		m_bIsDDSFormat;	

		// ����Tile
		AtlasTileVec m_kAtlasTileVec;

		// ���Surface
		IDirect3DSurface9*	m_pkTargetSurface;
	};
	NiSmartPointer(sdSurfaceAtlas);
}
#endif
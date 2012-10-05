//*************************************************************************************************
// 内容:	地形编辑对外接口
//---------------------------------------------------------
// 作者:		
// 创建:		
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_EDITTERRAINHELPER_H__
#define _SD_GAMEEDITEX_EDITTERRAINHELPER_H__
#include "sdGameEditEx.h"
#include "sdTerrainDeformMode.h"
#include "sdTerrainSurfaceMode.h"

//
#include <sdTerrain.h>

class sdMap;
class sdTerrain;

namespace GameEditEx
{
	class sdTerrainDeformMode;
	class sdTerrainSurfaceMode;

	// 地形编辑对外接口,相当于一个Facade,避免上层逻辑直接调用FSM
	class sdEditTerrainHelper : public NiRefObject
	{
	public:
		sdEditTerrainHelper(sdTerrainDeformMode* pkTerrainDeformMode, 
			sdTerrainSurfaceMode* pkTerrainSurfaceMode);
		virtual ~sdEditTerrainHelper();

		// 地形高度编辑
		// @{
		// 笔刷类型(sdEditBrush::EBrushType)
		uint	GetDeformBrushType();
		void	SetActiveDeformBrush(uint uiBrush);

		void	SetDeformBrushVisible(bool bVisible);

		float	GetDeformBrushOutterRadius();
		void	SetDeformBrushOutterRadius(float fRadius);

		float	GetDeformBrushInnerRadius();
		void	SetDeformBrushInnerRadius(float fRadius);

		float	GetDeformBrushHardness();
		void	SetDeformBrushHardness(float fHardness);

		float	GetDeformBrushHeight();
		void	SetDeformBrushHeight(float fHeight);
		// @}

		// 地形纹理编辑
		// @{
		
		// 纹理图层
		uint GetLayerMapCount() const { return m_pkTerrain->GetLayerNum();}

		uint GetActiveLayerMap() const { return m_pkTerrainSurfaceMode->GetActiveLayerMap();}
		void SetActiveLayerMap(uint uiIndex) { m_pkTerrainSurfaceMode->SetActiveLayerMap(uiIndex);}

		const char* GetLayerMapName(uint uiIndex) const;
		void SetLayerMapName(uint uiIndex, const char* szName);

		//const char* GetLayerMapDiffuseMap(uint uiIndex) const;
		//void SetLayerMapDiffuseMap(uint uiIndex, const char* szName);

		//const char* GetLayerMapNormalMap(uint uiIndex) const;
		//void SetLayerMapNormalMap(uint uiIndex, const char* szName);

		//const NiPoint3& GetLayerMapTranslate(uint uiIndex) const;
		//void SetLayerMapTranslate(uint uiIndex, NiPoint3& kTranslate);

		//const NiMatrix3& GetLayerMapRotate(uint uiIndex) const;
		//void SetLayerMapRotate(uint uiIndex, NiMatrix3& kRotate);

		//float GetLayerMapScale(uint uiIndex) const;
		//void SetLayerMapScale(uint uiIndex, float fScale);

		//float GetLayerMapMipmapBias(uint uiIndex) const;
		//void SetLayerMapBias(uint uiIndex, float fBias);

		//bool AppendLayerMap(const char* szName, const char* szDiffuseMapName, const char* szNormalMapName);
		//bool DeleteLayerMap(uint uiIndex);
		//bool SwapLayerMap(uint uiLIndex, uint uiRIndex);

		// @}

	protected:
		sdMap*		m_pkMap;
		sdTerrain*	m_pkTerrain;
		sdTerrainDeformMode*	m_pkTerrainDeformMode;
		sdTerrainSurfaceMode*	m_pkTerrainSurfaceMode;
	};
	NiSmartPointer(sdEditTerrainHelper);
#include "sdEditTerrainHelper.inl"
}
#endif
#include "Stdafx.h"
#include "MEditTerrainHelper.h"
#include "sdWorldEditor.h"
#include "MMacros.h"

//
#include <sdWorldEditFSM.h>
#include <sdEditTerrainHelper.h>

//
using namespace GameEditEx;

namespace WorldEditorFramework 
{
	//-----------------------------------------------------------------------------
	MEditTerrainHelper::MEditTerrainHelper()
	{
		sdWorldEditFSM* pkWorldEditFSM = sdWorldEditor::Instance().GetWorldEditFSM();
		m_pkEditTerrainHelper = pkWorldEditFSM->GetEditTerrainHelper();
	}
	//-----------------------------------------------------------------------------
	int MEditTerrainHelper::DeformBrushType::get()
	{
		return m_pkEditTerrainHelper->GetDeformBrushType();
	}
	//-----------------------------------------------------------------------------
	void MEditTerrainHelper::DeformBrushType::set(int iType)
	{
		m_pkEditTerrainHelper->SetActiveDeformBrush(iType);
	}
	//-----------------------------------------------------------------------------
	float MEditTerrainHelper::DeformBrushOutterRadius::get()
	{
		return m_pkEditTerrainHelper->GetDeformBrushOutterRadius();
	}
	//-----------------------------------------------------------------------------
	void MEditTerrainHelper::DeformBrushOutterRadius::set(float fRadius)
	{
		return m_pkEditTerrainHelper->SetDeformBrushOutterRadius(fRadius);
	}
	//-----------------------------------------------------------------------------
	float MEditTerrainHelper::DeformBrushInnerRadius::get()
	{
		return m_pkEditTerrainHelper->GetDeformBrushInnerRadius();
	}
	//-----------------------------------------------------------------------------
	void MEditTerrainHelper::DeformBrushInnerRadius::set(float fRadius)
	{
		return m_pkEditTerrainHelper->SetDeformBrushInnerRadius(fRadius);
	}
	//-----------------------------------------------------------------------------
	float MEditTerrainHelper::DeformBrushHardness::get()
	{
		return m_pkEditTerrainHelper->GetDeformBrushHardness();
	}
	//-----------------------------------------------------------------------------
	void MEditTerrainHelper::DeformBrushHardness::set(float fHardness)
	{
		return m_pkEditTerrainHelper->SetDeformBrushHardness(fHardness);
	}
	//-----------------------------------------------------------------------------
	float MEditTerrainHelper::DeformBrushHeight::get()
	{
		return m_pkEditTerrainHelper->GetDeformBrushHeight();
	}
	//-----------------------------------------------------------------------------
	void MEditTerrainHelper::DeformBrushHeight::set(float fHeight)
	{
		return m_pkEditTerrainHelper->SetDeformBrushHeight(fHeight);
	}
	//-----------------------------------------------------------------------------
	unsigned int MEditTerrainHelper::GetLayerMapCount()
	{
		return m_pkEditTerrainHelper->GetLayerMapCount();
	}
	//-----------------------------------------------------------------------------
	unsigned int MEditTerrainHelper::GetActiveLayerMap()
	{
		return m_pkEditTerrainHelper->GetActiveLayerMap();
	}
	//-----------------------------------------------------------------------------
	void MEditTerrainHelper::SetActiveLayerMap(unsigned int uiIndex)
	{
		m_pkEditTerrainHelper->SetActiveLayerMap(uiIndex);
	}
	//-----------------------------------------------------------------------------
	System::String^ MEditTerrainHelper::GetLayerMapName(unsigned int uiIndex)
	{
		const char* szName = m_pkEditTerrainHelper->GetLayerMapName(uiIndex);
		return (szName == 0) ? "" : gcnew System::String(szName);
	}
	//-----------------------------------------------------------------------------
	void MEditTerrainHelper::SetLayerMapName(unsigned int uiIndex, System::String^ szName)
	{
		const char* szChar = MStringToCharPointer(szName);
		m_pkEditTerrainHelper->SetLayerMapName(uiIndex, szChar);
		MFreeCharPointer(szChar);
	}
	//-----------------------------------------------------------------------------
}
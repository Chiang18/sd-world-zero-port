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
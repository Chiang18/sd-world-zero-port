#include "Stdafx.h"
#include "sdEditTerrainHelper.h"

//
#include <sdMap.h>
#include <sdRenderSystem_DX9.h>

//
using namespace RenderSystem;

namespace GameEditEx
{
//-------------------------------------------------------------------------------------------------
sdEditTerrainHelper::sdEditTerrainHelper(sdTerrainDeformMode* pkTerrainDeformMode, 
	sdTerrainSurfaceMode* pkTerrainSurfaceMode)
: m_pkTerrainDeformMode(pkTerrainDeformMode)
, m_pkTerrainSurfaceMode(pkTerrainSurfaceMode)
{
	NIASSERT(pkTerrainDeformMode);
	NIASSERT(pkTerrainSurfaceMode);

	//
	sdRenderSystem* pkRenderSystem = sdRenderSystem_DX9::InstancePtr();
	NIASSERT(pkRenderSystem);

	m_pkMap = pkRenderSystem->GetMap();
	NIASSERT(m_pkMap);

	m_pkTerrain = m_pkMap->GetTerrain();
	NIASSERT(m_pkTerrain);
}
//-------------------------------------------------------------------------------------------------
sdEditTerrainHelper::~sdEditTerrainHelper()
{

}
//-------------------------------------------------------------------------------------------------
const char* sdEditTerrainHelper::GetLayerMapName(uint uiIndex) const
{
	sdLayerMap* pkLayerMap = m_pkTerrain->GetLayerMap(uiIndex);
	if (pkLayerMap)
	{
		return pkLayerMap->GetName().c_str();
	}

	return NULL;
}
//-------------------------------------------------------------------------------------------------
void sdEditTerrainHelper::SetLayerMapName(uint uiIndex, const char* szName)
{
	sdLayerMap* pkLayerMap = m_pkTerrain->GetLayerMap(uiIndex);
	if (pkLayerMap)
	{
		return pkLayerMap->SetName(szName);
	}
}
//-------------------------------------------------------------------------------------------------
}
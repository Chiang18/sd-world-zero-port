#include "Stdafx.h"
#include "sdEditBrush.h"
#include "sdTerrain.h"

namespace GameEditEx
{
//-------------------------------------------------------------------------------------------------
sdTerrainDeformPoolBrush::sdTerrainDeformPoolBrush()
{
}
//-------------------------------------------------------------------------------------------------
sdTerrainDeformPoolBrush::~sdTerrainDeformPoolBrush()
{
	m_pkEditShape = 0;
	m_spBrushShape = 0;
}
//-------------------------------------------------------------------------------------------------
void sdTerrainDeformPoolBrush::Apply()
{
	sdTerrain* pkTerrain = sdTerrain::InstancePtr();
	NIASSERT(pkTerrain);

	// 尝试拾取地形点
	
}
//-------------------------------------------------------------------------------------------------
}

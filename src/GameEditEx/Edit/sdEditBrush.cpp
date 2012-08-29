#include "Stdafx.h"
#include "sdEditBrush.h"

namespace GameEditEx
{
//-------------------------------------------------------------------------------------------------
sdTerrainDeformPoolBrush::sdTerrainDeformPoolBrush()
{
	m_pkEditShape = NiNew sdDualCircleShape;
	NIASSERT(m_pkEditShape);
}
//-------------------------------------------------------------------------------------------------
sdTerrainDeformPoolBrush::~sdTerrainDeformPoolBrush()
{
	m_pkEditShape = 0;
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

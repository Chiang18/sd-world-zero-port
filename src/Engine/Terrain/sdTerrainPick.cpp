#include "Stdafx.h"
#include "sdTerrainPick.h"
#include "sdHeightMap.h"

//-------------------------------------------------------------------------------------------------
sdTerrainPick::sdTerrainPick()
: m_pkHeightMap(NULL)
, m_bCullBack(true)
, m_fMaxDistance(FLT_MAX)
{

}
//-------------------------------------------------------------------------------------------------
sdTerrainPick::sdTerrainPick(sdHeightMap* pkHeightMap)
: m_pkHeightMap(pkHeightMap)
, m_bCullBack(true)
, m_fMaxDistance(FLT_MAX)
{
	NIASSERT(pkHeightMap);
}
//-------------------------------------------------------------------------------------------------
bool sdTerrainPick::Pick(const NiPoint3& kOrigin, const NiPoint3& kDir, NiPoint3& kIntersect)
{
	// 计算Ray与HeightMap的AABB的交点


}
//-------------------------------------------------------------------------------------------------
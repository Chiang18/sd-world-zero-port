#include "Stdafx.h"
#include "sdTerrainPick.h"
#include "sdHeightMap.h"

//
#include <sdMath.h>
#include <sdRay.h>
#include <sdAxisAlignedBox.h>

using namespace Base::Math;
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
bool sdTerrainPick::Pick(const sdRay& kRay, NiPoint3& kIntersect)
{
	// 计算包围盒
	uint uiSize = m_pkHeightMap->GetSize();
	sdAxisAlignedBox kAABB;
	kAABB.SetMinimum(0, 0, m_pkHeightMap->GetMinHeight());
	kAABB.SetMaximum((float)uiSize, (float)uiSize, m_pkHeightMap->GetMaxHeight());

	// 计算Ray与HeightMap的AABB的交点
	float fStart = 0.0f;
	float fEnd = 0.0f;
	bool bIntersect = sdMath::Intersects(kRay, kAABB, fStart, fEnd);
	if (!bIntersect)
		return false;

	if (fEnd > m_fMaxDistance)
		fEnd = m_fMaxDistance;

	// 从起始点到终止点进行固定步长微分查找
	float fSampleDelta = 0.5f;
	sdVector3 kDeltaPos(fSampleDelta * kRay.GetDirection());
	sdVector3 kStartPos(kRay.GetOrigin());
	sdVector3 kEndPos(kRay.GetOrigin() + kDeltaPos);
	for (;fStart <= fEnd; fStart += fSampleDelta)
	{
		
		
	

		
	}

	return true;
}
//-------------------------------------------------------------------------------------------------
bool 
//-------------------------------------------------------------------------------------------------
bool sdTerrainPick::Intersect(const sdVector3& kV1, const sdVector3& kV2, const sdVector3& kV3, const sdRay& kRay, sdVector3& kIntersect)
{
	// 有多种方法:
	//	1.计算射线与平面交点,然后将三角形投影到一个平面(XY/YZ/ZX),判断2维点是否在2维三角形内部
	//	2.参考DX的Pick例子和《实时计算及图形学》的P322
	// 这里是第2种的一个近似
	//
	// 计算Triangle边矢量和法线(这里假设三角形以正面看顺时针顺序编号)
	sdVector3 kEdge1 = kV2 - kV1;
	sdVector3 kEdge2 = kV3 - kV2;
	

	// 计算
	const sdVector3& kOrigin = kRay.GetOrigin();
	const sdVector3& kDir = kRay.GetDirection();

	sdVector3 kP = kDir.CrossProduct(kEdge1);
	float fDet = kEdge1.DotProduct(kP);

	float fR, fS, fT;
	float fTolerance = 1e-05f;
	if (fDet <= -fTolerance)
	{
		// 正面刺穿三角形所在平面
		sdVector3 kS = kOrigin - kV1;
		fS = kS.DotProduct(kP);
		if (fS > 0.0f || fS < fDet)
			return false;

		sdVector3 kQ = kS.CrossProduct(kEdge1);
		fT = kDir.DotProduct(kQ);
		if (fT > 0.0f || fS + fT < fDet)
			return false;

		fR = kEdge2.DotProduct(kQ);
		if (fR > 0.0f)
			return false;
	}
	else if (fDet >= fTolerance)
	{
		// 背面刺穿三角形所在平面
		//
		// 不进行背面求交
		if (m_bCullBack)
			return false;

		sdVector3 kS = kOrigin - kV1;
		fS = kS.DotProduct(kP);
		if (fS < 0.0f || fS > fDet)
			return false;

		sdVector3 kQ = kS.CrossProduct(kEdge1);
		fT = kDir.DotProduct(kQ);
		if (fT < 0.0f || fS + fT > fDet)
			return false;

		fR = kEdge2.DotProduct(kQ);
		if (fR < 0.0f)
			return false;
	}
	else
	{
		// 射线与三角形所在平面平行
		return false;
	}

	// 计算交点
	float fInvDet = 1.0f / fDet;
	fS *= fInvDet;
	fT *= fInvDet;
	fR *= fInvDet;

	kIntersect = kOrigin + fR * kDir;
	
	return true;
}
//-------------------------------------------------------------------------------------------------
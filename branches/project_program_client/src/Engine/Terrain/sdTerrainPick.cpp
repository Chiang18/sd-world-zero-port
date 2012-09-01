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
bool sdTerrainPick::Pick(const sdRay& kRay, sdVector3& kIntersect)
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
	// (原始值是0.5f,个人觉得应该是0.707)
	float fSampleDelta = 0.707f;
	sdVector3 kDeltaPos(fSampleDelta * kRay.GetDirection());
	sdVector3 kDeltaStartPos(kRay.GetOrigin());

	uint uiStartX = UINT_MAX;
	uint uiStartY = UINT_MAX;
	uint uiEndX = UINT_MAX;
	uint uiEndY = UINT_MAX;
	for (bool bFirst = true;fStart <= fEnd; fStart += fSampleDelta, bFirst = false)
	{
		// 对齐到栅格
		uiEndX = (uint)kDeltaStartPos.m_fX;
		uiEndY = (uint)kDeltaStartPos.m_fY;
		
		// 只有线段端点不位于同一个高度格子内才进行处理
		//   +---+---+
		//   |   |   |
		//	 |   |   |
		//	 +---+---+
		//   |   |   |
		//	 |   |   |
		//	 +---+---+
		// (总感觉这里不严密,貌似应该先排序一下)
		if (uiStartX == uiEndX || uiStartY != uiEndY)
		{
			// 两个端点位于斜对角的两个格子,则处理另外两个对角的格子
			// (可能侧漏)
			if (uiStartX != uiEndX && uiStartX != uiEndY && bFirst == false)
			{
				if (Intersect(uiEndX, uiStartY, kRay, kIntersect))
					return true;

				if (Intersect(uiStartX, uiEndY, kRay, kIntersect))
					return true;
			}

			// 处理起始格子
			if (Intersect(uiStartX, uiStartX, kRay, kIntersect))
				return true;

			//
			uiStartX = uiEndX;
			uiStartY = uiEndY;
		}

		// 下一个点
		kDeltaStartPos += kDeltaPos;
	}

	return false;
}
//-------------------------------------------------------------------------------------------------
bool sdTerrainPick::Intersect(uint uiX, uint uiY, const sdRay& kRay, sdVector3& kIntersect)
{
	if (uiX >= m_pkHeightMap->GetSize() || uiY >= m_pkHeightMap->GetSize())
		return false;

	// 三角型组织方式
	//	1--3
	//	|\ |
	//  | \|
	//  0--2
	//
	// 获取四个点
	sdVector3 kPt0((float)uiX,		(float)uiY,		m_pkHeightMap->GetRawHeight(uiX, uiY));
	sdVector3 kPt1((float)uiX,		(float)(uiY+1),	m_pkHeightMap->GetRawHeight(uiX, uiY+1));
	sdVector3 kPt2((float)(uiX+1),	(float)uiY,		m_pkHeightMap->GetRawHeight(uiX+1, uiY));
	sdVector3 kPt3((float)(uiX+1),	(float)(uiY+1),	m_pkHeightMap->GetRawHeight(uiX+1, uiY+1));

	// 分别于0-1-2和2-1-3求交
	sdVector3 kIntersectLeft, kIntersectRight;
	bool bLeft = Intersect(kPt0, kPt1, kPt2, kRay, kIntersectLeft);
	bool bRight = Intersect(kPt2, kPt1, kPt3, kRay, kIntersectRight);

	// 判断结果(这里可以根据Ray的判断先相交的三角形,从而可能减少依次Intersect)
	if (bLeft && bRight)
	{
		// 若两个都相交则取最近值
		float fLeft = (kIntersectLeft - kRay.GetOrigin()).SquaredLength();
		float fRight = (kIntersectLeft - kRay.GetOrigin()).SquaredLength();
		kIntersect = fLeft < fRight ? kIntersectLeft : kIntersectRight;
		return true;
	}
	else if (bLeft || bRight)
	{
		// 只有一个相交取相交值
		kIntersect = bLeft ? kIntersectLeft : kIntersectRight;
		return true;
	}
	else
	{
		// 不相交
		return false;
	}
}
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
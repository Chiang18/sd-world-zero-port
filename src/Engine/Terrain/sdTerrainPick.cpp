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
	// �����Χ��
	uint uiSize = m_pkHeightMap->GetSize();
	sdAxisAlignedBox kAABB;
	kAABB.SetMinimum(0, 0, m_pkHeightMap->GetMinHeight());
	kAABB.SetMaximum((float)uiSize, (float)uiSize, m_pkHeightMap->GetMaxHeight());

	// ����Ray��HeightMap��AABB�Ľ���
	float fStart = 0.0f;
	float fEnd = 0.0f;
	bool bIntersect = sdMath::Intersects(kRay, kAABB, fStart, fEnd);
	if (!bIntersect)
		return false;

	if (fEnd > m_fMaxDistance)
		fEnd = m_fMaxDistance;

	// ����ʼ�㵽��ֹ����й̶�����΢�ֲ���
	// (ԭʼֵ��0.5f,���˾���Ӧ����0.707)
	float fSampleDelta = 0.707f;
	sdVector3 kDeltaPos(fSampleDelta * kRay.GetDirection());
	sdVector3 kDeltaStartPos(kRay.GetOrigin());

	uint uiStartX = UINT_MAX;
	uint uiStartY = UINT_MAX;
	uint uiEndX = UINT_MAX;
	uint uiEndY = UINT_MAX;
	for (bool bFirst = true;fStart <= fEnd; fStart += fSampleDelta, bFirst = false)
	{
		// ���뵽դ��
		uiEndX = (uint)kDeltaStartPos.m_fX;
		uiEndY = (uint)kDeltaStartPos.m_fY;
		
		// ֻ���߶ζ˵㲻λ��ͬһ���߶ȸ����ڲŽ��д���
		//   +---+---+
		//   |   |   |
		//	 |   |   |
		//	 +---+---+
		//   |   |   |
		//	 |   |   |
		//	 +---+---+
		// (�ܸо����ﲻ����,ò��Ӧ��������һ��)
		if (uiStartX == uiEndX || uiStartY != uiEndY)
		{
			// �����˵�λ��б�Խǵ���������,�������������Խǵĸ���
			// (���ܲ�©)
			if (uiStartX != uiEndX && uiStartX != uiEndY && bFirst == false)
			{
				if (Intersect(uiEndX, uiStartY, kRay, kIntersect))
					return true;

				if (Intersect(uiStartX, uiEndY, kRay, kIntersect))
					return true;
			}

			// ������ʼ����
			if (Intersect(uiStartX, uiStartX, kRay, kIntersect))
				return true;

			//
			uiStartX = uiEndX;
			uiStartY = uiEndY;
		}

		// ��һ����
		kDeltaStartPos += kDeltaPos;
	}

	return false;
}
//-------------------------------------------------------------------------------------------------
bool sdTerrainPick::Intersect(uint uiX, uint uiY, const sdRay& kRay, sdVector3& kIntersect)
{
	if (uiX >= m_pkHeightMap->GetSize() || uiY >= m_pkHeightMap->GetSize())
		return false;

	// ��������֯��ʽ
	//	1--3
	//	|\ |
	//  | \|
	//  0--2
	//
	// ��ȡ�ĸ���
	sdVector3 kPt0((float)uiX,		(float)uiY,		m_pkHeightMap->GetRawHeight(uiX, uiY));
	sdVector3 kPt1((float)uiX,		(float)(uiY+1),	m_pkHeightMap->GetRawHeight(uiX, uiY+1));
	sdVector3 kPt2((float)(uiX+1),	(float)uiY,		m_pkHeightMap->GetRawHeight(uiX+1, uiY));
	sdVector3 kPt3((float)(uiX+1),	(float)(uiY+1),	m_pkHeightMap->GetRawHeight(uiX+1, uiY+1));

	// �ֱ���0-1-2��2-1-3��
	sdVector3 kIntersectLeft, kIntersectRight;
	bool bLeft = Intersect(kPt0, kPt1, kPt2, kRay, kIntersectLeft);
	bool bRight = Intersect(kPt2, kPt1, kPt3, kRay, kIntersectRight);

	// �жϽ��(������Ը���Ray���ж����ཻ��������,�Ӷ����ܼ�������Intersect)
	if (bLeft && bRight)
	{
		// ���������ཻ��ȡ���ֵ
		float fLeft = (kIntersectLeft - kRay.GetOrigin()).SquaredLength();
		float fRight = (kIntersectLeft - kRay.GetOrigin()).SquaredLength();
		kIntersect = fLeft < fRight ? kIntersectLeft : kIntersectRight;
		return true;
	}
	else if (bLeft || bRight)
	{
		// ֻ��һ���ཻȡ�ֵཻ
		kIntersect = bLeft ? kIntersectLeft : kIntersectRight;
		return true;
	}
	else
	{
		// ���ཻ
		return false;
	}
}
//-------------------------------------------------------------------------------------------------
bool sdTerrainPick::Intersect(const sdVector3& kV1, const sdVector3& kV2, const sdVector3& kV3, const sdRay& kRay, sdVector3& kIntersect)
{
	// �ж��ַ���:
	//	1.����������ƽ�潻��,Ȼ��������ͶӰ��һ��ƽ��(XY/YZ/ZX),�ж�2ά���Ƿ���2ά�������ڲ�
	//	2.�ο�DX��Pick���Ӻ͡�ʵʱ���㼰ͼ��ѧ����P322
	// �����ǵ�2�ֵ�һ������
	//
	// ����Triangle��ʸ���ͷ���(������������������濴˳ʱ��˳����)
	sdVector3 kEdge1 = kV2 - kV1;
	sdVector3 kEdge2 = kV3 - kV2;
	
	// ����
	const sdVector3& kOrigin = kRay.GetOrigin();
	const sdVector3& kDir = kRay.GetDirection();

	sdVector3 kP = kDir.CrossProduct(kEdge1);
	float fDet = kEdge1.DotProduct(kP);

	float fR, fS, fT;
	float fTolerance = 1e-05f;
	if (fDet <= -fTolerance)
	{
		// ����̴�����������ƽ��
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
		// ����̴�����������ƽ��
		//
		// �����б�����
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
		// ����������������ƽ��ƽ��
		return false;
	}

	// ���㽻��
	float fInvDet = 1.0f / fDet;
	fS *= fInvDet;
	fT *= fInvDet;
	fR *= fInvDet;

	kIntersect = kOrigin + fR * kDir;
	
	return true;
}
//-------------------------------------------------------------------------------------------------
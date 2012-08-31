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
//*************************************************************************************************
// ����:	����ʰȡ
//---------------------------------------------------------
// ����:		
// ����:		2012-08-29
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_TERRAIN_PICK_H__
#define _SD_ENGINE_TERRAIN_PICK_H__
#include "sdEngine.h"

namespace Base
{
	namespace Math
	{
		class sdVector3;
		class sdRay;
	}
}
class sdHeightMap;

// ����ʰȡ,����ԭ���Ƕ����߾�����ÿһ���߶ȸ������ཻ�ж�
// ���÷�����:
//	1.������Ԥ����,��Ray��AABB��ʼ���㵽��ֹ������й̶�����΢�ֲ���
//	2.������Ԥ����,��Ray��AABB��ʼ���㵽��ֹ�������Bresenham΢�ֲ���
//	3.Ԥ����ΪQuadTree��AABB
//	4.Ԥ����Ϊ������HeightMap
// 
// ����ʹ�õ�һ��--����,������ϲο���NiPick
class sdTerrainPick : public NiRefObject
{
public:
	sdTerrainPick();
	sdTerrainPick(sdHeightMap* pkHeightMap);

	// ʰȡ,��������ϵ��HeightMap����ϵ�ĸ�����ʽ
	bool Pick(const Base::Math::sdRay& kRay, Base::Math::sdVector3& kIntersect);

	// ʰȡ����
	void SetHeightMap(sdHeightMap* pkHeightMap){NIASSERT(pkHeightMap);m_pkHeightMap = pkHeightMap;}
	void SetCullBack(bool bCullBack){ m_bCullBack = bCullBack;}
	void SetMaxDistance(float fMaxDistance){ NIASSERT(fMaxDistance >= 0); m_fMaxDistance = fMaxDistance;}

protected:
	// ����������ཻ
	bool Intersect(uint uiX, uint uiY, const Base::Math::sdRay& kRay, Base::Math::sdVector3& kIntersect);
	
	// ��������������
	bool Intersect(const Base::Math::sdVector3& kV1, 
		const Base::Math::sdVector3& kV2, 
		const Base::Math::sdVector3& kV3,
		const Base::Math::sdRay& kRay,
		Base::Math::sdVector3& kIntersect);

protected:
	sdHeightMap*	m_pkHeightMap;
	bool	m_bCullBack;
	float	m_fMaxDistance;
};
NiSmartPointer(sdTerrainPick);
#endif
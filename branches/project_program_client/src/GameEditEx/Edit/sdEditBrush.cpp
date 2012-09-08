#include "Stdafx.h"
#include "sdEditBrush.h"
#include "sdTerrain.h"

//
#include <sdRay.h>
#include <sdTimeMgr.h>

//
using namespace Base;
using namespace Base::Math;

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
	m_pkBrushShape = 0;
}
//-------------------------------------------------------------------------------------------------
void sdTerrainDeformPoolBrush::Apply(const sdRay& kRay)
{
	NIASSERT(m_pkBrushShape);
	NIASSERT(m_pkEditShape);

	//
	sdTerrain* pkTerrain = sdTerrain::InstancePtr();
	NIASSERT(pkTerrain);

	// ����ʰȡ���ε�
	sdVector3 kIntersect;
	if (pkTerrain->Pick(kRay, kIntersect))	
	{
		//
		sdDualCircleShape* pkDualCirlceShape = (sdDualCircleShape*)(sdEditShape*)m_pkEditShape;
		NIASSERT(pkDualCirlceShape);

		pkDualCirlceShape->SetCenter(kIntersect.m_fX, kIntersect.m_fY);
		
		// ���㷶Χ(��������)
		float fOuterRadius = pkDualCirlceShape->GetOuterRadius();
		float fStartX	= kIntersect.m_fX - fOuterRadius;
		float fStartY	= kIntersect.m_fY - fOuterRadius;
		float fEndX	= kIntersect.m_fX + fOuterRadius;
		float fEndY = kIntersect.m_fY + fOuterRadius;

		// ���㷶Χ(�߶�ͼ����)
		float fScale = pkTerrain->GetScale();
		const NiPoint3& kOrigin = pkTerrain->GetOrigin();
		float fHStartX = (fStartX - kOrigin.x) / fScale;
		float fHStartY = (fStartY - kOrigin.y) / fScale;
		float fHEndX = (fEndX - kOrigin.x) / fScale;
		float fHEndY = (fEndY - kOrigin.y) / fScale;

		// ǯλ����Χ������
		uint uiSize = pkTerrain->GetTerrainSize();

		if (fHStartX > (float)uiSize)	return;
		if (fHStartY > (float)uiSize)	return;
		if (fHEndX < 0)	return;
		if (fHEndY < 0) return;

		if (fHStartX < 0)	fHStartX = 0.0f;
		if (fHStartY < 0)	fHStartY = 0.0f;
		if (fHEndX > (float)uiSize)	fHEndX = (float)uiSize;
		if (fHEndY > (float)uiSize)	fHEndY = (float)uiSize;

		uint uiHStartX = (uint)fHStartX;
		uint uiHStartY = (uint)fHStartY;
		uint uiHEndX = (uint)(fHEndX);
		uint uiHEndY = (uint)(fHEndY);

		// �����߶�����,����
		//	1.���ƫ��,�任����������ϵ
		//	2.��ȡȨֵ
		//	3.�任
		float fFrameTime = sdTimeMgr::Instance().GetCurFrameTime();
		for (uint uiHY = uiHStartY; uiHY < uiHEndY; ++uiHY)
		{
			float fY = (uiHY + 0.5f) * fScale + kOrigin.y; 
			for (uint uiHX = uiHStartX; uiHX < uiHEndX; ++uiHX)
			{
				float fX = (uiHX + 0.5f) * fScale + kOrigin.x;
				float fWeight = pkDualCirlceShape->GetWeight(fX, fY);
				
				float fOldHeight = pkTerrain->GetRawHeight(uiHX, uiHY);
				pkTerrain->SetRawHeight(uiHX, uiHY, fOldHeight + fFrameTime * 10.0f);
			}
		}

		// ����Mesh
		pkTerrain->UpdateGeometry(kIntersect.m_fX, kIntersect.m_fY, fOuterRadius);

		// ���±�ˢ
		m_pkBrushShape->SetTranslate(kIntersect.m_fX, kIntersect.m_fY, 0);
		m_pkBrushShape->Update(0.0f);
		m_pkBrushShape->UpdateShape();
	}
}
//-------------------------------------------------------------------------------------------------
}

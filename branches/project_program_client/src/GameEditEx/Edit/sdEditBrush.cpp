#include "Stdafx.h"
#include "sdEditBrush.h"
#include "sdTerrain.h"

//
#include <sdRay.h>
#include <sdMatrix3.h>
#include <sdTimeMgr.h>

//
using namespace Base;
using namespace Base::Math;

namespace GameEditEx
{
//-------------------------------------------------------------------------------------------------
void sdTerrainDeformPoolBrush::Apply(const sdRay& kRay)
{
	NIASSERT(m_pkBrushShape);
	NIASSERT(m_pkEditShape);

	//
	sdTerrain* pkTerrain = sdTerrain::InstancePtr();
	NIASSERT(pkTerrain);

	// 尝试拾取地形点
	sdVector3 kIntersect;
	if (pkTerrain->Pick(kRay, kIntersect))	
	{
		//
		sdDualCircleShape* pkDualCirlceShape = (sdDualCircleShape*)(sdEditShape*)m_pkEditShape;
		NIASSERT(pkDualCirlceShape);

		pkDualCirlceShape->SetCenter(kIntersect.m_fX, kIntersect.m_fY);
		
		// 计算范围(世界坐标)
		float fOuterRadius = pkDualCirlceShape->GetOuterRadius();
		float fStartX	= kIntersect.m_fX - fOuterRadius;
		float fStartY	= kIntersect.m_fY - fOuterRadius;
		float fEndX	= kIntersect.m_fX + fOuterRadius;
		float fEndY = kIntersect.m_fY + fOuterRadius;

		// 计算范围(高度图坐标)
		float fScale = pkTerrain->GetScale();
		const NiPoint3& kOrigin = pkTerrain->GetOrigin();
		float fHStartX = (fStartX - kOrigin.x) / fScale;
		float fHStartY = (fStartY - kOrigin.y) / fScale;
		float fHEndX = (fEndX - kOrigin.x) / fScale;
		float fHEndY = (fEndY - kOrigin.y) / fScale;

		// 钳位到范围内整数(高度图坐标)
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

		// 遍历高度像素,处理
		//	1.添加偏移,变换到世界坐标系
		//	2.获取权值
		//	3.变换
		float fFrameTime = sdTimeMgr::Instance().GetCurFrameTime();
		for (uint uiHY = uiHStartY; uiHY < uiHEndY; ++uiHY)
		{
			float fY = (uiHY + 0.5f) * fScale + kOrigin.y; 
			for (uint uiHX = uiHStartX; uiHX < uiHEndX; ++uiHX)
			{
				float fX = (uiHX + 0.5f) * fScale + kOrigin.x;
				if (pkDualCirlceShape->InShape(fX, fY))
				{
					float fWeight = pkDualCirlceShape->GetWeight(fX, fY);
					float fOldHeight = pkTerrain->GetRawHeight(uiHX, uiHY);
					pkTerrain->SetRawHeight(uiHX, uiHY, fOldHeight + fFrameTime * fWeight * 3.0f);
				}
			}
		}

		// 更新Mesh
		pkTerrain->UpdateGeometry(kIntersect.m_fX, kIntersect.m_fY, fOuterRadius);

		// 更新笔刷
		m_pkBrushShape->SetTranslate(kIntersect.m_fX, kIntersect.m_fY, 0);
		m_pkBrushShape->Update(0.0f);
		m_pkBrushShape->UpdateShape();
	}
}
//-------------------------------------------------------------------------------------------------
void sdTerrainDeformSmoothBrush::Apply(const Base::Math::sdRay& kRay)
{
	NIASSERT(m_pkBrushShape);
	NIASSERT(m_pkEditShape);

	//
	sdTerrain* pkTerrain = sdTerrain::InstancePtr();
	NIASSERT(pkTerrain);

	// 尝试拾取地形点
	sdVector3 kIntersect;
	if (pkTerrain->Pick(kRay, kIntersect))	
	{
		//
		sdDualCircleShape* pkDualCirlceShape = (sdDualCircleShape*)(sdEditShape*)m_pkEditShape;
		NIASSERT(pkDualCirlceShape);

		pkDualCirlceShape->SetCenter(kIntersect.m_fX, kIntersect.m_fY);

		// 计算范围(世界坐标)
		float fOuterRadius = pkDualCirlceShape->GetOuterRadius();
		float fStartX	= kIntersect.m_fX - fOuterRadius;
		float fStartY	= kIntersect.m_fY - fOuterRadius;
		float fEndX	= kIntersect.m_fX + fOuterRadius;
		float fEndY = kIntersect.m_fY + fOuterRadius;

		// 计算范围(高度图坐标)
		float fScale = pkTerrain->GetScale();
		const NiPoint3& kOrigin = pkTerrain->GetOrigin();
		float fHStartX = (fStartX - kOrigin.x) / fScale;
		float fHStartY = (fStartY - kOrigin.y) / fScale;
		float fHEndX = (fEndX - kOrigin.x) / fScale;
		float fHEndY = (fEndY - kOrigin.y) / fScale;

		// 钳位到范围内整数
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

		// 遍历高度像素,处理
		//	1.添加偏移,变换到世界坐标系
		//	2.获取权值
		//	3.变换
		// @{
		// 
		sdMatrix3 kFilter33(0.1f, 0.1f, 0.1f, 0.1f, 0.2f, 0.1f, 0.1f, 0.1f, 0.1f);

		// 遍历处理()
		float fFrameTime = sdTimeMgr::Instance().GetCurFrameTime();
		for (uint uiHY = uiHStartY; uiHY < uiHEndY; ++uiHY)
		{
			float fY = (uiHY + 0.5f) * fScale + kOrigin.y; 
			for (uint uiHX = uiHStartX; uiHX < uiHEndX; ++uiHX)
			{
				float fX = (uiHX + 0.5f) * fScale + kOrigin.x;
				if (pkDualCirlceShape->InShape(fX, fY))
				{
					// 计算平距高度
					float fAverageHeight = 0.0f;
					for (int iDeltaY = 0; iDeltaY < 3; ++iDeltaY)
					{
						for (int iDeltaX = 0; iDeltaX < 3; ++iDeltaX)
						{
							int iHX = (int)uiHX + iDeltaX - 1;
							int iHY = (int)uiHY + iDeltaY - 1;
							
							if (iHX < 0)	iHX = 0;
							if (iHY < 0)	iHY = 0;
							if (iHX > (int)uiSize)	iHX = (int)uiSize;
							if (iHY > (int)uiSize)	iHY = (int)uiSize;

							fAverageHeight += pkTerrain->GetRawHeight((uint)iHX, (uint)iHY) * 
								kFilter33.m[iDeltaX][iDeltaY];
						}
					}

					// 插值
					float fFactor = pkDualCirlceShape->GetWeight(fX, fY);
					float fOldHeight = pkTerrain->GetRawHeight(uiHX, uiHY);
					pkTerrain->SetRawHeight(uiHX, uiHY, sdMath::Lerp(fOldHeight, fAverageHeight, fFactor));
				}
			}
		}
		// @}

		// 更新Mesh
		pkTerrain->UpdateGeometry(kIntersect.m_fX, kIntersect.m_fY, fOuterRadius);

		// 更新笔刷
		m_pkBrushShape->SetTranslate(kIntersect.m_fX, kIntersect.m_fY, 0);
		m_pkBrushShape->Update(0.0f);
		m_pkBrushShape->UpdateShape();
	}
}
//-------------------------------------------------------------------------------------------------
void sdTerrainSurfaceLayerBrush::Apply(const sdRay& kRay)
{
	NIASSERT(m_pkBrushShape);
	NIASSERT(m_pkEditShape);

	//
	sdTerrain* pkTerrain = sdTerrain::InstancePtr();
	NIASSERT(pkTerrain);

	// 尝试拾取地形点
	sdVector3 kIntersect;
	if (pkTerrain->Pick(kRay, kIntersect))	
	{
		//
		sdDualCircleShape* pkDualCirlceShape = (sdDualCircleShape*)(sdEditShape*)m_pkEditShape;
		NIASSERT(pkDualCirlceShape);

		pkDualCirlceShape->SetCenter(kIntersect.m_fX, kIntersect.m_fY);

		// 计算范围(世界坐标)
		float fOuterRadius = pkDualCirlceShape->GetOuterRadius();
		float fStartX	= kIntersect.m_fX - fOuterRadius;
		float fStartY	= kIntersect.m_fY - fOuterRadius;
		float fEndX	= kIntersect.m_fX + fOuterRadius;
		float fEndY = kIntersect.m_fY + fOuterRadius;

		// 计算范围(BlendMap坐标)
		float fScale = pkTerrain->GetScale() * pkTerrain->GetBlendMapScale();
		const NiPoint3& kOrigin = pkTerrain->GetOrigin();
		float fBStartX = (fStartX - kOrigin.x) / fScale;
		float fBStartY = (fStartY - kOrigin.y) / fScale;
		float fBEndX = (fEndX - kOrigin.x) / fScale;
		float fBEndY = (fEndY - kOrigin.y) / fScale;

		// 钳位到范围内整数(BlendMap坐标)
		uint uiSize = pkTerrain->GetBlendMapSize();

		if (fBStartX > (float)uiSize)	return;
		if (fBStartY > (float)uiSize)	return;
		if (fBEndX < 0)	return;
		if (fBEndY < 0) return;

		if (fBStartX < 0)	fBStartX = 0.0f;
		if (fBStartY < 0)	fBStartY = 0.0f;
		if (fBEndX > (float)uiSize)	fBEndX = (float)uiSize;
		if (fBEndY > (float)uiSize)	fBEndY = (float)uiSize;

		uint uiBStartX = (uint)fBStartX;
		uint uiBStartY = (uint)fBStartY;
		uint uiBEndX = (uint)(fBEndX);
		uint uiBEndY = (uint)(fBEndY);

		// 遍历BlendMap像素,处理
		//	1.变换到世界坐标系
		//	3.填充新值
		for (uint uiBY = uiBStartY; uiBY < uiBEndY; ++uiBY)
		{
			float fY = (uiBY + 0.5f) * fScale + kOrigin.y; 
			for (uint uiBX = uiBStartX; uiBX < uiBEndX; ++uiBX)
			{
				float fX = (uiBX + 0.5f) * fScale + kOrigin.x;
				if (pkDualCirlceShape->InShape(fX, fY))
				{
					float fWeight = pkDualCirlceShape->GetWeight(fX, fY);

					uint uiOldBlend = pkTerrain->GetBlendMapData(uiBX, uiBY, 0);
					uiOldBlend += 10;
					uiOldBlend = uiOldBlend > 255 ? 255 : uiOldBlend;

					pkTerrain->SetBlendMapData(uiBX, uiBY, 0, uiOldBlend);
				}
			}
		}

		// 更新纹理
		

		// 更新笔刷
		m_pkBrushShape->SetTranslate(kIntersect.m_fX, kIntersect.m_fY, 0);
		m_pkBrushShape->Update(0.0f);
		m_pkBrushShape->UpdateShape();
	}
}
//-------------------------------------------------------------------------------------------------
}

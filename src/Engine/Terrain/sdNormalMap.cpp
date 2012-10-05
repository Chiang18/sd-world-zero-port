#include "Stdafx.h"
#include "sdNormalMap.h"
#include "sdHeightMap.h"
#include "sdDynamicTexture.h"

using namespace RenderSystem;
//-------------------------------------------------------------------------------------------------
sdNormalMap::sdNormalMap(uint uiSize, sdHeightMap* pkHeightMap)
{
	NIASSERT(pkHeightMap);
	m_pkHeightMap = pkHeightMap;

	m_pkDynamicNormalTexture = NiNew sdDynamicTexture(uiSize, uiSize, sdDynamicTexture::E_FORMAT_A8R8G8B8);
	NIASSERT(m_pkDynamicNormalTexture);

	// 预计算的坡度阈值(WZ原值是65度天顶角)
	m_kPlanarThreshold.z = cos(65.f / 180.0f * NI_PI);
	m_kPlanarThreshold.x = sin(65.f / 180.0f * NI_PI) / sqrt(2.0f);
	m_kPlanarThreshold.y = m_kPlanarThreshold.x;
}
//-------------------------------------------------------------------------------------------------
sdNormalMap::sdNormalMap(NiTexture* spNormalTexture)
{
	NIASSERT(spNormalTexture);
	m_spNormalTexture = spNormalTexture;
}
//-------------------------------------------------------------------------------------------------
sdNormalMap::~sdNormalMap()
{
	m_pkHeightMap = NULL;
	m_spNormalTexture = 0;
	m_pkDynamicNormalTexture = 0;
}
//-------------------------------------------------------------------------------------------------
NiTexture* sdNormalMap::GetGBTexture() const
{
	if (m_spNormalTexture)				
		return m_spNormalTexture;
	else
		return m_pkDynamicNormalTexture->GetGBTexture();
}
//-------------------------------------------------------------------------------------------------
void sdNormalMap::Update()
{
	NIASSERT(m_pkHeightMap);
	NIASSERT(m_pkDynamicNormalTexture);

	uint uiSize = m_pkDynamicNormalTexture->GetWidth();
	Update(0, 0, uiSize, uiSize);
}
//-------------------------------------------------------------------------------------------------
void sdNormalMap::Update(uint uiX, uint uiY, uint uiW, uint uiH)
{
	NIASSERT(m_pkHeightMap);
	NIASSERT(m_pkDynamicNormalTexture);

	// 锁定目标纹理
	sdDynamicTexture::sdLockedRect kLockedRect = m_pkDynamicNormalTexture->LockRegion(uiX, uiY, uiW, uiH);
	if (!kLockedRect.pBits)
		return;

	// 获取法线并填充
	uint uiSize = m_pkDynamicNormalTexture->GetWidth();
	uint uiStartX = uiX;
	uint uiStartY = uiY;
	uint uiEndX = min(uiSize, uiX+uiW);
	uint uiEndY = max(uiSize, uiY+uiH);

	float fNX = 0.0f, fNY = 0.0f, fNZ = 0.0f;
	float fWX = 0.0f, fWY = 0.0f, fWZ = 0.0f;
	uchar ucNX = 0, ucNY = 0;
	uchar ucWX = 0, ucWY = 0;
	uint  uiPitch = kLockedRect.uiPitch / 4;
	uint* puiCurLine = (uint*)kLockedRect.pBits;
	for (uint uiRow = uiStartY; uiRow < uiEndY; ++uiRow)
	{
		uint* puiCurData = puiCurLine;
		for (uint uiCol = uiStartX; uiCol < uiEndX; ++uiCol)
		{
			// 获取法线
			m_pkHeightMap->GetNormal(uiCol, uiRow, fNX, fNY, fNZ);

			// 压缩法线([-1,1]到[0,255])
			ucNX = (uchar)((fNX * 0.5f + 0.5f) * 255.0f);
			ucNY = (uchar)((fNY * 0.5f + 0.5f) * 255.0f);

			// 计算倾斜权值
			// (不解为何这样计算,感觉有很大缺陷)
			fWX = max(pow((fabs(fNX) - m_kPlanarThreshold.x) * 7.0f, 3.0f), 0);
			fWY = max(pow((fabs(fNY) - m_kPlanarThreshold.y) * 7.0f, 3.0f), 0);
			fWZ = max(pow((fabs(fNZ) - m_kPlanarThreshold.z) * 7.0f, 3.0f), 0);

			float fTotalW = fWX + fWY + fWZ;
			fWX /= fTotalW;
			fWY /= fTotalW;

			fWX = floor(fWX * 255.f + 0.5f);
			fWY = floor(fWY * 255.f + 0.5f);
			if (fWX + fWY > 255.f)
			{
				// 按比例缩减
				float fTotal = fWX + fWY;
				fWX *= 255.f / fTotal;
				fWY *= 255.f / fTotal;
			}

			ucWX = (uchar)fWX;
			ucWY = (uchar)fWY;
		
			// 填充到数据(注意纹理格式是ARGB)
			//					R,X			G,Y			B,Y			A,W
			*puiCurData = (ucNX << 16) | (ucNY << 8) | (ucWX) | (ucWY << 24);

			// 下一个
			++puiCurData;
		}

		// 下一行
		puiCurLine += uiPitch;
	}

	//
	m_pkDynamicNormalTexture->UnlockRegion();
}
//-------------------------------------------------------------------------------------------------
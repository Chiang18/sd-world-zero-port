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
	uchar ucNX = 0, ucNY = 0, ucNZ = 0;
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
			//ucNZ = (uchar)((fNZ * 0.5f + 0.5f) * 255.0f);

			//***********
			// 未完成(关于权重部分,大于65度法线改变一些值)
			//***********
		
			// 填充到数据
			// (注意纹理格式是ARGB)
			*puiCurData = (ucNX << 16) | (ucNY << 8);

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
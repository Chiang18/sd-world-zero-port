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

	// ����Ŀ������
	sdDynamicTexture::sdLockedRect kLockedRect = m_pkDynamicNormalTexture->LockRegion(uiX, uiY, uiW, uiH);
	if (!kLockedRect.pBits)
		return;

	// ��ȡ���߲����
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
			// ��ȡ����
			m_pkHeightMap->GetNormal(uiCol, uiRow, fNX, fNY, fNZ);

			// ѹ������([-1,1]��[0,255])
			ucNX = (uchar)((fNX * 0.5f + 0.5f) * 255.0f);
			ucNY = (uchar)((fNY * 0.5f + 0.5f) * 255.0f);
			//ucNZ = (uchar)((fNZ * 0.5f + 0.5f) * 255.0f);

			//***********
			// δ���(����Ȩ�ز���,����65�ȷ��߸ı�һЩֵ)
			//***********
		
			// ��䵽����
			// (ע�������ʽ��ARGB)
			*puiCurData = (ucNX << 16) | (ucNY << 8);

			// ��һ��
			++puiCurData;
		}

		// ��һ��
		puiCurLine += uiPitch;
	}

	//
	m_pkDynamicNormalTexture->UnlockRegion();
}
//-------------------------------------------------------------------------------------------------
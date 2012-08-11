#include "Stdafx.h"
#include "sdTextureAtlas.h"
#include "NiDX9TextureData.h"

namespace RenderSystem
{
//-------------------------------------------------------------------------------------------------
sdTextureAtlas::sdTextureAtlas()
{

}
//-------------------------------------------------------------------------------------------------
sdTextureAtlas::~sdTextureAtlas()
{

}
//-------------------------------------------------------------------------------------------------
bool sdTextureAtlas::Update()
{
	

	return true;
}
//-------------------------------------------------------------------------------------------------
uint sdTextureAtlas::AddTexture(NiTexture *spTexture, float fUVRepeat)
{
	return InsertTexture(m_kTexturePageInfoVec.size(), spTexture, fUVRepeat);
}
//-------------------------------------------------------------------------------------------------
bool sdTextureAtlas::RemoveTexture()
{
	return RemoveTexture(m_kTexturePageInfoVec.size() - 1);
}
//-------------------------------------------------------------------------------------------------
uint sdTextureAtlas::InsertTexture(uint uiIndex, NiTexture *spTexture, float fUVRepeat)
{
	if (uiIndex > m_kTexturePageInfoVec.size())	return -1;	
	if (NULL == spTexture)	return -1;
	if (fUVRepeat <= 0.0f)	return -1;

	// �����ͼ�Ƿ��Ѿ�����
	bool bExist = false;
	TexturePageInfoVecItr itr_tex = m_kTexturePageInfoVec.begin();
	TexturePageInfoVecItr itr_tex_end = m_kTexturePageInfoVec.end();
	for (; itr_tex != itr_tex_end; ++itr_tex)
	{
		if ((*itr_tex)->m_spTexture == spTexture)
		{
			bExist = true;
			break;
		}
	}

	// ��ȡD3D����,��������Ƿ�����Ҫ��
	//	1.��һ��D3D����
	//	2.��������һ���ڴ�����
	//	3.�������ʽ����ͼ����ͬ
	//	4.��������һ�ŷ�������
	//	5.������ߴ�ΪPower(2,n)
	// @{
	NiDX9TextureData* spDX9TextureData = (NiDX9TextureData*)spTexture->GetRendererData();
	IDirect3DTexture9* spD3DTexture = (IDirect3DTexture9*)spDX9TextureData->GetD3DTexture();
	
	if (NULL == spD3DTexture)	return -1;
	if (D3DRTYPE_TEXTURE != spD3DTexture->GetType())	return -1;

	D3DSURFACE_DESC kDesc;
	spD3DTexture->GetLevelDesc(0, &kDesc);
	if (kDesc.Format != m_eFormat)			return -1;
	if (D3DPOOL_SYSTEMMEM != kDesc.Pool)	return -1;
	if ((kDesc.Width != kDesc.Height) || !NiIsPowerOf2(kDesc.Width))	return -1;
	// @}

	// �½�����ҳ
	// @{
	TexturePageInfo* pkTexPageInfo = NiNew TexturePageInfo;
	NIASSERT(pkTexPageInfo);
	pkTexPageInfo->m_spTexture = spTexture;
	pkTexPageInfo->m_fUVRepeat = fUVRepeat;

	// @}

	// �������ҳ���б�
	// @{
	if (uiIndex == m_kTexturePageInfoVec.size())
	{
		m_kTexturePageInfoVec.push_back(pkTexPageInfo);
	}
	else
	{
		m_kTexturePageInfoVec.insert(m_kTexturePageInfoVec.begin() + uiIndex, pkTexPageInfo);
	}
	// @}

	return uiIndex;
}
//-------------------------------------------------------------------------------------------------
bool sdTextureAtlas::RemoveTexture(uint uiIndex)
{

	return true;
}
//-------------------------------------------------------------------------------------------------
bool sdTextureAtlas::UpdateAtlasAndTable()
{
	if (m_kTexturePageInfoVec.empty())
		return true;


}
//-------------------------------------------------------------------------------------------------
bool sdTextureAtlas::OnDeviceLost()
{
	DestroyAtlasTexture();
	DestroyAtlasTableTexture();
	return true;
}
//-------------------------------------------------------------------------------------------------
bool sdTextureAtlas::OnDeviceReset(bool bBefore)
{
	if (bBefore)
	{
		// �豸����ǰ
		DestroyAtlasTexture();
		DestroyAtlasTableTexture();
		return true;
	}
	else
	{
		// �豸�Ѿ�����
		UpdateRenderTexture();
		return true;
	}
}
//-------------------------------------------------------------------------------------------------
}
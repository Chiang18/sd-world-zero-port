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

	// 检查贴图是否已经存在
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

	// 获取D3D纹理,检查纹理是否满足要求
	//	1.是一张D3D纹理
	//	2.该纹理是一张内存纹理
	//	3.该纹理格式与贴图集相同
	//	4.该纹理是一张方形纹理
	//	5.该纹理尺寸为Power(2,n)
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

	// 新建纹理页
	// @{
	TexturePageInfo* pkTexPageInfo = NiNew TexturePageInfo;
	NIASSERT(pkTexPageInfo);
	pkTexPageInfo->m_spTexture = spTexture;
	pkTexPageInfo->m_fUVRepeat = fUVRepeat;

	// @}

	// 添加纹理页到列表
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
		// 设备重设前
		DestroyAtlasTexture();
		DestroyAtlasTableTexture();
		return true;
	}
	else
	{
		// 设备已经重设
		UpdateRenderTexture();
		return true;
	}
}
//-------------------------------------------------------------------------------------------------
}
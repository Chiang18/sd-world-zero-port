#include "Stdafx.h"
#include "sdRenderDevice_DX9.h"

namespace RenderSystem
{
//-------------------------------------------------------------------------------------------------
sdRenderDevice_DX9::sdRenderDevice_DX9()
{

}
//-------------------------------------------------------------------------------------------------
sdRenderDevice_DX9::~sdRenderDevice_DX9()
{
	if (m_bIsInitialized)
	{
		Destroy();
		m_bIsInitialized = false;
	}
}
//-------------------------------------------------------------------------------------------------
bool sdRenderDevice_DX9::Initialize()
{
	m_spDX9Render = NiDX9Renderer::GetRenderer();
	NIASSERT(m_spDX9Render);

	m_spDX9RenderState = m_spDX9Render->GetRenderState();
	NIASSERT(m_spDX9RenderState);

	return (m_bIsInitialized = true);
}
//-------------------------------------------------------------------------------------------------
void sdRenderDevice_DX9::Destroy()
{
	m_spDX9RenderState = 0;
	m_spDX9Render = 0;
	m_spRenderTarget = 0;

	m_bIsInitialized = false;
}
//-------------------------------------------------------------------------------------------------
void sdRenderDevice_DX9::SetRenderState(uint uiState, uint uiValue)
{
	NIASSERT(m_spDX9RenderState);
	m_spDX9RenderState->SetRenderState((D3DRENDERSTATETYPE)uiState, uiValue);
}

uint sdRenderDevice_DX9::GetRenderState(uint uiState)
{
	NIASSERT(m_spDX9RenderState);
	return m_spDX9RenderState->GetRenderState((D3DRENDERSTATETYPE)uiState);
}

void sdRenderDevice_DX9::SaveAllRenderStates()
{
	NIASSERT(m_spDX9RenderState);
	m_spDX9RenderState->SaveRenderState();
}

void sdRenderDevice_DX9::RestoreAllRenderStates()
{
	NIASSERT(m_spDX9RenderState);
	m_spDX9RenderState->RestoreRenderState();
}
//-------------------------------------------------------------------------------------------------
void sdRenderDevice_DX9::SetSampleState(uint uiSlot, uint uiState, uint uiValue)
{
	NIASSERT(m_spDX9RenderState);
	m_spDX9RenderState->SetSamplerState(uiSlot, (D3DSAMPLERSTATETYPE)uiState, uiValue);
}

uint sdRenderDevice_DX9::GetSampleState(uint uiSlot, uint uiState)
{
	NIASSERT(m_spDX9RenderState);
	return m_spDX9RenderState->GetSamplerState(uiSlot, (D3DSAMPLERSTATETYPE)uiState);
}

void sdRenderDevice_DX9::SaveAllSampleStates()
{
	NIASSERT(m_spDX9RenderState);
	m_spDX9RenderState->SaveTextureStageState();
}

void sdRenderDevice_DX9::RestoreAllSampleStates()
{
	NIASSERT(m_spDX9RenderState);
	m_spDX9RenderState->RestoreTextureStageState();
}
//-------------------------------------------------------------------------------------------------
void sdRenderDevice_DX9::ClearVertexBinding()
{
	// 这里直接操作D3D是很不好的,以后考虑修改渲染器来加入到RenderState里面进行缓存
	IDirect3DDevice9* d3ddev = m_spDX9Render->GetD3DDevice();
	d3ddev->SetStreamSource(0, NULL, 0, 0);
	d3ddev->SetStreamSource(1, NULL, 0, 0);
	d3ddev->SetStreamSource(2, NULL, 0, 0);
	d3ddev->SetStreamSource(3, NULL, 0, 0);
}

void sdRenderDevice_DX9::Clear(uint* puiColor, float* pfDepth, uint* puiStencil)
{
	// 这里直接操作D3D是很不好的,貌似GB的NiDX9Renderer::ClearBuffer有此功能,不知道为什么不使用
	IDirect3DDevice9* d3ddev = m_spDX9Render->GetD3DDevice();
	DWORD		uiFlag			= 0;
	D3DCOLOR	vClearColor		= 0;
	float		fClearDepth		= 0.0f;
	DWORD		uiClearStencil	= 0;
	if (puiColor)		{ uiFlag |= D3DCLEAR_TARGET;	vClearColor = * puiColor;}
	if (pfDepth)		{ uiFlag |= D3DCLEAR_ZBUFFER;	fClearDepth = *pfDepth;}
	if (uiClearStencil)	{ uiFlag |= D3DCLEAR_STENCIL;	uiClearStencil = *puiStencil;}
	d3ddev->Clear(0, NULL, uiFlag, vClearColor, fClearDepth, uiClearStencil);
}
//-------------------------------------------------------------------------------------------------
void sdRenderDevice_DX9::SetCameraData(NiCamera* spCamera)
{
	NIASSERT(spCamera);
	m_spDX9Render->SetCameraData(spCamera);
}
//-------------------------------------------------------------------------------------------------
void sdRenderDevice_DX9::SetRenderTargetGroup(NiRenderTargetGroup* spRenderTarget)
{
	m_spRenderTarget = spRenderTarget;

	if (m_spDX9Render->IsRenderTargetGroupActive())
		m_spDX9Render->EndUsingRenderTargetGroup();

	if (m_spRenderTarget)
		m_spDX9Render->BeginUsingRenderTargetGroup(m_spRenderTarget, NiRenderer::CLEAR_NONE);
}

NiRenderTargetGroup* sdRenderDevice_DX9::GetRenderTargetGroup()
{
	return m_spRenderTarget;
}

NiRenderTargetGroup* sdRenderDevice_DX9::GetDefaultRenderTargetGroup()
{
	return m_spDX9Render->GetDefaultRenderTargetGroup();
}
//-------------------------------------------------------------------------------------------------
NiMaterial* sdRenderDevice_DX9::CreateMaterial(const sdFixedString& szName)
{
	return NiMaterialLibrary::CreateMaterial(szName);
}
//-------------------------------------------------------------------------------------------------
NiRenderedTexturePtr sdRenderDevice_DX9::CreateRenderTexture(TextureSpec& kSpec)
{
	return NiRenderedTexture::Create(kSpec.uiWidth, kSpec.uiHeight, m_spDX9Render, kSpec.kFormat);
}

NiRenderTargetGroupPtr sdRenderDevice_DX9::CreateRenderTarget(uint uiNum)
{
	return NiRenderTargetGroup::Create(uiNum, m_spDX9Render);
}
//-------------------------------------------------------------------------------------------------
//NiSourceTexturePtr sdRenderDevice_DX9::LoadTexture(const char* szTexName)
//{
//	
//}
//-------------------------------------------------------------------------------------------------
uint sdRenderDevice_DX9::AddLostEventDelegate(LostEventDelegate pfFunc, void* pData)
{
	NIASSERT(m_spDX9Render);
	return m_spDX9Render->AddLostDeviceNotificationFunc(pfFunc, pData);
}

uint sdRenderDevice_DX9::AddResetEventDelegate(ResetEventDelegate pfFunc, void* pData)
{
	NIASSERT(m_spDX9Render);
	return m_spDX9Render->AddResetNotificationFunc(pfFunc, pData);
}

bool sdRenderDevice_DX9::RemoveLostEventDelegate(uint uiIndex)
{
	NIASSERT(m_spDX9Render);
	return m_spDX9Render->RemoveLostDeviceNotificationFunc(uiIndex);
}

bool sdRenderDevice_DX9::RemoveResetEventDelegate(uint uiIndex)
{
	NIASSERT(m_spDX9Render);
	return m_spDX9Render->RemoveResetNotificationFunc(uiIndex);
}

void sdRenderDevice_DX9::RemoveAllEventDelegate()
{
	NIASSERT(m_spDX9Render);
	m_spDX9Render->RemoveAllLostDeviceNotificationFuncs();
	m_spDX9Render->RemoveAllResetNotificationFuncs();
}
//-------------------------------------------------------------------------------------------------
}
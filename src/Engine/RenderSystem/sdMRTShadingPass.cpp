#include "Stdafx.h"
#include "sdMRTShadingPass.h"
#include "sdRenderSystem_DX9.h"
#include "sdRenderDevice_DX9.h"

namespace RenderSystem
{
//-------------------------------------------------------------------------------------------------
sdMRTShadingPass::sdMRTShadingPass(sdRenderObjectAlloc* pkAlloc)
: sdRenderPass(pkAlloc)
, m_uiStaticMeshStencilID(0)
, m_uiStaticMeshStencilIDMask(0)
{

}
//-------------------------------------------------------------------------------------------------
sdMRTShadingPass::~sdMRTShadingPass()
{
	if (m_bInitialized)
	{
		Destroy();
		m_bInitialized = false;
	}
}
//-------------------------------------------------------------------------------------------------
bool sdMRTShadingPass::Initialize(uint uiStaticMeshStencilID, uint uiStaticMeshStencilIDMask, 
	NiRenderedTexture* spLightTexture, NiRenderedTexture* spGeomTexture, 
	NiRenderedTexture* spAlbedoTexture, NiRenderedTexture* spGlossTexture)
{
	NIASSERT(uiStaticMeshStencilID);
	NIASSERT(uiStaticMeshStencilIDMask);
	NIASSERT(spLightTexture);
	NIASSERT(spGeomTexture);
	NIASSERT(spAlbedoTexture);
	NIASSERT(spGlossTexture);

	if (m_bInitialized)
	{
		NIASSERT(0);
		return false;
	}

	//
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// 设置模版参数
	m_uiStaticMeshStencilID = uiStaticMeshStencilID;
	m_uiStaticMeshStencilIDMask = uiStaticMeshStencilIDMask;

	// 设置输入纹理
	m_spGeomTexture = spGeomTexture;
	m_spLightTexture = spLightTexture;
	m_spAlbedoTexture = spAlbedoTexture;
	m_spGlossTexture = spGlossTexture;

	// 初始化材质
	m_spStaticShadingMaterial = pkRenderDevice->CreateMaterial("MRT3Shading0x00000000");
	NIASSERT(m_spStaticShadingMaterial);

	// 初始化纹理属性
	NiTexturingPropertyPtr spTexturingProp  = NiNew NiTexturingProperty;
	NIASSERT(spTexturingProp);
	spTexturingProp->SetShaderMap(0, NiNew NiTexturingProperty::ShaderMap(m_spLightTexture, 0));
	spTexturingProp->SetShaderMap(1, NiNew NiTexturingProperty::ShaderMap(m_spGeomTexture, 0));
	spTexturingProp->SetShaderMap(2, NiNew NiTexturingProperty::ShaderMap(m_spAlbedoTexture, 0));
	spTexturingProp->SetShaderMap(3, NiNew NiTexturingProperty::ShaderMap(m_spGlossTexture, 0));
	m_kPropertyList.AddTail((NiTexturingProperty*)spTexturingProp);

	return (m_bInitialized = true);
}
//-------------------------------------------------------------------------------------------------
void sdMRTShadingPass::Destroy()
{
	m_kPropertyList.RemoveAll();

	m_spStaticShadingMaterial = 0;
	m_spSkinnedShadingMaterial = 0;

	m_uiStaticMeshStencilID = 0;
	m_uiStaticMeshStencilIDMask = 0;

	m_spGeomTexture = 0;
	m_spLightTexture = 0;
	m_spAlbedoTexture = 0;
	m_spGlossTexture = 0;

	m_bInitialized = false;
}
//-------------------------------------------------------------------------------------------------
void sdMRTShadingPass::Draw()
{
	D3DPERF_BeginEvent(0xff000000, L"MRTShadingPass");

	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	sdRenderSystem* pkRenderSystem = sdRenderSystem_DX9::InstancePtr();
	NIASSERT(pkRenderSystem);

	// 清除数据绑定
	pkRenderDevice->ClearVertexBinding();
	//pkRenderDevice->ClearTextureBinding();

	// CullMode
	pkRenderDevice->SetRenderState(D3DRS_CULLMODE,			D3DCULL_NONE);

	// Alpha
	pkRenderDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,	false);
	pkRenderDevice->SetRenderState(D3DRS_ALPHATESTENABLE,	false);

	// Z
	pkRenderDevice->SetRenderState(D3DRS_ZENABLE,			false);
	pkRenderDevice->SetRenderState(D3DRS_ZWRITEENABLE,		false);

	// Stencil
	pkRenderDevice->SetRenderState(D3DRS_STENCILENABLE,		true);
	pkRenderDevice->SetRenderState(D3DRS_STENCILFUNC,		D3DCMP_EQUAL);
	pkRenderDevice->SetRenderState(D3DRS_STENCILFAIL,		D3DSTENCILOP_KEEP);
	pkRenderDevice->SetRenderState(D3DRS_STENCILZFAIL,		D3DSTENCILOP_KEEP);
	pkRenderDevice->SetRenderState(D3DRS_STENCILPASS,		D3DSTENCILOP_KEEP);
	pkRenderDevice->SetRenderState(D3DRS_STENCILREF,		m_uiStaticMeshStencilID);
	pkRenderDevice->SetRenderState(D3DRS_STENCILMASK,		m_uiStaticMeshStencilIDMask);

	// 绘制
	// @{
	NiMesh* spScreenMesh = pkRenderSystem->GetDefaultMesh(sdRenderSystem::E_SCREEN_QUAD);
	NIASSERT(spScreenMesh);

	NiPropertyState* spPropState = spScreenMesh->GetPropertyState();
	NIASSERT(spPropState);
	spPropState->SwapProperties(m_kPropertyList);
	spScreenMesh->ApplyAndSetActiveMaterial(m_spStaticShadingMaterial);

	NiRenderer* spRenderer = NiRenderer::GetRenderer();
	spScreenMesh->RenderImmediate(spRenderer);

	spScreenMesh->SetActiveMaterial(NULL);
	spPropState->SwapProperties(m_kPropertyList);
	// @}

	D3DPERF_EndEvent();
}
//-------------------------------------------------------------------------------------------------
}
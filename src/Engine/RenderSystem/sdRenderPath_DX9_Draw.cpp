#include "Stdafx.h"
#include "sdRenderPath_DX9.h"
#include "sdRenderDevice_DX9.h"
#include "sdRenderSystem.h"

namespace RenderSystem
{
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::Draw()
{
	// ������Ⱦ��ֱ�ӷ���
	if (!m_bEnableRendering)
		return;

	// ����������Ⱦ״̬
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);
	pkRenderDevice->SaveAllRenderStates();

	// ���ó�ʼ��Ⱦ״̬
	PrepareRenderStates();

	// ���ñ任����(ò����������ӳ�һ�µȴ�RenderTarget���������)
	pkRenderDevice->SetCameraData(m_spCurCam);

	// ������ɫ��ȫ�ֱ���
	PrepareShaderConstants();

	// ��Ⱦ
	//@{
	// Ԥ������ȵ�DepthStencilBuffer
	//DrawEarlyZPass();

	// ���Ƶ���Depth���ض�����,����Ⱦ��ģ�建��
	DrawTerrainDepthPass();

	// ���������GeometryBuffer,����Ⱦ��ģ�建��(����Building/Tree/Doodads/Terrain)
	DrawGeometryPasses();

	// ����
	//DrawPreLightPass();

	// ������ɫ�ϳ�
	DrawShadingPasses();

	// AO?

	// ȫ����
	//DrawFogPass();

	// ˮ��͸�����

	// ˮ

	// ˮ��͸�����

	// ������Ч
	//DrawPostProcessEffect();

	// ����֮���

	//@}

	// �ָ�������Ⱦ״̬
	pkRenderDevice->RestoreAllRenderStates();
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::PrepareRenderStates()
{
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// ��դ��ģʽ(��ʼֵ)
	//pkRenderState->SetRenderState(D3DRS_FILLMODE,	D3DFILL_SOLID);

	// ��
	// @{
	// ����Ӳ��ȫ����(ȫ��ֵ)
	// D3DRS_FOGENABLE
	// D3DRS_RANGEFOGENABLE
	// D3DRS_FOGCOLOR
	// D3DRS_FOGTABLEMODE
	// D3DRS_FOGSTART
	// D3DRS_FOGEND
	// D3DRS_FOGDENSITY
	pkRenderDevice->SetRenderState(D3DRS_FOGENABLE, false);
	// @}

	// ����Ӳ��sRGB����(ȫ��ֵ)
	pkRenderDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, 0);

	// ���
	// @{
	// ��Ȼ���(��ʼֵ)
	//pkRenderDevice->SetRenderState(D3DRS_ZENABLE,		true);
	//pkRenderDevice->SetRenderState(D3DRS_ZWRITEENABLE,	true);
	//pkRenderDevice->SetRenderState(D3DRS_ZFUNC,			D3DCMP_LESSEQUAL);

	// ���ƫ��(��ʼֵ)
	// D3DRS_DEPTHBIAS
	// D3DRS_SLOPESCALEDEPTHBIAS
	//pkRenderDevice->SetRenderState(D3DRS_DEPTHBIAS,				0);
	//pkRenderDevice->SetRenderState(D3DRS_SLOPESCALEDEPTHBIAS,	0)
	// @}

	// ģ��
	// @{
	// ģ�����
	// D3DRS_STENCILENABLE
	// D3DRS_STENCILFAIL
	// D3DRS_STENCILZFAIL
	// D3DRS_STENCILPASS
	// D3DRS_STENCILFUNC
	// D3DRS_STENCILREF
	// D3DRS_STENCILMASK
	// D3DRS_STENCILWRITEMASK
	//
	// D3DRS_TWOSIDEDSTENCILMODE
	// D3DRS_CCW_STENCILFAIL
	// D3DRS_CCW_STENCILZFAIL
	// D3DRS_CCW_STENCILPASS
	// D3DRS_CCW_STENCILFUNC
	//pkRenderDevice->SetRenderState(D3DRS_STENCILENABLE,		false);
	// @}

	// ͸��
	// @{
	// ͸������
	// D3DRS_ALPHATESTENABLE
	// D3DRS_ALPHAREF
	// D3DRS_ALPHAFUNC
	//pkRenderDevice->SetRenderState(D3DRS_ALPHATESTENABLE,	false);

	// ͸�����
	// D3DRS_ALPHABLENDENABLE
	// D3DRS_BLENDOP
	// D3DRS_SRCBLEND
	// D3DRS_DESTBLEND
	//
	// D3DRS_SEPARATEALPHABLENDENABLE
	// D3DRS_BLENDOPALPHA
	// D3DRS_SRCBLENDALPHA
	// D3DRS_DESTBLENDALPHA
	//pkRenderDevice->SetRenderState(D3DRS_ALPHABLENDENABLE,	false);
	// @}

	// ��ɫ
	// @{
	// ������ɫ���(��ʼֵ)
	// D3DRS_COLORWRITEENABLE
	// D3DRS_COLORWRITEENABLE1
	// D3DRS_COLORWRITEENABLE2
	// D3DRS_COLORWRITEENABLE3
	pkRenderDevice->SetRenderState(D3DRS_COLORWRITEENABLE,	D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN |	D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA);
	// @}
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::PrepareShaderConstants()
{
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// ���
	// @{
	// �����̬
	NiPoint3 kCamDir = m_spCurCam->GetWorldDirection();
	NiPoint3 kCamUp	 = m_spCurCam->GetWorldUpVector();
	NiPoint3 kCamRight = m_spCurCam->GetWorldRightVector();
	kCamUp.Unitize();
	kCamRight.Unitize();
	pkRenderDevice->SetGlobalShaderConstant("g_vCameraUp", sizeof(NiPoint3), &kCamUp);
	pkRenderDevice->SetGlobalShaderConstant("g_vCameraRight", sizeof(NiPoint3), &kCamRight);

	// �����׶��(��׶�������ǶԳƵ�)
	const NiFrustum& kCamFrustum = m_spCurCam->GetViewFrustum();
	NiPoint4 kFrustumData(kCamFrustum.m_fRight, kCamFrustum.m_fTop, kCamFrustum.m_fNear, kCamFrustum.m_fFar);
	pkRenderDevice->SetGlobalShaderConstant("g_fCameraFarClip", sizeof(kCamFrustum.m_fFar), &kCamFrustum.m_fFar);
	pkRenderDevice->SetGlobalShaderConstant("g_vCameraFrustum", sizeof(NiPoint4), &kFrustumData);

	// �任����
	


	// @}

	
	// �ƹ�
	// @{
	bool bMainLight		= m_kRenderParams.IsEnableChannel(sdRenderParams::E_BUILDING, sdRenderParams::E_MAINLIGHT);
	bool bAssistLight	= m_kRenderParams.IsEnableChannel(sdRenderParams::E_BUILDING, sdRenderParams::E_ASSISTLIGHT);
	bool bAmbientLight	= m_kRenderParams.IsEnableChannel(sdRenderParams::E_BUILDING, sdRenderParams::E_AMBIENTLIGHT);
	bool bLocalLight	= m_kRenderParams.IsEnableChannel(sdRenderParams::E_BUILDING, sdRenderParams::E_LOCALLIGHT);

	NiColor kTerrainAmbientColor(0.0f, 0.0f, 0.0f);
	NiColor kSkyAmbientColor(1.0f, 1.0f, 1.0f);

	NiPoint3 kMainLightDir(1.0f, 1.0f, -1.0f);
	NiColor kMainLightColor(1.0f, 1.0f, 1.0f);
	NiColor kMainLightAmbient(0.0f, 0.0f, 0.0f);

	NiPoint3 kAssistLightDir(1.0f, -1.0f, -1.0f);
	NiColor kAssistLightColor(1.0f, 1.0f, 1.0f);
	NiColor kAssistLightAmbient(0.0f, 0.0f, 0.0f);

	NiPoint4 kLightFactor(1.0f, 1.0f, 1.0f, 1.0f);

	pkRenderDevice->SetGlobalShaderConstant("g_vTerraimAmbientColor", sizeof(kTerrainAmbientColor), &kTerrainAmbientColor);
	pkRenderDevice->SetGlobalShaderConstant("g_vSkyAmbientColor", sizeof(kSkyAmbientColor), &kSkyAmbientColor);

	pkRenderDevice->SetGlobalShaderConstant("g_vMainLightDir", sizeof(kMainLightDir), &kMainLightDir);
	pkRenderDevice->SetGlobalShaderConstant("g_vMainLightColor", sizeof(kMainLightColor), &kMainLightColor);
	pkRenderDevice->SetGlobalShaderConstant("g_vMainLightAmbientColor", sizeof(kMainLightAmbient), &kMainLightAmbient);

	pkRenderDevice->SetGlobalShaderConstant("g_vAssistLightDir", sizeof(kAssistLightDir), &kAssistLightDir);
	pkRenderDevice->SetGlobalShaderConstant("g_vAssistLightColor", sizeof(kAssistLightColor), &kAssistLightColor);
	pkRenderDevice->SetGlobalShaderConstant("g_vAssistLightAmbientColor", sizeof(kAssistLightAmbient), &kAssistLightAmbient);
	
	pkRenderDevice->SetGlobalShaderConstant("g_vLightFactor", sizeof(kLightFactor), &kLightFactor);
	// }@


	// ȫ����
	// @{


	// @}

	//
	NiPoint4 kNormalScale(1.0f, 1.0f, 1.0f, 1.0f);
	pkRenderDevice->SetGlobalShaderConstant("g_vNormalScale", sizeof(kNormalScale), &kNormalScale);
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::DrawEarlyZPass()
{
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// ����Ŀ�껺��
	pkRenderDevice->SetRenderTargetGroup(m_spCurFinalRenderTarget);

	// ��ջ���(��Ȼ����ģ�建��)
	float	fFarDepth		= 1.0f;
	uint	uiBlackStencil	= 0x0;
	pkRenderDevice->Clear(NULL, &fFarDepth, &uiBlackStencil);

	// ����
	m_pkEarlyZPass->Draw();
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::DrawTerrainDepthPass()
{
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// ����Ŀ�껺��(������Ȼ���)
	pkRenderDevice->SetRenderTargetGroup(m_spTerrainDepthTarget);

	// ��ջ���(��ɫ����,��������Ȼ���)
	uint	uiBlackColor	= 0x00000000;
	float	fFarDepth		= 1.0f;
	uint	uiBlackStencil	= 0x0;
	pkRenderDevice->Clear(&uiBlackColor, &fFarDepth, &uiBlackStencil);

	// ����TerrainMesh
	m_pkTerrainDepthPass->Draw();
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::DrawGeometryPasses()
{
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// ����Ŀ�껺��(���λ���)
	pkRenderDevice->SetRenderTargetGroup(m_spGeometryAlbedoMRT);

	// ��ջ���(��ɫ����)
	uint uiBlackColor = 0x00000000;
	pkRenderDevice->Clear(&uiBlackColor, NULL, NULL);

	// ����StaticMesh
	m_pkMRTZGeometryPass->Draw();
	
	// ����SkinnedMesh

	// ����Doodads

	// ����Terrain
	m_pkTerrainAtlasGeometryPass->Draw();
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::DrawPreLightPass()
{
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// ����Ŀ�껺��(���ջ���)
	pkRenderDevice->SetRenderTargetGroup(m_spLightTarget);

	// ��ջ���(��ɫ����)
	uint uiBlackColor = 0x00000000;
	pkRenderDevice->Clear(&uiBlackColor, NULL, NULL);

	// ����
	//
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::DrawShadingPasses()
{
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// ����Ŀ�껺��(���λ���)
	pkRenderDevice->SetRenderTargetGroup(m_spCurFinalRenderTarget);

	// ��ջ���(��ɫ����)
	NiColor& kFogColor = m_kEnvironmentParams.fogColor;
	uint uiFogR = (uint)(kFogColor.r * 255);
	uint uiFogG = (uint)(kFogColor.g * 255);
	uint uiFogB = (uint)(kFogColor.b * 255);
	uint uiD3DFogColor = (uiFogR << 16) | (uiFogG << 8) | uiFogB;
	pkRenderDevice->Clear(&uiD3DFogColor, NULL, NULL);

	// ��ɫ����
	m_pkTerrainAtlasShadingPass->Draw();

	// ��ɫ��·

	// ��ɫStaticMesh
	m_pkMRTShadingPass->Draw();

	// ��ɫSkinnedMesh

	// ��ɫDecal��DecalShadow

	// ��ɫ�ر�Doodads

	// ��ɫ��պ�

}
//-------------------------------------------------------------------------------------------------
}
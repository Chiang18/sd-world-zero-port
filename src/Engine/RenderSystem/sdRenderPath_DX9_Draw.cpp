#include "Stdafx.h"
#include "sdRenderPath_DX9.h"
#include "sdRenderDevice_DX9.h"

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

	// ���ñ任����
	pkRenderDevice->SetCameraData(m_spCurCam);

	// ������ɫ��ȫ�ֱ���
	PrepareShaderConstants();

	// ��Ⱦ
	//@{
	// Ԥ������ȵ�DepthStencilBuffer
	DrawEarlyZPass();

	// ���Ƶ���Depth���ض�����
	//DrawTerrainDepthPass();

	// ���������GeometryBuffer(����Building/Tree/Doodads/Terrain)
	DrawGeometryPasses();

	// ����
	//DrawPreLightPass();

	// ������ɫ�ϳ�
	//DrawShadingPasses();

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

}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::DrawEarlyZPass()
{
	if (m_bUseEarlyZ)
	{
		sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
		NIASSERT(pkRenderDevice);

		// ����Ŀ�껺��
		pkRenderDevice->SetRenderTargetGroup(m_spCurFinalRenderTarget);

		// ��ջ���(��Ȼ����ģ�建��)
		//uint	uiBlackColor	= 0x00000000;
		float	fFarDepth		= 1.0f;
		uint	uiBlackStencil	= 0x00;
		pkRenderDevice->Clear(NULL, &fFarDepth, &uiBlackStencil);

		// ����
		m_pkEarlyZPass->Draw();
	}
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
}
//-------------------------------------------------------------------------------------------------
}
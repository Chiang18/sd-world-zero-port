#include "Stdafx.h"
#include "sdRenderPath_DX9.h"
#include "sdRenderDevice_DX9.h"
#include "sdRenderObject.h"
#include "sdRenderObjectAlloc.h"
#include "sdRenderSystem.h"

// World����
#include "sdMap.h"

namespace RenderSystem
{

//-------------------------------------------------------------------------------------------------
sdRenderPath_DX9::sdRenderPath_DX9()
: m_pkCurMap(NULL)
, m_spCurCam(NULL)
, m_spCurFinalRenderTarget(NULL)
, m_bEnableRendering(true)
, m_bUseEarlyZ(true)
{
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// �����յ�RenderTargetGroup
	// @{
	// G-Buffer
	m_spGeometryAlbedoMRT	= pkRenderDevice->CreateRenderTarget(3);
	NIASSERT(m_spGeometryAlbedoMRT);
	// @}

	// ������Ⱦ����
	m_pkRenderParams		= NiNew sdRenderParams;
	m_pkEnvironmentParams	= NiNew sdEnvironmentParams;
	m_pkPostProcessParams	= NiNew sdPostProcessParams;

	// ����RenderObject����������
	m_pkRenderObjectAlloc = NiNew sdRenderObjectAlloc(1024 * 1024 * 5, 1024 * 1024);
	NIASSERT(m_pkRenderObjectAlloc);

	// ����RenderPass
	//@{
	m_pkEarlyZPass = NiNew sdEarlyZPass(m_pkRenderObjectAlloc);
	NIASSERT(m_pkEarlyZPass);

	m_pkMRTZGeometryPass = NiNew sdMRTZGeometryPass(m_pkRenderObjectAlloc);
	NIASSERT(m_pkMRTZGeometryPass);
	//@}
}
//-------------------------------------------------------------------------------------------------
sdRenderPath_DX9::~sdRenderPath_DX9()
{
	if (m_bInitialized)
	{
		Destroy();
		m_bInitialized = false;
	}

	// ���RenderPass
	// @{
	m_pkEarlyZPass = 0;
	m_pkMRTZGeometryPass = 0;
	// }@

	// ���RenderTargetGroup
	// @{
	m_spGeometryAlbedoMRT	= 0;
	// @}
}
//-------------------------------------------------------------------------------------------------
bool sdRenderPath_DX9::Initialize()
{
	// �Ѿ���ʼ����ֱ�ӷ���
	if (m_bInitialized)
	{
		NIASSERT(0);
		return false;
	}

	//
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	NiRenderTargetGroup* spRenderTarget = pkRenderDevice->GetDefaultRenderTargetGroup();
	NIASSERT(spRenderTarget);
	uint uiWidth = spRenderTarget->GetWidth(0);
	uint uiHeight = spRenderTarget->GetHeight(0);

	// ��ʼ��Shader����
	NiPoint2 kWindowSize((float)uiWidth, (float)uiHeight);
	NiShaderFactory::UpdateGlobalShaderConstant("g_vWindowSize", sizeof(NiPoint2), &kWindowSize);

	// ����Texture
	CreateAllRenderedTexture();

	// ��ʼ��RenderPass
	m_pkEarlyZPass->Initialize(E_SID_STATIC_MESH, E_SID_MASK);
	m_pkMRTZGeometryPass->Initialize(E_SID_STATIC_MESH, E_SID_MASK);

	return (m_bInitialized = true);
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::Destroy()
{
	// �Ѿ���������ֱ�ӷ���
	if (!m_bInitialized)
		return;

	// ����RenderPass
	m_pkMRTZGeometryPass->Destroy();
	m_pkEarlyZPass->Destroy();

	// ����Textute
	ReleaseAllRenderTexture();
	
	m_bInitialized = false;
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::CreateAllRenderedTexture()
{
	typedef sdRenderDevice::TextureSpec TextureSpec;

	//
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	// ��ȡ��Ļ�ߴ�
	NiRenderTargetGroup* spRenderTarget = pkRenderDevice->GetDefaultRenderTargetGroup();
	NIASSERT(spRenderTarget);
	uint uiWidth = spRenderTarget->GetWidth(0);
	uint uiHeight = spRenderTarget->GetHeight(0);

	//
	NiTexture::FormatPrefs kFormatARGB8;
	kFormatARGB8.m_ePixelLayout = NiTexture::FormatPrefs::SINGLE_COLOR_32;
	kFormatARGB8.m_eAlphaFmt	= NiTexture::FormatPrefs::SMOOTH;
	kFormatARGB8.m_eMipMapped	= NiTexture::FormatPrefs::NO;

	TextureSpec kFullTextureSpec	= {uiWidth,		uiHeight,	kFormatARGB8};
	TextureSpec kHalfTextureSpec	= {uiWidth/2,	uiHeight/2, kFormatARGB8};
	TextureSpec kQuaterTextureSpec	= {uiWidth/4,	uiHeight/4, kFormatARGB8};

	// ��������(WZ��NiRenderedTexture�����һ��Resize����,ʡ�������)
	// @{
	// Geometry-Buffer
	m_spGeometryTexture = pkRenderDevice->CreateRenderTexture(kFullTextureSpec);
	NIASSERT(m_spGeometryTexture);

	m_spAlbedoTexture = pkRenderDevice->CreateRenderTexture(kFullTextureSpec);
	NIASSERT(m_spAlbedoTexture);

	m_spGlossTexture = pkRenderDevice->CreateRenderTexture(kFullTextureSpec);
	NIASSERT(m_spGlossTexture);

	// @}

}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::ReleaseAllRenderTexture()
{
	m_spGeometryTexture = 0;
	m_spAlbedoTexture = 0;
	m_spGeometryTexture = 0;
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::UpdateRenderParams(const sdRenderParams& kRenderParams)
{
	NIASSERT(m_pkRenderParams);
	*m_pkRenderParams = kRenderParams;
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::UpdateEnvironmentParams(const sdEnvironmentParams& kEnvParams)
{
	NIASSERT(m_pkEnvironmentParams);
	*m_pkEnvironmentParams = kEnvParams;
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::UpdatePostProcessParams(const sdPostProcessParams& kPostProcessParams)
{
	NIASSERT(m_pkPostProcessParams);
	*m_pkPostProcessParams = kPostProcessParams;
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::RenderStaticMesh(NiMesh* spMesh)
{
	// �������Ԥ��Ⱦͨ��
	//m_pkEarlyZPass->InsertStaticMesh(spMesh);

	// ����G-Buffer����ͨ��
	m_pkMRTZGeometryPass->InsertStaticMesh(spMesh);
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::RenderScene(sdMap* pkMap, NiCamera* spCamera, NiRenderTargetGroup* spRenderTarget)
{
	NIASSERT(pkMap);
	NIASSERT(spCamera);
	NIASSERT(spRenderTarget);

	BeginRenderScene(pkMap, spCamera, spRenderTarget);

	// �ü�
	Render();

	// ��Ⱦ
	Draw();

	EndRenderScene();	
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::BeginRenderScene(sdMap* pkMap, NiCamera* spCamera, NiRenderTargetGroup* spRenderTarget)
{
	// �����Ⱦ����
	m_pkRenderObjectAlloc->DeallocAll();

	// ����
	Update(pkMap, spCamera, spRenderTarget);

	// ��ʼRenderPass����
	m_pkEarlyZPass->Begin();
	m_pkMRTZGeometryPass->Begin();
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::EndRenderScene()
{
	// ����RenderPass����
	m_pkMRTZGeometryPass->End();
	m_pkEarlyZPass->End();

	// ����
	Update(NULL, NULL, NULL);

	// �����Ⱦ����
	m_pkRenderObjectAlloc->DeallocAll();
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::Update(sdMap* pkMap, NiCamera* spCamera, NiRenderTargetGroup* spRenderTarget)
{
	m_pkCurMap = pkMap;
	m_spCurCam = spCamera;

	// 
	UpdateRenderTargets(spRenderTarget);
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::UpdateRenderTargets(NiRenderTargetGroup* spRenderTarget)
{
	m_spCurFinalRenderTarget = spRenderTarget;

	if (spRenderTarget)
	{
		// Depth/Stencil Buffer
		NiDepthStencilBuffer* spDSBuffer = spRenderTarget->GetDepthStencilBuffer();
		NIASSERT(spDSBuffer);

		// G-Buffer
		m_spGeometryAlbedoMRT->AttachBuffer(m_spGeometryTexture->GetBuffer(),	0);
		m_spGeometryAlbedoMRT->AttachBuffer(m_spAlbedoTexture->GetBuffer(),		1);
		m_spGeometryAlbedoMRT->AttachBuffer(m_spGlossTexture->GetBuffer(),		2);
		m_spGeometryAlbedoMRT->AttachDepthStencilBuffer(spDSBuffer);
	}
	else
	{
		
	}
}
//-------------------------------------------------------------------------------------------------

}
#include "Stdafx.h"
#include "sdRenderPath_DX9.h"
#include "sdRenderDevice_DX9.h"
#include "sdRenderObject.h"
#include "sdRenderObjectAlloc.h"

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
	
	// Light-Buffer
	m_spLightTarget = pkRenderDevice->CreateRenderTarget(1);
	NIASSERT(m_spLightTarget);

	// Terrain-Depth-Buffer
	m_spTerrainDepthTarget = pkRenderDevice->CreateRenderTarget(1);
	NIASSERT(m_spTerrainDepthTarget);
	// @}

	// ����RenderObject����������
	m_pkRenderObjectAlloc = NiNew sdRenderObjectAlloc(1024 * 1024 * 5, 1024 * 1024);
	NIASSERT(m_pkRenderObjectAlloc);

	// ����RenderPass
	//@{
	m_pkEarlyZPass = NiNew sdEarlyZPass(m_pkRenderObjectAlloc);
	NIASSERT(m_pkEarlyZPass);

	m_pkMRTZGeometryPass = NiNew sdMRTZGeometryPass(m_pkRenderObjectAlloc);
	NIASSERT(m_pkMRTZGeometryPass);

	m_pkMRTShadingPass = NiNew sdMRTShadingPass(m_pkRenderObjectAlloc);
	NIASSERT(m_pkMRTShadingPass);

	m_pkTerrainDepthPass = NiNew sdTerrainDepthPass(m_pkRenderObjectAlloc);
	NIASSERT(m_pkTerrainDepthPass);

	m_pkTerrainAtlasGeometryPass = NiNew sdTerrainAtlasGeometryPass(m_pkRenderObjectAlloc);
	NIASSERT(m_pkTerrainAtlasGeometryPass);

	m_pkTerrainAtlasShadingPass = NiNew sdTerrainAtlasShadingPass(m_pkRenderObjectAlloc);
	NIASSERT(m_pkTerrainAtlasShadingPass);

	m_pkDebugGizmoPass = NiNew sdDebugGizmoPass(m_pkRenderObjectAlloc);
	NIASSERT(m_pkDebugGizmoPass);
	//@}


	// ��ʼ����Ⱦ����
	m_kRenderParams.Reset();
	m_kEnvironmentParams.Reset();
	m_kPostProcessParams.Reset();
	m_kTerrainParams.Reset();
}
//-------------------------------------------------------------------------------------------------
sdRenderPath_DX9::~sdRenderPath_DX9()
{
	if (m_bInitialized)
	{
		Destroy();
		m_bInitialized = false;
	}

	// ���Params
	// @{
	m_kRenderParams.Reset();
	m_kEnvironmentParams.Reset();
	m_kPostProcessParams.Reset();
	m_kTerrainParams.Reset();
	// @}

	// ���RenderPass
	// @{
	m_pkEarlyZPass = 0;
	m_pkMRTZGeometryPass = 0;
	m_pkMRTShadingPass = 0;
	m_pkTerrainDepthPass = 0;
	m_pkTerrainAtlasGeometryPass = 0;
	m_pkTerrainAtlasShadingPass = 0;
	m_pkDebugGizmoPass = 0;
	// }@

	// ���RenderTargetGroup
	// @{
	m_spGeometryAlbedoMRT	= 0;
	m_spLightTarget	= 0;
	m_spTerrainDepthTarget = 0;
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

	// ��ʼ��Shader����(��Ҫ����Ļ�ߴ����)
	// @{
	// ��Ļ�ߴ�
	NiPoint2 kWindowSize((float)uiWidth, (float)uiHeight);
	pkRenderDevice->SetGlobalShaderConstant("g_vWindowSize", sizeof(NiPoint2), &kWindowSize);

	// ��Ļ������ƫ��
	NiPoint2 kHalfPixelOffset(0.5f / (float)uiWidth, 0.5f / (float)uiHeight);
	pkRenderDevice->SetGlobalShaderConstant("g_vHalfPixelOffset", sizeof(NiPoint2), &kHalfPixelOffset);

	// ͶӰ���굽��Ļ���������ƫ��
	NiPoint2 kPixelToTexelOffset(0.5f + 0.5f / (float)uiWidth, 0.5f + 0.5f / (float)uiHeight);
	pkRenderDevice->SetGlobalShaderConstant("g_vPixToTexOffset", sizeof(NiPoint2), &kPixelToTexelOffset);
	// @}


	// ����Texture
	CreateAllRenderedTexture();

	// ��ʼ��RenderPass
	// @{
	m_pkEarlyZPass->Initialize(E_SID_STATIC_MESH, E_SID_MASK);

	m_pkMRTZGeometryPass->Initialize(E_SID_STATIC_MESH, E_SID_MASK);
	m_pkMRTShadingPass->Initialize(E_SID_STATIC_MESH, E_SID_MASK, m_spDepthOrLightTexture, m_spGeometryTexture, m_spAlbedoTexture, m_spGlossTexture);
	
	m_pkTerrainDepthPass->Initialize(E_SID_TERRAIN, E_SID_MASK);
	m_pkTerrainAtlasGeometryPass->Initialize(E_SID_TERRAIN, E_SID_MASK, E_PSB_DISTANCE_TERRAIN, m_spDepthOrLightTexture);
	m_pkTerrainAtlasShadingPass->Initialize(E_SID_TERRAIN, E_SID_MASK, m_spDepthOrLightTexture, m_spGeometryTexture);
	
	m_pkDebugGizmoPass->Initialize();
	// @}

	return (m_bInitialized = true);
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::Destroy()
{
	// �Ѿ���������ֱ�ӷ���
	if (!m_bInitialized)
		return;

	// ����RenderPass
	m_pkEarlyZPass->Destroy();
	m_pkMRTZGeometryPass->Destroy();
	m_pkMRTShadingPass->Destroy();
	m_pkTerrainDepthPass->Destroy();
	m_pkTerrainAtlasGeometryPass->Destroy();
	m_pkTerrainAtlasShadingPass->Destroy();
	m_pkDebugGizmoPass->Destroy();

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
	kFormatARGB8.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
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

	m_spDepthOrLightTexture = pkRenderDevice->CreateRenderTexture(kFullTextureSpec);
	NIASSERT(m_spDepthOrLightTexture);
	// @}
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::ReleaseAllRenderTexture()
{
	m_spGeometryTexture = 0;
	m_spAlbedoTexture = 0;
	m_spGeometryTexture = 0;
	m_spDepthOrLightTexture = 0;
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::UpdateRenderParams(const sdRenderParams& kRenderParams)
{
	m_kRenderParams = kRenderParams;

	// ���µ�����RenderPass(������Ϊʲô�ÿ���)
	m_pkEarlyZPass->SetRenderParams(&kRenderParams);
	m_pkMRTZGeometryPass->SetRenderParams(&kRenderParams);
	m_pkMRTShadingPass->SetRenderParams(&kRenderParams);
	m_pkTerrainDepthPass->SetRenderParams(&kRenderParams);
	m_pkTerrainAtlasGeometryPass->SetRenderParams(&kRenderParams);
	m_pkTerrainAtlasShadingPass->SetRenderParams(&m_kRenderParams);
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::UpdateEnvironmentParams(const sdEnvironmentParams& kEnvParams)
{
	m_kEnvironmentParams = kEnvParams;
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::UpdatePostProcessParams(const sdPostProcessParams& kPostProcessParams)
{
	m_kPostProcessParams = kPostProcessParams;
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::UpdateTerrainParams(const sdTerrainParams& kTerrainParams)
{
	// ���ټ������Ƿ����ı�
	// (����sdTerrainParams�̳���NiMemObject�ʿ���ʹ��memcmp����)
	if (0 != memcmp(&m_kTerrainParams, &kTerrainParams, sizeof(sdTerrainParams)))
	{
		m_kTerrainParams = kTerrainParams;

		// ���µ�����RenderPass
		m_pkTerrainAtlasGeometryPass->SetTerrainParams(kTerrainParams);
		m_pkTerrainAtlasShadingPass->SetTerrainParams(kTerrainParams);
	}
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::RenderStaticMesh(NiMesh* spMesh)
{
	NIASSERT(spMesh);

	// �������Ԥ��Ⱦͨ��
	//m_pkEarlyZPass->InsertStaticMesh(spMesh);


	//***************************************
	// ����DarkMap
	NiPropertyStatePtr spPropState = spMesh->GetPropertyState();
	NiTexturingPropertyPtr spTexProp = spPropState->GetTexturing();
	spTexProp->SetDarkMap(NULL);
	spTexProp->SetParallaxMap(NULL);
	//***************************************

	// ����G-Buffer����ͨ��
	m_pkMRTZGeometryPass->InsertStaticMesh(spMesh);
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::RenderTerrainMesh(NiMesh* spMesh)
{
	NIASSERT(spMesh);

	// ������������Ⱦͨ��
	m_pkTerrainDepthPass->InsertStaticMesh(spMesh);
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
	m_pkMRTShadingPass->Begin();
	m_pkTerrainDepthPass->Begin();
	m_pkTerrainAtlasGeometryPass->Begin();
	m_pkTerrainAtlasShadingPass->Begin();
	m_pkDebugGizmoPass->Begin();
}
//-------------------------------------------------------------------------------------------------
void sdRenderPath_DX9::EndRenderScene()
{
	// ����RenderPass����
	m_pkEarlyZPass->End();
	m_pkMRTZGeometryPass->End();
	m_pkMRTShadingPass->End();
	m_pkTerrainDepthPass->End();
	m_pkTerrainAtlasGeometryPass->End();
	m_pkTerrainAtlasShadingPass->End();
	m_pkDebugGizmoPass->End();

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

		// Geometry-Buffer
		m_spGeometryAlbedoMRT->AttachBuffer(m_spGeometryTexture->GetBuffer(),	0);
		m_spGeometryAlbedoMRT->AttachBuffer(m_spAlbedoTexture->GetBuffer(),		1);
		m_spGeometryAlbedoMRT->AttachBuffer(m_spGlossTexture->GetBuffer(),		2);
		m_spGeometryAlbedoMRT->AttachDepthStencilBuffer(spDSBuffer);

		// Light-Buffer
		m_spLightTarget->AttachBuffer(m_spDepthOrLightTexture->GetBuffer(),		0);
		m_spLightTarget->AttachDepthStencilBuffer(spDSBuffer);

		// Terrain-Depth-Buffer
		m_spTerrainDepthTarget->AttachBuffer(m_spDepthOrLightTexture->GetBuffer(),	0);
		m_spTerrainDepthTarget->AttachDepthStencilBuffer(spDSBuffer);
	}
	else
	{
		
	}
}
//-------------------------------------------------------------------------------------------------

}
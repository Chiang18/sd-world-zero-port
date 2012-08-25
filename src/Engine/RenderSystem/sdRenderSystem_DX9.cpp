#include "Stdafx.h"
#include "sdRenderSystem_DX9.h"
#include "sdRenderDevice_DX9.h"
#include "sdRenderPath_DX9.h"
#include <sdFileUtil.h>

// ����
#include "sdMap.h"
#include "sdTerrain.h"

namespace RenderSystem
{
//-------------------------------------------------------------------------------------------------
sdRenderSystem_DX9::sdRenderSystem_DX9()
: m_pkRenderDevice(NULL)
, m_pkRenderPath(NULL)
, m_pkMap(NULL)
{

}
//-------------------------------------------------------------------------------------------------
sdRenderSystem_DX9::~sdRenderSystem_DX9()
{
	if (m_bInitialized)
	{
		Destroy();
		m_bInitialized = false;
	}
}
//-------------------------------------------------------------------------------------------------
bool sdRenderSystem_DX9::Initialize()
{
	using namespace std;

	// �Ѿ���ʼ����ֱ�ӷ���
	if (m_bInitialized) 
	{
		NIASSERT(0);
		return false;
	}

	// GB��Ⱦ�豸û�г�ʼ����ֱ�ӷ���
	NiRenderer* spRender = NiRenderer::GetRenderer();
	if (!spRender)
	{
		NIASSERT(0);	
		return false;
	}
		
	// ����sd��Ⱦ�豸,����ʼ��
	m_pkRenderDevice = NiNew sdRenderDevice_DX9;
	if (!m_pkRenderDevice || !m_pkRenderDevice->Initialize())
	{
		NIASSERT(0);
		Destroy();
		return false;
	}

	// ��ʼ����ɫ��
	// @{
	string szProgramPath = sdFileUtil::GetModulePath() + "\\..\\dependence\\engine\\shader\\DX9";
	string szShaderPath = sdFileUtil::GetModulePath() + "\\..\\dependence\\engine\\shader";
	const char* apcProgramDirectories[1] = { szProgramPath.c_str()};
	const char* apcShaderDirectories[1] = { szShaderPath.c_str()};

	m_pkRenderDevice->SetupShaderSystem(apcProgramDirectories, 1, apcShaderDirectories, 1);
	// @}

	// ����sd��Ⱦ·��
	m_pkRenderPath = NiNew sdRenderPath_DX9;
	NIASSERT(m_pkRenderPath);
	m_pkRenderPath->Initialize();
	
	// ��ȡĬ����ȾĿ��
	m_spRenderTargetGroup = m_pkRenderDevice->GetDefaultRenderTargetGroup();
	NIASSERT(m_spRenderTargetGroup);

	// ����Ĭ�����


	// ����Ĭ����Դ 
	// @{
	// ����Ĭ�ϼ�����
	m_spScreenQuadMesh = CreateScreenQuadMesh();

	// ����Ĭ�ϲ���
	char* szMaterialNames[NUM_DEFAULT_MATERIALS] = { 
		"StaticMesh_FlatShading", 
		"SkinnedMesh_FlatShading"};
	for (int i = 0; i < NUM_DEFAULT_MATERIALS; ++i)
	{
		m_spDefaultMaterials[i] = m_pkRenderDevice->CreateMaterial(szMaterialNames[i]);
	}

	// ����Ĭ������
	// }@


	return (m_bInitialized = true);
}
//-------------------------------------------------------------------------------------------------
void sdRenderSystem_DX9::Destroy()
{
	// �Ѿ���������ֱ�ӷ���
	if (!m_bInitialized)
		return;

	//
	
	m_bInitialized = false;
}
//-------------------------------------------------------------------------------------------------
void sdRenderSystem_DX9::UpdateFrame(float fAccumTime, float fFrameTime)
{

}
//-------------------------------------------------------------------------------------------------
void sdRenderSystem_DX9::RenderFrame()
{
	//sdPF_Func_Auto;

	//sdEntity::TickSysRenderFrameId();

	// ��ȡGB��Ⱦ�豸
	NiDX9Renderer* spDX9Renderer = NiDX9Renderer::GetRenderer();
	NIASSERT(spDX9Renderer);

	// ����豸��ʧ
	if (spDX9Renderer->IsDeviceLost())
	{
		return;
	}

	// ���浱ǰ��ȾĿ��
	//NiRenderTargetGroupPtr spCurRenderTarget = spDX9Renderer->GetCurrentRenderTargetGroup();

	// ��ʼ��Ⱦ֡
	if (!spDX9Renderer->GetInsideFrameState())
	{
		spDX9Renderer->BeginFrame();
	}

	// ��ʼ����Ⱦ֡
	D3DPERF_BeginEvent(0xffff0000, L"MainFrame");
	DrawOpaqueUI();
	DrawScene();
	DrawTranslucentUI();
	D3DPERF_EndEvent();

	// ������Ⱦ֡
	if (spDX9Renderer->GetInsideFrameState())
	{
		spDX9Renderer->EndFrame();
	}

	// �ָ�֮ǰ��ȾĿ��
	//if (spCurRenderTarget)
	//{
	//	if (spDX9Renderer->GetCurrentRenderTargetGroup())
	//		spDX9Renderer->EndUsingRenderTargetGroup;
	//	spDX9Renderer->BeginUsingRenderTargetGroup(spCurRenderTarget, NiRenderer::CLEAR_NONE);
	//}
}
//-------------------------------------------------------------------------------------------------
void sdRenderSystem_DX9::DisplayFrame()
{
	// ��ȡGB��Ⱦ�豸
	NiDX9Renderer* spDX9Renderer = NiDX9Renderer::GetRenderer();
	NIASSERT(spDX9Renderer);

	// ��ʾ
	spDX9Renderer->DisplayFrame();
}
//-------------------------------------------------------------------------------------------------
void sdRenderSystem_DX9::DrawOpaqueUI()
{

}
//-------------------------------------------------------------------------------------------------
void sdRenderSystem_DX9::DrawTranslucentUI()
{ 
	
}
//-------------------------------------------------------------------------------------------------
void sdRenderSystem_DX9::DrawScene()
{
	using namespace RenderSystem;

	//sdPF_Func_Auto

	// ��鳡��/���/��ȾĿ��/��Ⱦ�豸/��Ⱦ·����׼�����
	if (!m_pkMap)		return;
	if (!m_spCamera)	return;
	if (!m_spRenderTargetGroup)	return;
	if (!m_pkRenderDevice || !m_pkRenderDevice->IsInitialized())	return;
	if (!m_pkRenderPath || !m_pkRenderPath->IsInitialized())		return;

	// �ӳ�����ȡ��Ⱦ����,�����µ���Ⱦ��
	// @{
	const sdRenderParams& kRenderParams = m_pkMap->GetRenderParams();
	m_pkRenderPath->UpdateRenderParams(kRenderParams);

	const sdEnvironmentParams& kEnvironmentParams = m_pkMap->GetEnvironmentParams();
	m_pkRenderPath->UpdateEnvironmentParams(kEnvironmentParams);

	const sdPostProcessParams& kPostProcessParams = m_pkMap->GetPostProcessParams();
	m_pkRenderPath->UpdatePostProcessParams(kPostProcessParams);

	sdTerrain* pkTerrain = m_pkMap->GetTerrain();
	if (pkTerrain && pkTerrain->IsVisible())
	{
		const sdTerrainParams& kTerrainParams = pkTerrain->GetTerrainParams();
		m_pkRenderPath->UpdateTerrainParams(kTerrainParams);
	}
	// @}

	// ��Ⱦ
	m_pkRenderPath->RenderScene(m_pkMap, m_spCamera, m_spRenderTargetGroup);
}
//-------------------------------------------------------------------------------------------------
}

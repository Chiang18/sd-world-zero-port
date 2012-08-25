#include "Stdafx.h"
#include "sdRenderSystem_DX9.h"
#include "sdRenderDevice_DX9.h"
#include "sdRenderPath_DX9.h"
#include <sdFileUtil.h>

// 场景
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

	// 已经初始化则直接返回
	if (m_bInitialized) 
	{
		NIASSERT(0);
		return false;
	}

	// GB渲染设备没有初始化则直接返回
	NiRenderer* spRender = NiRenderer::GetRenderer();
	if (!spRender)
	{
		NIASSERT(0);	
		return false;
	}
		
	// 创建sd渲染设备,并初始化
	m_pkRenderDevice = NiNew sdRenderDevice_DX9;
	if (!m_pkRenderDevice || !m_pkRenderDevice->Initialize())
	{
		NIASSERT(0);
		Destroy();
		return false;
	}

	// 初始化着色器
	// @{
	string szProgramPath = sdFileUtil::GetModulePath() + "\\..\\dependence\\engine\\shader\\DX9";
	string szShaderPath = sdFileUtil::GetModulePath() + "\\..\\dependence\\engine\\shader";
	const char* apcProgramDirectories[1] = { szProgramPath.c_str()};
	const char* apcShaderDirectories[1] = { szShaderPath.c_str()};

	m_pkRenderDevice->SetupShaderSystem(apcProgramDirectories, 1, apcShaderDirectories, 1);
	// @}

	// 创建sd渲染路径
	m_pkRenderPath = NiNew sdRenderPath_DX9;
	NIASSERT(m_pkRenderPath);
	m_pkRenderPath->Initialize();
	
	// 获取默认渲染目标
	m_spRenderTargetGroup = m_pkRenderDevice->GetDefaultRenderTargetGroup();
	NIASSERT(m_spRenderTargetGroup);

	// 创建默认相机


	// 创建默认资源 
	// @{
	// 创建默认几何体
	m_spScreenQuadMesh = CreateScreenQuadMesh();

	// 创建默认材质
	char* szMaterialNames[NUM_DEFAULT_MATERIALS] = { 
		"StaticMesh_FlatShading", 
		"SkinnedMesh_FlatShading"};
	for (int i = 0; i < NUM_DEFAULT_MATERIALS; ++i)
	{
		m_spDefaultMaterials[i] = m_pkRenderDevice->CreateMaterial(szMaterialNames[i]);
	}

	// 创建默认纹理
	// }@


	return (m_bInitialized = true);
}
//-------------------------------------------------------------------------------------------------
void sdRenderSystem_DX9::Destroy()
{
	// 已经被销毁则直接返回
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

	// 获取GB渲染设备
	NiDX9Renderer* spDX9Renderer = NiDX9Renderer::GetRenderer();
	NIASSERT(spDX9Renderer);

	// 检查设备丢失
	if (spDX9Renderer->IsDeviceLost())
	{
		return;
	}

	// 保存当前渲染目标
	//NiRenderTargetGroupPtr spCurRenderTarget = spDX9Renderer->GetCurrentRenderTargetGroup();

	// 开始渲染帧
	if (!spDX9Renderer->GetInsideFrameState())
	{
		spDX9Renderer->BeginFrame();
	}

	// 开始主渲染帧
	D3DPERF_BeginEvent(0xffff0000, L"MainFrame");
	DrawOpaqueUI();
	DrawScene();
	DrawTranslucentUI();
	D3DPERF_EndEvent();

	// 结束渲染帧
	if (spDX9Renderer->GetInsideFrameState())
	{
		spDX9Renderer->EndFrame();
	}

	// 恢复之前渲染目标
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
	// 获取GB渲染设备
	NiDX9Renderer* spDX9Renderer = NiDX9Renderer::GetRenderer();
	NIASSERT(spDX9Renderer);

	// 显示
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

	// 检查场景/相机/渲染目标/渲染设备/渲染路径的准备情况
	if (!m_pkMap)		return;
	if (!m_spCamera)	return;
	if (!m_spRenderTargetGroup)	return;
	if (!m_pkRenderDevice || !m_pkRenderDevice->IsInitialized())	return;
	if (!m_pkRenderPath || !m_pkRenderPath->IsInitialized())		return;

	// 从场景获取渲染参数,并更新到渲染器
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

	// 渲染
	m_pkRenderPath->RenderScene(m_pkMap, m_spCamera, m_spRenderTargetGroup);
}
//-------------------------------------------------------------------------------------------------
}

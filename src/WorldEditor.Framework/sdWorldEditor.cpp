#include "Stdafx.h"
#include "sdWorldEditor.h"

//
#include <NiDX9Renderer.h>
#include <NiDX9SystemDesc.h>

//
#include <sdFileUtil.h>

//
#include <sdCameraEditState.h>

using namespace Base;
using namespace RenderSystem;
using namespace GameCore;
using namespace GameEditEx;
//-------------------------------------------------------------------------------------------------
sdWorldEditor::sdWorldEditor()
{
	m_hWnd = NULL;
	m_spRenderer = NULL;
}

sdWorldEditor::~sdWorldEditor()
{

}
//-------------------------------------------------------------------------------------------------
bool sdWorldEditor::Initialize(HWND hWnd)
{
	// ���ô��ھ��
	m_hWnd = hWnd;

	// ��ʼ��Gamebryo
	NiInitOptions* spInitOptions = NiExternalNew NiInitOptions;
	spInitOptions->SetFloodgateParallelExecution(false);
	NiInit(spInitOptions);

	// ���ö�̬Shader��Ĭ������Ŀ¼
	std::string szWorkingDir = sdFileUtil::GetModulePath() + "\\dependence\\Shader\\Generated";
	NiMaterial::SetDefaultWorkingDirectory(szWorkingDir.c_str());

	// ����PerHUD�豸,������ʹ��֮
	const NiDX9SystemDesc* spDesc = NiDX9Renderer::GetSystemDesc();
	NiDX9Renderer::DeviceDesc eDevType = NiDX9Renderer::DEVDESC_PURE;
	unsigned int uiAdapter = D3DADAPTER_DEFAULT;
	unsigned int uiAdapterCount = spDesc->GetAdapterCount();
	for (unsigned int i = 0; i < uiAdapterCount; ++i)
	{
		const NiDX9AdapterDesc* spAdapter = spDesc->GetAdapter(i);
		const char* pcAdapterName = spAdapter->GetStringDescription();
		if (strstr(pcAdapterName, "NVIDIA PerfHUD"))
		{
			uiAdapter = i;
			eDevType = NiDX9Renderer::DEVDESC_REF_HWVERTEX;
			break;
		}
	}

	// ��ȡ������Ϣ
	RECT kRect;
	::GetClientRect(m_hWnd, &kRect);
	uint uiWidth = kRect.right - kRect.left;
	uint uiHeight = kRect.bottom - kRect.top;

	// ����Gamebryo�豸
	NIASSERT(!m_spRenderer);
	m_spRenderer = NiDX9Renderer::Create(
		uiWidth, uiHeight,
		NiDX9Renderer::USE_MULTITHREADED,
		m_hWnd, m_hWnd,
		uiAdapter,
		eDevType,
		NiDX9Renderer::FBFMT_X8R8G8B8,
		NiDX9Renderer::DSFMT_D24S8,
		NiDX9Renderer::PRESENT_INTERVAL_IMMEDIATE,
		NiDX9Renderer::SWAPEFFECT_DEFAULT,
		NiDX9Renderer::FBMODE_DEFAULT);
	NIASSERT(m_spRenderer);
	m_spRenderer->IncRefCount();

	// ��ʼ����Ⱦϵͳ
	m_kRenderSystem.Initialize();


	//*************************************************
	// ��ʱ����
	m_pkMap = NiNew sdMap;
	NIASSERT(m_pkMap);
	m_pkMap->CreateScene();

	m_kRenderSystem.SetMap((sdMap*)m_pkMap);
	//*************************************************


	// ��ʼ�����ϵͳ
	// @{
	m_kCameraFSM.Initialize();

	sdEditCameraState* pkEditFreeCamState = new sdEditFreeCameraState;
	m_kCameraFSM.AddState(pkEditFreeCamState);

	m_kCameraFSM.SetState(sdEditCameraState::E_EDIT_CAMERA_FREE);
	m_kRenderSystem.SetCamera(m_kCameraFSM.GetCamera());
	// @}

	return true;
}
//-------------------------------------------------------------------------------------------------
void sdWorldEditor::Destroy()
{

}
//-------------------------------------------------------------------------------------------------
void sdWorldEditor::Update()
{
	if (m_hWnd)
	{
		// ����
		// @{
		// ����ʱ��
		m_kTimeMgr.Update();

		// �������
		m_kCameraFSM.UpdateState();
		// @}

		// ��Ⱦ
		// @{
		m_kRenderSystem.RenderFrame();
		m_kRenderSystem.DisplayFrame();
		// @}
	}
}
//-------------------------------------------------------------------------------------------------
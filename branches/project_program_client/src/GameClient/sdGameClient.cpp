#include "Stdafx.h"
#include "sdGameClient.h"
#include <sdFileUtil.h>
#include <NiDX9SystemDesc.h>

//-------------------------------------------------------------------------------------------------
LRESULT CALLBACK sdGameClient::WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	// 默认消息处理
	sdGameClient* pkGameClient = sdGameClient::InstancePtr();
	if (!pkGameClient || !pkGameClient->IsInitialized())
	{
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	// 自定义的特殊消息处理
	LRESULT lRresult = 0;
	switch (uMsg)
	{
	//case WM_INPUTLANGCHANGEREQUEST: 
	//	break;

	//case WM_SETCURSOR:
	//	break;

	// 窗口关闭时,三个消息顺序是WM_CLOSE,WM_DESTROY,WM_QUIT
	//case WM_CLOSE:
	//	break;

	// 发送WM_QUIT消息
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;

	//case WM_ACTIVATE:
	//case WM_ACTIVATEAPP:

	// 最大化与最小化消息
	//case WM_SYSCOMMAND:

	// 系统按键消息
	//case WM_SYSKEYDOWN:
	//	if (wParam == VK_MENU)
	//		return 0;
	//	else if (wParam == VK_F10)
	//		return 0;
	//	break;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
//-------------------------------------------------------------------------------------------------
bool sdGameClient::OnLostDevice(void* pData)
{
	return true;
}
//-------------------------------------------------------------------------------------------------
bool sdGameClient::OnResetDevice(bool bBefore, void* pData)
{
	return true;
}
//-------------------------------------------------------------------------------------------------
sdGameClient::sdGameClient()
{
	m_bInitialize = false;
	m_bRunning = false;
}
//-------------------------------------------------------------------------------------------------
sdGameClient::~sdGameClient()
{

}
//-------------------------------------------------------------------------------------------------
bool sdGameClient::Initialize()
{
	// 创建窗口
	if (!CreateGameWindow())
	{
		::MessageBox(NULL, "创建窗口失败", "错误", MB_OK);
		return false;
	}

	// 初始化Gamebryo
	NiInitOptions* spInitOptions = NiExternalNew NiInitOptions;
	spInitOptions->SetFloodgateParallelExecution(false);
	NiInit(spInitOptions);

	// 创建Gamebryo渲染器
	if (!CreateGameRenderer())
	{
		::MessageBox(NULL, "创建窗口失败", "错误", MB_OK);
		return false;
	}

	// 初始化WZ渲染器
	RenderSystem::sdRenderSystem_DX9::Instance().Initialize();

	//*************************************************
	// 临时代码
	NiStream kStream;
	kStream.Load("E:\\project_game\\dependence\\engine\\TheClockAndTheOcularis.nif");
	NiAVObjectPtr pAVObject = (NiAVObject*)kStream.GetObjectAt(0);
	pAVObject->Update(0.0f);
	pAVObject->UpdateProperties();
	pAVObject->UpdateEffects();

	m_pkMap = NiNew sdMap;
	m_pkMap->m_pkRoot = pAVObject;

	RenderSystem::sdRenderSystem_DX9::Instance().SetMap((sdMap*)m_pkMap);
	//*************************************************

	return (m_bInitialize = true), (m_bRunning = true);
}
//-------------------------------------------------------------------------------------------------
void sdGameClient::Destroy()
{
	using namespace RenderSystem;

	// 销毁WZ渲染器
	sdRenderSystem_DX9::Instance().Destroy();

	// 销毁Gamebryo渲染器
	m_spRenderer = 0;

	// 销毁Gamebryo
	NiShutdown();

	// 销毁窗口
	::DestroyWindow(m_hWnd);
}
//-------------------------------------------------------------------------------------------------
bool sdGameClient::Run()
{
	// 窗口消息分发
	MSG kMsg;
	if (::PeekMessage(&kMsg, NULL, 0, 0, PM_REMOVE))
	{
		// 退出消息,则设置退出标记
		if (kMsg.message == WM_QUIT)
			return (m_bRunning = false);

		// 普通消息则进行分发
		if (!::TranslateAccelerator(m_hWnd, NULL, &kMsg))
		{
			::TranslateMessage(&kMsg);
			::DispatchMessage(&kMsg);
		}
	}

	// 限帧

	// 计时与更新
	{
		//
		//MeasureTime();

		// 逻辑更新(始终是每秒不高于30FPS)
		UpdateFrame();

		// 渲染与显示
		RenderFrame();

		//
		//MeasureTime();
	}

	return m_bRunning;
}
//-------------------------------------------------------------------------------------------------
bool sdGameClient::CreateGameWindow()
{
	// 游戏分辨率
	sdGameConfig* pkGameConfig = sdGameConfig::InstancePtr();
	sdVideoConfig* pkVideoConfig = &(pkGameConfig->m_kVideoConfig);
	unsigned int uiWidth = pkVideoConfig->m_uiWidth;
	unsigned int uiHeight = pkVideoConfig->m_uiHeight;
	bool bFullScreen = pkVideoConfig->m_bFullScreen;

	// 屏幕分辨率
	HDC hDC = ::GetDC(NULL);
	unsigned int uiScreenWidth = ::GetDeviceCaps(hDC, HORZRES);
	unsigned int uiScreenHeight = ::GetDeviceCaps(hDC, VERTRES);

	// 程序句柄
	HINSTANCE hInstance = (HINSTANCE)::GetCurrentThread();

	// 窗口标题
	static const char* lpszName = "WorldZero";

	// 注册窗口类
	WNDCLASS wc;
	::ZeroMemory(&wc, sizeof(WNDCLASS));
	wc.style			= CS_HREDRAW | CS_HREDRAW | CS_DBLCLKS;	// 窗口样式
	wc.lpfnWndProc		= sdGameClient::WndProc;				// 窗口回调函数
	wc.cbClsExtra		= 0;
	wc.cbWndExtra		= 0;
	wc.hInstance		= hInstance;
	wc.hIcon			= ::LoadIcon(NULL, IDI_ERROR);
	wc.hCursor			= ::LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground	= (HBRUSH)::GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName		= 0;
	wc.lpszClassName	= lpszName;
	::RegisterClass(&wc);

	// 根据客户区尺寸,窗口样式计算窗口尺寸,位置
	RECT rect = {0, 0, uiWidth, uiHeight};
	DWORD dwStyle = 0;
	if (bFullScreen)
		dwStyle = WS_POPUP;
	else
		dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX;
	::AdjustWindowRect(&rect, dwStyle, FALSE);

	unsigned int uiWindowWidth = rect.right - rect.left;
	unsigned int uiWindowHeight = rect.bottom - rect.top;

	if (!bFullScreen)
	{
		// 窗口状态下要减去标题栏和任务栏高度
		if (uiWindowWidth > uiScreenWidth)		uiWindowWidth = uiScreenWidth;
		if (uiWindowHeight > uiScreenHeight)	uiWindowHeight = uiScreenHeight;
	}

	unsigned int uiX = (uiScreenWidth - uiWindowWidth) / 2;
	unsigned int uiY = (uiScreenHeight - uiWindowHeight) / 2;

	m_uiWindowWidth = uiWindowWidth;
	m_uiWindowHeight = uiWindowHeight;

	// 创建窗口
	m_hWnd = ::CreateWindow(lpszName, lpszName,
		dwStyle,
		uiX, uiY,
		uiWindowWidth, uiWindowHeight,
		NULL, NULL,
		hInstance,
		NULL);
	if (!m_hWnd)
		return false;

	// 隐藏窗口
	::ShowWindow(m_hWnd, SW_HIDE);

	// 更新窗口
	::UpdateWindow(m_hWnd);

	return true;
}
//-------------------------------------------------------------------------------------------------
bool sdGameClient::CreateGameRenderer()
{
	sdGameConfig* pkGameConfig = sdGameConfig::InstancePtr();
	sdVideoConfig* pkVideoConfig = &(pkGameConfig->m_kVideoConfig);
	bool bVSync = pkVideoConfig->m_bVSync;

	// 设置动态Shader的默认生成目录
	std::string szWorkingDir = sdFileUtil::GetModulePath() + "\\dependence\\Shader\\Generated";
	NiMaterial::SetDefaultWorkingDirectory(szWorkingDir.c_str());

	// 查找PerHUD设备,存在则使用之
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

	// 获取渲染窗口客户区尺寸
	RECT rect;
	::GetClientRect(m_hWnd, &rect);
	unsigned int uiWidth = rect.right - rect.left;
	unsigned int uiHeight = rect.bottom - rect.top;

	// 创建Gamebryo设备
	NIASSERT(!m_spRenderer);
	m_spRenderer = NiDX9Renderer::Create(
		uiWidth, uiHeight,
		NiDX9Renderer::USE_NOFLAGS,
		m_hWnd, m_hWnd,
		uiAdapter,
		eDevType,
		NiDX9Renderer::FBFMT_X8R8G8B8,
		NiDX9Renderer::DSFMT_D24S8,
		bVSync ? NiDX9Renderer::PRESENT_INTERVAL_ONE : NiDX9Renderer::PRESENT_INTERVAL_IMMEDIATE,
		NiDX9Renderer::SWAPEFFECT_DEFAULT,
		NiDX9Renderer::FBMODE_DEFAULT);

	if (!m_spRenderer)
		return false;

	// 设置设备回调函数
	m_spRenderer->AddLostDeviceNotificationFunc(OnLostDevice, 0);
	m_spRenderer->AddResetNotificationFunc(OnResetDevice, 0);
	m_spRenderer->SetMipmapSkipLevel(pkVideoConfig->m_uiMipmapSkipLevel);

	return true;
}
//-------------------------------------------------------------------------------------------------
void sdGameClient::UpdateFrame()
{

}
//-------------------------------------------------------------------------------------------------
void sdGameClient::RenderFrame()
{
	//sdPF_Func_Auto;

	// 貌似意思是在非窗口最小化时才渲染
	if (!::IsIconic(m_hWnd))
	{
		m_kRenderSystem.RenderFrame();
		m_kRenderSystem.DisplayFrame();
	}
}
//-------------------------------------------------------------------------------------------------
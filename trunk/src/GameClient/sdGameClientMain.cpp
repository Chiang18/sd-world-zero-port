#include "Stdafx.h"
#include "sdGameClient.h"
#include "resource.h"

// 全局变量
HINSTANCE		g_hModule = NULL;
sdGameClient*	g_pkGameClient = NULL;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// 游戏启动标志
	CreateMutex(NULL, TRUE, "wzclient");

	// 硬件与软件检查
	// CheckSystem()

	// 注册CRT退出回调函数
	// atexit(ExitDump)

	// 
	g_hModule = hInstance;

	// 加载进度条
	//g_pkLoadingDlg = new sdLoadingDlg;
	//g_pkLoadingDlg->Initialize();

	// 客户端初始化
	g_pkGameClient = new sdGameClient;
	assert(g_pkGameClient);
	g_pkGameClient->Initialize();

	// 销毁进度条
	//g_pkLoadingDlg->Destroy();
	//delete g_pkLoadingDlg;
	//g_pkLoadingDlg = NULL;

	// 显示客户端窗口
	::ShowWindow(g_pkGameClient->GetWindowHandle(), SW_SHOWNORMAL);
	::UpdateWindow(g_pkGameClient->GetWindowHandle());

	// 客户端运行循环
	while(g_pkGameClient->Run());
	
	// 客户端销毁
	g_pkGameClient->Destroy();
	delete g_pkGameClient;
	g_pkGameClient = NULL;

	// 客户端结束后调用浏览器之类的
	//CreateProcess()

	return 0;
};
#include "Stdafx.h"
#include "sdGameClient.h"
#include "resource.h"

// ȫ�ֱ���
HINSTANCE		g_hModule = NULL;
sdGameClient*	g_pkGameClient = NULL;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	// ��Ϸ������־
	CreateMutex(NULL, TRUE, "wzclient");

	// Ӳ����������
	// CheckSystem()

	// ע��CRT�˳��ص�����
	// atexit(ExitDump)

	// 
	g_hModule = hInstance;

	// ���ؽ�����
	//g_pkLoadingDlg = new sdLoadingDlg;
	//g_pkLoadingDlg->Initialize();

	// �ͻ��˳�ʼ��
	g_pkGameClient = new sdGameClient;
	assert(g_pkGameClient);
	g_pkGameClient->Initialize();

	// ���ٽ�����
	//g_pkLoadingDlg->Destroy();
	//delete g_pkLoadingDlg;
	//g_pkLoadingDlg = NULL;

	// ��ʾ�ͻ��˴���
	::ShowWindow(g_pkGameClient->GetWindowHandle(), SW_SHOWNORMAL);
	::UpdateWindow(g_pkGameClient->GetWindowHandle());

	// �ͻ�������ѭ��
	while(g_pkGameClient->Run());
	
	// �ͻ�������
	g_pkGameClient->Destroy();
	delete g_pkGameClient;
	g_pkGameClient = NULL;

	// �ͻ��˽�������������֮���
	//CreateProcess()

	return 0;
};
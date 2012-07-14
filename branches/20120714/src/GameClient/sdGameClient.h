//*************************************************************************************************
// ����:	�ͻ������
//---------------------------------------------------------
// ����:		
// ����:		2012-07-07
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _GAME_CLIENT_H__
#define _GAME_CLIENT_H__
#include <sdSingleton.h>
#include <sdRenderSystem_DX9.h>
#include <sdGameConfig.h>
#include <sdMap.h>


// �ͻ������
class sdGameClient : sdTSingleton<sdGameClient>
{
public:
	sdGameClient();
	~sdGameClient();

	// ��ʼ��������
	bool Initialize();
	bool Run();
	void Destroy();

	inline bool IsInitialized(){ return m_bInitialize;};
	inline bool	IsRunning() { return m_bRunning;};

	inline HWND GetWindowHandle(){ return m_hWnd;};

protected:
	bool CreateGameWindow();
	bool CreateGameRenderer();

	void UpdateFrame();
	void RenderFrame();

	// ������Ϣ�ص�����
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// ��ʾ�豸�ص�����
	static bool OnLostDevice(void* pData);
	static bool	OnResetDevice(bool bBefore, void* pData);

protected:
	bool	m_bInitialize;
	bool	m_bRunning;

	// ���ھ��
	HWND	m_hWnd;				

	// ���ڳߴ�(�����˱߿�ͱ�����,��D3D��ʾ����Ҫ��)
	unsigned int	m_uiWindowWidth;	
	unsigned int	m_uiWindowHeight;

	// ����(�������ļ���ȡ)
	sdGameConfig	m_kGameConfig;

	//(�����Ժ��Ǳ���װ����һ��EngineCore/EngineLogic����ȥ)
	//
	// Gamebryo��Ⱦ��
	NiDX9RendererPtr	m_spRenderer;

	// WZ��Ⱦ��
	RenderSystem::sdRenderSystem_DX9	m_kRenderSystem;

	//*****************************************************
	sdMapPtr	m_pkMap;
	//*****************************************************
};

#endif

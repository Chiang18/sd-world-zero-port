//*************************************************************************************************
// 内容:	客户端入口
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-07
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _GAME_CLIENT_H__
#define _GAME_CLIENT_H__
#include <sdSingleton.h>
#include <sdRenderSystem_DX9.h>
#include <sdGameConfig.h>
#include <sdMap.h>


// 客户端入口
class sdGameClient : sdTSingleton<sdGameClient>
{
public:
	sdGameClient();
	~sdGameClient();

	// 初始化与销毁
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

	// 窗口消息回调函数
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// 显示设备回调函数
	static bool OnLostDevice(void* pData);
	static bool	OnResetDevice(bool bBefore, void* pData);

protected:
	bool	m_bInitialize;
	bool	m_bRunning;

	// 窗口句柄
	HWND	m_hWnd;				

	// 窗口尺寸(加上了边框和标题栏,比D3D显示区域要大)
	unsigned int	m_uiWindowWidth;	
	unsigned int	m_uiWindowHeight;

	// 配置(从配置文件读取)
	sdGameConfig	m_kGameConfig;

	//(可能以后是被封装到下一层EngineCore/EngineLogic里面去)
	//
	// Gamebryo渲染器
	NiDX9RendererPtr	m_spRenderer;

	// WZ渲染器
	RenderSystem::sdRenderSystem_DX9	m_kRenderSystem;

	//*****************************************************
	sdMapPtr	m_pkMap;
	//*****************************************************
};

#endif

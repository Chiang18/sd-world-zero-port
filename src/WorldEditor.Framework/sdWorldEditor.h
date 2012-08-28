#pragma once
#include <sdGameEditEx.h>
#include <sdGameCore.h>
#include <sdRenderSystem_DX9.h>
#include <sdMap.h>
#include <sdEventMgr.h>
#include <sdCameraFSM.h>
#include <sdTimeMgr.h>
#include <sdWorldEditFSM.h>

class sdWorldEditor : public NiMemObject
{
public:
	sdWorldEditor();
	~sdWorldEditor();

	// 初始化与销毁
	// @{
	bool	Initialize(HWND hWnd);
	void	Destroy();
	void	Update();
	// @}

	// 相机设置
	// @{

	// @}

	// 编辑模块
	// @{


	// @}

	// 输入消息分发处理器
	void	WndProc(int iMsg, WPARAM wParam, LPARAM lParam);

protected:
	// 主窗口
	HWND	m_hWnd;	

	// 时间
	Base::sdTimeMgr	m_kTimeMgr;

	// 相机控制
	GameCore::sdCameraFSM	m_kCameraFSM;

	// Gamebryo渲染器
	NiDX9RendererPtr	m_spRenderer;

	// WZ渲染系统
	RenderSystem::sdRenderSystem_DX9	m_kRenderSystem;

	// 消息中兴
	GameCore::sdEventMgr m_kEventMgr;

	// 编辑管理器
	GameEditEx::sdWorldEditFSM	m_kWorldEditFSM;

	//*****************************************************
	sdMapPtr	m_pkMap;
	//*****************************************************
};
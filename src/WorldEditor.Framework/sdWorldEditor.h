#pragma once
#include <sdEngineEx.h>
#include <sdGameCore.h>
#include <sdRenderSystem_DX9.h>
#include <sdMap.h>
#include <sdCameraFSM.h>

class sdWorldEditor : public NiMemObject
{
public:
	sdWorldEditor();
	~sdWorldEditor();

	// 
	// @{
	bool	Initialize(HWND hWnd);
	void	Destroy();
	void	Update();
	// @}


	// 输入事件
	// @{
	//void	OnKeyDown(WPARAM wParam, LPARAM lParam);
	//void	OnKeyUp(WPARAM wParam, LPARAM lParam);
	//void	OnMouseMove(WPARAM wParam, LPARAM lParam, const POINT& ptPos);
	//void	OnLeftButtonDown(WPARAM wParam, LPARAM lParam, const POINT& ptPos);
	//void	OnLeftButtonUp(WPARAM wParam, LPARAM lParam, const POINT& ptPos);
	//void	OnRightButtonDown(WPARAM wParam, LPARAM lParam, const POINT& ptPos);
	//void	OnRightButtonUp(WPARAM wParam, LPARAM lParam, const POINT& ptPos);
	// @}

protected:
	// 主窗口
	HWND	m_hWnd;	

	// 相机控制
	GameCore::sdCameraFSM	m_kCameraFSM;

	// Gamebryo渲染器
	NiDX9RendererPtr	m_spRenderer;

	// WZ渲染系统
	RenderSystem::sdRenderSystem_DX9	m_kRenderSystem;

	//*****************************************************
	sdMapPtr	m_pkMap;
	//*****************************************************
};
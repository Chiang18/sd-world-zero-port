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


	// �����¼�
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
	// ������
	HWND	m_hWnd;	

	// �������
	GameCore::sdCameraFSM	m_kCameraFSM;

	// Gamebryo��Ⱦ��
	NiDX9RendererPtr	m_spRenderer;

	// WZ��Ⱦϵͳ
	RenderSystem::sdRenderSystem_DX9	m_kRenderSystem;

	//*****************************************************
	sdMapPtr	m_pkMap;
	//*****************************************************
};
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

	// ��ʼ��������
	// @{
	bool	Initialize(HWND hWnd);
	void	Destroy();
	void	Update();
	// @}

	// �������
	// @{

	// @}

	// �༭ģ��
	// @{


	// @}

	// ������Ϣ�ַ�������
	void	WndProc(int iMsg, WPARAM wParam, LPARAM lParam);

protected:
	// ������
	HWND	m_hWnd;	

	// ʱ��
	Base::sdTimeMgr	m_kTimeMgr;

	// �������
	GameCore::sdCameraFSM	m_kCameraFSM;

	// Gamebryo��Ⱦ��
	NiDX9RendererPtr	m_spRenderer;

	// WZ��Ⱦϵͳ
	RenderSystem::sdRenderSystem_DX9	m_kRenderSystem;

	// ��Ϣ����
	GameCore::sdEventMgr m_kEventMgr;

	// �༭������
	GameEditEx::sdWorldEditFSM	m_kWorldEditFSM;

	//*****************************************************
	sdMapPtr	m_pkMap;
	//*****************************************************
};
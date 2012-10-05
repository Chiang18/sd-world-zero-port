#pragma once
#include <sdGameEditEx.h>
#include <sdGameCore.h>
#include <sdRenderSystem_DX9.h>
#include <sdResourceSystem.h>
#include <sdMap.h>
#include <sdEventMgr.h>
#include <sdCameraFSM.h>
#include <sdTimeMgr.h>
#include <sdWorldEditFSM.h>

class sdWorldEditor : public Base::sdTSingleton<sdWorldEditor>
{
public:
	sdWorldEditor();
	~sdWorldEditor();

	// ��ʼ��������
	// @{
	bool	Initialize(HWND hWnd);
	void	Destroy();
	void	Update();

	bool	SaveScene();
	// @}

	// �������
	// @{

	// @}

	// ���α༭ģ��
	// @{
	//void SetTerrainBrushInnerRadius(float fRadius);
	//float GetTerrainBrushInnerRadius();

	//void SetTerrainBrushOutterRadius(float fRadius);
	//float GetTerrainBrushInnerRadius();

	// @}

	// ���Է���
	GameEditEx::sdWorldEditFSM* GetWorldEditFSM(){ return &m_kWorldEditFSM;}

	// ������Ϣ�ַ�������
	void	WndProc(int iMsg, HWND hWnd, WPARAM wParam, LPARAM lParam);

protected:
	// ������
	HWND	m_hWnd;	

	// ʱ��
	Base::sdTimeMgr	m_kTimeMgr;

	// �������
	GameCore::sdCameraFSM	m_kCameraFSM;

	// Gamebryo��Ⱦ��
	NiDX9RendererPtr	m_spRenderer;

	// ��Ⱦϵͳ
	RenderSystem::sdRenderSystem_DX9	m_kRenderSystem;

	// ��Դϵͳ
	ResourceSystem::sdResourceSystem m_kResourceSystem;

	// ��Ϣ����
	GameCore::sdEventMgr m_kEventMgr;

	// �༭������
	GameEditEx::sdWorldEditFSM	m_kWorldEditFSM;

	// ����
	sdMap*	m_pkMap;
};
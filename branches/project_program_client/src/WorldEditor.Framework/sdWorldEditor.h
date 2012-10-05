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

	// 初始化与销毁
	// @{
	bool	Initialize(HWND hWnd);
	void	Destroy();
	void	Update();

	bool	SaveScene();
	// @}

	// 相机设置
	// @{

	// @}

	// 地形编辑模块
	// @{
	//void SetTerrainBrushInnerRadius(float fRadius);
	//float GetTerrainBrushInnerRadius();

	//void SetTerrainBrushOutterRadius(float fRadius);
	//float GetTerrainBrushInnerRadius();

	// @}

	// 属性访问
	GameEditEx::sdWorldEditFSM* GetWorldEditFSM(){ return &m_kWorldEditFSM;}

	// 输入消息分发处理器
	void	WndProc(int iMsg, HWND hWnd, WPARAM wParam, LPARAM lParam);

protected:
	// 主窗口
	HWND	m_hWnd;	

	// 时间
	Base::sdTimeMgr	m_kTimeMgr;

	// 相机控制
	GameCore::sdCameraFSM	m_kCameraFSM;

	// Gamebryo渲染器
	NiDX9RendererPtr	m_spRenderer;

	// 渲染系统
	RenderSystem::sdRenderSystem_DX9	m_kRenderSystem;

	// 资源系统
	ResourceSystem::sdResourceSystem m_kResourceSystem;

	// 消息中心
	GameCore::sdEventMgr m_kEventMgr;

	// 编辑管理器
	GameEditEx::sdWorldEditFSM	m_kWorldEditFSM;

	// 场景
	sdMap*	m_pkMap;
};
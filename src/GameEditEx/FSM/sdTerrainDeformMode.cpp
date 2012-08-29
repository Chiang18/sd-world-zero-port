#include "Stdafx.h"
#include "sdTerrainDeformMode.h"
#include "sdEditBrush.h"

//
#include <sdRenderSystem_DX9.h>
#include <sdRenderDevice_DX9.h>
#include <sdTerrain.h>

//
#include <windef.h>

using namespace RenderSystem;

namespace GameEditEx
{
//-------------------------------------------------------------------------------------------------
sdTerrainDeformMode::sdTerrainDeformMode()
: m_bLeftButtonDown(false)
{
	m_kMousePos.x = m_kMousePos.y = 0;
}
//-------------------------------------------------------------------------------------------------
sdTerrainDeformMode::~sdTerrainDeformMode()
{

}
//-------------------------------------------------------------------------------------------------
bool sdTerrainDeformMode::Initialize()
{
	m_spTerrainDeformPoolBrush = NiNew sdTerrainDeformPoolBrush;
	NIASSERT(m_spTerrainDeformPoolBrush);

	return true;
}
//-------------------------------------------------------------------------------------------------
void sdTerrainDeformMode::Destroy()
{
	m_spTerrainDeformPoolBrush = 0;
}
//-------------------------------------------------------------------------------------------------
int sdTerrainDeformMode::Update()
{
	// 计算屏幕射线
	// @{
	// 获取屏幕尺寸
	sdRenderDevice* pkRenderDevice = sdRenderDevice_DX9::InstancePtr();
	NIASSERT(pkRenderDevice);

	NiRenderTargetGroup* spRenderTargetGroup = pkRenderDevice->GetDefaultRenderTargetGroup();
	NIASSERT(spRenderTargetGroup);

	uint uiWidth = spRenderTargetGroup->GetWidth(0);
	uint uiHeight = spRenderTargetGroup->GetHeight(0);

	// 获取相机
	sdRenderSystem* pkRenderSystem = sdRenderSystem_DX9::InstancePtr();
	NIASSERT(pkRenderSystem);

	NiCamera* spCamera = pkRenderSystem->GetCamera();
	NIASSERT(spCamera);

	// 计算Pick射线
	NiPoint3 kOrigin;
	NiPoint3 kDir;
	NiViewMath::MouseToRay((float)m_kMousePos.x, (float)m_kMousePos.y, uiWidth, uiHeight, spCamera, kOrigin, kDir);
	// @}


	// 拾取地形
	sdTerrain* pkTerrain = sdTerrain::InstancePtr();
	NIASSERT(pkTerrain);

	



	return __super::Update();;
}
//-------------------------------------------------------------------------------------------------
void sdTerrainDeformMode::OnLeftButtonDown(WPARAM wParam, LPARAM lParam)
{
	__super::OnLeftButtonDown(wParam,lParam);
	m_bLeftButtonDown = true;
}
//-------------------------------------------------------------------------------------------------
void sdTerrainDeformMode::OnLeftButtonUp(WPARAM wParam, LPARAM lParam)
{
	__super::OnLeftButtonUp(wParam, lParam);
	m_bLeftButtonDown = false;
}
//-------------------------------------------------------------------------------------------------
void sdTerrainDeformMode::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	__super::OnMouseMove(wParam, lParam);

	// 提取鼠标屏幕位置
	m_kMousePos.x = (int)(short)LOWORD(lParam);
	m_kMousePos.y = (int)(short)HIWORD(lParam);
}
//-------------------------------------------------------------------------------------------------
}
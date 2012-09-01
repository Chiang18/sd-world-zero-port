#include "Stdafx.h"
#include "sdTerrainDeformMode.h"
#include "sdEditBrush.h"

//
#include <sdRay.h>

//
#include <sdRenderSystem_DX9.h>
#include <sdRenderDevice_DX9.h>
#include <sdMap.h>
#include <sdTerrain.h>

//
#include <windef.h>

using namespace Base::Math;
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
	sdRenderSystem* pkRenderSystem = sdRenderSystem::InstancePtr();
	NIASSERT(pkRenderSystem);

	sdMap* pkMap = pkRenderSystem->GetMap();
	NIASSERT(pkMap);

	m_pkDualCircleMesh = NiNew sdDualCircleMesh;
	NIASSERT(m_pkDualCircleMesh);
	pkMap->GetDebugNode()->AttachChild((NiAVObject*)(sdDualCircleMesh*)m_pkDualCircleMesh);

	sdDualCircleShapePtr pkDualCircleShape = NiNew sdDualCircleShape;
	NIASSERT(pkDualCircleShape);

	m_pkTerrainDeformPoolBrush = NiNew sdTerrainDeformPoolBrush;
	NIASSERT(m_pkTerrainDeformPoolBrush);
	m_pkTerrainDeformPoolBrush->SetBrushShape((NiAVObject*)(sdDualCircleShape*)m_pkDualCircleShape);
	m_pkTerrainDeformPoolBrush->SetEditShape((sdDualCircleShape*)pkDualCircleShape);

	return true;
}
//-------------------------------------------------------------------------------------------------
void sdTerrainDeformMode::Destroy()
{
	m_pkTerrainDeformPoolBrush = 0;
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
	
	sdRay kRay(sdVector3(kOrigin.x, kOrigin.y, kOrigin.z), sdVector3(kDir.x, kDir.y, kDir.z));
	// @}


	// 拾取地形
	//m_pkTerrainDeformPoolBrush->Apply();

	sdTerrain* pkTerrain = sdTerrain::InstancePtr();
	NIASSERT(pkTerrain);

	sdVector3 kIntersect;
	if (pkTerrain->Pick(kRay, kIntersect))	
	{
		m_pkDualCircleMesh->SetTranslate(kOrigin);
		m_pkDualCircleMesh->Update(0.0f);
		m_pkDualCircleMesh->UpdateShape();
	}



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
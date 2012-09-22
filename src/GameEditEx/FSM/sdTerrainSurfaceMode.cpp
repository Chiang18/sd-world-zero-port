#include "Stdafx.h"
#include "sdTerrainSurfaceMode.h"
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
sdTerrainSurfaceMode::sdTerrainSurfaceMode()
: m_bLeftButtonDown(false)
{
	m_kMousePos.x = m_kMousePos.y = 0;
	SetStateID(E_EDITMODE_TERRAIN_SURFACE);
}
//-------------------------------------------------------------------------------------------------
sdTerrainSurfaceMode::~sdTerrainSurfaceMode()
{

}
//-------------------------------------------------------------------------------------------------
bool sdTerrainSurfaceMode::Initialize()
{
	sdRenderSystem* pkRenderSystem = sdRenderSystem::InstancePtr();
	NIASSERT(pkRenderSystem);

	sdMap* pkMap = pkRenderSystem->GetMap();
	NIASSERT(pkMap);

	sdDualCircleMeshPtr pkDualCircleMesh = NiNew sdDualCircleMesh;
	NIASSERT(pkDualCircleMesh);
	pkDualCircleMesh->SetOuterRadius(10.0f);
	pkDualCircleMesh->SetInnerRadius(10.0f);

	sdDualCircleShapePtr pkDualCircleShape = NiNew sdDualCircleShape;
	NIASSERT(pkDualCircleShape);
	pkDualCircleShape->SetOuterRadius(10.0f);
	pkDualCircleShape->SetInnerRadius(10.0f);

	sdTerrainSurfaceLayerBrushPtr pkTerrainSurfaceLayerBrush = NiNew sdTerrainSurfaceLayerBrush;
	NIASSERT(pkTerrainSurfaceLayerBrush);
	pkTerrainSurfaceLayerBrush->SetBrushShape((sdDualCircleMesh*)pkDualCircleMesh);
	pkTerrainSurfaceLayerBrush->SetEditShape((sdDualCircleShape*)pkDualCircleShape);

	m_pkEditBrush = pkTerrainSurfaceLayerBrush;

	return true;
}
//-------------------------------------------------------------------------------------------------
void sdTerrainSurfaceMode::Destroy()
{
	m_pkEditBrush = 0;
}
//-------------------------------------------------------------------------------------------------
void sdTerrainSurfaceMode::Enter()
{
	__super::Enter();

	//
	sdRenderSystem* pkRenderSystem = sdRenderSystem::InstancePtr();
	NIASSERT(pkRenderSystem);

	sdMap* pkMap = pkRenderSystem->GetMap();
	NIASSERT(pkMap);

	//
	if (m_pkEditBrush)
		pkMap->GetDebugNode()->AttachChild(m_pkEditBrush->GetBrushShape());
}
//-------------------------------------------------------------------------------------------------
void sdTerrainSurfaceMode::Leave()
{
	__super::Leave();

	//
	sdRenderSystem* pkRenderSystem = sdRenderSystem::InstancePtr();
	NIASSERT(pkRenderSystem);

	sdMap* pkMap = pkRenderSystem->GetMap();
	NIASSERT(pkMap);

	//
	if (m_pkEditBrush)
		pkMap->GetDebugNode()->DetachChild(m_pkEditBrush->GetBrushShape());
}
//-------------------------------------------------------------------------------------------------
int sdTerrainSurfaceMode::Update()
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


	if (m_bLeftButtonDown)
	{
		// 左键按下
		//
		// 更新地形笔刷
		m_pkEditBrush->Apply(kRay);
	}
	else
	{
		// 左键未按下
		//
		// 仅仅更新绘制笔刷
		sdTerrain* pkTerrain = sdTerrain::InstancePtr();
		NIASSERT(pkTerrain);

		sdVector3 kIntersect;
		if (pkTerrain->Pick(kRay, kIntersect))	
		{
			sdBrushShape* pkBrushShape = m_pkEditBrush->GetBrushShape();
			NIASSERT(pkBrushShape);

			pkBrushShape->SetTranslate(kIntersect.m_fX, kIntersect.m_fY, 0);
			pkBrushShape->Update(0.0f);
			pkBrushShape->UpdateShape();
		}
	}

	return __super::Update();;
}
//-------------------------------------------------------------------------------------------------
void sdTerrainSurfaceMode::OnLeftButtonDown(WPARAM wParam, LPARAM lParam)
{
	__super::OnLeftButtonDown(wParam,lParam);

	// 提取鼠标屏幕位置
	m_kMousePos.x = (int)(short)LOWORD(lParam);
	m_kMousePos.y = (int)(short)HIWORD(lParam);

	//
	m_bLeftButtonDown = true;
}
//-------------------------------------------------------------------------------------------------
void sdTerrainSurfaceMode::OnLeftButtonUp(WPARAM wParam, LPARAM lParam)
{
	__super::OnLeftButtonUp(wParam, lParam);
	m_bLeftButtonDown = false;
}
//-------------------------------------------------------------------------------------------------
void sdTerrainSurfaceMode::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	__super::OnMouseMove(wParam, lParam);

	// 提取鼠标屏幕位置
	m_kMousePos.x = (int)(short)LOWORD(lParam);
	m_kMousePos.y = (int)(short)HIWORD(lParam);
}
//-------------------------------------------------------------------------------------------------
}
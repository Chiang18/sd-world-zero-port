#include "Stdafx.h"
#include "sdCameraEditState.h"

//
#include <sdTimeMgr.h>

//
#include <sdCameraFSM.h>

using namespace Base;
using namespace GameCore;

namespace GameEditEx
{
//-------------------------------------------------------------------------------------------------
sdEditFreeCameraState::sdEditFreeCameraState()
: m_bMoveForward(false)
, m_bMoveBack(false)
, m_bMoveLeft(false)
, m_bMoveRight(false)
, m_bRightButtonDown(false)
, m_iX(0)
, m_iY(0)
, m_iLastX(0)
, m_iLastY(0)
, m_fFreeCameraSpeed(50.0f)
, m_fFreeCameraRotate(0.001f)
{
	SetStateID(E_EDIT_CAMERA_FREE);
}
//-------------------------------------------------------------------------------------------------
void sdEditFreeCameraState::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	if ('W' == wParam)
	{
		m_bMoveForward = true;
		m_bMoveBack = false;
	}
	else if ('S' == wParam)
	{
		m_bMoveForward = false;
		m_bMoveBack = true;
	}
	else if ('A' == wParam)
	{
		m_bMoveLeft = true;
		m_bMoveRight = false;
	}
	else if ('D' == wParam)
	{
		m_bMoveLeft = false;
		m_bMoveRight = true;
	}
}
//-------------------------------------------------------------------------------------------------
void sdEditFreeCameraState::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	if ('W' == wParam)
	{
		m_bMoveForward = false;
	}
	else if ('S' == wParam)
	{
		m_bMoveBack = false;
	}
	else if ('A' == wParam)
	{
		m_bMoveLeft = false;
	}
	else if ('D' == wParam)
	{
		m_bMoveRight = false;
	}
}
//-------------------------------------------------------------------------------------------------
void sdEditFreeCameraState::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	m_iLastX = m_iX;
	m_iLastY = m_iY;
	m_iX = (int)(short)LOWORD(lParam);
	m_iY = (int)(short)HIWORD(lParam);
}
//-------------------------------------------------------------------------------------------------
void sdEditFreeCameraState::OnRightButtonDown(WPARAM wParam, LPARAM lParam)
{
	m_bRightButtonDown = true;
}
//-------------------------------------------------------------------------------------------------
void sdEditFreeCameraState::OnRightButtonUp(WPARAM wParam, LPARAM lParam)
{
	m_bRightButtonDown = false;
}
//-------------------------------------------------------------------------------------------------
int sdEditFreeCameraState::Update()
{
	sdCameraFSM* pkCameraFSM = sdCameraFSM::InstancePtr();
	NIASSERT(pkCameraFSM);

	Base::sdTimeMgr* pkTimeMgr = Base::sdTimeMgr::InstancePtr();
	NIASSERT(pkTimeMgr);

	NiCamera* spCamera = pkCameraFSM->GetCamera();
	NiPoint3 kCamPosition = spCamera->GetWorldTranslate();

	// 关于平移
	// @{
	// 计算平移
	float fFrameTime = pkTimeMgr->GetCurFrameTime();
	if (m_bMoveForward)		kCamPosition += spCamera->GetWorldDirection() * fFrameTime * m_fFreeCameraSpeed;
	if (m_bMoveBack)		kCamPosition -= spCamera->GetWorldDirection() * fFrameTime * m_fFreeCameraSpeed;	
	if (m_bMoveLeft)		kCamPosition -= spCamera->GetWorldRightVector() * fFrameTime * m_fFreeCameraSpeed;
	if (m_bMoveRight)		kCamPosition += spCamera->GetWorldRightVector() * fFrameTime * m_fFreeCameraSpeed;

	// 应用平移
	spCamera->SetWorldTranslate(kCamPosition);
	// @}


	// 关于旋转
	// @{
	if (m_bRightButtonDown)
	{
		// 计算旋转
		NiMatrix3 kRotation = NiViewMath::Look(
			(m_iX - m_iLastX) * m_fFreeCameraRotate, 
			(m_iY - m_iLastY) * m_fFreeCameraRotate, 
			spCamera->GetWorldRotate(), 
			NiPoint3::UNIT_Z);

		// 应用旋转
		spCamera->SetWorldRotate(kRotation);
	}
	// @}


	return 0;
}
//-------------------------------------------------------------------------------------------------
}
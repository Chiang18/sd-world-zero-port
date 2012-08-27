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

	// ¼ÆËãÆ½ÒÆ
	float fFreeCameraSpeed = 50.0f;
	float fFrameTime = pkTimeMgr->GetCurFrameTime();
	if (m_bMoveForward)		kCamPosition += spCamera->GetWorldDirection() * fFrameTime * fFreeCameraSpeed;
	if (m_bMoveBack)		kCamPosition -= spCamera->GetWorldDirection() * fFrameTime * fFreeCameraSpeed;	
	if (m_bMoveLeft)		kCamPosition -= spCamera->GetWorldRightVector() * fFrameTime * fFreeCameraSpeed;
	if (m_bMoveRight)		kCamPosition += spCamera->GetWorldRightVector() * fFrameTime * fFreeCameraSpeed;

	spCamera->SetWorldTranslate(kCamPosition);

	return 0;
}
//-------------------------------------------------------------------------------------------------
}
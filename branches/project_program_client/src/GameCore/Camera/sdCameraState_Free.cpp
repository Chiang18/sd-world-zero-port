#include "Stdafx.h"
#include "sdCameraState.h"
#include "sdCameraFSM.h"
#include "sdInputSystem.h"
#include <sdTimeMgr.h>

namespace GameCore
{
//-------------------------------------------------------------------------------------------------
sdCameraFreeState::sdCameraFreeState()
{
	SetStateID(E_CAMERA_FREE);
}
//-------------------------------------------------------------------------------------------------
sdCameraFreeState::~sdCameraFreeState()
{
	
}
//-------------------------------------------------------------------------------------------------
int sdCameraFreeState::Update()
{
	sdCameraFSM* pkCameraFSM = sdCameraFSM::InstancePtr();
	NIASSERT(pkCameraFSM);

	sdInputSystem* pkInputSystem = sdInputSystem::InstancePtr();
	NIASSERT(pkInputSystem);

	Base::sdTimeMgr* pkTimeMgr = Base::sdTimeMgr::InstancePtr();
	NIASSERT(pkTimeMgr);

	// 窗口没有被激活则直接返回
	if (!pkInputSystem->GetActive())
		return 0;

	// 检测键盘方向键
	bool bUp	  = pkInputSystem->IsInputKeyDown(E_INPUTKEY_UP);
	bool bDown	  = pkInputSystem->IsInputKeyDown(E_INPUTKEY_DOWN);
	bool bLeft	  = pkInputSystem->IsInputKeyDown(E_INPUTKEY_TURNLEFT);
	bool bRight   = pkInputSystem->IsInputKeyDown(E_INPUTKEY_TURNRIGHT);
	bool bAscend  = pkInputSystem->IsInputKeyDown(E_INPUTKEY_ASCEND);
	bool bDescend = pkInputSystem->IsInputKeyDown(E_INPUTKEY_DESCEND);
	
	NiCamera* spCamera = pkCameraFSM->GetCamera();
	NiPoint3 kCamPosition = spCamera->GetWorldTranslate();

	// 计算平移
	float fFreeCameraSpeed = 50.0f;
	float fFrameTime = pkTimeMgr->GetCurFrameTime();
	if (bUp)		kCamPosition += spCamera->GetWorldDirection() * fFrameTime * fFreeCameraSpeed;
	if (bDown)		kCamPosition -= spCamera->GetWorldDirection() * fFrameTime * fFreeCameraSpeed;	
	if (bLeft)		kCamPosition -= spCamera->GetWorldRightVector() * fFrameTime * fFreeCameraSpeed;
	if (bRight)		kCamPosition += spCamera->GetWorldRightVector() * fFrameTime * fFreeCameraSpeed;
	if (bAscend)	kCamPosition += spCamera->GetWorldUpVector() * fFrameTime * fFreeCameraSpeed;
	if (bDescend)	kCamPosition -= spCamera->GetWorldUpVector() * fFrameTime * fFreeCameraSpeed;

	spCamera->SetWorldTranslate(kCamPosition);

	// 检测鼠标
	int iX = 0, iY = 0, iZ = 0;
	pkInputSystem->GetPositionDelta(iX, iY, iZ);

	bool bRightButtonDown = pkInputSystem->IsButtonDown(E_MOUSE_RBUTTON);

	// 计算旋转
	if (bRightButtonDown)
	{
		float fMouseLookScalar = 0.001f;
		NiMatrix3 kRotation = NiViewMath::Look(iX * fMouseLookScalar, 
			iY * fMouseLookScalar, 
			spCamera->GetWorldRotate(), 
			NiPoint3::UNIT_Z);
		spCamera->SetWorldRotate(kRotation);
	}

	return 0;
}
//-------------------------------------------------------------------------------------------------
}
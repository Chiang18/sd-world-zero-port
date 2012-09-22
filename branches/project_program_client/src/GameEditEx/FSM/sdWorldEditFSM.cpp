#include "Stdafx.h"
#include "sdWorldEditFSM.h"
#include "sdEditMode.h"
#include "sdEditEventType.h"
#include "sdTerrainDeformMode.h"
#include "sdTerrainSurfaceMode.h"

//
#include <windef.h>

//
#include <sdEventHandler.h>
#include <sdEventMgr.h>

using namespace GameCore;

namespace GameEditEx
{
//-------------------------------------------------------------------------------------------------
sdWorldEditFSM::sdWorldEditFSM()
{
	
}
//-------------------------------------------------------------------------------------------------
sdWorldEditFSM::~sdWorldEditFSM()
{

}
//-------------------------------------------------------------------------------------------------
bool sdWorldEditFSM::Initialize()
{
	// 建立编辑状态,加入到状态机
	sdTerrainDeformMode* pkTerrainDeformMode = new sdTerrainDeformMode;
	AddState(pkTerrainDeformMode);

	sdTerrainSurfaceMode* pkTerrainSurfaceMode = new sdTerrainSurfaceMode;
	AddState(pkTerrainSurfaceMode);

	// 初始化所有编辑状态
	StateMapItr itr_state = m_kStateMap.begin();
	StateMapItr itr_state_end = m_kStateMap.end();
	for (; itr_state != itr_state_end; ++itr_state)
	{
		sdEditMode* pkEditMode = (sdEditMode*)(itr_state->second);
		NIASSERT(pkEditMode);
		pkEditMode->Initialize();
	}

	// 初始化帮助对象
	m_pkEditHelper = NiNew sdEditHelper;
	NIASSERT(m_pkEditHelper);

	m_pkEditTerrainHelper = NiNew sdEditTerrainHelper(pkTerrainDeformMode, pkTerrainSurfaceMode);
	NIASSERT(m_pkEditTerrainHelper);

	// 绑定事件
	BEGIN_EVENT(sdWorldEditFSM)
		SUBSCRIBE_EVENT2((eEventID)E_EDITEVENT_TERRAIN_DEFORM,	OnEditModeTerrainDeform)
		SUBSCRIBE_EVENT2((eEventID)E_EDITEVENT_TERRAIN_SURFACE,	OnEditModeTerrainSurface)
	END_EVENT()

	return true;
}
//-------------------------------------------------------------------------------------------------
void sdWorldEditFSM::Destroy()
{
	// 清理所有编辑状态
	StateMapItr itr_state = m_kStateMap.begin();
	StateMapItr itr_state_end = m_kStateMap.end();
	for (; itr_state != itr_state_end; ++itr_state)
	{
		sdEditMode* pkEditMode = (sdEditMode*)(itr_state->second);
		NIASSERT(pkEditMode);
		pkEditMode->Destroy();
	}

	//
	m_pkEditHelper = 0;
	m_pkEditTerrainHelper = 0;
}
//-------------------------------------------------------------------------------------------------
bool sdWorldEditFSM::OnEditModeSelect(const GameCore::stEventArg& kArgs)
{
	SetState(sdEditMode::E_EDITMODE_SELECT);
	return true;
}

bool sdWorldEditFSM::OnEditModeTranslate(const GameCore::stEventArg& kArgs)
{
	SetState(sdEditMode::E_EDITMODE_TRANSLATE);
	return true;
}

bool sdWorldEditFSM::OnEditModeRotate(const GameCore::stEventArg& kArgs)
{
	SetState(sdEditMode::E_EDITMODE_ROTATE);
	return true;
}

bool sdWorldEditFSM::OnEditModeScale(const GameCore::stEventArg& kArgs)
{
	SetState(sdEditMode::E_EDITMODE_SCALE);
	return true;
}

bool sdWorldEditFSM::OnEditModeTerrainDeform(const GameCore::stEventArg& kArgs)
{
	SetState(sdEditMode::E_EDITMODE_TERRAIN_DEFORM);
	return true;
}

bool sdWorldEditFSM::OnEditModeTerrainSurface(const GameCore::stEventArg& kArgs)
{
	SetState(sdEditMode::E_EDITMODE_TERRAIN_SURFACE);
	return true;
}
//-------------------------------------------------------------------------------------------------
}
//*************************************************************************************************
// 内容:	世界编辑状态机
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-28
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_WORLDEDITFSM_H__
#define _SD_GAMEEDITEX_WORLDEDITFSM_H__
#include "sdGameEditEx.h"
#include <sdEditFSM.h>
#include <sdEventArg.h>

namespace GameEditEx
{
	// 世界编辑状态机
	class sdWorldEditFSM : public sdEditFSM
	{
	public:
		sdWorldEditFSM();
		virtual ~sdWorldEditFSM();

		// 虚函数继承
		virtual bool Initialize();
		virtual void Destroy();

	protected:
		// 事件处理,用于与Event绑定
		// @{
		virtual bool OnEditModeSelect(const GameCore::stEventArg& kArgs);
		virtual bool OnEditModeTranslate(const GameCore::stEventArg& kArgs);
		virtual bool OnEditModeRotate(const GameCore::stEventArg& kArgs);
		virtual bool OnEditModeScale(const GameCore::stEventArg& kArgs);
		virtual bool OnEditModeTerrainDeform(const GameCore::stEventArg& kArgs);
		virtual bool OnEditModeTerrainSurface(const GameCore::stEventArg& kArgs);
		// @}
	};
}
#endif
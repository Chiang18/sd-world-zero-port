//*************************************************************************************************
// 内容:	编辑状态机
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-27
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_EDITFSM_H__
#define _SD_GAMEEDITEX_EDITFSM_H__
#include "sdGameEditEx.h"
#include <sdSingleton.h>
#include <sdFSM.h>

namespace GameEditEx
{
	// 编辑状态机基类
	class sdEditFSM : public Base::sdTSingleton<sdEditFSM>, public sdFSM
	{
	public:
		sdEditFSM(){};
		virtual ~sdEditFSM(){}; 

		// 初始化与销毁
		virtual bool Initialize(){ return true;};
		virtual void Destroy() {};

		// 输入消息
		// @{
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam);
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam);
		virtual void OnMouseWheel(WPARAM wParam, LPARAM lParam);
		virtual void OnMouseMove(WPARAM wParam, LPARAM lParam);
		virtual void OnLeftButtonDown(WPARAM wParam, LPARAM lParam);
		virtual void OnLeftButtonUp(WPARAM wParam, LPARAM lParam);
		virtual void OnLeftButtonDBClick(WPARAM wParam, LPARAM lParam);
		virtual void OnRightButtonDown(WPARAM wParam, LPARAM lParam);
		virtual void OnRightButtonUp(WPARAM wParam, LPARAM lParam);
		virtual void OnRightButtonDBClick(WPARAM wParam, LPARAM lParam);
		virtual void OnMiddleButtonDown(WPARAM wParam, LPARAM lParam);
		virtual void OnMiddleButtonUp(WPARAM wParam, LPARAM lParam);
		virtual void OnMiddleButtonDBClick(WPARAM wParam, LPARAM lParam);
		// @}
	};
}
#endif
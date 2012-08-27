//*************************************************************************************************
// 内容:	编辑模式
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-27
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_EDITMODE_H__
#define _SD_GAMEEDITEX_EDITMODE_H__
#include "sdGameEditEx.h"

namespace GameEditEx
{
	// 编辑模式
	class sdEditMode : public sdState
	{
	public:

		// 输入消息
		// @{
		virtual void OnKeyDown(WPARAM wParam, LPARAM lParam){};
		virtual void OnKeyUp(WPARAM wParam, LPARAM lParam){};
		virtual void OnMouseWheel(WPARAM wParam, LPARAM lParam){};
		virtual void OnMouseMove(WPARAM wParam, LPARAM lParam){};
		virtual void OnLeftButtonDown(WPARAM wParam, LPARAM lParam){};
		virtual void OnLeftButtonUp(WPARAM wParam, LPARAM lParam){};
		virtual void OnLeftButtonDBClick(WPARAM wParam, LPARAM lParam){};
		virtual void OnRightButtonDown(WPARAM wParam, LPARAM lParam){};
		virtual void OnRightButtonUp(WPARAM wParam, LPARAM lParam){};
		virtual void OnRightButtonDBClick(WPARAM wParam, LPARAM lParam){};
		virtual void OnMiddleButtonDown(WPARAM wParam, LPARAM lParam){};
		virtual void OnMiddleButtonUp(WPARAM wParam, LPARAM lParam){};
		virtual void OnMiddleButtonDBClick(WPARAM wParam, LPARAM lParam){};
		// @}

	protected:


	};

}
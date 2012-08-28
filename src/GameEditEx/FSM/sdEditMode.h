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
#include <sdFSM.h>

namespace GameEditEx
{
	// 编辑模式基类
	class sdEditMode : public Base::sdState
	{
	public:
		// 编辑模式
		enum eEditMode
		{
			// 物件
			E_EDITMODE_SELECT = 0,
			E_EDITMODE_TRANSLATE,
			E_EDITMODE_ROTATE,
			E_EDITMODE_SCALE,
			E_EDITMODE_CREATE,
			E_EDITMODE_REMOVE,

			// 地形
			E_EDITMODE_TERRAIN_DEFORM,
			E_EDITMODE_TERRAIN_SURFACE,

			NUM_EDITMODES = E_EDITMODE_TERRAIN_SURFACE,
		};

	public:
		sdEditMode(){};
		virtual ~sdEditMode(){};

		// 初始化与销毁
		virtual bool Initialize() { return true;};
		virtual void Destroy() {};

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
	};
}
#endif
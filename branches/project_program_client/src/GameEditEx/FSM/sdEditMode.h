//*************************************************************************************************
// ����:	�༭ģʽ
//---------------------------------------------------------
// ����:		
// ����:		2012-08-27
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_EDITMODE_H__
#define _SD_GAMEEDITEX_EDITMODE_H__
#include "sdGameEditEx.h"

namespace GameEditEx
{
	// �༭ģʽ
	class sdEditMode : public sdState
	{
	public:

		// ������Ϣ
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
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
#include <sdFSM.h>

namespace GameEditEx
{
	// �༭ģʽ����
	class sdEditMode : public Base::sdState
	{
	public:
		// �༭ģʽ
		enum eEditMode
		{
			// ���
			E_EDITMODE_SELECT = 0,
			E_EDITMODE_TRANSLATE,
			E_EDITMODE_ROTATE,
			E_EDITMODE_SCALE,
			E_EDITMODE_CREATE,
			E_EDITMODE_REMOVE,

			// ����
			E_EDITMODE_TERRAIN_DEFORM,
			E_EDITMODE_TERRAIN_SURFACE,

			NUM_EDITMODES = E_EDITMODE_TERRAIN_SURFACE,
		};

	public:
		sdEditMode(){};
		virtual ~sdEditMode(){};

		// ��ʼ��������
		virtual bool Initialize() { return true;};
		virtual void Destroy() {};

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
	};
}
#endif
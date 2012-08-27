#include "Stdafx.h"
#include "sdWorldEditor.h"
#include <sdCameraEditState.h>

using namespace GameEditEx;
//-------------------------------------------------------------------------------------------------
void sdWorldEditor::WndProc(int iMsg, WPARAM wParam, LPARAM lParam)
{
	// 
	sdEditCameraState* pkCurEditCamState = (sdEditCameraState*)m_kCameraFSM.GetState();
	NIASSERT(pkCurEditCamState);

	// 分发消息
	if (iMsg >= WM_KEYFIRST && iMsg <= WM_KEYLAST)
	{
		// 键盘输入
		switch (iMsg)
		{
			case WM_KEYDOWN:	
				m_kEditFSM.OnKeyDown(wParam, lParam);	
				pkCurEditCamState->OnKeyDown(wParam, lParam);
				break;

			case WM_KEYUP:		
				m_kEditFSM.OnKeyUp(wParam, lParam);	
				pkCurEditCamState->OnKeyUp(wParam, lParam);
				break;	
		}
	}
	else if (iMsg >= WM_MOUSEFIRST && iMsg <= WM_MOUSELAST)
	{
		// 鼠标输入
		switch (iMsg)
		{
			case WM_MOUSEWHEEL:		
				m_kEditFSM.OnMouseWheel(wParam, lParam);	
				pkCurEditCamState->OnMouseWheel(wParam, lParam);
				break;

			case WM_MOUSEMOVE:		
				m_kEditFSM.OnMouseMove(wParam, lParam);	
				pkCurEditCamState->OnMouseMove(wParam, lParam);
				break;

			case WM_LBUTTONDOWN:	
				m_kEditFSM.OnLeftButtonDown(wParam, lParam);		
				break;

			case WM_LBUTTONUP:		
				m_kEditFSM.OnLeftButtonUp(wParam, lParam);			
				break;	

			case WM_LBUTTONDBLCLK:	
				m_kEditFSM.OnLeftButtonDBClick(wParam, lParam);	
				break;	

			case WM_RBUTTONDOWN:	
				m_kEditFSM.OnRightButtonDown(wParam, lParam);	
				pkCurEditCamState->OnRightButtonDown(wParam, lParam);
				break;

			case WM_RBUTTONUP:		
				m_kEditFSM.OnRightButtonUp(wParam, lParam);	
				pkCurEditCamState->OnRightButtonUp(wParam, lParam);
				break;	

			case WM_RBUTTONDBLCLK:	
				m_kEditFSM.OnRightButtonDBClick(wParam, lParam);	
				break;	

			case WM_MBUTTONDOWN:	
				m_kEditFSM.OnMiddleButtonDown(wParam, lParam);		
				break;

			case WM_MBUTTONUP:		
				m_kEditFSM.OnMiddleButtonUp(wParam, lParam);		
				break;	

			case WM_MBUTTONDBLCLK:	
				m_kEditFSM.OnMiddleButtonDBClick(wParam, lParam);	
				break;	
		}
	}
}
//-------------------------------------------------------------------------------------------------
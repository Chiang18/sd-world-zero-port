#include "Stdafx.h"
#include "sdEditFSM.h"

//
#include <windef.h>

namespace GameEditEx
{
//-------------------------------------------------------------------------------------------------
sdEditFSM::sdEditFSM()
{

}
//-------------------------------------------------------------------------------------------------
sdEditFSM::~sdEditFSM()
{
	
}
//-------------------------------------------------------------------------------------------------
void sdEditFSM::OnKeyDown(WPARAM wParam, LPARAM lParam)
{

}
//-------------------------------------------------------------------------------------------------
void sdEditFSM::OnKeyUp(WPARAM wParam, LPARAM lParam)
{

}
//-------------------------------------------------------------------------------------------------
void sdEditFSM::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	UINT uiLines;
	SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &uiLines, 0);
	int iZ = (int)(short)HIWORD(wParam) / WHEEL_DELTA * uiLines;

}
//-------------------------------------------------------------------------------------------------
void sdEditFSM::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	POINT ptPos;
	ptPos.x = (int)(short)LOWORD(lParam);
	ptPos.y = (int)(short)HIWORD(lParam);
}
//-------------------------------------------------------------------------------------------------
void sdEditFSM::OnLeftButtonDown(WPARAM wParam, LPARAM lParam)
{
	
}
//-------------------------------------------------------------------------------------------------
void sdEditFSM::OnLeftButtonUp(WPARAM wParam, LPARAM lParam)
{

}
//-------------------------------------------------------------------------------------------------
void sdEditFSM::OnLeftButtonDBClick(WPARAM wParam, LPARAM lParam)
{

}
//-------------------------------------------------------------------------------------------------
void sdEditFSM::OnRightButtonDown(WPARAM wParam, LPARAM lParam)
{

}
//-------------------------------------------------------------------------------------------------
void sdEditFSM::OnRightButtonUp(WPARAM wParam, LPARAM lParam)
{

}
//-------------------------------------------------------------------------------------------------
void sdEditFSM::OnRightButtonDBClick(WPARAM wParam, LPARAM lParam)
{

}
//-------------------------------------------------------------------------------------------------
void sdEditFSM::OnMiddleButtonDown(WPARAM wParam, LPARAM lParam)
{

}
//-------------------------------------------------------------------------------------------------
void sdEditFSM::OnMiddleButtonUp(WPARAM wParam, LPARAM lParam)
{

}
//-------------------------------------------------------------------------------------------------
void sdEditFSM::OnMiddleButtonDBClick(WPARAM wParam, LPARAM lParam)
{

}
//-------------------------------------------------------------------------------------------------
}
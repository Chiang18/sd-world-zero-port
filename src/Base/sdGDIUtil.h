//*************************************************************************************************
// 内容:	Win32 GDI的一些功能
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-09
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _GDI_UTIL_H__
#define _GDI_UTIL_H__

namespace Base
{

	class sdGDIUtil
	{
	public:
		// 获取屏幕尺寸
		// GetSystemMetrics()
		// GetDeviceCaps()
		// SystemParametersInfo()
		// EnumDisplaySettings()
		// GetWindowRect()
		static GetScreenInfo(unsigned int uiWidth, unsigned int uiHeight, bool bFullScreen)
		{
			HDC hDC = ::GetDC(NULL);
			uiWidth = ::GetDeviceCaps(hDC, HORZRES);
			uiHeight = ::GetDeviceCaps(hDC, VERTRES);
			::ReleaseDC(NULL, hDC);
		}
	};
}
#endif
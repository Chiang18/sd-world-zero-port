//*************************************************************************************************
// ����:	Win32 GDI��һЩ����
//---------------------------------------------------------
// ����:		
// ����:		2012-07-09
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _GDI_UTIL_H__
#define _GDI_UTIL_H__

namespace Base
{

	class sdGDIUtil
	{
	public:
		// ��ȡ��Ļ�ߴ�
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
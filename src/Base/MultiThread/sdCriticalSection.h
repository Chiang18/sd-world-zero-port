//*************************************************************************************************
// 内容:		临界区
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-16
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_CRITICAL_SECTION_H__
#define _SD_BASE_CRITICAL_SECTION_H__
#include "sdBase.h"

namespace Base
{
	// Win32临界区(参考自GB)
	class sdCriticalSection
	{
	public:
		sdCriticalSection()
		{
			::InitializeCriticalSection(&m_kCriticalSection);
		}

		~sdCriticalSection()
		{
			::DeleteCriticalSection(&m_kCriticalSection);
		}

		void Lock()
		{
			::EnterCriticalSection(&m_kCriticalSection);
		}

		void Unlock()
		{
			::LeaveCriticalSection(&m_kCriticalSection);
		}

	protected:
		CRITICAL_SECTION	m_kCriticalSection;
	};
}
#endif
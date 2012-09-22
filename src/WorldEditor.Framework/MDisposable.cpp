#include "Stdafx.h"
#include "MDisposable.h"


namespace WorldEditorFramework 
{
	//-----------------------------------------------------------------------------
	MDisposable::MDisposable(): m_bDisposed(false)
	{

	}
	//-----------------------------------------------------------------------------
	MDisposable::~MDisposable()
	{
		// 释放托管资源和非托管资源
		__try
		{
			System::Threading::Monitor::Enter(this);

			if (!m_bDisposed)
			{
				// 释放非托管资源
				Do_Dispose(true);

				// 释放托管资源
				System::GC::SuppressFinalize(this);

				m_bDisposed = true;
			}
		}
		__finally
		{
			System::Threading::Monitor::Exit(this);
		}
	}
	//-----------------------------------------------------------------------------
	MDisposable::!MDisposable()
	{
		// 释放非托管资源
		__try
		{
			System::Threading::Monitor::Enter(this);

			if (!m_bDisposed)
			{
				// 释放非托管资源
				Do_Dispose(false);

				m_bDisposed = true;
			}
		}
		__finally
		{
			System::Threading::Monitor::Exit(this);
		}
	}
	//-----------------------------------------------------------------------------
}
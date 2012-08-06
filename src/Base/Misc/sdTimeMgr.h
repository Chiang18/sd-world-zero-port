//*************************************************************************************************
// 内容:	系统时间
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-16
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_TIME_MGR_H__
#define _SD_BASE_TIME_MGR_H__
#include "sdBase.h"
#include "sdSingleton.h"

namespace Base
{
	// 简单的时间管理
	// ::GetTickCount()					获取操作系统启动到现在所经过(elapsed)的毫秒数
	// ::QueryPerformanceCounter();		获取高精度时间
	// ::QueryPerformanceFrequency();	
	class sdTimeMgr : public sdTSingleton<sdTimeMgr>
	{
	public:
		sdTimeMgr();
		~sdTimeMgr();

		// 更新
		void Update();

		//
		float	GetCurTime();
		float	GetCurFrameTime();

		void	SetSCDelayTime(float fDelayTime);
		float	GetSCDelayTime();

	protected:
		// 高精度时钟Tick
		LARGE_INTEGER	m_liStartTime;	// 程序启动时钟计数(Ticks)
		LARGE_INTEGER	m_liFrequency;	// 高精度时钟频率(Ticks/s)
		DWORD			m_uiStartTick;	// 系统时间(ms)

		// 保存更新时间用于调整高精度时钟时使用
		LONGLONG	m_llLastTime;

		// 计时信息
		float	m_fFrameTime;		// 每次Tick之间时间间隔
		float	m_fAccumTime;		// 程序运行累计计时
		float	m_fSCDelayTime;	

		// 进程信息
		HANDLE	m_hProc;
		HANDLE	m_hThread;	
		DWORD	m_uiProcMask;
		DWORD	m_uiSysMask;
	};
	//----------------------------------------------------------------------------
	inline float sdTimeMgr::GetCurTime(){	return m_fAccumTime;};
	inline float sdTimeMgr::GetCurFrameTime() { return m_fFrameTime;};
	inline float sdTimeMgr::GetSCDelayTime() {return m_fSCDelayTime;};
	inline void sdTimeMgr::SetSCDelayTime(float fDelayTime) { m_fSCDelayTime = fDelayTime;};
	//----------------------------------------------------------------------------
}
#endif
//*************************************************************************************************
// ����:	ϵͳʱ��
//---------------------------------------------------------
// ����:		
// ����:		2012-07-16
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_TIME_MGR_H__
#define _SD_BASE_TIME_MGR_H__
#include "sdBase.h"
#include "sdSingleton.h"

namespace Base
{
	// �򵥵�ʱ�����
	// ::GetTickCount()					��ȡ����ϵͳ����������������(elapsed)�ĺ�����
	// ::QueryPerformanceCounter();		��ȡ�߾���ʱ��
	// ::QueryPerformanceFrequency();	
	class sdTimeMgr : public sdTSingleton<sdTimeMgr>
	{
	public:
		sdTimeMgr();
		~sdTimeMgr();

		// ����
		void Update();

		//
		float	GetCurTime();
		float	GetCurFrameTime();

		void	SetSCDelayTime(float fDelayTime);
		float	GetSCDelayTime();

	protected:
		// �߾���ʱ��Tick
		LARGE_INTEGER	m_liStartTime;	// ��������ʱ�Ӽ���(Ticks)
		LARGE_INTEGER	m_liFrequency;	// �߾���ʱ��Ƶ��(Ticks/s)
		DWORD			m_uiStartTick;	// ϵͳʱ��(ms)

		// �������ʱ�����ڵ����߾���ʱ��ʱʹ��
		LONGLONG	m_llLastTime;

		// ��ʱ��Ϣ
		float	m_fFrameTime;		// ÿ��Tick֮��ʱ����
		float	m_fAccumTime;		// ���������ۼƼ�ʱ
		float	m_fSCDelayTime;	

		// ������Ϣ
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
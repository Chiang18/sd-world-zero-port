//*************************************************************************************************
// 内容:	事件中心
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-27
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMECORE_EVENTMGR_H__
#define _SD_GAMECORE_EVENTMGR_H__
#include "sdGameCore.h"
#include "sdEvent.h"
#include <sdSingleton.h>

namespace GameCore
{
	class sdEventMgr : public Base::sdTSingleton<sdEventMgr> 
	{
	public:
		sdEventMgr();
		virtual ~sdEventMgr();

		// 事件订阅与取消订阅
		void	SubscribeEvent(eEventID eID, sdEventHandle* pkSubscriber);
		void	UnsubscribeEvent(eEventID eID, sdEventHandle* pkSubscriber);
		void	FireEvent(eEventID eID, const stEventArg& kArg);

	protected:
		// 事件列表
		typedef std::vector<sdEvent> EventVec;
		typedef std::vector<sdEvent>::iterator EventVecItr;
		EventVec m_kEventVec;
	};
}
#endif
//*************************************************************************************************
// ����:	�¼�
//---------------------------------------------------------
// ����:		
// ����:		2012-08-27
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMECORE_EVENT_H__
#define _SD_GAMECORE_EVENT_H__
#include "sdGameCore.h"
#include "sdEventType.h"
#include "sdEventArg.h"

namespace GameCore
{
	class sdEventHandle;

	class sdEvent
	{
	public:
		sdEvent(eEventID eID);
		virtual ~sdEvent();

		//
		eEventID GetEventID() const { return m_eEventID;}

		// �¼�������ȡ������
		void	Subscribe(sdEventHandle* pkSubscriber);
		void	Unsubscribe(sdEventHandle* pkSubscriber, bool bAutoDelete = true);
		void	UnsubscribeAll();

		// ִ�лص�
		void	operator()(const stEventArg& pkEventArg);

	protected:
		// �¼�
		eEventID	m_eEventID;

		// �������б�
		typedef std::set<sdEventHandle*> SubscriberSet;
		typedef std::set<sdEventHandle*>::iterator SubscriberSetItr;
		SubscriberSet m_kSubscriberSet;
	};
}
#endif
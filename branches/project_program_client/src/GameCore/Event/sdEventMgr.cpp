#include "Stdafx.h"
#include "sdEventMgr.h"

namespace GameCore
{
//-------------------------------------------------------------------------------------------------
sdEventMgr::sdEventMgr()
{
	m_kEventVec.reserve(NUM_EVENTS);
	for (uint i = 0; i < NUM_EVENTS; ++i)
		m_kEventVec.push_back(sdEvent((eEventID)i));
}
//-------------------------------------------------------------------------------------------------
sdEventMgr::~sdEventMgr()
{
	EventVecItr itr = m_kEventVec.begin();
	EventVecItr itr_end = m_kEventVec.end();
	for (; itr!=itr_end; ++itr)
	{
		itr->UnsubscribeAll();
	}
	m_kEventVec.clear();
}
//-------------------------------------------------------------------------------------------------
void sdEventMgr::SubscribeEvent(eEventID eID, sdEventHandle* pkSubscriber)
{
	NIASSERT(eID >= 0 && eID < NUM_EVENTS)
	m_kEventVec[eID].Subscribe(pkSubscriber);
}
//-------------------------------------------------------------------------------------------------
void sdEventMgr::UnsubscribeEvent(eEventID eID, sdEventHandle* pkSubscriber)
{
	NIASSERT(eID >= 0 && eID < NUM_EVENTS)
	m_kEventVec[eID].Unsubscribe(pkSubscriber);
}
//-------------------------------------------------------------------------------------------------
void sdEventMgr::FireEvent(eEventID eID, const stEventArg& kArg)
{
	(*(&m_kEventVec[eID]))(kArg);
}
//-------------------------------------------------------------------------------------------------
}
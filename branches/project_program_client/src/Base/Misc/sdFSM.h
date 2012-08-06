//*************************************************************************************************
// 内容:	有限状态机基类
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-26
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_FSM_H__
#define _SD_BASE_FSM_H__
#include "sdBase.h"

namespace Base
{

	//*************************************************************************
	// 状态基类
	class sdState
	{
	public:
		sdState() : m_iStateID(-1) {};
		virtual ~sdState() {};

		// 状态切换回调
		virtual void Enter() {};
		virtual	void Leave() {};
		virtual int	 Update() { return 0;};

		// 
		void SetStateID(int iID) { m_iStateID = iID;};
		int GetStateID() { return m_iStateID;};

	protected:
		int	m_iStateID;
	};

	//*************************************************************************
	// 有限状态机
	class sdFSM
	{
	public:
		sdFSM();
		virtual ~sdFSM();

		void		AddState(sdState* pkState);
		sdState*	GetState();
		sdState*	FindState(int iStateID);

		// 切换状态(立即切换与延迟切换)
		void		SetState(int iStateID);
		void		DelaySetState(int iStateID);

		virtual int UpdateState();

	protected:
		// 状态切换
		virtual void OnBeforeLeave(int iStateID) {};
		virtual void OnAfterEnter(int iStateID) {};

	protected:
		sdState*	m_pkState;
		sdState*	m_pkNextState;
		sdState*	m_pkDelayNextState;
		
		typedef std::map<int, sdState*>	StateMap;
		typedef std::map<int, sdState*>::iterator StateMapItr;
		StateMap m_kStateMap;
	};
	//-------------------------------------------------------------------------
	inline sdState* sdFSM::GetState()
	{
		return m_pkState;
	}
	//*************************************************************************
}
#endif
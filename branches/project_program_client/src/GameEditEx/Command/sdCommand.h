//*************************************************************************************************
// ����:	���������(����֧Ԯ���˺�����)
//---------------------------------------------------------
// ����:		
// ����:		2012-08-27
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_COMMAND_H__
#define _SD_GAMEEDITEX_COMMAND_H__
#include "sdGameEditEx.h"
#include <sdSingleton.h>

namespace GameEditEx
{
	//*************************************************************************
	// ����ӿ�
	class sdCommand
	{
	public:
		virtual ~sdCommand(){};

		virtual bool Redo() = 0;	// ����
		virtual bool Undo() = 0;	// ����
	};

	//*************************************************************************
	// ���������
	class sdCommandMgr : public Base::sdTSingleton<sdCommandMgr>
	{
	public:
		sdCommandMgr();
		~sdCommandMgr();

	protected:
		typedef std::list<sdCommand*> CommandList;
		typedef std::list<sdCommand*>::iterator CommandListItr;
		CommandList m_kRedoList;
		CommandList m_kUndoList;
	};
}
#endif
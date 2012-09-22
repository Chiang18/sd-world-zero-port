//*************************************************************************************************
// ����:	����༭״̬��
//---------------------------------------------------------
// ����:		
// ����:		2012-08-28
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_WORLDEDITFSM_H__
#define _SD_GAMEEDITEX_WORLDEDITFSM_H__
#include "sdGameEditEx.h"
#include <sdEditFSM.h>
#include <sdEventArg.h>

#include "sdEditHelper.h"
#include "sdEditTerrainHelper.h"

namespace GameEditEx
{
	// ����༭״̬��
	class sdWorldEditFSM : public sdEditFSM
	{
	public:
		sdWorldEditFSM();
		virtual ~sdWorldEditFSM();

		// �麯���̳�
		virtual bool Initialize();
		virtual void Destroy();

		// ���Է���
		sdEditHelper* GetEditHelper() const { return m_pkEditHelper;}
		sdEditTerrainHelper* GetEditTerrainHelper() const { return m_pkEditTerrainHelper;}

	protected:
		// �¼�����,������Event��
		// @{
		virtual bool OnEditModeSelect(const GameCore::stEventArg& kArgs);
		virtual bool OnEditModeTranslate(const GameCore::stEventArg& kArgs);
		virtual bool OnEditModeRotate(const GameCore::stEventArg& kArgs);
		virtual bool OnEditModeScale(const GameCore::stEventArg& kArgs);
		virtual bool OnEditModeTerrainDeform(const GameCore::stEventArg& kArgs);
		virtual bool OnEditModeTerrainSurface(const GameCore::stEventArg& kArgs);
		// @}

	protected:
		sdEditHelperPtr			m_pkEditHelper;
		sdEditTerrainHelperPtr	m_pkEditTerrainHelper;
	};
}
#endif
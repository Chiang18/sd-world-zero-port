//*************************************************************************************************
// ����:	��Ϸ���ù���
//---------------------------------------------------------
// ����:		
// ����:		2012-07-09
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _GAME_CONFIG_H__
#define _GAME_CONFIG_H__
#include <sdSingleton.h>
#include "sdVideoConfig.h"

// ��ȡ��Ϸ����ini,��ȡ��ǰ��������Ϣ
class sdGameConfig : public sdTSingleton<sdGameConfig>
{
public:



public:
	sdVideoConfig	m_kVideoConfig;

	
};

#endif
//*************************************************************************************************
// 内容:	游戏配置管理
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-09
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _GAME_CONFIG_H__
#define _GAME_CONFIG_H__
#include <sdSingleton.h>
#include "sdVideoConfig.h"

// 读取游戏配置ini,获取当前的配置信息
class sdGameConfig : public sdTSingleton<sdGameConfig>
{
public:



public:
	sdVideoConfig	m_kVideoConfig;

	
};

#endif
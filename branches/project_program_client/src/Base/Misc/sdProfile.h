//*************************************************************************************************
// ����:	���ܷ�������
//---------------------------------------------------------
// ����:		
// ����:		2012-07-16
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_BASE_PROFILE_H__
#define _SD_BASE_PROFILE_H__
#include "sdBase.h"
#include "sdSingleton.h"

namespace Base
{


	// ����Ϊ�궨��,���ڴ���
	#if (SD_PROFILING == 1)
		//
		//
		//
	#else
		//#define OgreProfile( a )
		//#define OgreProfileBegin( a )
		//#define OgreProfileEnd( a )
	#endif
}
#endif
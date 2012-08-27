#pragma once
#ifndef _SD_EDNGINEEX_STDAFX_H__
#define _SD_EDNGINEEX_STDAFX_H__

//
#include "sdGameEditEx.h"

//
#ifdef GAMEEDITEX_EXPORT
	#define GAMEEDITEX_ENTRY __declspec(dllexport)
#else
	#ifdef GAMEEDITEX_IMPORT
		#define GAMEEDITEX_ENTRY __declspec(dllimport)
	#else
		#define GAMEEDITEX_ENTRY
	#endif
#endif

#endif
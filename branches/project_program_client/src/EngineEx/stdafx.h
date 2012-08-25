#pragma once
#ifndef _SD_EDNGINEEX_STDAFX_H__
#define _SD_EDNGINEEX_STDAFX_H__

//
#include "sdEngineEx.h"

//
#ifdef ENGINEEX_EXPORT
	#define ENGINEEX_ENTRY __declspec(dllexport)
#else
	#ifdef ENGINEEX_IMPORT
		#define ENGINEEX_ENTRY __declspec(dllimport)
	#else
		#define ENGINEEX_ENTRY
	#endif
#endif

#endif
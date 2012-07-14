#pragma once
#ifndef _STDAFX_H__
#define _STDAFX_H__

// 
//#include <sdBase.h>

//
#include "sdEngine.h"

//
#ifdef ENGINE_EXPORT
	#define ENGINE_ENTRY __declspec(dllexport)
#else
	#ifdef ENGINE_IMPORT
		#define ENGINE_ENTRY __declspec(dllimport)
	#else
		#define ENGINE_ENTRY
	#endif
#endif
#endif
//*************************************************************************************************
// ����:	����,����һ����ˢ����
//---------------------------------------------------------
// ����:		
// ����:		2012-09-03
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_EDITCANVAS_H__
#define _SD_GAMEEDITEX_EDITCANVAS_H__
#include "sdGameEditEx.h"

namespace GameEditEx
{
	//*************************************************************************
	// ����һ����ˢ����
	class sdEditCanvas : public NiRefObject
	{
	public:
		sdEditCanvas(){};
		virtual ~sdEditCanvas(){};
	};
	NiSmartPointer(sdEditCanvas);
}
#endif
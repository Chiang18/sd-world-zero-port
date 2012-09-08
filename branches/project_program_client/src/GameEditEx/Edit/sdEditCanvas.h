//*************************************************************************************************
// 内容:	画布,描述一个画刷对象
//---------------------------------------------------------
// 作者:		
// 创建:		2012-09-03
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_EDITCANVAS_H__
#define _SD_GAMEEDITEX_EDITCANVAS_H__
#include "sdGameEditEx.h"

namespace GameEditEx
{
	//*************************************************************************
	// 描述一个画刷对象
	class sdEditCanvas : public NiRefObject
	{
	public:
		sdEditCanvas(){};
		virtual ~sdEditCanvas(){};
	};
	NiSmartPointer(sdEditCanvas);
}
#endif
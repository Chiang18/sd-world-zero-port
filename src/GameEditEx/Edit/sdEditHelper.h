//*************************************************************************************************
// 内容:	编辑辅助类
//---------------------------------------------------------
// 作者:		
// 创建:		
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_EDITHELPER_H__
#define _SD_GAMEEDITEX_EDITHELPER_H__
#include "sdGameEditEx.h"

class sdMap;

namespace GameEditEx
{
	// 编辑辅助类
	//	1.实现一些与状态无关的辅助编辑功能
	//	2.对底层功能进行抽象
	class sdEditHelper : public NiRefObject
	{
	public:
		sdEditHelper();

		// 底层抽象访问功能
		sdMap*	GetMap() const { return m_pkMap;};


	protected:
		sdMap*	m_pkMap;	//当前编辑的地图

	};
	NiSmartPointer(sdEditHelper);
}
#endif
//*************************************************************************************************
// 内容:	地形笔刷图形(用于显示)
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-28
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_EDITBRUSHSHAPE_H__
#define _SD_GAMEEDITEX_EDITBRUSHSHAPE_H__
#include "sdGameEditEx.h"

namespace GameEditEx
{
	//****************************************************************************
	// 双圆环笔刷
	class sdBrushDoubalCircle : public NiNode
	{
	public:
		sdBrushDoubalCircle();
		~sdBrushDoubalCircle();

		// 根据当前位置,重新拾取地形高度,更新显示
		void Update();

	protected:
		void CreateInnerCircle();
		void CreateOutterCirlce();
		void UpdateInnerCircle();
		void UpdateOutterCirlce();	

	protected:
		float	m_fInnerRadius;
		float	m_fOuterRadius;
		NiMeshPtr	m_spInnerCircle;
		NiMeshPtr	m_spOuterCircle;
	};
}
#endif
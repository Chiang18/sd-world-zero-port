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
	class sdDualCircleMesh : public NiNode
	{
	public:
		sdDualCircleMesh();
		~sdDualCircleMesh();

		// 根据当前位置,重新拾取地形高度,更新显示
		void UpdateShape();

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

		// 圆刷子圆周采样点数
		const static uint ms_uiCirclePointCount = 50;
	};
	NiSmartPointer(sdDualCircleMesh);
}
#endif
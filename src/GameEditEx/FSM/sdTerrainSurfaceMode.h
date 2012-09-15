//*************************************************************************************************
// 内容:	地形纹理编辑模式
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-28
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_TERRAINSURFACEMODE_H__
#define _SD_GAMEEDITEX_TERRAINSURFACEMODE_H__
#include "sdGameEditEx.h"
#include "sdEditMode.h"
#include "sdEditBrush.h"
#include "sdEditBrushShape.h"

namespace GameEditEx
{
	class sdTerrainSurfaceMode : public sdEditMode
	{
	public:
		sdTerrainSurfaceMode();
		~sdTerrainSurfaceMode();

		// 虚函数继承
		virtual bool Initialize();
		virtual void Destroy();
		virtual int Update();

		virtual void OnMouseMove(WPARAM wParam, LPARAM lParam);
		virtual void OnLeftButtonDown(WPARAM wParam, LPARAM lParam);
		virtual void OnLeftButtonUp(WPARAM wParam, LPARAM lParam);

	protected:
		// 鼠标状态
		bool	m_bLeftButtonDown;
		POINT	m_kMousePos;

		// 笔刷
		sdEditBrushPtr m_pkEditBrush;
	};
}
#endif
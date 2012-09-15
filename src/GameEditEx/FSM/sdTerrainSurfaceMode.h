//*************************************************************************************************
// ����:	��������༭ģʽ
//---------------------------------------------------------
// ����:		
// ����:		2012-08-28
// ����޸�:
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

		// �麯���̳�
		virtual bool Initialize();
		virtual void Destroy();
		virtual int Update();

		virtual void OnMouseMove(WPARAM wParam, LPARAM lParam);
		virtual void OnLeftButtonDown(WPARAM wParam, LPARAM lParam);
		virtual void OnLeftButtonUp(WPARAM wParam, LPARAM lParam);

	protected:
		// ���״̬
		bool	m_bLeftButtonDown;
		POINT	m_kMousePos;

		// ��ˢ
		sdEditBrushPtr m_pkEditBrush;
	};
}
#endif
//*************************************************************************************************
// ����:	���θ߶ȱ༭ģʽ
//---------------------------------------------------------
// ����:		
// ����:		2012-08-27
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_TERRAINDEFORMMODE_H__
#define _SD_GAMEEDITEX_TERRAINDEFORMMODE_H__
#include "sdGameEditEx.h"
#include "sdEditMode.h"
#include "sdEditBrush.h"
#include "sdEditBrushShape.h"

namespace GameEditEx
{
	class sdTerrainDeformMode : public sdEditMode
	{
	public:
		sdTerrainDeformMode();
		~sdTerrainDeformMode();

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
		sdEditBrushPtr m_pkkEditBrushArray[sdEditBrush::NUM_BRUSHES];
		sdEditBrushPtr m_pkActiveBrush;
	};
}
#endif
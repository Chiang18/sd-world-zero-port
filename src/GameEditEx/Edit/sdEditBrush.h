//*************************************************************************************************
// ����:	���α�ˢ, wzԭ���Ϊ:
//				Brush(��ˢ)			��װ��
//				Shape(��״)			�����ṩһ��������������ģ��Ȩ��
//				Canvas(����)		���ڸ�����Ч��ģλ�õ�Ȩ�ز�������
//				BrushShape(��״)	ˢ����ʾ����
//---------------------------------------------------------
// ����:		
// ����:		2012-08-28
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_EDITBRUSH_H__
#define _SD_GAMEEDITEX_EDITBRUSH_H__
#include "sdGameEditEx.h"
#include "sdEditShape.h"
#include "sdEditCanvas.h"
#include "sdEditBrushShape.h"

//
namespace Base
{
	namespace Math
	{
		class sdRay;
	}
}

namespace GameEditEx
{
	//****************************************************************************
	// �༭��ˢ����
	class sdEditBrush : public NiRefObject
	{
	public:
		// �༭��ˢ
		enum eBrushType
		{
			E_BRUSH_PULL = 0,	// �ر�����
			E_BRUSH_SMOOTH,		// �ر�ƽ��
			E_BRUSH_NOISE,		// �ر�����
			E_BRUSH_AVERAGE,
			E_BRUSH_FLAT,		// �ر�̤ƽ
			E_BRUSH_DETAIL,		// �ر�������
			NUM_BRUSHES,		
		};	

	public:
		sdEditBrush(){};
		virtual ~sdEditBrush() {};

		void SetEditCanvas(sdEditCanvas* pkEditCanvas) { m_pkEditCanvas = pkEditCanvas;}
		sdEditCanvas* GetEditCanvas() const {return m_pkEditCanvas;}

		void SetEditShape(sdEditShape* pkEditShape) { m_pkEditShape = pkEditShape;}
		sdEditShape* GetEditShape() const {return m_pkEditShape;}

		void SetBrushShape(sdBrushShape* pkBrushShape) { m_pkBrushShape = pkBrushShape;}
		sdBrushShape* GetBrushShape() const { return m_pkBrushShape;}

		virtual void Apply(const Base::Math::sdRay& kRay) = 0;

	protected:
		sdBrushShapePtr	m_pkBrushShape;
		sdEditShapePtr	m_pkEditShape;
		sdEditCanvasPtr m_pkEditCanvas;
	};
	NiSmartPointer(sdEditBrush);

	//****************************************************************************
	// �ر�߶������ˢ
	class sdTerrainDeformPoolBrush : public sdEditBrush
	{
	public:
		sdTerrainDeformPoolBrush();
		virtual ~sdTerrainDeformPoolBrush();

		// �麯���̳�
		virtual void Apply(const Base::Math::sdRay& kRay);
	};
	NiSmartPointer(sdTerrainDeformPoolBrush);

	//****************************************************************************
	// �ر�����༭��ˢ
}
#endif
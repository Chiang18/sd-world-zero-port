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
		enum EBrushType
		{
			E_BRUSH_PULL = 0,	// �ر�����
			E_BRUSH_SMOOTH,		// �ر�ƽ��
			E_BRUSH_NOISE,		// �ر�����
			E_BRUSH_AVERAGE,	// �ر�ƽ��
			E_BRUSH_FLAT,		// �ر�̤ƽ
			E_BRUSH_LAYER,		// �ر�������
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

		virtual EBrushType GetType() = 0;
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
		// �麯���̳�
		virtual EBrushType GetType() { return E_BRUSH_PULL;}
		virtual void Apply(const Base::Math::sdRay& kRay);
	};
	NiSmartPointer(sdTerrainDeformPoolBrush);

	// �ر�߶�ƽ����ˢ
	class sdTerrainDeformSmoothBrush : public sdEditBrush
	{
	public:
		// �麯���̳�
		virtual EBrushType GetType() { return E_BRUSH_SMOOTH;}
		virtual void Apply(const Base::Math::sdRay& kRay);
	};
	NiSmartPointer(sdTerrainDeformSmoothBrush);

	// �ر�߶�������ˢ
	//class sdTerrainDeformNoiseBrush : public sdEditBrush
	//{
	//public:
	//	// �麯���̳�
	//	virtual EBrushType GetType() { return E_BRUSH_NOISE;}
	//	virtual void Apply(const Base::Math::sdRay& kRay);
	//};
	//NiSmartPointer(sdTerrainDeformNoiseBrush);

	// �ر�߶�̤ƽ��ˢ
	//class sdTerrainDeformFlatBrush : public sdEditBrush
	//{
	//public:
	//	// �麯���̳�
	//	virtual EBrushType GetType() { return E_BRUSH_FLAT;}
	//	virtual void Apply(const Base::Math::sdRay& kRay);
	//};
	//NiSmartPointer(sdTerrainDeformFlatBrush);

	// �ر�߶�ƽ����ˢ
	//class sdTerrainDeformAverageBrush : public sdEditBrush
	//{
	//public:
	//	// �麯���̳�
	//	virtual EBrushType GetType() { return E_BRUSH_AVERAGE;}
	//	virtual void Apply(const Base::Math::sdRay& kRay);
	//};
	//NiSmartPointer(sdTerrainDeformAverageBrush);

	//****************************************************************************
	// �ر�����༭��ˢ
	class sdTerrainSurfaceLayerBrush : public sdEditBrush
	{
	public:
		sdTerrainSurfaceLayerBrush() : m_uiActiveLayerMap(0){}

		// �麯���̳�
		virtual EBrushType GetType() { return E_BRUSH_LAYER;}
		virtual void Apply(const Base::Math::sdRay& kRay);

		// �������������
		uint GetActiveLayerMap() const { return m_uiActiveLayerMap;}
		void SetActiveLayerMap(uint uiIndex) { m_uiActiveLayerMap = uiIndex;}

	protected:
		// ��ǰ����ͼ��
		uint	m_uiActiveLayerMap;	
	};
	NiSmartPointer(sdTerrainSurfaceLayerBrush);
}
#endif
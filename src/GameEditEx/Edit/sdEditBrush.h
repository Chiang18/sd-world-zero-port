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

namespace GameEditEx
{
	//****************************************************************************
	// �༭��ˢ����
	class sdEditBrush : public NiRefObject
	{
	public:
		sdEditBrush(){};
		virtual ~sdEditBrush() {};

		void SetEditShape(sdEditShape* pkEditShape) { m_pkEditShape = pkEditShape;}
		void SetBrushShape(NiAVObject* spAVObject) { m_spBrushShape = spAVObject;}
		virtual void Apply() = 0;

	protected:
		NiAVObjectPtr	m_spBrushShape;
		sdEditShapePtr	m_pkEditShape;
		
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
		virtual void Apply();

	protected:
	
	};
	NiSmartPointer(sdTerrainDeformPoolBrush);

	//****************************************************************************
	// �ر�����༭��ˢ
}
#endif
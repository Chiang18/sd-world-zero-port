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

namespace GameEditEx
{
	//****************************************************************************
	// �༭��ˢ����
	class sdEditBrush : public NiRefObject
	{
	public:
		sdEditBrush(){};
		virtual ~sdEditBrush() {};

	protected:
	};
	NiSmartPointer(sdEditBrush);

	//****************************************************************************
	// �ر�߶ȱ༭��ˢ
	class sdTerrainDeformEditBrush : public sdEditBrush
	{
	public:
		sdTerrainDeformEditBrush();
		virtual ~sdTerrainDeformEditBrush();
	};
	NiSmartPointer(sdTerrainDeformEditBrush);

	//****************************************************************************
	// �ر�����༭��ˢ
}
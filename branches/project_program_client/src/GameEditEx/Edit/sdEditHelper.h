//*************************************************************************************************
// ����:	�༭������
//---------------------------------------------------------
// ����:		
// ����:		
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMEEDITEX_EDITHELPER_H__
#define _SD_GAMEEDITEX_EDITHELPER_H__
#include "sdGameEditEx.h"

class sdMap;

namespace GameEditEx
{
	// �༭������
	//	1.ʵ��һЩ��״̬�޹صĸ����༭����
	//	2.�Եײ㹦�ܽ��г���
	class sdEditHelper : public NiRefObject
	{
	public:
		sdEditHelper();

		// �ײ������ʹ���
		sdMap*	GetMap() const { return m_pkMap;};


	protected:
		sdMap*	m_pkMap;	//��ǰ�༭�ĵ�ͼ

	};
	NiSmartPointer(sdEditHelper);
}
#endif
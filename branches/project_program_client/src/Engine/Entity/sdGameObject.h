//*************************************************************************************************
// ����:	�ڽ���Streaming��RTTI����
//---------------------------------------------------------
// ����:		
// ����:		2012-07-10
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _GAME_OBJECT_H__
#define _GAME_OBJECT_H__

// Ŀǰ��ʱ�̳���NiNode, ���������ڴ�ṹʱȥ��
class sdGameObject : public NiNode
{
	NiDeclareRTTI;
	NiDeclareStream;

public:
	sdGameObject();
	virtual ~sdGameObject();

protected:


};

#endif
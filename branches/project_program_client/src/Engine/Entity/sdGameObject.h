//*************************************************************************************************
// ����:	�ڽ���Streaming��RTTI����
//---------------------------------------------------------
// ����:		
// ����:		2012-07-10
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_SCENE_GAME_OBJECT_H__
#define _SD_ENGINE_SCENE_GAME_OBJECT_H__

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
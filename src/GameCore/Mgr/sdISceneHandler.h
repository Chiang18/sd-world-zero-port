//*************************************************************************************************
// ����:	�����¼��ص��ӿ�
//---------------------------------------------------------
// ����:		
// ����:		2012-09-06	
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_GAMECORE_ISCENE_HANDLER_H__
#define _SD_GAMECORE_ISCENE_HANDLER_H__

namespace GameCore
{
	// һЩ���ó����¼��Ļص��ӿ�(����һЩ��֤�����Ⱥ�˳������)
	class sdISceneHandler : public NiMemObject
	{
	public:
		virtual ~sdISceneHandler() = 0;

		// ����
		virtual void Update() = 0;

		// ��ͼ������ж��
		// @{
		virtual void OnMapBeforeLoad() = 0;
		virtual void OnMapAfterLoad() = 0;
		virtual void OnMapBeforeUnload() = 0;
		virtual void OnMapBeforeUnload() = 0;
		// @}
	};
}
#endif

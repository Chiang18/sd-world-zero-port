//*************************************************************************************************
// ����:	ʹ��Ĭ�ϲ�����Ⱦ����,���ڻ��Ƶ��Զ���
//---------------------------------------------------------
// ����:		
// ����:		2012-09-03
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_RENDERSYSTEM_DEBUGGIZMOPPASS_H__
#define _SD_ENGINE_RENDERSYSTEM_DEBUGGIZMOPPASS_H__
#include "sdRenderPass.h"

namespace RenderSystem
{
	// �Բ����Mesh,ʹ��GB��Ĭ�϶��������Ƶ����ջ���
	class sdDebugGizmoPass : public sdRenderPass
	{
	public:
		sdDebugGizmoPass(sdRenderObjectAlloc* pkAlloc);
		virtual ~sdDebugGizmoPass();

		// �麯���̳�
		virtual void Draw();

		// ��ʼ��������
		bool Initialize();
		void Destroy();

		// ��ӻ��ƶ���
		void InsertStaticMesh(NiMesh* spMesh);
	};
	NiSmartPointer(sdDebugGizmoPass);

}
#endif
//*************************************************************************************************
// ����:	�������״̬��
//---------------------------------------------------------
// ����:		
// ����:		2012-07-30
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _CAMERA_FSM_H__
#define _CAMERA_FSM_H__
#include <sdSingleton.h>
#include <sdFSM.h>

namespace GameCore
{
	// �������״̬��,�������豸��ȡ״̬,���Ƶ�ǰ���
	class sdCameraFSM : public Base::sdTSingleton<sdCameraFSM>, public Base::sdFSM
	{
	public:
		sdCameraFSM();
		~sdCameraFSM();

		// ��ʼ��������
		bool Initialize();
		void Destroy();

		// �麯���̳�
		virtual int UpdateState();

		// 
		NiCamera*	GetCamera() { return m_spCamera;};

	protected:
		// ����ֱ��ʹ�����,���Կ��ǹҽ���NiNode��ʹ��任����������ϵһ��
		NiCameraPtr	m_spCamera;
	};
}
#endif
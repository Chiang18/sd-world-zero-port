//*************************************************************************************************
// ����:	���״̬
//---------------------------------------------------------
// ����:		
// ����:		2012-07-30
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _CAMERA_STATE_H__
#define _CAMERA_STATE_H__
#include <sdFSM.h>

namespace GameCore
{

	//*************************************************************************
	class sdCameraState : public Base::sdState
	{
	public:
		// �������ģʽ
		enum eCameraMode
		{
			E_CAMERA_FOLLOW = 0,	// �������ģʽ
			E_CAMERA_MOVIE,			// ��������ģʽ
			E_CAMERA_FREE,			// �����ӽ�ģʽ
			NUM_CAMERA_MODE = E_CAMERA_FREE,
		};

	public:
		virtual void Reset(){};
	};

	//*************************************************************************
	class sdCameraFreeState : public sdCameraState
	{
	public:
		sdCameraFreeState();
		~sdCameraFreeState();

		// �麯���̳�
		virtual int Update();
	};
	//*************************************************************************
}
#endif
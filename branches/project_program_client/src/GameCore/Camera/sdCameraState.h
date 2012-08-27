//*************************************************************************************************
// 内容:	相机状态
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-30
// 最后修改:
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
		// 相机操作模式
		enum eCameraMode
		{
			E_CAMERA_FOLLOW = 0,	// 相机跟随模式
			E_CAMERA_MOVIE,			// 过场动画模式
			E_CAMERA_FREE,			// 自由视角模式
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

		// 虚函数继承
		virtual int Update();
	};
	//*************************************************************************
}
#endif
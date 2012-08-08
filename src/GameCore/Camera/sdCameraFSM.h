//*************************************************************************************************
// 内容:	相机控制状态机
//---------------------------------------------------------
// 作者:		
// 创建:		2012-07-30
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _CAMERA_FSM_H__
#define _CAMERA_FSM_H__
#include <sdSingleton.h>
#include <sdFSM.h>

namespace GameCore
{
	// 相机控制状态机,从输入设备提取状态,控制当前相机
	class sdCameraFSM : public Base::sdTSingleton<sdCameraFSM>, public Base::sdFSM
	{
	public:
		// 相机操作模式
		enum eCameraMode
		{
			E_CAMERA_FOLLOW = 0,	// 相机跟随模式
			E_CAMERA_MOVIE,			// 过场动画模式
			E_CAMERA_FREE,			// 自由视角模式
		};

	public:
		sdCameraFSM();
		~sdCameraFSM();

		// 初始化与销毁
		bool Initialize();
		void Destroy();

		// 
		NiCamera*	GetCamera() { return m_spCamera;};

		// 虚函数继承
		virtual int UpdateState();

	protected:
		// 这里直接使用相机,可以考虑挂接在NiNode上使其变换与世界坐标系一致
		NiCameraPtr	m_spCamera;
	};
}
#endif
//*************************************************************************************************
// 内容:	物理系统
//---------------------------------------------------------
// 作者:		
// 创建:		2012-08-17
// 最后修改:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_PHYSICS_SYSTEM_H__
#define _SD_ENGINE_PHYSICS_SYSTEM_H__
#include <sdEngine.h>
#include <sdSingleton.h>

namespace PhysicsSystem
{

	// 物理系统接口
	class sdPhysicsSystem : public Base::sdTSingleton<sdPhysicsSystem> 
	{
	public:
		sdPhysicsSystem();
		~sdPhysicsSystem();

		// 系统操作
		bool	Initialize();
		void	Destroy();
		void	Update();

		// 物理场景操作
		bool	CreateScene();
		bool	LoadScene();
		bool	SaveScene();
		void	DestroyScene();

		// 射线查询接口

	protected:
		bool	m_bInitialized;
		bool	m_bSceneLoaded;

		NxPhysicsSDK*		m_pkPhysicsSDK;
		NxUserAllocator*	m_pkAllocator;
		NxUserOutputStream*	m_pkOutputStream;
		NxScene*			m_pkScene;
		NxCookingInterface* m_pkCooking;
		//NxControllerManager*	m_pkControlManager;


	};
}
#endif
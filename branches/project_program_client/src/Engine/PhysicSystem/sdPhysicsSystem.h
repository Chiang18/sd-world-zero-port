//*************************************************************************************************
// ����:	����ϵͳ
//---------------------------------------------------------
// ����:		
// ����:		2012-08-17
// ����޸�:
//*************************************************************************************************
#pragma once
#ifndef _SD_ENGINE_PHYSICS_SYSTEM_H__
#define _SD_ENGINE_PHYSICS_SYSTEM_H__
#include <sdEngine.h>
#include <sdSingleton.h>

namespace PhysicsSystem
{

	// ����ϵͳ�ӿ�
	class sdPhysicsSystem : public Base::sdTSingleton<sdPhysicsSystem> 
	{
	public:
		sdPhysicsSystem();
		~sdPhysicsSystem();

		// ϵͳ����
		bool	Initialize();
		void	Destroy();
		void	Update();

		// ����������
		bool	CreateScene();
		bool	LoadScene();
		bool	SaveScene();
		void	DestroyScene();

		// ���߲�ѯ�ӿ�

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
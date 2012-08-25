#include "Stdafx.h"
#include "sdPhysicsSystem.h"
#include "sdPhysXOutputStream.h"

namespace PhysicsSystem
{
//-------------------------------------------------------------------------------------------------
sdPhysicsSystem::sdPhysicsSystem()
: m_bInitialized(false)
, m_bSceneLoaded(false)
, m_pkPhysicsSDK(NULL)
, m_pkAllocator(NULL)
, m_pkOutputStream(NULL)
, m_pkCooking(NULL)
{
}
//-------------------------------------------------------------------------------------------------
sdPhysicsSystem::~sdPhysicsSystem()
{
	if (m_bInitialized)
	{
		Destroy();
		m_bInitialized = false;
	}
}
//-------------------------------------------------------------------------------------------------
bool sdPhysicsSystem::Initialize()
{
	if (m_bInitialized)
	{
		NIASSERT(0);

		Destroy();
		m_bInitialized = false;
	}

	//// 创建内存分配器
	//m_pkAllocator = NiNew sdPhysXAllocator;
	//NIASSERT(m_pkAllocator);

	//// 创建调试输出数据流
	//m_pkOutputStream = NiNew sdPhysXOutputStream;
	//NIASSERT(m_pkOutputStream);

	//// 创建PhysX,初始化参数
	//NxSDKCreateError kErrorCode;
	//NxPhysicsSDKDesc kDesc;
	//m_pkPhysicsSDK = NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, NULL, m_pkOutputStream, kDesc, &kErrorCode);
	//NIASSERT(m_pkPhysicsSDK);

	//// 创建Cook库
	//m_pkCooking = NxGetCookingLib(NX_PHYSICS_SDK_VERSION);
	//NIASSERT(m_pkCooking);
	//m_pkCooking->NxInitCooking();

	return m_bInitialized = true;
}
//-------------------------------------------------------------------------------------------------
void sdPhysicsSystem::Destroy()
{
	if (m_bInitialized)
	{
		//NiDelete m_pkAllocator;
		//m_pkAllocator = NULL;

		//NiDelete m_pkOutputStream;
		//m_pkOutputStream = NULL;
	
		//m_pkCooking->NxCloseCooking();
		//m_pkCooking = NULL;

		//NxReleasePhysicsSDK(m_pkPhysicsSDK);
		//m_pkPhysicsSDK = NULL;

		//m_bInitialized = false;
	}
}
//-------------------------------------------------------------------------------------------------
}
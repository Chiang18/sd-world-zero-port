// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2009 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

#include "efdPhysXPCH.h"
#include "../PhysXSDKManager.h"

#include <efd/UniversalTypes.h>

using namespace efdPhysX;

//---------------------------------------------------------------------------
efd::Bool PhysXSDKManager::PlatformSpecificPostSDKInit()
{
    // No Win32 specific initialization required.
    return true;
}
//---------------------------------------------------------------------------
void PhysXSDKManager::PlatformSpecificSDKShutdown()
{
    // Deliberately left empty.
}
//---------------------------------------------------------------------------
NxScene* PhysXSDKManager::CreateSceneOnDifferentCore(
    const NxSceneDesc& desc, const efd::ProcessorAffinity&)
{
    // Simply create the scene. Windows will retarget the thread, and Intel
    // has actually advocated avoiding the use of affinities.
    NxScene* pScene = m_pPhysXSDK->createScene(desc);
    EE_ASSERT(pScene);

    return pScene;
}
//---------------------------------------------------------------------------
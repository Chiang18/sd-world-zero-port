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
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#include "NiPhysXPCH.h"

#include "NiPhysX.h"

#include <NiFilename.h>

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

NiPhysXManager* NiPhysXManager::ms_pPhysXManager;

//---------------------------------------------------------------------------
NiPhysXManager::NiPhysXManager()
{
    m_pkPhysXSDK = 0;
}
//---------------------------------------------------------------------------
NiPhysXManager::~NiPhysXManager()
{
}
//---------------------------------------------------------------------------
bool NiPhysXManager::Initialize(NxUserAllocator* pkAllocator,
        NxUserOutputStream* pkOutputStream, const NxPhysicsSDKDesc& kSDKDesc,
        NxSDKCreateError* eErrorCode)
{
    bool bResult = efdPhysX::PhysXSDKManager::GetManager()->Initialize(
        pkAllocator, pkOutputStream, kSDKDesc, eErrorCode);
    
    if (bResult)
    {
        m_pkPhysXSDK = efdPhysX::PhysXSDKManager::GetManager()->m_pPhysXSDK;
    }
    
    return bResult;
    
}
//---------------------------------------------------------------------------
void NiPhysXManager::Shutdown()
{
    m_pkPhysXSDK = 0;

    efdPhysX::PhysXSDKManager::GetManager()->Shutdown();    
}
//---------------------------------------------------------------------------
const char* NiPhysXManager::GetSDKCreateErrorString(
    NxSDKCreateError* peErrorCode)
{
    return efdPhysX::PhysXSDKManager::GetManager()->
        GetSDKCreateErrorString(peErrorCode);
}
//---------------------------------------------------------------------------
NxScene* NiPhysXManager::CreateSceneOnDifferentCore(
    const NxSceneDesc& kDesc, const NiProcessorAffinity& kAffinity)
{
    return efdPhysX::PhysXSDKManager::GetManager()->
        CreateSceneOnDifferentCore(kDesc, kAffinity);
}
//---------------------------------------------------------------------------

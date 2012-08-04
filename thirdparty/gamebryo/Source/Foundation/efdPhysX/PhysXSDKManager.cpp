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

#include <efd/Asserts.h>
#include <efd/ILogger.h>
#include <efd/MemoryDefines.h>
#include <efd/UniversalTypes.h>
#include <efd/utf8string.h>
#include <efd/efdLogIDs.h>

#include "PhysXSDKManager.h"

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

using namespace efd;
using namespace efdPhysX;

PhysXSDKManager* PhysXSDKManager::ms_pPhysXManager;
const utf8string PhysXSDKManager::kConfigSection("PhysX.SDK");
const utf8string PhysXSDKManager::kConfigDefaultSleepEnergy("DefaultSleepEnergy");
const utf8string PhysXSDKManager::kConfigDefaultSleepLinVel("DefaultSleepLinear");
const utf8string PhysXSDKManager::kConfigDefaultSleepAngVel("DefaultSleepAngular");
const utf8string PhysXSDKManager::kConfigVRDHost("VRDHost");
const utf8string PhysXSDKManager::kConfigVRDPort("VRDPort");
const utf8string PhysXSDKManager::kConfigVRDEventMask("VRDEventMask");

//-------------------------------------------------------------------------------------------------
PhysXSDKManager::PhysXSDKManager()
{
    m_pAllocator = 0;
    m_pOutputStream = 0;
    m_pPhysXSDK = 0;
}
//-------------------------------------------------------------------------------------------------
PhysXSDKManager::~PhysXSDKManager()
{
}
//-------------------------------------------------------------------------------------------------
Bool PhysXSDKManager::Initialize(NxUserAllocator* pAllocator, NxUserOutputStream* pOutputStream,
    const NxPhysicsSDKDesc& sdkDesc, NxSDKCreateError* errorCode)
{
    // If allocator or output stream are NULL, then allocate and use the
    // defaults. If someone is already using an SDK, our suggestions will be
    // ignored.
    if (!pAllocator)
    {
        m_pAllocator = EE_NEW PhysXAllocator;
        pAllocator = m_pAllocator;
    }
    if (!pOutputStream)
    {
        m_pOutputStream = EE_NEW PhysXUserOutput;
        pOutputStream = m_pOutputStream;
    }
    
    WaitSDKLock();
    m_pPhysXSDK =
        NxCreatePhysicsSDK(NX_PHYSICS_SDK_VERSION, pAllocator, pOutputStream, sdkDesc, errorCode);
    ReleaseSDKLock();
    
    if (m_pPhysXSDK == 0)
    {
        EE_LOG(efd::kPhysicsSystem, efd::ILogger::kERR1,
            ("PhysXSDKManager: Failed to create PhysX SDK: %s\n",
            GetSDKCreateErrorString(errorCode)));
        return false;
    }
    
    if (!PlatformSpecificPostSDKInit())
    {
        Shutdown();
        return false;
    }

    return true;
}
//-------------------------------------------------------------------------------------------------
void PhysXSDKManager::Shutdown()
{
    if (m_pPhysXSDK)
    {    
        // Clean up the SDK
        WaitSDKLock();
        NxReleasePhysicsSDK(m_pPhysXSDK);
        m_pPhysXSDK = 0;
        ReleaseSDKLock();
    }
    
    // Clean up the allocator and output stream
    EE_DELETE m_pOutputStream;
    m_pOutputStream = 0;
    EE_DELETE m_pAllocator;
    m_pAllocator = 0;
    
    // Clear out the meshes
    WaitMeshLock();
    m_clothMeshes.clear();
    m_convexMeshes.clear();
    m_triangleMeshes.clear();
    ReleaseMeshLock();

    PlatformSpecificSDKShutdown();
}
//-------------------------------------------------------------------------------------------------
void PhysXSDKManager::Configure(IConfigManager* pConfigManager)
{
    EE_ASSERT(m_pPhysXSDK);

    const ISection *pSection = pConfigManager->GetConfiguration()->FindSection(kConfigSection);

    if (pSection)
    {
        efd::utf8string defaultSleepEnergy = pSection->FindValue(kConfigDefaultSleepEnergy);
        if (!defaultSleepEnergy.empty())
        {
            NxReal value = 0.005f;
            value = (NxReal)atof(defaultSleepEnergy.c_str());
            m_pPhysXSDK->setParameter(NX_DEFAULT_SLEEP_ENERGY, value);
        }

        efd::utf8string defaultSleepLinVel = pSection->FindValue(kConfigDefaultSleepLinVel);
        if (!defaultSleepLinVel.empty())
        {
            NxReal value = 0.005f;
            value = (NxReal)atof(defaultSleepLinVel.c_str());
            m_pPhysXSDK->setParameter(NX_DEFAULT_SLEEP_LIN_VEL_SQUARED, value);
        }

        efd::utf8string defaultSleepAngVel = pSection->FindValue(kConfigDefaultSleepAngVel);
        if (!defaultSleepAngVel.empty())
        {
            NxReal value = 0.005f;
            value = (NxReal)atof(defaultSleepAngVel.c_str());
            m_pPhysXSDK->setParameter(NX_DEFAULT_SLEEP_ANG_VEL_SQUARED, value);
        }

        efd::utf8string vrdHost = pSection->FindValue(kConfigVRDHost);
        if (!vrdHost.empty())
        {
            efd::UInt32 port = NX_DBG_DEFAULT_PORT;
            efd::UInt32 eventMask = NX_DBG_EVENTMASK_EVERYTHING;
            
            efd::utf8string vrdPort = pSection->FindValue(kConfigVRDPort);
            if (!vrdPort.empty())
            {
                port = atoi(vrdPort.c_str());
            }
            
            efd::utf8string vrdEventMask = pSection->FindValue(kConfigVRDEventMask);
            if (!vrdEventMask.empty())
            {
                eventMask = atoi(vrdPort.c_str());
            }
            
            m_pPhysXSDK->getFoundationSDK().getRemoteDebugger()->connect(
                vrdHost.c_str(), port, eventMask);
        }
    }
}
//-------------------------------------------------------------------------------------------------
const char* PhysXSDKManager::GetSDKCreateErrorString(NxSDKCreateError* pErrorCode)
{
    if (!pErrorCode)
        return NULL;
        
    switch (*pErrorCode)
    {
        case NXCE_NO_ERROR:
            return "No errors occurred when creating the Physics SDK.";
            
        case NXCE_PHYSX_NOT_FOUND:
            return "Unable to find the PhysX libraries. The PhysX drivers are "
                "not installed correctly.";
            
        case NXCE_WRONG_VERSION:
            return "The application supplied a version number that does not "
                "match with the library's.";
                  
        case NXCE_DESCRIPTOR_INVALID:
            return "The supplied SDK descriptor is invalid.";
            
        case NXCE_CONNECTION_ERROR:
            return "A PhysX card was found, but there are problems when "
                "communicating with the card.";
                
        case NXCE_RESET_ERROR:
            return "A PhysX card was found, but it did not reset (or "
                "initialize) properly.";
                
        case NXCE_IN_USE_ERROR:
            return "A PhysX card was found, but it is already in use by "
                "another application.";

        case NXCE_BUNDLE_ERROR:
            return "A PhysX card was found, but there are issues with loading "
                "the firmware.";
                
        default:
            EE_ASSERT(false && "Unknown PhysX SDK Create error code");
    }
    
    return NULL;
}
//-------------------------------------------------------------------------------------------------
efd::Bool PhysXSDKManager::SetClothMesh(const efd::utf8string& name, NxClothMesh* pMesh)
{
    EE_STL_NAMESPACE::pair<efd::utf8string, NxClothMesh*> entry(name, pMesh);

    WaitMeshLock();

    EE_STL_NAMESPACE::pair<map<efd::utf8string, NxClothMesh*>::iterator, bool> result =
        m_clothMeshes.insert(entry);

    ReleaseMeshLock();

    return result.second;
}
//-------------------------------------------------------------------------------------------------
NxClothMesh* PhysXSDKManager::GetClothMesh(const efd::utf8string& meshName)
{
    WaitMeshLock();

    map<efd::utf8string, NxClothMesh*>::iterator itor = m_clothMeshes.find(meshName);
    
    if (itor == m_clothMeshes.end())
    {
        ReleaseMeshLock();
        return NULL;
    }
    
    NxClothMesh* pMesh = itor->second;

    ReleaseMeshLock();

    return pMesh;
}
//-------------------------------------------------------------------------------------------------
efd::utf8string PhysXSDKManager::GetClothMeshName(const NxClothMesh* pMesh)
{
    WaitMeshLock();

    map<efd::utf8string, NxClothMesh*>::iterator itor;
    for (itor = m_clothMeshes.begin(); itor != m_clothMeshes.end(); ++itor)
    {
        if (itor->second == pMesh)
        {
            ReleaseMeshLock();

            return itor->first;
        }
    }
    
    ReleaseMeshLock();
    
    efd::utf8string empty;
    return empty;
}
//-------------------------------------------------------------------------------------------------
efd::Bool PhysXSDKManager::RemoveClothMesh(const efd::utf8string& name)
{
    WaitMeshLock();

    map<efd::utf8string, NxClothMesh*>::iterator itor = m_clothMeshes.find(name);

    if (itor == m_clothMeshes.end())
    {
        ReleaseMeshLock();
        return false;
    }

    NxClothMesh* pMesh = itor->second;

    if (pMesh->getReferenceCount() != 0)
    {
        ReleaseMeshLock();
        return false;
    }

    m_pPhysXSDK->releaseClothMesh(*pMesh);

    m_clothMeshes.erase(itor);

    ReleaseMeshLock();

    return true;
}
//-------------------------------------------------------------------------------------------------
efd::Bool PhysXSDKManager::SetConvexMesh(const efd::utf8string& name, NxConvexMesh* pMesh)
{
    EE_STL_NAMESPACE::pair<efd::utf8string, NxConvexMesh*> entry(name, pMesh);

    WaitMeshLock();

    EE_STL_NAMESPACE::pair<map<efd::utf8string, NxConvexMesh*>::iterator, bool> result =
        m_convexMeshes.insert(entry);

    ReleaseMeshLock();

    return result.second;
}
//-------------------------------------------------------------------------------------------------
NxConvexMesh* PhysXSDKManager::GetConvexMesh(const efd::utf8string& meshName)
{
    WaitMeshLock();

    map<efd::utf8string, NxConvexMesh*>::iterator itor = m_convexMeshes.find(meshName);
    
    if (itor == m_convexMeshes.end())
    {
        ReleaseMeshLock();
        return NULL;
    }
    
    NxConvexMesh* pMesh = itor->second;

    ReleaseMeshLock();

    return pMesh;
}
//-------------------------------------------------------------------------------------------------
efd::utf8string PhysXSDKManager::GetConvexMeshName(const NxConvexMesh* pMesh)
{
    WaitMeshLock();

    map<efd::utf8string, NxConvexMesh*>::iterator itor;
    for (itor = m_convexMeshes.begin(); itor != m_convexMeshes.end(); ++itor)
    {
        if (itor->second == pMesh)
        {
            ReleaseMeshLock();

            return itor->first;
        }
    }
    
    ReleaseMeshLock();
    
    efd::utf8string empty;
    return empty;
}
//-------------------------------------------------------------------------------------------------
efd::Bool PhysXSDKManager::RemoveConvexMesh(const efd::utf8string& name)
{
    WaitMeshLock();

    map<efd::utf8string, NxConvexMesh*>::iterator itor = m_convexMeshes.find(name);

    if (itor == m_convexMeshes.end())
    {
        ReleaseMeshLock();
        return false;
    }

    NxConvexMesh* pMesh = itor->second;

    if (pMesh->getReferenceCount() != 0)
    {
        ReleaseMeshLock();
        return false;
    }

    m_pPhysXSDK->releaseConvexMesh(*pMesh);

    m_convexMeshes.erase(itor);

    ReleaseMeshLock();

    return true;
}
//-------------------------------------------------------------------------------------------------
efd::Bool PhysXSDKManager::SetTriangleMesh(const efd::utf8string& name, NxTriangleMesh* pMesh)
{
    EE_STL_NAMESPACE::pair<efd::utf8string, NxTriangleMesh*> entry(name, pMesh);

    WaitMeshLock();

    EE_STL_NAMESPACE::pair<map<efd::utf8string, NxTriangleMesh*>::iterator, bool> result =
        m_triangleMeshes.insert(entry);

    ReleaseMeshLock();

    return result.second;
}
//-------------------------------------------------------------------------------------------------
NxTriangleMesh* PhysXSDKManager::GetTriangleMesh(const efd::utf8string& meshName)
{
    WaitMeshLock();

    map<efd::utf8string, NxTriangleMesh*>::iterator itor = m_triangleMeshes.find(meshName);
    
    if (itor == m_triangleMeshes.end())
    {
        ReleaseMeshLock();
        return NULL;
    }
    
    NxTriangleMesh* pMesh = itor->second;

    ReleaseMeshLock();

    return pMesh;
}
//-------------------------------------------------------------------------------------------------
efd::utf8string PhysXSDKManager::GetTriangleMeshName(const NxTriangleMesh* pMesh)
{
    WaitMeshLock();

    map<efd::utf8string, NxTriangleMesh*>::iterator itor;
    for (itor = m_triangleMeshes.begin(); itor != m_triangleMeshes.end(); ++itor)
    {
        if (itor->second == pMesh)
        {
            ReleaseMeshLock();

            return itor->first;
        }
    }
    
    ReleaseMeshLock();
    
    efd::utf8string empty;
    return empty;
}
//-------------------------------------------------------------------------------------------------
efd::Bool PhysXSDKManager::RemoveTriangleMesh(const efd::utf8string& name)
{
    WaitMeshLock();

    map<efd::utf8string, NxTriangleMesh*>::iterator itor = m_triangleMeshes.find(name);

    if (itor == m_triangleMeshes.end())
    {
        ReleaseMeshLock();
        return false;
    }

    NxTriangleMesh* pMesh = itor->second;

    if (pMesh->getReferenceCount() != 0)
    {
        ReleaseMeshLock();
        return false;
    }

    m_pPhysXSDK->releaseTriangleMesh(*pMesh);

    m_triangleMeshes.erase(itor);

    ReleaseMeshLock();

    return true;
}
//-------------------------------------------------------------------------------------------------
void PhysXSDKManager::RemoveUnusedMeshes()
{
    vector<efd::utf8string> meshesToRemove;

    map<efd::utf8string, NxClothMesh*>::iterator clothItor;
    for (clothItor = m_clothMeshes.begin(); clothItor != m_clothMeshes.end(); ++clothItor)
    {
        if (clothItor->second->getReferenceCount() == 0)
        {
            meshesToRemove.push_back(clothItor->first);
        }
    }
    for (efd::UInt32 ui = 0; ui < meshesToRemove.size(); ++ui)
        RemoveClothMesh(meshesToRemove[ui]);
    meshesToRemove.clear();

    map<efd::utf8string, NxConvexMesh*>::iterator convItor;
    for (convItor = m_convexMeshes.begin(); convItor != m_convexMeshes.end(); ++convItor)
    {
        if (convItor->second->getReferenceCount() == 0)
        {
            meshesToRemove.push_back(convItor->first);
        }
    }
    for (efd::UInt32 ui = 0; ui < meshesToRemove.size(); ++ui)
        RemoveConvexMesh(meshesToRemove[ui]);
    meshesToRemove.clear();
    
    map<efd::utf8string, NxTriangleMesh*>::iterator triItor;
    for (triItor = m_triangleMeshes.begin(); triItor != m_triangleMeshes.end(); ++triItor)
    {
        if (triItor->second->getReferenceCount() == 0)
        {
            meshesToRemove.push_back(triItor->first);
        }
    }
    for (efd::UInt32 ui = 0; ui < meshesToRemove.size(); ++ui)
        RemoveTriangleMesh(meshesToRemove[ui]);
    meshesToRemove.clear();
}
//---------------------------------------------------------------------------

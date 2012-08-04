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

#include "NiSceneGraphUpdateServicePCH.h"

#include <efd/ServiceManager.h>

#include "NiViewerSceneGraphUpdate.h"
#include "NiSceneGraphUpdateService.h"

#include <NiPSParticleSystem.h>
#include <NiPSMeshParticleSystem.h>

// The maximum amount of time to clamp the particles to
#define MAX_PARTICLE_RUN_UP_TIME 10.0f

//--------------------------------------------------------------------------------------------------
NiViewerSceneGraphUpdate::NiViewerSceneGraphUpdate()
{
    m_bAnimationsStopped = false;
    m_fPlaybackStartTime = 0.0f;
    m_fPlaybackEndTime = 0.0f;
}
//--------------------------------------------------------------------------------------------------
NiViewerSceneGraphUpdate::~NiViewerSceneGraphUpdate()
{
}
//--------------------------------------------------------------------------------------------------
void NiViewerSceneGraphUpdate::InitializeViewer(
    NiViewerSceneGraphUpdate* pkViewerSceneGraphUpdate /* = NULL */,
        efd::ServiceManager* pkServiceManager /* = NULL */)
{
    if (!pkViewerSceneGraphUpdate)
    {
        pkViewerSceneGraphUpdate = NiNew NiViewerSceneGraphUpdate;
    }

    NiSceneGraphUpdate::Initialize(pkViewerSceneGraphUpdate, pkServiceManager);

    // Make sure we have a valid service manager
    if (!pkServiceManager)
    {
        pkServiceManager = ms_pkThis->m_pkServiceManager;
    }

    // Register the update service
    pkServiceManager->RegisterSystemService(
       NiSceneGraphUpdateService::CLASS_ID,
       EE_NEW NiSceneGraphUpdateService(true));

    NiSceneGraphUpdate::GetInstance()->Start();
}
//--------------------------------------------------------------------------------------------------
void NiViewerSceneGraphUpdate::ShutdownViewer()
{
    if (NiSceneGraphUpdate::GetInstance())
    {
        NiSceneGraphUpdate::GetInstance()->Stop();
        NiSceneGraphUpdate::Shutdown();
    }
}
//--------------------------------------------------------------------------------------------------
bool NiViewerSceneGraphUpdate::Update()
{
    // If we own the service manager we need to update it.
    if (m_bOwnsServiceManager)
    {
        if (!UpdateServiceManager())
            return false;
    }

    CheckForPlaybackEndTime();

    if (!IsReady())
        return false;

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiViewerSceneGraphUpdate::Start(NiUInt32 uiWaitForNetworkInitInSec /* = 0 */)
{
    NiSceneGraphUpdate::GetInstance()->SendImmediate(NiSceneGraphUpdate::MESSAGE_VIEWER_ADDED);
    return NiSceneGraphUpdate::Start(uiWaitForNetworkInitInSec);
}
//--------------------------------------------------------------------------------------------------
bool NiViewerSceneGraphUpdate::Stop()
{
    m_kObjects.RemoveAll();
    m_kNiObjects.RemoveAll();
    m_kRootNodes.RemoveAll();
    m_kActiveCameras.RemoveAll();

    // Revert all the inserted nodes
    NiTMapIterator kIter = m_kInsertNodes.GetFirstPos();
    while (kIter)
    {
        NiSceneGraphUpdateObjectId kId;
        NiNodePtr spNodeParent;
        m_kInsertNodes.GetNext(kIter, kId, spNodeParent);

        RemoveInsertNode(kId);
    }

    NiSceneGraphUpdate::GetInstance()->SendImmediate(NiSceneGraphUpdate::MESSAGE_VIEWER_REMOVED);
    return NiSceneGraphUpdate::Stop();
}
//--------------------------------------------------------------------------------------------------
void NiViewerSceneGraphUpdate::PostHandleUpdateObject()
{
    NiSceneGraphUpdate::PostHandleUpdateObject();

    if (m_kSettings.GetRestartPlaybackAfterEdit())
    {
        RestartLoopingAnimations();
    }

    if (m_kSettings.GetRunUpParticlesAfterEdit())
    {
        RunUpAllParticleSystems();        
    }
}
//--------------------------------------------------------------------------------------------------
void NiViewerSceneGraphUpdate::PostHandleUpdateSettings()
{
    RestartLoopingAnimations();
}
//--------------------------------------------------------------------------------------------------
void NiViewerSceneGraphUpdate::RestartLoopingAnimations()
{
    m_fPlaybackStartTime = NiGetCurrentTimeInSec();
    m_fPlaybackEndTime = m_fPlaybackStartTime + (m_kSettings.GetPlaybackStopTimeInSec() -
        m_kSettings.GetPlaybackStartTimeInSec());

    NiNode* pkRootNode = GetRootNode();
    if (!pkRootNode)
    {
        return;
    }

    // Calculate the new animation start time based on the current time and the playback
    // window (this is effectively the start of the animation timeline)
    float fAnimationStartTime = m_fPlaybackStartTime;
    if (fAnimationStartTime > m_kSettings.GetPlaybackStartTimeInSec())
    {
        // Back up the animation start time so that we can run up to the m_fPlaybackStartTime
        fAnimationStartTime -= m_kSettings.GetPlaybackStartTimeInSec();
    }

    // Reset all animations (including particle systems) in the scene
    StopAnimationControllers(pkRootNode, false);
    StartAnimationControllers(pkRootNode, fAnimationStartTime);
    pkRootNode->Update(fAnimationStartTime);

    // Run up the scene at 30 fps until the current time
    for (float fStepTime = fAnimationStartTime; fStepTime < m_fPlaybackStartTime; 
        fStepTime += 0.0333f)
    {
        pkRootNode->Update(fStepTime);
    }

    m_bAnimationsStopped = false;
}
//--------------------------------------------------------------------------------------------------
void NiViewerSceneGraphUpdate::CheckForPlaybackEndTime()
{
    NiNode* pkRootNode = GetRootNode();
    if (!pkRootNode)
    {
        return;
    }

    if (m_kSettings.IsPlaybackStopTimeValid() && NiGetCurrentTimeInSec() >= m_fPlaybackEndTime)
    {
        switch(m_kSettings.GetPlaybackMode())
        {
        case NiSceneGraphUpdateSettings::SGU_PLAYBACK_ONCE:
            // This boolean prevents 'continuous stopping' once we've passed the end time.
            if (!m_bAnimationsStopped)
            {
                // Either stop everything or just the non-particle system animations (based
                // on the GetSimulatePastEndTime() setting)
                StopAnimationControllers(pkRootNode, m_kSettings.GetSimulatePastEndTime());
                m_bAnimationsStopped = true;
            }
            break;

        case NiSceneGraphUpdateSettings::SGU_PLAYBACK_LOOP:            
            RestartLoopingAnimations();
            break;

        default:
            NIASSERT(!"Unknown playback mode enum");
            break;
        }
    }
}
//--------------------------------------------------------------------------------------------------
void NiViewerSceneGraphUpdate::RunUpAllParticleSystems()
{
    NiNode* pkRootNode = GetRootNode();
    if (pkRootNode)
    {
        NiTPointerList<NiAVObject*> kObjects;
        pkRootNode->GetObjectsByType(&NiPSParticleSystem::ms_RTTI, kObjects);
        pkRootNode->GetObjectsByType(&NiPSMeshParticleSystem::ms_RTTI, kObjects);
        if (kObjects.GetSize() > 0)
        {
            NiTListIterator kIter = kObjects.GetHeadPos();
            while (kIter)
            {
                NiPSParticleSystem* pkParticleSystem = (NiPSParticleSystem*)kObjects.GetNext(kIter);
                float fRunUpTime = NiMin(pkParticleSystem->GetRunUpTime(), 
                    MAX_PARTICLE_RUN_UP_TIME);
                if (fRunUpTime > 0.0f)
                {
                    float fStartTime = NiGetCurrentTimeInSec() - fRunUpTime;
                    float fEndTime = fStartTime + fRunUpTime;
                    float fIterationTime = fRunUpTime / 30 * fRunUpTime;

                    for (float fTime = fStartTime; fTime <= fEndTime; fTime += fIterationTime)
                    {
                        pkParticleSystem->Update(fTime);
                    }
                }
            }
        }
    }
}
//--------------------------------------------------------------------------------------------------
void NiViewerSceneGraphUpdate::StartAnimationControllers(NiObjectNET* pkObj, float fTime)
{
    // Recurse down the scene graph calling start on all NiTimeController
    // derived classes that are within the current playback window.

    NiTimeController* pkControl = pkObj->GetControllers();
    for (/**/; pkControl; pkControl = pkControl->GetNext())
    {
        // Set animation type to APP_INIT so that the animation's internal start time
        // will be the time passed into Start() method
        pkControl->SetAnimType(NiTimeController::APP_INIT);
        pkControl->Start(fTime);
    }

    if (NiIsKindOf(NiAVObject, pkObj))
    {
        NiAVObject* pkAVObj = (NiAVObject*) pkObj;

        // recurse on properties
        NiTListIterator kPos = pkAVObj->GetPropertyList().GetHeadPos();
        while (kPos)
        {
            NiProperty* pkProperty = pkAVObj->GetPropertyList().GetNext(kPos);
            if (pkProperty && pkProperty->GetControllers())
                StartAnimationControllers(pkProperty, fTime);
        }
    }

    if (NiIsKindOf(NiNode, pkObj))
    {
        NiNode* pkNode = (NiNode*) pkObj;

        // recurse on children
        for (unsigned int i = 0; i < pkNode->GetArrayCount(); i++)
        {
            NiAVObject* pkChild;

            pkChild = pkNode->GetAt(i);
            if (pkChild)
                StartAnimationControllers(pkChild, fTime);
        }
    }
}
//--------------------------------------------------------------------------------------------------
void NiViewerSceneGraphUpdate::StopAnimationControllers(NiObjectNET* pkObj, bool bSkipParticleSystems)
{
    // Recurse down the scene graph calling stop on all NiTimeController
    // derived classes.

    NiTimeController* pkControl = pkObj->GetControllers();
    for (/**/; pkControl; pkControl = pkControl->GetNext())
    {
        if (!bSkipParticleSystems || !IsPSController(pkControl))
        {
            pkControl->Stop();
        }
    }

    if (NiIsKindOf(NiAVObject, pkObj))
    {
        NiAVObject* pkAVObj = (NiAVObject*) pkObj;

        // recurse on properties
        NiTListIterator kPos = pkAVObj->GetPropertyList().GetHeadPos();
        while (kPos)
        {
            NiProperty* pProperty = pkAVObj->GetPropertyList().GetNext(kPos);
            if (pProperty && pProperty->GetControllers())
                StopAnimationControllers(pProperty, bSkipParticleSystems);
        }
    }

    if (NiIsKindOf(NiNode, pkObj))
    {
        NiNode* pkNode;
        unsigned int i;

        pkNode = (NiNode*) pkObj;

        // recurse on children
        for (i = 0; i < pkNode->GetArrayCount(); i++)
        {
            NiAVObject* pkChild;

            pkChild = pkNode->GetAt(i);
            if (pkChild)
                StopAnimationControllers(pkChild, bSkipParticleSystems);
        }
    }
}
//--------------------------------------------------------------------------------------------------

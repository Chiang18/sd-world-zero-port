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

#include "NiEditorSceneGraphUpdate.h"
#include "NiSceneGraphUpdateService.h"

#include "NiSourceTexture.h"

#include <efdNetwork/ChannelManagerService.h>

#include <efd/ServiceManager.h>

#if defined(WIN32)
#include <efdNetwork/BridgeService.h>
#include <efd/DynamicModule.h>
#include <efd/IUSBComm.h>

efd::DynamicModule* NiEditorSceneGraphUpdate::ms_pkNetLibWiiUSBModule = NULL;
#endif

//--------------------------------------------------------------------------------------------------
NiEditorSceneGraphUpdate::NiEditorSceneGraphUpdate()
{
    m_uiProcessingMessageCount = 0;
}
//--------------------------------------------------------------------------------------------------
NiEditorSceneGraphUpdate::~NiEditorSceneGraphUpdate()
{
}
//--------------------------------------------------------------------------------------------------
void NiEditorSceneGraphUpdate::InitializeEditor(
    NiEditorSceneGraphUpdate* pkEditorSceneGraphUpdate /* = NULL */, 
    efd::ServiceManager* pkServiceManager /* = NULL */)
{
    NiSourceTexture::SetDestroyAppDataFlag(false);

    // Allocate the editor if one is not already provided
    if (!pkEditorSceneGraphUpdate)
    {
        pkEditorSceneGraphUpdate = NiNew NiEditorSceneGraphUpdate;
    }

    NiSceneGraphUpdate::Initialize(pkEditorSceneGraphUpdate, pkServiceManager);

    if (!pkServiceManager)
    {
        pkServiceManager = GetInstance()->m_pkServiceManager;
    }

    // Attempt to create bridge from TCP to USB if running on the editor side in case
    // a Wii dev kit is connected.
#if defined(WIN32)
    efd::utf8string netLibWiiUSBStr = "efdNetLibWiiUSB";
    NIASSERT(ms_pkNetLibWiiUSBModule == NULL);
    ms_pkNetLibWiiUSBModule = new efd::DynamicModule;
    if (ms_pkNetLibWiiUSBModule->LoadModule(netLibWiiUSBStr))
    {
        efd::RegisterNetLibType fncRegisterNetLib = 
            (efd::RegisterNetLibType)ms_pkNetLibWiiUSBModule->GetMethod("RegisterNetLib");
        if (fncRegisterNetLib)
        {
            fncRegisterNetLib();
            efd::BridgeServicePtr spBridgeService = 
                EE_NEW efd::BridgeService(efd::NetService::kNetLibTCP,"NetLibWiiUSB");
            pkServiceManager->RegisterSystemService(
                efd::BridgeService::CLASS_ID,
                spBridgeService);
        }
    }
#endif

    // Register dependent services
    efd::ChannelManagerService* pkChannelManagerService = 
        pkServiceManager->GetSystemServiceAs<efd::ChannelManagerService>();
    if (!pkChannelManagerService)
    {
        pkServiceManager->RegisterSystemService(
            efd::ChannelManagerService::CLASS_ID,
            EE_NEW efd::ChannelManagerService);
    }

    // Register the update service
    pkServiceManager->RegisterSystemService(
       NiSceneGraphUpdateService::CLASS_ID,
       EE_NEW NiSceneGraphUpdateService(false));
}
//--------------------------------------------------------------------------------------------------
void NiEditorSceneGraphUpdate::ShutdownEditor()
{
    NiSceneGraphUpdate::GetInstance()->SendImmediate(NiSceneGraphUpdate::MESSAGE_EDITOR_STOPPED);

    NiSceneGraphUpdate::Shutdown();

#if defined(WIN32)
    if (ms_pkNetLibWiiUSBModule != NULL)
    {
        if (ms_pkNetLibWiiUSBModule->IsLoaded())
        {
            efd::ShutdownNetLibType fncShutdownNetLib = 
                (efd::ShutdownNetLibType)ms_pkNetLibWiiUSBModule->GetMethod("ShutdownNetLib");
            if (fncShutdownNetLib)
            {
                fncShutdownNetLib();
            }
        }
        delete ms_pkNetLibWiiUSBModule;
        ms_pkNetLibWiiUSBModule = NULL;
    }
#endif
}
//--------------------------------------------------------------------------------------------------
bool NiEditorSceneGraphUpdate::Update()
{
    // If we own the service manager we need to update it.
    if (m_bOwnsServiceManager)
    {
        if (!UpdateServiceManager())
            return false;
    }

    if (!IsReady())
        return false;

    return true;
}
//--------------------------------------------------------------------------------------------------
#if defined(WIN32)
bool NiEditorSceneGraphUpdate::IsWiiConnectionAvailable()
{
    return efd::IUSBComm::Instance()->IsInitialized();
}
#endif
//--------------------------------------------------------------------------------------------------

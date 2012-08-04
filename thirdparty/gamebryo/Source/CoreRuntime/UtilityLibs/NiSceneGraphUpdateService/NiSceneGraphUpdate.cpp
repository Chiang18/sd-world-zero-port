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

#include "NiSceneGraphUpdate.h"
#include "NiSceneGraphUpdateService.h"

#include <NiLight.h>

#include <efd/ServiceManager.h>
#include <efd/ConfigManager.h>
#include <efdNetwork/NetService.h>
#include <efd/MessageService.h>
#include <efd/FileDestination.h>

#if defined(_WII)
#include <efd/Wii/WiiHeapManager.h>
#include <NiWiiHeapManager.h>
#include <efdNetLibWiiUSB/NetLibWiiUSB.h>
#include <efdNetLibWiiUSB/USBSocket.h>
#endif

#if defined(WIN32)
// Required to enumerate network adapters on Windows
#include <iphlpapi.h>
#endif

NiSceneGraphUpdate* NiSceneGraphUpdate::ms_pkThis = NULL;

// A function to simply make all the children point to the parent.
//--------------------------------------------------------------------------------------------------
void FixupChildren(NiNode* pkNode)
{
    if (!pkNode)
        return;

    NiUInt32 uiCount = pkNode->GetArrayCount();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            if (pkChild->GetParent() != pkNode)
                pkChild->AttachParent(pkNode);

            NiNode* pkChildNode = NiDynamicCast(NiNode, pkChild);
            FixupChildren(pkChildNode);
        }
    }
}

//--------------------------------------------------------------------------------------------------
NiSceneGraphUpdate::NiSceneGraphUpdate()
{
#if defined(_WII)
    // Initialize Foundation heap manager with already-initialized Gamebryo heap manager
    if (!efd::WiiHeapManager::IsInitialized())
    {
        // Give Foundation access to the memory arenas allocated by Gamebryo
        efd::WiiHeapManager::Initialize(NiWiiHeapManager::GetHeap(NI_WII_MEM1_ARENA), 
            NiWiiHeapManager::GetHeap(NI_WII_MEM2_ARENA));
    }
#endif

    m_pkSceneReadyCallbackParam = NULL;
    m_pfnSceneReadyCallback = NULL;
    m_bOwnsServiceManager = false;
    m_pkServiceManager = NULL;
    m_bIsReady = false;
    m_bIsDirty = true;
    m_uiInstanceId = 0;
    m_uiProcessingMessageCount = 0;
}
//--------------------------------------------------------------------------------------------------
NiSceneGraphUpdate::~NiSceneGraphUpdate()
{
    RemoveAllObjects();

    if (m_bOwnsServiceManager)
    {
        m_pkServiceManager->ShutdownServices();
        EE_DELETE m_pkServiceManager;
        m_pkServiceManager = NULL;
    }
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdate::Initialize(NiSceneGraphUpdate* pkSceneGraphUpdate /* = NULL */, 
    efd::ServiceManager* pkServiceManager /* = NULL */)
{
   NIASSERT(!ms_pkThis);

   ms_pkThis = pkSceneGraphUpdate;

   if (!pkServiceManager)
   {
       pkServiceManager = EE_NEW efd::ServiceManager();
       
       pkServiceManager->SetProgramType(efd::ServiceManager::kProgType_Server);
       ms_pkThis->m_pkServiceManager = pkServiceManager;
       ms_pkThis->m_bOwnsServiceManager = true;
   }
   else
   {
       ms_pkThis->m_pkServiceManager = pkServiceManager;
       ms_pkThis->m_bOwnsServiceManager = false;
   }

   // Register dependent services
   efd::IConfigManager* pkConfigManager = 
       pkServiceManager->GetSystemServiceAs<efd::IConfigManager>();
    if (!pkConfigManager)
    {
        NiString kConfigFilename;
        NiSceneGraphUpdate::GetConfigFilename(kConfigFilename);
        pkConfigManager = EE_NEW efd::ConfigManager(static_cast<const char*>(kConfigFilename),
                0, NULL, false);

        pkServiceManager->RegisterSystemService(
            efd::IConfigManager::CLASS_ID,
            pkConfigManager);
    }

#if defined(_WII)
    RegisterNetLib();
#endif

    efd::NetService* pkNetService = pkServiceManager->GetSystemServiceAs<efd::NetService>();
    if (!pkNetService)
    {
        pkServiceManager->RegisterSystemService(
            efd::NetService::CLASS_ID,
#if defined(_WII)
            EE_NEW efd::NetService("NetLibWiiUSB"));
#else
            EE_NEW efd::NetService);
#endif
    }

    efd::MessageService* pkMessageService = 
        pkServiceManager->GetSystemServiceAs<efd::MessageService>();
    if (!pkMessageService)
    {
       pkServiceManager->RegisterSystemService(
           efd::MessageService::CLASS_ID,
           EE_NEW efd::MessageService);
    }
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdate::UpdateServiceManager()
{
    if (m_bOwnsServiceManager && m_pkServiceManager)
    {
        // Run the first time
        m_pkServiceManager->RunOnce();
        m_pkServiceManager->RunOnce();

        // If we still need to process the messages update the services until we are done.
        if (m_uiProcessingMessageCount > 0)
        {
            NiSystemClockTimer kTimer;
            while (m_uiProcessingMessageCount > 0)
            {
                m_pkServiceManager->RunOnce();
                m_pkServiceManager->RunOnce();
                if (kTimer.GetElapsed() > 10.0f)
                {
                    m_uiProcessingMessageCount = 0;
                    return false;
                }
            }
        }

        return true;
    }

    return false;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdate::GetConfigFilename(NiString& kConfigFilename)
{
    kConfigFilename = "RemoteViewerNetwork.ini";

    // If Config.ini file exists in the working directory, use it.
    if (NiFile::Access(kConfigFilename, NiFile::READ_ONLY))
    {
        return true;
    }

    // If running on the PC, also check for .\sdk\Win32\ToolPlugins\Data\Config.ini
#if defined(WIN32)
    char acEnvValue[NI_MAX_PATH];
    size_t stEnvValueLength;
    NiGetenv(&stEnvValueLength, acEnvValue, sizeof(acEnvValue), "EMERGENT_PATH");
    if (stEnvValueLength > 0)
    {
        // Prepend path to the config filename
        NiString kConfigPath = acEnvValue;
        kConfigPath += "\\Media\\ToolPluginsData\\";
        kConfigFilename.Insert(kConfigPath, 0);

        if (NiFile::Access(kConfigFilename, NiFile::READ_ONLY))
        {
            return true;
        }
    }
#endif

    return false;
}
#if defined(WIN32)
//--------------------------------------------------------------------------------------------------
NiString NiSceneGraphUpdate::GetPrimaryIPAddress()
{
    NiString kPrimaryIPAddress;

    PIP_ADAPTER_INFO pAdapterInfo = NULL;

    // Get buffer size
    ULONG ulOutBufLen = 0;
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW)
    {
        // NiExternalFree(pAdapterInfo);
        pAdapterInfo = (IP_ADAPTER_INFO *) NiExternalMalloc(ulOutBufLen);
        if (pAdapterInfo == NULL) 
        {
            NILOG("Unable to allocate memory for enumerating network adapaters\n");
            return false;
        }
    }

    // The first adapter returned by this method is the "primary adapter", use it
    // if it has a valid IP address. If not use the next adapter with a valid address.
    DWORD dwRetVal = 0;
    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR)
    {
        PIP_ADAPTER_INFO pCurrentAdapter = pAdapterInfo;
        while (pCurrentAdapter)
        {
            kPrimaryIPAddress = pCurrentAdapter->IpAddressList.IpAddress.String;
            if (!kPrimaryIPAddress.IsEmpty() && kPrimaryIPAddress != "0.0.0.0")
            {
                NILOG("Using [%s] for RemoteViewer network communication.", 
                    pCurrentAdapter->Description);
                break;
            }
            pCurrentAdapter = pCurrentAdapter->Next;
        }
    }
    else 
    {
        NILOG("Unable to get network adapter info. GetAdaptersInfo() returned %d\n", dwRetVal);
    }
    if (pAdapterInfo)
    {
        NiExternalFree(pAdapterInfo);
    }

    return kPrimaryIPAddress;
}
#endif
//--------------------------------------------------------------------------------------------------
NiUInt16 NiSceneGraphUpdate::GetListenPort()
{
    NiUInt16 usPort = NiSceneGraphUpdate::GetInstance()->GetRemotePort();
    if (usPort == NiSceneGraphUpdate::NI_INVALID_PORT)
    {
        usPort = NiSceneGraphUpdate::NI_DEFAULT_PORT;
        NILOG("Unable to determine listen network port number for Gamebryo Viewer. "
            "Using default %d.", usPort);
    }

    return usPort;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdate::Connect(const char* pcChannelMgrHost, NiUInt16 usChannelMgrPort)
{
    NIASSERT(m_pkServiceManager);

    NiSceneGraphUpdateService* pkSceneGraphUpdateService = 
        m_pkServiceManager->GetSystemServiceAs<NiSceneGraphUpdateService>();
    if (pkSceneGraphUpdateService)
    {        
        // Tick the service twice so that the NetService::m_spNetLib is initialized prior to
        // calling connect.
        if (m_bOwnsServiceManager)
        {
            m_pkServiceManager->RunOnce();
            m_pkServiceManager->RunOnce();
        }

        pkSceneGraphUpdateService->Connect(pcChannelMgrHost, 
            usChannelMgrPort);
        return true;
    }

    return false;
}
//--------------------------------------------------------------------------------------------------
NiUInt16 NiSceneGraphUpdate::GetRemotePort()
{
    NIASSERT(m_pkServiceManager);

    NiUInt16 usPort = NI_INVALID_PORT;

    NiSceneGraphUpdateService* pkSceneGraphUpdateService = 
        m_pkServiceManager->GetSystemServiceAs<NiSceneGraphUpdateService>();
    if (pkSceneGraphUpdateService)
    {        
        usPort = pkSceneGraphUpdateService->GetRemotePort();
    }
    
    return usPort;
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdate::Shutdown()
{
    ms_pkThis->Stop();

#if defined(_WII)
    efd::USBSocket::ShutdownUSB();
#endif

    NiDelete ms_pkThis;
    ms_pkThis = NULL;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdate::AddObjects(const NiTMap<NiSceneGraphUpdateObjectId, 
    NiObjectPtr>& kAddObjects)
{
    // Initialize all the object in the scene
    NiAddObjects kAddObjectsStream(kAddObjects, this);

    return kAddObjectsStream.IsValid();
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdate::IsInsertNode(const NiNode* pkNode) const
{
    NiTMapIterator kIter = m_kInsertNodes.GetFirstPos();
    while (kIter)
    {
        NiSceneGraphUpdateObjectId kOldId;
        NiNodePtr spOldNode;
        m_kInsertNodes.GetNext(kIter, kOldId, spOldNode);
        if (pkNode == spOldNode)
            return true;
    }

    return false;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdate::IsInsertNode(const NiSceneGraphUpdateObjectId& kId) const
{
    NiNodePtr spNode;
    if (m_kInsertNodes.GetAt(kId, spNode))
        return true;

    return false;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdate::AddInsertNode(const NiSceneGraphUpdateObjectId& kId, NiNode* pkNode)
{
    if (!pkNode)
        return false;

    // Make sure we are inserting an object
    NiSceneGraphUpdateObjectPtr spUpdateObject = GetObject(kId);
    if (!spUpdateObject)
        return false;

    // Make sure we are inserting an object
    NiNodePtr spUpdateNode = NiDynamicCast(NiNode, spUpdateObject->GetObject());
    if (!spUpdateNode)
        return false;

    // Make sure we have not already inserted this object
    NiTMapIterator kIter = m_kInsertNodes.GetFirstPos();
    while (kIter)
    {
        NiSceneGraphUpdateObjectId kOldId;
        NiNodePtr spOldNode;
        m_kInsertNodes.GetNext(kIter, kOldId, spOldNode);
        if (kOldId == kId || spOldNode == pkNode)
            return false;
    }

    // Lets create a backup to place the current nodes children
    NiNode* pkNodeBackup = NiNew NiNode;
    NiUInt32 uiCount = pkNode->GetArrayCount();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            pkNodeBackup->AttachChild(pkChild);
        }
    }

    pkNode->RemoveAllChildren();
    pkNode->AttachChild(spUpdateNode);

    m_kInsertNodes.SetAt(kId, pkNode);
    m_kInsertNodesBackup.SetAt(kId, pkNodeBackup);

    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdate::RemoveInsertNode(const NiSceneGraphUpdateObjectId& kId)
{
    // Check to see if we are active object
    NiNodePtr spNode;
    if (m_kInsertNodes.GetAt(kId, spNode))
    {
        m_kInsertNodes.RemoveAt(kId);

        NiNodePtr spNodeBackup;
        if (m_kInsertNodesBackup.GetAt(kId, spNodeBackup))
        {
            m_kInsertNodesBackup.RemoveAt(kId);

            // Lets remove all children. This will cause the child that is owned by this library to
            // lose its parent.
            spNode->RemoveAllChildren();

            NiUInt32 uiCount = spNodeBackup->GetArrayCount();
            for (NiUInt32 ui = 0; ui < uiCount; ui++)
            {
                NiAVObject* pkChild = spNodeBackup->GetAt(ui);
                if (pkChild)
                {
                    spNode->AttachChild(pkChild);
                }
            }

            // Make sure our children are fixup correctly
            FixupChildren(GetRootNode());

            return true;
        }
    }

    return false;
}
//--------------------------------------------------------------------------------------------------
NiSceneGraphUpdateObjectId NiSceneGraphUpdate::GetNextObjectId()
{
    static NiUInt16 s_usObjectId = 0;
    NiSceneGraphUpdateObjectId kId((NiUInt8)GetInstanceId(), s_usObjectId);
    while (GetObject(kId))
    {
        kId.SetObjectId(s_usObjectId++);
    }
    s_usObjectId++;

    return kId;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdate::CanUpdateObject(const NiObject* pkObject) const
{
    if (pkObject->IsKindOf(&NiPSParticleSystem::ms_RTTI))
        return true;

    if (pkObject->IsKindOf(&NiNode::ms_RTTI))
        return true;

    if (pkObject->IsKindOf(&NiCamera::ms_RTTI))
        return true;

    // We have to do meshes to let mesh particles work.
    if (pkObject->IsKindOf(&NiMesh::ms_RTTI))
        return true;

    // We have to do lights so we maintain there memory footprint.
    if (pkObject->IsKindOf(&NiLight::ms_RTTI))
        return true;

    return false;
}
//--------------------------------------------------------------------------------------------------
NiSceneGraphUpdateObject* NiSceneGraphUpdate::AddObject(const NiSceneGraphUpdateObjectId& kId)
{
    NiSceneGraphUpdateObjectPtr spObject;
    m_kObjects.GetAt(kId, spObject);
    if (!spObject)
    {
        spObject = NiNew NiSceneGraphUpdateObject(kId);
        m_kObjects.SetAt(kId, spObject);
    }

    return spObject;
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdate::SetNiObject(const NiSceneGraphUpdateObjectId& kId, NiObject* pkObject)
{
    NiSceneGraphUpdateObjectPtr spObject;
    m_kObjects.GetAt(kId, spObject);
    NIASSERT(spObject);
    if (spObject && spObject->GetObject() != pkObject)
    {
        m_kNiObjects.RemoveAt(spObject->GetObject());
        spObject->SetObject(pkObject);
        m_kNiObjects.SetAt(pkObject, kId);
    }
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdate::RemoveObjectLocal(const NiSceneGraphUpdateObjectId& kId)
{
    // Remove it from the NiObject map
    NiSceneGraphUpdateObjectPtr spObject;
    m_kObjects.GetAt(kId, spObject);
    if (spObject)
    {
        NiObject* pkNiObject = spObject->GetObject();
        if (pkNiObject)
        {
            if (!m_kNiObjects.RemoveAt(pkNiObject))
            {
                NiTMapIterator kIter = m_kNiObjects.GetFirstPos();
                while (kIter)
                {
                    const NiObject* pkNiObject;
                    NiSceneGraphUpdateObjectId kOtherId;
                    m_kNiObjects.GetNext(kIter, pkNiObject, kOtherId);
                    if (kOtherId == kId)
                    {
                        m_kNiObjects.RemoveAt(pkNiObject);
                        break;
                    }
                }
            }

            // Detach the object from the parent
            NiAVObject* pkAVObject = NiDynamicCast(NiAVObject, pkNiObject);
            if (pkAVObject && pkAVObject->GetParent())
            {
                pkAVObject->GetParent()->DetachChild(pkAVObject);
            }

            // Try to remove this object from the replace queue if it exists. Ok to fail.
            RemoveInsertNode(kId);
        }
    }

    // Remove it from the root node map
    NiTMapIterator kIter = m_kRootNodes.GetFirstPos();
    while (kIter)
    {
        NiUInt32 uiInstanceId;
        NiSceneGraphUpdateObjectId kOtherId;
        m_kRootNodes.GetNext(kIter, uiInstanceId, kOtherId);
        if (kOtherId == kId)
        {
            m_kRootNodes.RemoveAt(uiInstanceId);
            break;
        }
    }

    // Remove it from the camera node map
    kIter = m_kActiveCameras.GetFirstPos();
    while (kIter)
    {
        NiUInt32 uiInstanceId;
        NiSceneGraphUpdateObjectId kOtherId;
        m_kActiveCameras.GetNext(kIter, uiInstanceId, kOtherId);
        if (kOtherId == kId)
        {
            m_kActiveCameras.RemoveAt(uiInstanceId);
            break;
        }
    }

    // Remove it from the main map
    m_kObjects.RemoveAt(kId);
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdate::AddObject(const NiSceneGraphUpdateObjectId kId, NiObject* pkNiObject)
{
    UpdateObject(kId, pkNiObject);
    return true;
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdate::UpdateObject(const NiSceneGraphUpdateObjectId& kId, NiObject* pkNiObject)
{
    NIASSERT(CanUpdateObject(pkNiObject));

    NiSceneGraphUpdateObject* pkObject = NiNew NiSceneGraphUpdateObject(kId);
    pkObject->SetObject(pkNiObject);

    // Lets check to see if we need to replace
    NiSceneGraphUpdateObjectPtr spCurrentObject;
    m_kObjects.GetAt(kId, spCurrentObject);
    if (spCurrentObject && spCurrentObject->GetObject() &&
        spCurrentObject->GetObject()->GetRTTI() != pkNiObject->GetRTTI())
    {
        // Replace the object because the RTTI is different
        Send(MESSAGE_REPLACE_OBJECT, kId, pkObject);

        // We have to update all the objects so that the pointer will be fixed up correctly.
        NiTMapIterator kIter = m_kObjects.GetFirstPos();
        while (kIter)
        {
            NiSceneGraphUpdateObjectId kOtherId;
            NiSceneGraphUpdateObjectPtr spOtherObject;
            m_kObjects.GetNext(kIter, kOtherId, spOtherObject);

            if (kOtherId != kId)
                Send(MESSAGE_UPDATE_OBJECT, kOtherId);
        }
    }
    else
    {
        Send(MESSAGE_UPDATE_OBJECT, kId, pkObject);
    }
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdate::RemoveObject(const NiSceneGraphUpdateObjectId& kId)
{
    Send(MESSAGE_REMOVE_OBJECT, kId);
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdate::UpdateAllObjects()
{
    NiTMapIterator kIter = m_kObjects.GetFirstPos();
    while (kIter)
    {
        NiSceneGraphUpdateObjectId kId;
        NiSceneGraphUpdateObjectPtr spObject;
        m_kObjects.GetNext(kIter, kId, spObject);

        Send(MESSAGE_UPDATE_OBJECT, kId);
    }

    NiSceneGraphUpdateObjectId kRootNodeId = GetRootNodeId();
    if (kRootNodeId != NiSceneGraphUpdateObjectId::NULL_OBJECT_ID)
        Send(MESSAGE_SET_ROOT_NODE, kRootNodeId);

    NiSceneGraphUpdateObjectId kActiveCameraId = GetActiveCameraId();
    if (kActiveCameraId != NiSceneGraphUpdateObjectId::NULL_OBJECT_ID)
        Send(MESSAGE_SET_ACTIVE_CAMERA, kActiveCameraId);

    // Send the messages now
    SendImmediate(MESSAGE_UPDATE_SETTINGS);
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdate::RemoveAllObjects()
{
    NiTMapIterator kIter = m_kObjects.GetFirstPos();
    while (kIter)
    {
        NiSceneGraphUpdateObjectId kId;
        NiSceneGraphUpdateObjectPtr spObject;
        m_kObjects.GetNext(kIter, kId, spObject);

        Send(MESSAGE_REMOVE_OBJECT, kId);
    }

    // Send the messages now
    SendImmediate(MESSAGE_UPDATE_SETTINGS);
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdate::PostHandleMessageCleanup()
{
    // Make sure we do not have any dangling ids hanging around
    NiTMapIterator kIter = m_kObjects.GetFirstPos();
    while (kIter)
    {
        NiSceneGraphUpdateObjectId kId;
        NiSceneGraphUpdateObjectPtr spObject;
        m_kObjects.GetNext(kIter, kId, spObject);

        // If this is the root node simply skip it. We need to keep it around.
        if (GetRootNodeId() == kId)
            continue;

        if (spObject)
        {
            NiObject* pkObject = spObject->GetObject();
            if (pkObject)
            {
                if (pkObject->GetRefCount() == 1)
                {
                    RemoveObject(kId);
                }
            }
        }
    }

    // Mark the scene clean
    Send(MESSAGE_SCENE_CLEAN);

    // If we are an inserted node the next stage will cover us
    if (!IsInsertNode(GetRootNodeId()))
    {
        // When changes are made to the scene graph make sure things are up to date
        NiNode* pkRootNode = NiSceneGraphUpdate::GetInstance()->GetRootNode();
        if (pkRootNode)
        {
            // Make sure all the children know who there parent is
            FixupChildren(pkRootNode);

            NiNode* pkRootNode = NiSceneGraphUpdate::GetInstance()->GetRootNode();
            if (pkRootNode)
            {
                pkRootNode->Update(NiGetCurrentTimeInSec());
                NiMesh::CompleteSceneModifiers(pkRootNode);
                pkRootNode->UpdateProperties();
                pkRootNode->UpdateEffects();
            }
        }
    }
    

    // Fixup all the children for the inserted objects. This needs to be done after the previous
    // fixup.
    kIter = m_kInsertNodes.GetFirstPos();
    while (kIter)
    {
        NiSceneGraphUpdateObjectId kId;
        NiNodePtr spInsertNode;
        m_kInsertNodes.GetNext(kIter, kId, spInsertNode);

        FixupChildren(spInsertNode);

        NiNode* pkRootNode = NiSceneGraphUpdate::GetInstance()->GetRootNode();
        if (pkRootNode)
        {
            pkRootNode->Update(NiGetCurrentTimeInSec());
            NiMesh::CompleteSceneModifiers(pkRootNode);
            pkRootNode->UpdateProperties();
            pkRootNode->UpdateEffects();
        }
    }
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdate::PostHandleUpdateObject()
{
    // Check to see if we need to do a replace with a preexisting object

    PostHandleMessageCleanup();
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdate::PostHandleRemoveObject()
{
    PostHandleMessageCleanup();
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdate::PostHandleUpdateSettings()
{
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdate::CleanupObjects()
{
    // When changes are made to the scene graph make sure things are up to date
    NiNode* pkRootNode = NiSceneGraphUpdate::GetInstance()->GetRootNode();
    if (pkRootNode)
    {
        pkRootNode->UpdateProperties();
        pkRootNode->UpdateEffects();
    }
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdate::PreHandleAddMessage()
{
    // When we first get notified that we are about to process messages set the flag
    m_uiProcessingMessageCount ++;
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdate::PostHandleRemoveMessage()
{
    // When we remove a message we need to tell the service that we have finished 
    // sending the message.
    NIASSERT(m_uiProcessingMessageCount > 0);
    m_uiProcessingMessageCount--;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdate::Start(NiUInt32 uiWaitForNetworkInitInSec /* = 0 */)
{
    m_bIsReady = false;

    NIASSERT(m_pkServiceManager);

    NiSceneGraphUpdateService* pkSceneGraphUpdateService = 
        m_pkServiceManager->GetSystemServiceAs<NiSceneGraphUpdateService>();
    if (pkSceneGraphUpdateService)
    {   
        NiSystemClockTimer kTimer;
        kTimer.Start();
        while (!pkSceneGraphUpdateService->IsConnectedToNetwork() && 
            kTimer.GetElapsed() < uiWaitForNetworkInitInSec)
        {   
            Update();
        }

        // We are ready if we successfully connected OR the caller does not want to wait
        // for the network connection.
        m_bIsReady = (pkSceneGraphUpdateService->IsConnectedToNetwork() || 
            (uiWaitForNetworkInitInSec == 0));
    }


    return m_bIsReady;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdate::Stop()
{
    // Update three last time before we stop this makes sure we sent our messages
    if (Update())
        if (Update())
            Update();

    m_bIsReady = false;
    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdate::Refresh()
{
    return true;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdate::Send(MessageType eType, const NiSceneGraphUpdateObjectId& kId,
    NiSceneGraphUpdateObject* pkObject)
{
    if (m_pkServiceManager)
    {
        NiSceneGraphUpdateService* pkSceneGraphUpdateService = 
            m_pkServiceManager->GetSystemServiceAs<NiSceneGraphUpdateService>();
        
        if (pkSceneGraphUpdateService)
        {
            // If we do not have a valid object try to find one
            if (!pkObject)
            {
                pkObject = GetObject(kId);
            }

            return pkSceneGraphUpdateService->Send(eType, kId, pkObject);
        }
    }

    return false;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdate::SendImmediate(MessageType eType, const NiSceneGraphUpdateObjectId& kId,
    NiSceneGraphUpdateObject* pkObject)
{
    if (Send(eType, kId, pkObject))
    {
        // Update services to send the message now
        return UpdateServiceManager();
    }

    // Did not send the message
    return false;
}
//--------------------------------------------------------------------------------------------------
const char* NiSceneGraphUpdate::GetStatusMsg() const
{
    if (m_pkServiceManager)
    {
        NiSceneGraphUpdateService* pkSceneGraphUpdateService = 
            m_pkServiceManager->GetSystemServiceAs<NiSceneGraphUpdateService>();
        if (pkSceneGraphUpdateService)
        {
            return pkSceneGraphUpdateService->GetStatusMsg();
        }
    }

    return "Unable to get service information";
}
//--------------------------------------------------------------------------------------------------
NiSceneGraphUpdate::NiAddObjects::NiAddObjects(const NiTMap<NiSceneGraphUpdateObjectId, 
    NiObjectPtr>& kAddObjects, NiSceneGraphUpdate* pkThis)
{
    m_bIsValid = false;

    // NiStream functions
    NiTMapIterator kIter = kAddObjects.GetFirstPos();
    while (kIter)
    {
        NiSceneGraphUpdateObjectId kId;
        NiObjectPtr spObject;
        kAddObjects.GetNext(kIter, kId, spObject);
        InsertObject(spObject);
    }

    // Register all the objects
    RegisterObjects();

    // Add each object to the scene graph update.
    NiUInt32 uiCount = m_kObjects.GetSize();
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        NiObject* pkObject = m_kObjects.GetAt(ui);

        // Make sure we can update this object
        if (!pkThis->CanUpdateObject(pkObject))
            continue;

        // Make sure we have not already added this object. This might happen if we added a node
        // that is the parent of an already existent particle system.
        if (pkThis->GetObjectId(pkObject) != NiSceneGraphUpdateObjectId::NULL_OBJECT_ID)
            continue;

        // Get the id if we passed it in.
        NiSceneGraphUpdateObjectId kNewId = NiSceneGraphUpdateObjectId::NULL_OBJECT_ID;
        NiTMapIterator kIter = kAddObjects.GetFirstPos();
        while (kIter)
        {
            NiSceneGraphUpdateObjectId kId;
            NiObjectPtr spObject;
            kAddObjects.GetNext(kIter, kId, spObject);

            if (spObject == pkObject)
            {
                kNewId = kId;
                break;
            }
        }

        // If we were not assigned an id make one.
        if (kNewId == NiSceneGraphUpdateObjectId::NULL_OBJECT_ID)
        {
            // Make sure we have not already been added
            kNewId = pkThis->GetNextObjectId();
        }

        // Add each object to the list if we have one maintain the 
        pkThis->AddObject(kNewId, pkObject);

        m_bIsValid = true;
    }

    m_kObjects.RemoveAll();
    m_kRegisterMap.RemoveAll();
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdate::SetupLogging(const char* pcLogName)
{
    if (!pcLogName)
    {
        NIASSERT(!"NiSceneGraphUpdate: Log name required");
        return;
    }

    efd::ILoggerPtr spLogger = efd::SystemLogger::CreateSystemLogger();
    if (!spLogger)
    {
        return;
    }
    efd::FileDestination::CreateAndOpen(spLogger, pcLogName);

#if defined(NISHIPPING) || defined(_WII)
    // Reduce the amount of log messages sent to stdout/stderr
    efd::GetLogger()->SetLogLevel(efd::kALL, efd::ILogger::kERR0);
#endif
}
//--------------------------------------------------------------------------------------------------

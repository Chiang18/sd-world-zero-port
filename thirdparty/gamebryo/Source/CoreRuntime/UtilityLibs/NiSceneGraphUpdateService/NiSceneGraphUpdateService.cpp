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

#include "NiSceneGraphUpdateService.h"
#include "NiSceneGraphUpdateMessage.h"
#include "NiSceneGraphUpdateObject.h"
#include "NiSceneGraphUpdate.h"

#include <efd/Ids.h>
#include <efd/ServiceManager.h>
#include <efd/MessageService.h>
#include <efdNetwork/NetService.h>

#include <NiMesh.h>

using namespace efd;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(NiSceneGraphUpdateService);

EE_HANDLER_SUBCLASS_WRAPPER(NiSceneGraphUpdateService, HandleStandardMessage, 
    IMessage, StreamMessage, kMSGID_NetServiceConnectionEstablished);

EE_HANDLER_SUBCLASS_WRAPPER(NiSceneGraphUpdateService, HandleStandardMessage, 
    IMessage, StreamMessage, kMSGID_NetServiceConnectionClosed);

EE_HANDLER(NiSceneGraphUpdateService, HandleMessage, NiSceneGraphUpdateMessage);


//--------------------------------------------------------------------------------------------------
NiSceneGraphUpdateService::NiSceneGraphUpdateService(const bool bViewerService)
{
    m_bViewerService = bViewerService;
    m_uiMessageIndex = 0;
    m_bConnectedToNetwork = false;
    m_bConnectedToEditor = false;
    m_spMessageService = NULL;
    memset(m_acStatusMsg, 0, sizeof(m_acStatusMsg));
#ifndef NISHIPPING
    memset(m_aacSentMessageStatus, 0, sizeof(m_aacSentMessageStatus));
    memset(m_aacReceivedMessageStatus, 0, sizeof(m_aacReceivedMessageStatus));
#endif
}
//--------------------------------------------------------------------------------------------------
NiSceneGraphUpdateService::~NiSceneGraphUpdateService()
{
    OnShutdown();
}
//--------------------------------------------------------------------------------------------------
efd::AsyncResult NiSceneGraphUpdateService::OnInit()
{
    m_spMessageService = m_pServiceManager->GetSystemServiceAs<MessageService>();
    if (!m_spMessageService)
        return efd::AsyncResult_Failure;

    m_spMessageService->RegisterFactoryMethod(NiSceneGraphUpdateMessage::CLASS_ID,
        NiSceneGraphUpdateMessage::FactoryMethod);

    m_spMessageService->Subscribe(this, efd::kCAT_LocalMessage);
    m_spMessageService->Subscribe(this, GetPrivateCategory(m_bViewerService));

    UpdateConnectionIPAddress();

    return efd::AsyncResult_Complete;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateService::Connect(const char* pcChannelMgrHost, NiUInt16 usChannelMgrPort)
{
    if (pcChannelMgrHost && pcChannelMgrHost[0] != '\0')
    {
        if (usChannelMgrPort == NiSceneGraphUpdate::NI_INVALID_PORT)
        {
            usChannelMgrPort = NiSceneGraphUpdate::NI_DEFAULT_PORT;
        }

        efd::NetService* pkNetService = m_pServiceManager->GetSystemServiceAs<efd::NetService>();
        if (pkNetService)
        {
            pkNetService->ConnectToChannelManager(efd::QOS_RELIABLE, pcChannelMgrHost, 
                usChannelMgrPort);
            NiStrcpy(m_acConnectionIPAddress, sizeof(m_acConnectionIPAddress), pcChannelMgrHost); 
            return true;
        }
    }

    return false;
}
//--------------------------------------------------------------------------------------------------
NiUInt16 NiSceneGraphUpdateService::GetRemotePort()
{
    NiUInt16 usPort = NiSceneGraphUpdate::NI_INVALID_PORT;

    efd::NetService* pkNetService = m_pServiceManager->GetSystemServiceAs<efd::NetService>();
    if (pkNetService)
    {
        usPort = pkNetService->GetChannelManagerConnectionID(efd::QOS_RELIABLE).GetRemotePort();
    }

    return usPort;
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdateService::UpdateConnectionIPAddress()
{
    efd::NetService* pkNetService = m_pServiceManager->GetSystemServiceAs<efd::NetService>();
    if (pkNetService && pkNetService->GetNetLib())
    {
        const efd::ConnectionID& kConnectionID = 
            pkNetService->GetChannelManagerConnectionID(efd::QOS_RELIABLE);
        NiSprintf(m_acConnectionIPAddress, sizeof(m_acConnectionIPAddress), "%s:%d", 
            pkNetService->GetNetLib()->IPToString(kConnectionID.GetIP()).c_str(),
            kConnectionID.GetRemotePort());
    }
}
//--------------------------------------------------------------------------------------------------
efd::AsyncResult NiSceneGraphUpdateService::OnTick()
{
    // Process messages
    // Translate input events to framework messages
    if (!m_spMessageService)
        return efd::AsyncResult_Failure;

    NiUInt32 uiMessageIndex = m_uiMessageIndex;
    m_uiMessageIndex = (m_uiMessageIndex == 1) ? 0 : 1;

    NiSceneGraphUpdateMessage* pkMessage = m_aspMessage[uiMessageIndex];

    if (pkMessage && pkMessage->IsActive())
    {
        // Make sure floodgate is done
        NiNode* pkRootNode = NiSceneGraphUpdate::GetInstance()->GetRootNode();
        if (pkRootNode)
            NiMesh::CompleteSceneModifiers(pkRootNode);

        // Do some initialization before we send the data
        pkMessage->PreSend();

        // Handle the message locally
        HandleMessage(pkMessage,efd::kCAT_LocalMessage);

        // Send it remotely
        m_spMessageService->SendRemote(pkMessage, GetPrivateCategory(!m_bViewerService));

        // Do some cleanup
        pkMessage->PostSend();
    }

    // Remove the message
    m_aspMessage[uiMessageIndex] = NULL;

    // Initialize the status message
    if (IsConnectedToNetwork())
    {
        if (IsConnectedToEditor())
        {
            NiStrcpy(m_acStatusMsg, sizeof(m_acStatusMsg), "Connected to Editor @ ");
            NiStrcat(m_acStatusMsg, sizeof(m_acStatusMsg), m_acConnectionIPAddress);
        }
        else
        {
            NiStrcpy(m_acStatusMsg, sizeof(m_acStatusMsg), "Connected to ");
            NiStrcat(m_acStatusMsg, sizeof(m_acStatusMsg), m_acConnectionIPAddress);
        }
    }       
    else
    {
        NiStrcpy(m_acStatusMsg, sizeof(m_acStatusMsg), "Trying to connect to ");
        NiStrcat(m_acStatusMsg, sizeof(m_acStatusMsg), m_acConnectionIPAddress);
    }

#ifndef NISHIPPING
    NiStrcat(m_acStatusMsg, sizeof(m_acStatusMsg), "\nSent Messages");

    for (NiUInt32 ui = 0; ui < 3; ui++)
    {
        NiStrcat(m_acStatusMsg, sizeof(m_acStatusMsg), m_aacSentMessageStatus[ui]);
    }

    NiStrcat(m_acStatusMsg, sizeof(m_acStatusMsg), "\nReceived Messages");

    for (NiUInt32 ui = 0; ui < 3; ui++)
    {
        NiStrcat(m_acStatusMsg, sizeof(m_acStatusMsg), m_aacReceivedMessageStatus[ui]);
    }
#endif // #ifndef NISHIPPING

    return efd::AsyncResult_Pending;
}
//--------------------------------------------------------------------------------------------------
efd::AsyncResult NiSceneGraphUpdateService::OnShutdown()
{
    if (!m_spMessageService)
        return efd::AsyncResult_Failure;

    m_spMessageService->Unsubscribe(this, efd::kCAT_LocalMessage);
    m_spMessageService->Unsubscribe(this, GetPrivateCategory(m_bViewerService));

    m_spMessageService = NULL;

    return efd::AsyncResult_Complete;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateService::CanSendMessage(NiSceneGraphUpdate::MessageType eMessage)
{
    // Check to see if we are a viewer (Viewers can only listen then can not update)
    // This prevents the viewer from sending update messages back to the editor.
    if (m_bViewerService)
    {
        switch (eMessage)
        {
        case NiSceneGraphUpdate::MESSAGE_VIEWER_ADDED:
        case NiSceneGraphUpdate::MESSAGE_VIEWER_REMOVED:
        case NiSceneGraphUpdate::MESSAGE_REQUEST_OBJECT:
        case NiSceneGraphUpdate::MESSAGE_UPDATE_SETTINGS:
            return true;
        // Not necessary, but prevents compiler warning.
        default:
            break;
        }
    }
    else
    {
        switch (eMessage)
        {
        case NiSceneGraphUpdate::MESSAGE_EDITOR_STARTED:
        case NiSceneGraphUpdate::MESSAGE_EDITOR_STOPPED:
        case NiSceneGraphUpdate::MESSAGE_SET_ROOT_NODE:
        case NiSceneGraphUpdate::MESSAGE_SET_ACTIVE_CAMERA:
        case NiSceneGraphUpdate::MESSAGE_UPDATE_OBJECT:
        case NiSceneGraphUpdate::MESSAGE_REPLACE_OBJECT:
        case NiSceneGraphUpdate::MESSAGE_REMOVE_OBJECT:
        case NiSceneGraphUpdate::MESSAGE_SCENE_DIRTY:
        case NiSceneGraphUpdate::MESSAGE_SCENE_CLEAN:
        case NiSceneGraphUpdate::MESSAGE_UPDATE_SETTINGS:
            return true;
        // Not necessary, but prevents compiler warning.
        default:
            break;
        }
    }

    return false;
}
//--------------------------------------------------------------------------------------------------
bool NiSceneGraphUpdateService::Send(NiSceneGraphUpdate::MessageType eMessage,
    const NiSceneGraphUpdateObjectId& kObjectId, NiSceneGraphUpdateObject* pkObject)
{
    if (CanSendMessage(eMessage))
    {
        // Allocate the message if we do not already have one
        if (!m_aspMessage[m_uiMessageIndex])
            m_aspMessage[m_uiMessageIndex] = EE_NEW NiSceneGraphUpdateMessage;

        m_aspMessage[m_uiMessageIndex]->AddObjectId(eMessage, kObjectId, pkObject);

#ifndef NISHIPPING
        NiStrcpy(m_aacSentMessageStatus[2], sizeof(m_aacSentMessageStatus[2]), 
            m_aacSentMessageStatus[1]);
        NiStrcpy(m_aacSentMessageStatus[1], sizeof(m_aacSentMessageStatus[1]), 
            m_aacSentMessageStatus[0]);

        float fTime = NiGetCurrentTimeInSec();
        switch (eMessage)
        {
        case NiSceneGraphUpdate::MESSAGE_VIEWER_ADDED:
            NiSprintf(m_aacSentMessageStatus[0], sizeof(m_aacSentMessageStatus[0]), 
                "\nSent %f: Viewer added message.", fTime);
            break;
        case NiSceneGraphUpdate::MESSAGE_VIEWER_REMOVED:
            NiSprintf(m_aacSentMessageStatus[0], sizeof(m_aacSentMessageStatus[0]), 
                "\nSent %f: Viewer removed message.", fTime);
            break;
        case NiSceneGraphUpdate::MESSAGE_EDITOR_STARTED:
            NiSprintf(m_aacSentMessageStatus[0], sizeof(m_aacSentMessageStatus[0]), 
                "\nSent %f: Editor started message.", fTime);
            break;
        case NiSceneGraphUpdate::MESSAGE_EDITOR_STOPPED:
            NiSprintf(m_aacSentMessageStatus[0], sizeof(m_aacSentMessageStatus[0]), 
                "\nSent %f: Editor stopped message.", fTime);
            break;
        case NiSceneGraphUpdate::MESSAGE_SET_ROOT_NODE:
            NiSprintf(m_aacSentMessageStatus[0], sizeof(m_aacSentMessageStatus[0]),
                "\nSent %f: Set root node %d message.", fTime, kObjectId.GetObjectId());
            break;
        case NiSceneGraphUpdate::MESSAGE_SET_ACTIVE_CAMERA:
            NiSprintf(m_aacSentMessageStatus[0], sizeof(m_aacSentMessageStatus[0]),
                "\nSent %f: Set active camera %d message.", fTime, kObjectId.GetObjectId());
            break;
        case NiSceneGraphUpdate::MESSAGE_UPDATE_OBJECT:
            NiSprintf(m_aacSentMessageStatus[0], sizeof(m_aacSentMessageStatus[0]),
                "\nSent %f: Update object %d message.", fTime, kObjectId.GetObjectId());
            break;
        case NiSceneGraphUpdate::MESSAGE_REPLACE_OBJECT:
            NiSprintf(m_aacSentMessageStatus[0], sizeof(m_aacSentMessageStatus[0]),
                "\nSent %f: Replace object %d message.", fTime, kObjectId.GetObjectId());
            break;
        case NiSceneGraphUpdate::MESSAGE_REMOVE_OBJECT:
            NiSprintf(m_aacSentMessageStatus[0], sizeof(m_aacSentMessageStatus[0]),
                "\nSent %f: Remove object %d message.", fTime, kObjectId.GetObjectId());
            break;
        case NiSceneGraphUpdate::MESSAGE_REQUEST_OBJECT:
            NiSprintf(m_aacSentMessageStatus[0], sizeof(m_aacSentMessageStatus[0]),
                "\nSent %f: Request object %d message.", fTime, kObjectId.GetObjectId());
            break;
        case NiSceneGraphUpdate::MESSAGE_SCENE_DIRTY:
            NiSprintf(m_aacSentMessageStatus[0], sizeof(m_aacSentMessageStatus[0]),
                "\nSent %f: Dirty scene message.", fTime);
            break;
        case NiSceneGraphUpdate::MESSAGE_SCENE_CLEAN:
            NiSprintf(m_aacSentMessageStatus[0], sizeof(m_aacSentMessageStatus[0]),
                "\nSent %f: Clean scene message.", fTime);
            break;
        case NiSceneGraphUpdate::MESSAGE_UPDATE_SETTINGS:
            NiSprintf(m_aacSentMessageStatus[0], sizeof(m_aacSentMessageStatus[0]),
                "\nSent %f: Animation range message.", fTime);
            break;
        default:
            NiSprintf(m_aacSentMessageStatus[0], sizeof(m_aacSentMessageStatus[0]),
                "\nSent %f: Unknown or invalid message. MessageType=%d", eMessage);
            break;
        }

#endif // #ifndef NISHIPPING

        return true;
    }

    return false;
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdateService::HandleStandardMessage(const efd::IMessage* pkMessage,
    efd::Category )
{
    NIASSERT(pkMessage);
    switch(pkMessage->GetClassID())
    {
    case kMSGID_NetServiceConnectionEstablished:
        {
            m_bConnectedToNetwork = true;
            UpdateConnectionIPAddress();

            if (m_bViewerService)
            {
                if (!m_bConnectedToEditor)
                {
                    NiSceneGraphUpdate::GetInstance()->Send(
                        NiSceneGraphUpdate::MESSAGE_VIEWER_ADDED);
                }
            }
            else
            {
                NiSceneGraphUpdate::GetInstance()->Send(
                    NiSceneGraphUpdate::MESSAGE_EDITOR_STARTED);
            }
        }
        break;
    case kMSGID_NetServiceConnectionClosed:
        {         
            m_bConnectedToNetwork = false;

            if (m_bViewerService)
            {
                // Dropped network connection, we are no longer connected to an editor
                m_bConnectedToEditor = false; 
            }
            else
            {
                NILOG("NiSceneGraphUpdateService: WARNING - Dropped connection to"
                    "Channel Manager\n");
            }
        }
        break;
    }
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdateService::HandleMessage(const NiSceneGraphUpdateMessage* pkMessage,
    efd::Category )
{
    if (!m_bViewerService && pkMessage->IsViewerAddedMsg())
    {
#ifndef NISHIPPING
        NiStrcpy(m_aacReceivedMessageStatus[2], sizeof(m_aacReceivedMessageStatus[2]), 
            m_aacReceivedMessageStatus[1]);
        NiStrcpy(m_aacReceivedMessageStatus[1], sizeof(m_aacReceivedMessageStatus[1]), 
            m_aacReceivedMessageStatus[0]);
        NiSprintf(m_aacReceivedMessageStatus[0], sizeof(m_aacReceivedMessageStatus[0]), 
            "\nReceived: Viewer added message.");
#endif

        NiSceneGraphUpdate::GetInstance()->Send(NiSceneGraphUpdate::MESSAGE_EDITOR_STARTED);
        NiSceneGraphUpdate::GetInstance()->UpdateAllObjects();
    }
    if (!m_bViewerService && pkMessage->IsViewerRemovedMsg())
    {
#ifndef NISHIPPING
        NiStrcpy(m_aacReceivedMessageStatus[2], sizeof(m_aacReceivedMessageStatus[2]), 
            m_aacReceivedMessageStatus[1]);
        NiStrcpy(m_aacReceivedMessageStatus[1], sizeof(m_aacReceivedMessageStatus[1]), 
            m_aacReceivedMessageStatus[0]);
        NiSprintf(m_aacReceivedMessageStatus[0], sizeof(m_aacReceivedMessageStatus[0]), 
            "\nReceived: Viewer removed message.");
#endif
    }

    if (!m_bViewerService && pkMessage->IsRequestObjectMsg())
    {
#ifndef NISHIPPING
        NiStrcpy(m_aacReceivedMessageStatus[2], sizeof(m_aacReceivedMessageStatus[2]), 
            m_aacReceivedMessageStatus[1]);
        NiStrcpy(m_aacReceivedMessageStatus[1], sizeof(m_aacReceivedMessageStatus[1]), 
            m_aacReceivedMessageStatus[0]);
        NiSprintf(m_aacReceivedMessageStatus[0], sizeof(m_aacReceivedMessageStatus[0]), 
            "\nReceived: Viewer request object message.");
#endif
        pkMessage->HandleRequestObject();
    }

    if (m_bViewerService && pkMessage->IsEditorStartedMsg())
    {
#ifndef NISHIPPING
        NiStrcpy(m_aacReceivedMessageStatus[2], sizeof(m_aacReceivedMessageStatus[2]), 
            m_aacReceivedMessageStatus[1]);
        NiStrcpy(m_aacReceivedMessageStatus[1], sizeof(m_aacReceivedMessageStatus[1]), 
            m_aacReceivedMessageStatus[0]);
        NiSprintf(m_aacReceivedMessageStatus[0], sizeof(m_aacReceivedMessageStatus[0]),
            "\nReceived: Editor started message.");
#endif
        m_bConnectedToEditor = true;
    }
    if (m_bViewerService && pkMessage->IsEditorStoppedMsg())
    {
#ifndef NISHIPPING
        NiStrcpy(m_aacReceivedMessageStatus[2], sizeof(m_aacReceivedMessageStatus[2]), 
            m_aacReceivedMessageStatus[1]);
        NiStrcpy(m_aacReceivedMessageStatus[1], sizeof(m_aacReceivedMessageStatus[1]), 
            m_aacReceivedMessageStatus[0]);
        NiSprintf(m_aacReceivedMessageStatus[0], sizeof(m_aacReceivedMessageStatus[0]), 
            "\nReceived: Editor stopped message.");
#endif
        m_bConnectedToEditor = false;
    }

    if (pkMessage->IsSceneDirtyMsg())
    {
#ifndef NISHIPPING
        NiStrcpy(m_aacReceivedMessageStatus[2], sizeof(m_aacReceivedMessageStatus[2]), 
            m_aacReceivedMessageStatus[1]);
        NiStrcpy(m_aacReceivedMessageStatus[1], sizeof(m_aacReceivedMessageStatus[1]), 
            m_aacReceivedMessageStatus[0]);
        NiSprintf(m_aacReceivedMessageStatus[0], sizeof(m_aacReceivedMessageStatus[0]), 
            "\nReceived: Dirty scene message.");
#endif
        pkMessage->HandleSceneDirty();
    }

    if (pkMessage->IsSceneCleanMsg())
    {
#ifndef NISHIPPING
        NiStrcpy(m_aacReceivedMessageStatus[2], sizeof(m_aacReceivedMessageStatus[2]), 
            m_aacReceivedMessageStatus[1]);
        NiStrcpy(m_aacReceivedMessageStatus[1], sizeof(m_aacReceivedMessageStatus[1]), 
            m_aacReceivedMessageStatus[0]);
        NiSprintf(m_aacReceivedMessageStatus[0], sizeof(m_aacReceivedMessageStatus[0]), 
            "\nReceived: Clean scene message.");
#endif
        pkMessage->HandleSceneClean();
    }

    if (pkMessage->IsUpdateSettingsMsg())
    {
#ifndef NISHIPPING
        NiStrcpy(m_aacReceivedMessageStatus[2], sizeof(m_aacReceivedMessageStatus[2]), 
            m_aacReceivedMessageStatus[1]);
        NiStrcpy(m_aacReceivedMessageStatus[1], sizeof(m_aacReceivedMessageStatus[1]), 
            m_aacReceivedMessageStatus[0]);
        NiSprintf(m_aacReceivedMessageStatus[0], sizeof(m_aacReceivedMessageStatus[0]), 
            "\nReceived: Set animation range message.");
#endif
        pkMessage->HandleUpdateSettings();
    }

    if (pkMessage->IsUpdateObjectMsg())
    {
#ifndef NISHIPPING
        NiStrcpy(m_aacReceivedMessageStatus[2], sizeof(m_aacReceivedMessageStatus[2]), 
            m_aacReceivedMessageStatus[1]);
        NiStrcpy(m_aacReceivedMessageStatus[1], sizeof(m_aacReceivedMessageStatus[1]), 
            m_aacReceivedMessageStatus[0]);
        NiSprintf(m_aacReceivedMessageStatus[0], sizeof(m_aacReceivedMessageStatus[0]), 
            "\nReceived: Update object message.");
#endif
        pkMessage->HandleUpdateObject();
    }

    if (pkMessage->IsSetRootNodeMsg())
    {
#ifndef NISHIPPING
        NiStrcpy(m_aacReceivedMessageStatus[2], sizeof(m_aacReceivedMessageStatus[2]), 
            m_aacReceivedMessageStatus[1]);
        NiStrcpy(m_aacReceivedMessageStatus[1], sizeof(m_aacReceivedMessageStatus[1]), 
            m_aacReceivedMessageStatus[0]);
        NiSprintf(m_aacReceivedMessageStatus[0], sizeof(m_aacReceivedMessageStatus[0]), 
            "\nReceived: Set root node message.");
#endif
        pkMessage->HandleSetRootNode();
    }

    if (pkMessage->IsSetActiveCameraMsg())
    {
#ifndef NISHIPPING
        NiStrcpy(m_aacReceivedMessageStatus[2], sizeof(m_aacReceivedMessageStatus[2]), 
            m_aacReceivedMessageStatus[1]);
        NiStrcpy(m_aacReceivedMessageStatus[1], sizeof(m_aacReceivedMessageStatus[1]), 
            m_aacReceivedMessageStatus[0]);
        NiSprintf(m_aacReceivedMessageStatus[0], sizeof(m_aacReceivedMessageStatus[0]), 
            "\nReceived: Set active camera message.");
#endif
        pkMessage->HandleSetActiveCamera();
    }

    if (pkMessage->IsRemoveObjectMsg())
    {
#ifndef NISHIPPING
        NiStrcpy(m_aacReceivedMessageStatus[2], sizeof(m_aacReceivedMessageStatus[2]), 
            m_aacReceivedMessageStatus[1]);
        NiStrcpy(m_aacReceivedMessageStatus[1], sizeof(m_aacReceivedMessageStatus[1]), 
            m_aacReceivedMessageStatus[0]);
        NiSprintf(m_aacReceivedMessageStatus[0], sizeof(m_aacReceivedMessageStatus[0]), 
            "\nReceived: Remove object message.");
#endif
        pkMessage->HandleRemoveObject();
    }
}
//--------------------------------------------------------------------------------------------------

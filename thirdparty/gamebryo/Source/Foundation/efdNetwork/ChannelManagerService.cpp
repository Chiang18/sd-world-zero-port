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

#include "efdNetworkPCH.h"

#include <efd/IConfigManager.h>
#include <efd/MessageService.h>
#include <efd/StreamMessage.h>
#include <efd/ConfigManager.h>
#include <efd/IDs.h>
#include <efd/ServiceManager.h>

#include <efdNetwork/INetLib.h>
#include <efdNetwork/NetService.h>
#include <efdNetwork/NetCategory.h>
#include <efdNetwork/NameResolutionService.h>
#include <efdNetwork/ChannelManagerService.h>


#include <efdNetwork/NetLibTCP.h>

using namespace efd;

namespace efd
{
//-------------------------------------------------------------------------------------------------
// Hidden internal callback class
class ConnectionCallback : public efd::MemObject, public INetCallback
{
    EE_DECLARE_CONCRETE_REFCOUNT

public:
    ConnectionCallback(ChannelManagerService* pChannelManagerService)
        : m_pChannelManagerService(pChannelManagerService)
    {
    }

    virtual void HandleNetMessage(
          const IMessage* pIncomingMessage,
          const ConnectionID& i_source)
    {
        switch (pIncomingMessage->GetClassID())
        {
            case kMSGID_ConnectionAcceptedMsg:
                EE_LOG(efd::kChannelManager, ILogger::kLVL1,
                    ("accepted %s", i_source.ToString().c_str()));
                //printf("ChannelManager accepted %s\n", i_source.ToString().c_str());
                m_pChannelManagerService->AddConnection(i_source);
                break;

            case kMSGID_ConnectionConnectedMsg:
                EE_LOG(efd::kChannelManager, ILogger::kLVL1,
                    ("connected %s", i_source.ToString().c_str()));
                m_pChannelManagerService->AddConnection(i_source);
                break;

            case kMSGID_ConnectionDisconnectedMsg:
                EE_LOG(efd::kChannelManager, ILogger::kLVL1,
                    ("disconnected %s", i_source.ToString().c_str()));
                //printf("ChannelManager disconnected %s\n", i_source.ToString().c_str());
                m_pChannelManagerService->RemoveConnection(i_source);
                break;

            case kMSGID_ConnectionDataReceivedMsg:
            {
                const EnvelopeMessage* pEnvelopeMessage =
                    EE_DYNAMIC_CAST(EnvelopeMessage, pIncomingMessage);
                EE_ASSERT(pEnvelopeMessage->GetSenderConnection() == i_source);
                m_pChannelManagerService->HandleMessage(
                    const_cast<EnvelopeMessage*>(pEnvelopeMessage));
            } break;

            case kMSGID_ConnectionFailedToAcceptMsg:
                EE_LOG(efd::kChannelManager, ILogger::kLVL1,
                    ("Failed To Accept %s", i_source.ToString().c_str()));
                break;
            default:
                EE_LOG(efd::kChannelManager, ILogger::kLVL1,
                    ("unhandled message type 0x%08X from %s",
                    pIncomingMessage->GetClassID(), i_source.ToString().c_str()));
                break;
        }
    }

protected:
    ChannelManagerService* m_pChannelManagerService;
};

} // end namespace efd



EE_IMPLEMENT_CONCRETE_CLASS_INFO(ChannelManagerService);

const /*static*/ char* ChannelManagerService::kConfigSection = "ChannelManager";
const /*static*/ char* ChannelManagerService::kListenIP = "ListenIP";
const /*static*/ char* ChannelManagerService::kListenPort = "ListenPort";
const /*static*/ char* ChannelManagerService::kHostname = "Hostname";
const /*static*/ char* ChannelManagerService::kPort = "Port";
const /*static*/ char* ChannelManagerService::kQualityOfService = "QualityOfService";
const /*static*/ char* ChannelManagerService::kQOS_Reliable = "QOS_RELIABLE";
const /*static*/ char* ChannelManagerService::kQOS_Unreliable = "QOS_UNRELIABLE";
const /*static*/ char* ChannelManagerService::kQOS_Connectionless = "QOS_CONNECTIONLESS";
const /*static*/ char* ChannelManagerService::kQOS_Invalid = "QOS_INVALID";

const /*static*/ efd::UInt16 ChannelManagerService::defaultListenPort = 13215;
static const char* defaultHostname = "127.0.0.1";
static const efd::QualityOfService defaultQualityOfService = QOS_RELIABLE;

//0 is kNetID_Invalid
static const efd::UInt32 ServerStartNetID = kNetID_Unassigned+1;
static const efd::UInt32 ClientStartNetID = kNetID_Client+1;

//-------------------------------------------------------------------------------------------------
efd::QualityOfService ChannelManagerService::StringToQualityOfService(efd::utf8string stringName)
{
    if (stringName == kQOS_Reliable)
        return QOS_RELIABLE;
    if (stringName == kQOS_Unreliable)
        return QOS_UNRELIABLE;
    if (stringName == kQOS_Connectionless)
        return QOS_CONNECTIONLESS;
    return QOS_INVALID;
}
//-------------------------------------------------------------------------------------------------
efd::utf8string ChannelManagerService::QualityOfServiceToString(efd::QualityOfService qos)
{
    if (qos == QOS_RELIABLE)
        return kQOS_Reliable;
    if (qos == QOS_UNRELIABLE)
        return kQOS_Unreliable;
    if (qos == QOS_CONNECTIONLESS)
        return kQOS_Connectionless;
    return kQOS_Invalid;
}
//-------------------------------------------------------------------------------------------------
ChannelManagerConfigListPtr ChannelManagerService::ReadConfig(IConfigManager* pConfigManager)
{
    ChannelManagerConfigListPtr spConfigList = EE_NEW ChannelManagerConfigList;
    // we need a configmanager to continue
    efd::UInt32 channelManagerIndex = 1;
    const ISection* pSection = NULL;
    if (pConfigManager)
    {
        do 
        {
            efd::utf8string channelManagerSection(
                Formatted, 
                "%s%d",
                ChannelManagerService::kConfigSection,
                channelManagerIndex);
            pSection = pConfigManager->GetConfiguration()->FindSection(
                channelManagerSection.c_str());
            if (pSection)
            {
                ChannelManagerConfigPtr spConfig = EE_NEW ChannelManagerConfig;
                // set the default values in case they are missing in the config file
                spConfig->m_port = defaultListenPort;
                spConfig->m_hostname = defaultHostname;
                spConfig->m_index = channelManagerIndex;
                spConfig->m_qualityOfService = defaultQualityOfService;

                // Find the ChannelManager parameters, overriding the default values
                efd::utf8string temp = pSection->FindValue(ChannelManagerService::kPort);
                if (!temp.empty())
                {
                    spConfig->m_port = (efd::UInt16)atoi(temp.c_str());
                }

                temp = pSection->FindValue(ChannelManagerService::kHostname);
                if (!temp.empty())
                {
                    spConfig->m_hostname = temp;
                }

                temp = pSection->FindValue(ChannelManagerService::kQualityOfService);
                if (!temp.empty())
                {
                    spConfig->m_qualityOfService = StringToQualityOfService(temp);
                    if (spConfig->m_qualityOfService == QOS_INVALID)
                    {
                        EE_FAIL_MESSAGE(("%s> '%s' is not a valid QulaityOfService!!!!",
                            __FUNCTION__, temp.c_str()));
                        return NULL;
                    }
                }
                spConfigList->push_back(spConfig);
            }
            ++channelManagerIndex;
        } while (pSection);
    }

    if (!spConfigList->size())
    {
        ChannelManagerConfigPtr spConfig = EE_NEW ChannelManagerConfig;
        // set reasonable defaults if no config is found
        spConfig->m_port = defaultListenPort;
        spConfig->m_hostname = defaultHostname;
        spConfig->m_index = 0;
        spConfig->m_qualityOfService = defaultQualityOfService;
        if (pConfigManager)
        {
            // try to read old style config...
            const ISection *pCMSection = pConfigManager->GetConfiguration()->FindSection(
                ChannelManagerService::kConfigSection);
            if (pCMSection)
            {
                EE_LOG(
                    efd::kChannelManager,
                    ILogger::kLVL1,
                    ("%s> Unable to find New ChannelManager section in Config file, using old",
                    __FUNCTION__));

                // Find the login parameters
                efd::utf8string temp = pCMSection->FindValue(ChannelManagerService::kListenPort);
                if (!temp.empty())
                {
                    spConfig->m_port = (efd::UInt16)atoi(temp.c_str());
                }

                temp = pCMSection->FindValue(ChannelManagerService::kListenIP);
                if (!temp.empty())
                {
                    spConfig->m_hostname = temp;
                }
            }
        }
        spConfigList->push_back(spConfig);
    }
    return spConfigList;
}

//-------------------------------------------------------------------------------------------------
ChannelManagerService::ChannelManagerService() :
    m_spNetLib(NULL),
    m_pConnectionCallback(NULL),
    m_listening(false),
    m_curNetID(ServerStartNetID),
    m_curClientNetID(ClientStartNetID)
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 5800;

    m_pConnectionCallback = EE_NEW ConnectionCallback(this);

    INetLib::RegisterNetLib<NetLibTCP>(NetService::kNetLibTCP);

    m_netLibType = NetService::kNetLibTCP;
}


//-------------------------------------------------------------------------------------------------
/*virtual*/ ChannelManagerService::~ChannelManagerService()
{
    // m_spNetLib is a smartpointer now
    m_spNetLib = NULL;

    EE_DELETE m_pConnectionCallback;
    m_pConnectionCallback = NULL;
}


//-------------------------------------------------------------------------------------------------
SyncResult ChannelManagerService::OnPreInit()
{
    IConfigManager* pConfigManager = m_pServiceManager->GetSystemServiceAs<IConfigManager>();
    m_spMessageService = m_pServiceManager->GetSystemServiceAs<MessageService>();
    EE_ASSERT(m_spMessageService);
    if (pConfigManager)
    {
        const ISection *pSection = pConfigManager->GetConfiguration()->FindSection(
            NetService::kConfigSection);

        if (pSection)
        {
            efd::utf8string netLibType = pSection->FindValue(NetService::kNetLibType);
            if (!netLibType.empty())
            {
                m_netLibType = netLibType;
            }
        }
    }
    m_spNetLib = INetLib::CreateNetLib(m_netLibType, m_spMessageService);

    // set the NetID for the INetLib ChannelManager owns
    m_spNetLib->SetNetID(AssignNetID());

    RegisterMessageWrapperFactory< NetMessage, kMSGID_Resubscribe >(m_spMessageService);
    RegisterMessageWrapperFactory< IMessage, kMSGID_ResubscribeComplete >(m_spMessageService);
    RegisterMessageWrapperFactory< RequestNetIDMessage, kMSGID_RequestNetID >(m_spMessageService);
    RegisterMessageWrapperFactory< AnnounceNetIDMessage, kMSGID_AnnounceNetID >(
        m_spMessageService);
    // used by ConnectionTCP to send version info.  Cannot be registered by ConnectionTCP until 
    // MessageFactory refactor is complete
    RegisterMessageWrapperFactory< StreamMessage, kMSGID_UnreliableVersion >(m_spMessageService);

    RegisterMessageWrapperFactory< StreamMessage, kMSGID_SubscribeExternal >(m_spMessageService);
    RegisterMessageWrapperFactory< StreamMessage, kMSGID_UnsubscribeExternal >(m_spMessageService);
    RegisterMessageWrapperFactory< StreamMessage, kMSGID_BeginCategoryProduction >(
        m_spMessageService);
    RegisterMessageWrapperFactory< StreamMessage, kMSGID_EndCategoryProduction >(
        m_spMessageService);
    RegisterMessageWrapperFactory< EnvelopeMessage, kMSGID_SendToProducer >(m_spMessageService);
    RegisterMessageWrapperFactory< NetMessage, kMSGID_Resubscribe >(m_spMessageService);
    RegisterMessageWrapperFactory< IMessage, kMSGID_ResubscribeComplete >(m_spMessageService);

    return efd::SyncResult_Success;
}


//-------------------------------------------------------------------------------------------------
/*virtual*/ AsyncResult ChannelManagerService::OnInit()
{
    // initialize INetLib
    if (INetLib::StartNet() <= 0)
    {
        return efd::AsyncResult_Failure;
    }

    // we want to receive system messages
    m_spNetLib->AddLocalConsumer(kCAT_NetSystem, QOS_RELIABLE, m_pConnectionCallback);
    m_spNetLib->BeginCategoryProduction(kCAT_NetSystem, QOS_RELIABLE);

    IConfigManager* pConfigManager = m_pServiceManager->GetSystemServiceAs<IConfigManager>();
    m_spConfigList = ChannelManagerService::ReadConfig(pConfigManager);
    EE_ASSERT_MESSAGE(m_spConfigList, ("%s> Failed to read ChannelManager config information!",
        __FUNCTION__));

    EE_LOG(efd::kChannelManager, ILogger::kLVL0, ("%s> Complete. using NetID=%d",
        __FUNCTION__, m_spNetLib->GetNetID()));
    return efd::AsyncResult_Complete;
}


//-------------------------------------------------------------------------------------------------
/*virtual*/ AsyncResult ChannelManagerService::OnTick()
{
    m_spNetLib->Tick();

    // check for pending subscriptions
    ChannelSubscriptionList::iterator it = m_pendingSubscriptions.begin();
    while(it != m_pendingSubscriptions.end())
    {
        ChannelSubscription channelSubscription = *it;
        ConnectionID cidToSubscribe;
        NetIDQosPair netIDQosPair(
            channelSubscription.m_netID,
            channelSubscription.m_qualityOfService);
        if (m_NetIDQosLookupMap.find(netIDQosPair, cidToSubscribe))
        {
            efd::Category catID = channelSubscription.m_category;
            efd::QualityOfService qualityOfService = channelSubscription.m_qualityOfService;
            if (channelSubscription.m_subscriptionType == kMSGID_SubscribeExternal)
            {
                m_spNetLib->AddRemoteConsumer(catID, cidToSubscribe);
                m_spNetLib->AddLocalConsumer(catID, qualityOfService, m_pConnectionCallback);
                m_spNetLib->SendAddLocalConsumerRequest(catID, qualityOfService);
            }
            else
            {
                m_spNetLib->RemoveRemoteConsumer(catID, cidToSubscribe);
                m_spNetLib->RemoveLocalConsumer(catID, m_pConnectionCallback);
                m_spNetLib->SendRemoveLocalConsumerRequest(catID, qualityOfService);
            }
            ChannelSubscriptionList::iterator eraseIt = it;
            ++it;
            m_pendingSubscriptions.erase(eraseIt);
        }
        else
        {
            ++it;
        }
    }

    // we have no processing to do.  Make sure the listen call completed though
    if(!m_listening)
    {
        m_listening = true;
        for(ChannelManagerConfigList::iterator configIt = m_spConfigList->begin();
            configIt != m_spConfigList->end();
            ++configIt)
        {
            ChannelManagerConfigPtr spConfig = *configIt;
            spConfig->m_connectionID = m_spNetLib->Listen(
                spConfig->m_port,
                spConfig->m_qualityOfService,
                m_pConnectionCallback);

            if (spConfig->m_connectionID == kCID_INVALID)
            {
                EE_LOG(efd::kChannelManager, efd::ILogger::kLVL1,
                    ("%s> Listening for QOS %d connections on port %d failed",
                    __FUNCTION__,
                    spConfig->m_qualityOfService, spConfig->m_port));
                m_listening = false;
            }
            else
            {
                // listen for connections
                EE_LOG(efd::kChannelManager, efd::ILogger::kLVL2,
                    ("%s> Listening for QOS %d connections on port %d",
                    __FUNCTION__,
                    spConfig->m_qualityOfService,
                    spConfig->m_port));
                // Listen for connections and get all connection related callbacks.
                // NetService/INetLib will not get any connection related callbacks,
                // so we are responsible for forwarding
                // any notifications to the local message service
            }
        }
    }

    return efd::AsyncResult_Pending;
}


//-------------------------------------------------------------------------------------------------
/*virtual*/ AsyncResult ChannelManagerService::OnShutdown()
{
    m_spNetLib->Shutdown();
    INetLib::StopNet();

    return efd::AsyncResult_Complete;
}


//-------------------------------------------------------------------------------------------------
void ChannelManagerService::HandleMessage(const efd::EnvelopeMessage* pEnvelopeMessage)
{
    efd::ClassID msgClassID = pEnvelopeMessage->GetContentsClassID();
    ConnectionID senderConnectionID = pEnvelopeMessage->GetSenderConnection();

    switch (msgClassID)
    {
    case kMSGID_SubscribeExternal:
    case kMSGID_UnsubscribeExternal:
        {
            // we only accept subscriptions over the reliable QOS
            if (senderConnectionID.GetQualityOfService() != QOS_RELIABLE)
                break;

            const IMessage* pMessage = pEnvelopeMessage->GetContents(m_spMessageService);
            EE_ASSERT(pMessage);
            const StreamMessage* pSystemMessage = EE_DYNAMIC_CAST(const StreamMessage, pMessage);
            EE_ASSERT(pSystemMessage);
            pSystemMessage->Reset();

            UInt8 numCats = 0;
            *pSystemMessage >> numCats;
            Category catID;
            UInt32 netID = pEnvelopeMessage->GetSenderNetID();

            for (UInt8 curCat = 0; curCat < numCats; curCat++)
            {
                SInt32 qosTemp;
                *pSystemMessage >> catID;
                *pSystemMessage >> qosTemp;
                QualityOfService qualityOfService = (QualityOfService)qosTemp;

                EE_LOG(efd::kChannelManager, efd::ILogger::kLVL2,
                    ("Received %s for cat %s, qos=%d, from %s NetID=%d",
                    pMessage->GetDescription().c_str(), catID.ToString().c_str(), qualityOfService,
                    senderConnectionID.ToString().c_str(), netID));

                //look up the ConnectionID of for the QoS/NetID subscribing.
                ConnectionID cidToSubscribe;
                if (qualityOfService != QOS_RELIABLE)
                {
                    NetIDQosPair netIDQosPair(netID,qualityOfService);
                    if (!m_NetIDQosLookupMap.find(netIDQosPair, cidToSubscribe))
                    {
                        // a connection to the netid subscribing has not yet come up,
                        // queue the subscription request to be processed after an appropriate 
                        // connection has been established
                        ChannelSubscription channelSubscription(
                            catID,
                            netID,
                            qualityOfService,
                            pMessage->GetClassID());
                        m_pendingSubscriptions.push_back(channelSubscription);
                        return;
                    }
                }
                else
                {
                    cidToSubscribe = senderConnectionID;
                }
                if (pMessage->GetClassID() == kMSGID_SubscribeExternal)
                {
                    m_spNetLib->AddRemoteConsumer(catID, cidToSubscribe);
                    m_spNetLib->AddLocalConsumer(catID, qualityOfService, m_pConnectionCallback);
                    m_spNetLib->SendAddLocalConsumerRequest(catID, qualityOfService);
                }
                else
                {
                    m_spNetLib->RemoveRemoteConsumer(catID, cidToSubscribe);
                    m_spNetLib->RemoveLocalConsumer(catID, m_pConnectionCallback);
                    m_spNetLib->SendRemoveLocalConsumerRequest(catID, qualityOfService);
                }
            }
        }
        return;

    case kMSGID_Resubscribe:
        {
            const IMessage* pMessage = pEnvelopeMessage->GetContents(m_spMessageService);
            EE_ASSERT(pMessage);
            EE_LOG(efd::kChannelManager, efd::ILogger::kLVL2,
                ("Received Resubscribe request from %s", senderConnectionID.ToString().c_str()));
            EE_UNUSED_ARG(pMessage);
            m_spNetLib->SendAddLocalConsumerRequests(senderConnectionID);
            m_spNetLib->SendCategoryProductionMessages(senderConnectionID);
            IMessage* pResubscribeCompleteMessage = 
                EE_NEW MessageWrapper<IMessage, kMSGID_ResubscribeComplete>;
            m_spNetLib->SendRemote(
                pResubscribeCompleteMessage,
                kCAT_NetSystem,
                senderConnectionID);
        }
        return;

    case kMSGID_ResubscribeComplete:
        EE_LOG(efd::kChannelManager, efd::ILogger::kLVL1,
            ("CM Received ResubscribeComplete message from %s",
            senderConnectionID.ToString().c_str()));
        EE_ASSERT(m_spMessageService);
        NetService::SendConnectionUpdate<kMSGID_ChannelManagerConnectionAccepted>(
            m_spMessageService, senderConnectionID);
        break;

    case kMSGID_AnnounceNetID:
        {
            EE_ASSERT(senderConnectionID.GetQualityOfService() == QOS_RELIABLE);
            const IMessage* pLocalMessage = pEnvelopeMessage->GetContents(m_spMessageService);
            EE_ASSERT(pLocalMessage);
            const AnnounceNetIDMessage* pNetIdAnnounceMessage =
                EE_DYNAMIC_CAST(AnnounceNetIDMessage, pLocalMessage);
            EE_ASSERT(pNetIdAnnounceMessage);
            UInt32 senderNetID = pEnvelopeMessage->GetSenderNetID();
            ConnectionID newConnectionID = pNetIdAnnounceMessage->GetAnnouncedConnectionID();
            QualityOfService newQulaityOfService = newConnectionID.GetQualityOfService();
            if (newQulaityOfService == QOS_RELIABLE)
            {
                EE_ASSERT(senderNetID==kNetID_Unassigned);
                // discard bogus announcement on reliable QOS.  ChannelManager assigns netid to 
                break;
            }
            ConnectionID remoteConnectionID(
                newConnectionID.GetQualityOfService(), 
                senderConnectionID.m_ipInfo.ip, 
                newConnectionID.GetRemotePort(), // swap remote and local ports
                newConnectionID.GetLocalPort());
            // make sure we have a matching connection

            EE_ASSERT(newConnectionID == senderConnectionID ||
                      newQulaityOfService != QOS_RELIABLE);

            NetIDQosPair netIDQos(senderNetID, newQulaityOfService);
            m_NetIDQosLookupMap[netIDQos]=remoteConnectionID;
        }
        break;

    case kMSGID_BeginCategoryProduction:
    case kMSGID_EndCategoryProduction:
        {
            const IMessage* pMessage = pEnvelopeMessage->GetContents(m_spMessageService);
            const StreamMessage* pSystemMessage = EE_DYNAMIC_CAST(const StreamMessage, pMessage);
            EE_ASSERT(pSystemMessage);
            pSystemMessage->Reset();

            Category catID = 0;
            efd::SInt32 qualityOfService;
            *pSystemMessage >> catID;
            *pSystemMessage >> qualityOfService;

            if(QOS_INVALID == qualityOfService)
            {
                EE_LOG(efd::kChannelManager, efd::ILogger::kERR0,
                    ("%s> bogus qos(%d) in %s NetID=%d",
                    __FUNCTION__,
                    qualityOfService,
                    pMessage->GetDescription().c_str(),
                    pEnvelopeMessage->GetSenderNetID()));
                return;
            }

            EE_LOG(efd::kChannelManager, ILogger::kLVL2,
                ("ChannelManager Received %s for %s, from %s NetID=%d",
                pMessage->GetDescription().c_str(),
                catID.ToString().c_str(),
                senderConnectionID.ToString().c_str(),
                pEnvelopeMessage->GetSenderNetID()));

            if(pMessage->GetClassID() == kMSGID_BeginCategoryProduction)
            {
                m_spNetLib->BeginCategoryProduction(
                    catID,
                    (efd::QualityOfService)qualityOfService);
                m_spNetLib->SendBeginCategoryProduction(
                    catID,
                    (efd::QualityOfService)qualityOfService);
                m_spNetLib->ProducerAssociate(senderConnectionID, catID);
            }
            else
            {
                m_spNetLib->EndCategoryProduction(catID);
                m_spNetLib->SendEndCategoryProduction(catID);
                m_spNetLib->ProducerDeassociate(senderConnectionID, catID);
            }
        }
        return;

    case kMSGID_RequestNetID:
        {
            efd::UInt32 netIDToAssign = AssignClientNetID();
            EE_LOG(efd::kChannelManager,
                ILogger::kLVL1,
                ("%s>kMSGID_RequestNetID Assigning NetID %d to %s",
                __FUNCTION__, netIDToAssign, senderConnectionID.ToString().c_str()));
            const IMessage* pMessage = pEnvelopeMessage->GetContents(m_spMessageService);
            const RequestNetIDMessage* pRequestMessage =
                EE_DYNAMIC_CAST(RequestNetIDMessage, pMessage);
            EE_ASSERT(pRequestMessage);
            AssignNetIDMessagePtr spResponseMessage = 
                EE_NEW AssignNetIDMessage(
                senderConnectionID,
                m_spNetLib->GetNetID(),
                netIDToAssign);
            m_spNetLib->SendRemote(
                spResponseMessage,
                pRequestMessage->GetPrivateResponseChannel(),
                senderConnectionID);
        }
        return;

    case kMSGID_SendToProducer:
        {
            const IMessage* pMessage = pEnvelopeMessage->GetContents(m_spMessageService);
            EE_ASSERT(pMessage);
            EnvelopeMessage* pInnerEnvelopeMessage =
                EE_DYNAMIC_CAST(EnvelopeMessage, const_cast<IMessage*>(pMessage));
            EE_ASSERT(pInnerEnvelopeMessage);
            EE_LOG(efd::kChannelManager, ILogger::kLVL2,
                ("SendToProducer %s for %s, from %s NetID=%d",
                pEnvelopeMessage->GetChildDescription().c_str(),
                pInnerEnvelopeMessage->GetDestinationCategory().ToString().c_str(),
                senderConnectionID.ToString().c_str(),
                pEnvelopeMessage->GetSenderNetID()));
            m_spNetLib->ProducerForward(const_cast<EnvelopeMessage*>(pEnvelopeMessage),
                pInnerEnvelopeMessage->GetDestinationCategory());
        }
        return;

    default:
        {
            EE_LOG(efd::kChannelManager, ILogger::kLVL2,
                ("Forwarding message %s for %s, from %s NetID=%d",
                pEnvelopeMessage->GetChildDescription().c_str(),
                pEnvelopeMessage->GetDestinationCategory().ToString().c_str(),
                senderConnectionID.ToString().c_str(),
                pEnvelopeMessage->GetSenderNetID()));
            m_spNetLib->Forward(const_cast<EnvelopeMessage*>(pEnvelopeMessage),
                pEnvelopeMessage->GetQualityOfService());
        }
        return;
    }
}
//-------------------------------------------------------------------------------------------------
void ChannelManagerService::AddConnection(const ConnectionID& i_source)
{
    // we want to receive all messages from this connection
    m_spNetLib->RegisterEventHandler(
        kMSGID_ConnectionDataReceivedMsg,
        m_pConnectionCallback,
        i_source);

    if (i_source.GetQualityOfService() == QOS_RELIABLE)
    {
        // Assign NetID to process that just connected
        efd::UInt32 netIDToAssign = AssignNetID();
        EE_LOG(efd::kChannelManager,
            ILogger::kLVL1,
            ("%s> kMSGID_RequestNetID Assigning NetID %d to %s",
            __FUNCTION__, netIDToAssign, i_source.ToString().c_str()));
        m_spNetLib->SendAddLocalConsumerRequests(i_source);
        m_spNetLib->SendCategoryProductionMessages(i_source);
        AssignNetIDMessagePtr spMessage = 
            EE_NEW AssignNetIDMessage(i_source, m_spNetLib->GetNetID(), netIDToAssign);
        m_spNetLib->SendRemote(spMessage, kCAT_NetSystem, i_source);
        IMessage* pResubscribeCompleteMessage = 
            EE_NEW MessageWrapper<IMessage, kMSGID_ResubscribeComplete>;
        m_spNetLib->SendRemote(pResubscribeCompleteMessage, kCAT_NetSystem, i_source);
    }
}
//-------------------------------------------------------------------------------------------------
void ChannelManagerService::RemoveConnection(const ConnectionID& i_source)
{
    m_spNetLib->RemoveRemoteConsumer(i_source);

    EE_ASSERT(m_spMessageService);
    NetService::SendConnectionUpdate<kMSGID_ChannelManagerConnectionClosed>(
        m_spMessageService, i_source);
}
//-------------------------------------------------------------------------------------------------
void ChannelManagerService::AnnounceService(const efd::utf8string& name)
{
    // Use the NameResolutionService to announce each connection that is available for this Channel
    // Manager.  The announced name is the passed in 'name' parameter + ":QOS_NAME".
    NameResolutionService* pNameService = 
        m_pServiceManager->GetSystemServiceAs<NameResolutionService>();
    EE_ASSERT (pNameService);

    if (m_spConfigList != NULL)
    {
        ChannelManagerConfigList::iterator configIt = m_spConfigList->begin();
        for (; configIt != m_spConfigList->end(); ++configIt)
        {
            ChannelManagerConfigPtr spConfig = *configIt;

            utf8string announce = name + ":" + 
                QualityOfServiceToString(spConfig->m_qualityOfService);
            pNameService->AnnounceService(
                ChannelManagerService::CLASS_ID,
                announce,
                spConfig->m_port);

            EE_LOG(efd::kChannelManager, efd::ILogger::kLVL1,
                ("%s> Announced '%s' on port %d",
                __FUNCTION__,
                announce.c_str(), spConfig->m_port));
        }
    }
}
//-------------------------------------------------------------------------------------------------
efd::UInt32 ChannelManagerService::AssignNetID()
{
    return m_curNetID++;
}
//-------------------------------------------------------------------------------------------------
efd::UInt32 ChannelManagerService::AssignClientNetID()
{
    return m_curClientNetID++;
}
//-------------------------------------------------------------------------------------------------

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

#include <efd/Metrics.h>
#include <efd/ILogger.h>
#include <efd/MessageService.h>
#include <efd/ServiceManager.h>
#include <efd/NetMessage.h>
#include <efd/StreamMessage.h>
#include <efd/IBase.h>
#include <efd/IConfigManager.h>
#include <efd/ServiceDiscoveryMessage.h>
#include <efd/IDs.h>

#include <efdNetwork/NetService.h>
#include <efdNetwork/NetListener.h>
#include <efdNetwork/INetLib.h>
#include <efdNetwork/ChannelManagerService.h>

#include <efdNetwork/NetLibTCP.h>

//-------------------------------------------------------------------------------------------------
using namespace efd;

const char* NetService::kConfigSection = "NetService";
const char* NetService::kNetLibType = "NetLibType";
const char* NetService::kNetLibTCP = "NetLibTCP";
const char* NetService::kWaitBeforeReconnect = "WaitBeforeReconnect";

//-------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(NetListener);



//-------------------------------------------------------------------------------------------------
void NetService::HandleNetMessage(
    const IMessage* pIncomingMessage,
    const ConnectionID& senderConnectionID)
{
    //check to see if this is a NetMessage w/o an envelope
    switch(pIncomingMessage->GetClassID())
    {
    case kMSGID_ConnectionAcceptedMsg:
        if (senderConnectionID.GetQualityOfService() == QOS_RELIABLE)
        {
            EE_FAIL("This case should never happen!  NetService does not Listen for connections.");
        }
        return;

    case kMSGID_ConnectionConnectedMsg:
        {
            efd::UInt32 ip = senderConnectionID.GetIP();
            efd::UInt16 remotePort = senderConnectionID.GetRemotePort();

            efd::utf8string ipStr = m_spNetLib->IPToString(ip);

            if (senderConnectionID.GetQualityOfService() == QOS_RELIABLE)
            {
                m_channelManagerIP = ipStr;
                m_channelManagerListenPort = remotePort;

                SetChannelManagerConnectionID(senderConnectionID);
                EE_LOG(efd::kNetMessage, efd::ILogger::kLVL1,
                    ("%s> Set ChannelManager connection ID to %s.",
                    __FUNCTION__, senderConnectionID.ToString().c_str()));
            }
            else
            {
                EE_ASSERT(m_channelManagerNetID != kNetID_Unassigned);
                SetChannelManagerConnectionID(senderConnectionID);
                NetIDQosPair netIDQos(
                    m_channelManagerNetID,
                    (QualityOfService)senderConnectionID.GetQualityOfService());
                m_NetIDQosLookupMap[netIDQos]=senderConnectionID;
                if(GetNetID() != kNetID_Unassigned)
                {
                    m_spNetLib->SendNetIDAnnouncement(
                        senderConnectionID,
                        GetChannelManagerConnectionID(QOS_RELIABLE));
                }
            }

            EE_ASSERT(m_spConfigList);
            for(ChannelManagerConfigList::iterator it = m_spConfigList->begin();
                it != m_spConfigList->end();
                ++it)
            {
                ChannelManagerConfigPtr spConfig = *it;
                if (spConfig->m_qualityOfService == senderConnectionID.m_qos)
                {
                    spConfig->m_connectionStatus = kCS_Connected;
                }
            }

            // we do not send connection notification here, we must wait until we have received all
            // subscriptions from the other side
        }
        return;

    case kMSGID_ConnectionDisconnectedMsg:
        m_spNetLib->RemoveRemoteConsumer(senderConnectionID);
        SendConnectionUpdate<kMSGID_NetServiceConnectionClosed>(
            m_spMessageService,
            senderConnectionID);
        // if the CM has disconnected, we attempt to reconnect to it

        if (senderConnectionID.m_qos == QOS_RELIABLE)
        {
            if (m_connectionStatus == kCS_Canceled)
            {
                EE_LOG(efd::kNetMessage,
                    efd::ILogger::kLVL1,
                    ("%s> kMSGID_ConnectionDisconnectedMsg m_connectionStatus = kCS_Disconnected",
                    __FUNCTION__));
                m_connectionStatus = kCS_Disconnected;
            }
            else
            {
                EE_LOG(efd::kNetMessage,
                    efd::ILogger::kLVL1,
                    ("%s> kMSGID_ConnectionDisconnectedMsg m_connectionStatus = kCS_Failed",
                    __FUNCTION__));
                m_connectionStatus = kCS_Failed;
            }

            efd::TimeType curTime = m_pServiceManager->GetServiceManagerTime();
            // attempt to reconnect at the specified time in the future
            m_reconnectTime = curTime + m_reconnectWaitTime;
            EE_LOG(efd::kNetMessage, efd::ILogger::kLVL1,
                ("%s> Set ChannelManager connection ID to %s because %s closed.",
                __FUNCTION__, kCID_INVALID.ToString().c_str(),
                senderConnectionID.ToString().c_str()));
            SetChannelManagerConnectionID(kCID_INVALID);
            SetNetID(kNetID_Unassigned);
            // close all connections because we lost our reliable connection
            EE_ASSERT(m_spConfigList);
        }
        else
        {
            // iterate through netid/qos to cid map so we can remove stale connections
            NetIDQosPairToConnectionIDMap::iterator it = m_NetIDQosLookupMap.begin();
            while (it != m_NetIDQosLookupMap.end())
            {
                if ((*it).second == senderConnectionID)
                {
                    NetIDQosPairToConnectionIDMap::iterator eraseIt = it++;
                    m_NetIDQosLookupMap.erase(eraseIt);
                }
                else
                {
                    ++it;
                }
            }
            NetIDQosPair netIDQos(
                m_channelManagerNetID,
                (QualityOfService)senderConnectionID.GetQualityOfService());
            ConnectionID cidToSubscribe;
            if (m_NetIDQosLookupMap.find(netIDQos, cidToSubscribe))
            {
                EE_ASSERT(cidToSubscribe == senderConnectionID);
            }
            m_NetIDQosLookupMap[netIDQos]=senderConnectionID;
        }

        for(ChannelManagerConfigList::iterator it = m_spConfigList->begin();
            it != m_spConfigList->end();
            ++it)
        {
            ChannelManagerConfigPtr spConfig = *it;
            if (spConfig->m_qualityOfService == senderConnectionID.m_qos)
            {
                if (spConfig->m_connectionStatus == kCS_Canceled)
                {
                    spConfig->m_connectionStatus = kCS_Disconnected;
                }
                else
                {
                    spConfig->m_connectionStatus = kCS_Failed;
                }
                if (spConfig->m_connectionID != kCID_INVALID)
                {
                    m_spNetLib->CloseConnection(spConfig->m_connectionID);
                }
                spConfig->m_connectionID = kCID_INVALID;
            }
        }

        return;

    case kMSGID_ConnectionFailedToConnectMsg:
        {
            EE_LOG(efd::kNetMessage, efd::ILogger::kLVL1,
                ("%s> Connection Failed %s",
                __FUNCTION__, senderConnectionID.ToString().c_str()));

            SendConnectionUpdate<kMSGID_NetServiceConnectionFailed>(
                m_spMessageService,
                senderConnectionID);

            EE_ASSERT(m_spConfigList);
            for(ChannelManagerConfigList::iterator it = m_spConfigList->begin();
                it != m_spConfigList->end();
                ++it)
            {
                ChannelManagerConfigPtr spConfig = *it;
                if (spConfig->m_qualityOfService == senderConnectionID.m_qos)
                {
                    spConfig->m_connectionStatus = kCS_Failed;
                }
            }

            // if we haven't received our NetID yet, try to reconnect
            // we don't need check for ChannelManagerService because we already tried to connect
            if (senderConnectionID.m_qos == QOS_RELIABLE)
            {
                EE_LOG(efd::kNetMessage,
                    efd::ILogger::kLVL1,
                    ("%s> kMSGID_ConnectionFailedToConnectMsg m_connectionStatus = kCS_Failed "
                    "for %s",
                    __FUNCTION__,
                    senderConnectionID.ToString().c_str()));
                m_connectionStatus = kCS_Failed;
                efd::TimeType curTime = m_pServiceManager->GetServiceManagerTime();
                // attempt to reconnect at the specified time in the future
                m_reconnectTime = curTime + m_reconnectWaitTime;
            }
        }
        return;

    case kMSGID_ConnectionDataReceivedMsg:
        {
            const EnvelopeMessage* pConstMessage = 
                EE_DYNAMIC_CAST(const EnvelopeMessage, pIncomingMessage);
            EnvelopeMessage* pEnvelopeMessage = const_cast<EnvelopeMessage*>(pConstMessage);
            // make sure we have the proper kind of message
            EE_ASSERT(pEnvelopeMessage);

            // check to see if the message originated from us through the ChannelManager
            if (pEnvelopeMessage->GetSenderNetID() == GetNetID() &&
                senderConnectionID == GetChannelManagerConnectionID(
                senderConnectionID.GetQualityOfService()))
            {
                EE_LOG(efd::kNetMessage, efd::ILogger::kLVL3,
                    ("%s> Discarding message from self received from ChannelManager %s, "
                    "cat = %s, connection = %s",
                    __FUNCTION__,
                    senderConnectionID.ToString().c_str(),
                    pEnvelopeMessage->GetDestinationCategory().ToString().c_str(),
                    senderConnectionID.ToString().c_str()));
                return;
            }
            // process the message...
            const Category& cat = pEnvelopeMessage->GetDestinationCategory();

            // first, check if a system message...
            if (cat == kCAT_NetSystem)
            {
                // Call the system message handler
                HandleSystemMessage(pEnvelopeMessage);
            }
            else
            {
                const IMessage* pMessage = pEnvelopeMessage->GetContents(m_spMessageService);
                EE_LOG(efd::kNetMessage, efd::ILogger::kERR0,
                    ("%s> Discarding message %s %s from %s NetID=%d",
                    __FUNCTION__,
                    pMessage->GetDescription().c_str(),
                    pEnvelopeMessage->GetDestinationCategory().ToString().c_str(),
                    senderConnectionID.ToString().c_str(),
                    pEnvelopeMessage->GetSenderNetID()));
                EE_UNUSED_ARG(pMessage);
            }
        }
        return;

    default:
        // Error getting message contents
        EE_LOG(efd::kNetMessage, efd::ILogger::kERR2,
            ("Error: Received message on cat %s, but %s type is not recognized",
            senderConnectionID.ToString().c_str(), pIncomingMessage->GetDescription().c_str()));

        // tick off another error occurred
        METRICS_PROBE_METRIC("NETSERVICE.MESSAGE.COUNT.ERROR", 1);

        return;
    }

}

//-------------------------------------------------------------------------------------------------
NetService::NetService(efd::utf8string netLibType)
    : m_spNetLib(NULL)
    , m_channelManagerListenPort(0)
    , m_channelManagerNetID(kNetID_Unassigned)
    , m_autoReconnectToChannelManager(true)
    , m_connectionStatus(kCS_Canceled)
    , m_reconnectTime(0.0)
    , m_reconnectWaitTime(10.0)
    , m_netLibType(netLibType)
    , m_readNetLibConfig(true)
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 5700;

    EE_LOG(efd::kNetMessage, efd::ILogger::kLVL0,
        ("Creating a NetService instance 0x%08X", this));

    INetLib::RegisterNetLib<NetLibTCP>(kNetLibTCP);

    // if netLibType is passed in we force creation of the type instead of reading the config value
    if (!m_netLibType.empty())
    {
        m_readNetLibConfig = false;
    }
    else
    {
        m_netLibType = kNetLibTCP;
    }
}


//-------------------------------------------------------------------------------------------------
NetService::~NetService()
{
    EE_LOG(efd::kNetMessage, efd::ILogger::kLVL0,
        ("Destroying a NetService instance 0x%08X", this));
    m_spNetLib = NULL;
}


//-------------------------------------------------------------------------------------------------
void NetService::Subscribe(
    const Category& cat, 
    INetCallback* pCallback, 
    QualityOfService qualityOfService)
{
    if (!pCallback)
    {
        pCallback = this;
    }
    EE_ASSERT(cat != kCAT_INVALID);
    EE_ASSERT(qualityOfService != QOS_INVALID);

    CategoryCallback categoryCallback(cat,pCallback);
    CategoryRefcount::iterator it= m_subscriberRefcount.find(categoryCallback);
    if (it != m_subscriberRefcount.end())
    {
        ++((*it).second);
    }
    else
    {
        // a Category can only be associated with a single QualityOfService
        // if you hit this assert it means that somewhere else has registered the same Category
        // with a different QOS
        EE_ASSERT(m_spNetLib->GetTransport(cat) == QOS_INVALID
            || (m_spNetLib->GetTransport(cat) == qualityOfService));

        m_spNetLib->AddLocalConsumer(cat, qualityOfService, pCallback);
        if(m_spNetLib->GetNetID() != kNetID_Unassigned)
        {
            m_spNetLib->SendAddLocalConsumerRequest(cat, qualityOfService);
        }
        NonAtomicRefCount refCount(1);
        m_subscriberRefcount[categoryCallback] = refCount;
    }
}


//-------------------------------------------------------------------------------------------------
void NetService::Unsubscribe(const Category& cat, INetCallback* pCallback)
{
    if (!pCallback)
    {
        pCallback = this;
    }
    EE_ASSERT(cat != kCAT_INVALID);
    CategoryCallback categoryCallback(cat,pCallback);
    CategoryRefcount::iterator it= m_subscriberRefcount.find(categoryCallback);
    if (it != m_subscriberRefcount.end())
    {
        --((*it).second);
    }
    else
    {
        EE_LOG(efd::kNetMessage, efd::ILogger::kERR2,
            ("%s> Tried to Unsubscribe %s not subscribed by callback 0x%08X",
            __FUNCTION__, cat.ToString().c_str(),pCallback));
        return;
    }

    if (((*it).second) <= 0)
    {
        QualityOfService qos = m_spNetLib->GetTransport(cat);
        if ((m_spNetLib->GetNetID() != kNetID_Unassigned)
            && (qos != QOS_INVALID))
        {
            m_spNetLib->SendRemoveLocalConsumerRequest(cat, qos);
        }
        m_spNetLib->RemoveLocalConsumer(cat, pCallback);
        m_subscriberRefcount.erase(it);
    }
}


//-------------------------------------------------------------------------------------------------
void NetService::Unsubscribe(INetCallback* callback)
{
    m_spNetLib->RemoveLocalConsumer(callback);
}


//-------------------------------------------------------------------------------------------------
void NetService::UnsubscribeAll()
{
    m_spNetLib->RemoveAllLocalConsumers();
    m_spNetLib->AddLocalConsumer(kCAT_NetSystem, QOS_RELIABLE, this);
}


//-------------------------------------------------------------------------------------------------
void NetService::SendRemote(IMessage* pMessage, const Category &cat, QualityOfService defaultQOS)
{
    EE_ASSERT(cat != kCAT_INVALID);
    if (defaultQOS == QOS_INVALID)
    {
        defaultQOS = m_spNetLib->GetTransport(cat);
        if (defaultQOS == QOS_INVALID)
        {
            EE_FAIL_MESSAGE(("%s| %s has no QOS, you must either call BeginCategoryProduction, or "
                "specify a valid QualityOfService in defaultQOS.",
                pMessage->GetDescription().c_str(),
                cat.ToString().c_str() ) );
            return;
        }
    }
    ConnectionID cmConnectionID = GetChannelManagerConnectionID(defaultQOS);

    if (cmConnectionID != kCID_INVALID)
    {
        // we have a valid connection to a ChannelManager
        m_spNetLib->SendRemote(pMessage, cat, cmConnectionID);
    }
}


//-------------------------------------------------------------------------------------------------
void NetService::SendRemote(IMessage* pMessage, const Category &cat, const ConnectionID& cid)
{
    EE_ASSERT(cat != kCAT_INVALID);
    m_spNetLib->SendRemote(pMessage, cat, cid);
}


//-------------------------------------------------------------------------------------------------
void NetService::ProducerSendRemote(
    IMessage* pMessage,
    const Category& categoryProduced,
    QualityOfService defaultQOS)
{
    EE_ASSERT(categoryProduced != kCAT_INVALID);
    // only ChannelManager uses INetLib::ProducerSendRemote
    // we just Send the message to the ChannelManager because only
    // ChannelManager can know who to deliver to w/o a race condition
    //m_spNetLib->ProducerSendRemote(pMessage, categoryProduced);
    EnvelopeMessagePtr spInnerEnvelopeMessage = 
        EE_NEW MessageWrapper< EnvelopeMessage, kMSGID_SendToProducer>;
    EE_ASSERT(spInnerEnvelopeMessage);
    spInnerEnvelopeMessage->SetChild(pMessage);
    spInnerEnvelopeMessage->SetDestinationCategory(categoryProduced);
    EE_MESSAGE_TRACE(
        pMessage,
        efd::kMessageTrace, 
        efd::ILogger::kLVL0,
        efd::ILogger::kLVL2,
        ("%s| sending to producer of %s",
        pMessage->GetDescription().c_str(),
        categoryProduced.ToString().c_str()));
    SendRemote(
        spInnerEnvelopeMessage,
        kCAT_SendToProducer,
        GetChannelManagerConnectionID(defaultQOS));
}

//-------------------------------------------------------------------------------------------------
void NetService::CreateNetLib(efd::utf8string netLibType)
{
    m_spNetLib = INetLib::CreateNetLib(netLibType, m_spMessageService);
}

//-------------------------------------------------------------------------------------------------
efd::SyncResult NetService::OnPreInit()
{
    EE_ASSERT(m_pServiceManager);
    IConfigManager* pConfigManager = m_pServiceManager->GetSystemServiceAs<IConfigManager>();
    if (pConfigManager)
    {
        const ISection *pSection = pConfigManager->GetConfiguration()->FindSection(kConfigSection);

        if (pSection)
        {
            if (m_readNetLibConfig)
            {
                efd::utf8string netLibType = pSection->FindValue(kNetLibType);
                if (!netLibType.empty())
                {
                    m_netLibType = netLibType;
                }
            }

            efd::utf8string strReconnectWaitTime = pSection->FindValue(kWaitBeforeReconnect);
            // already set to a reasonable default
            if (!strReconnectWaitTime.empty())
            {
                m_reconnectWaitTime = (efd::UInt16)atoi(strReconnectWaitTime.c_str());
            }
        }
    }

    if (!m_spMessageService)
    {
        m_spMessageService = m_pServiceManager->GetSystemServiceAs<MessageService>();
    }

    if (!m_spMessageService)
    {
        // The net service requires a Message service to run.  Both would need to be added at
        // the same time so if I can't find it just fail.
        EE_LOG(efd::kMessage, efd::ILogger::kERR0,
            ("%s> Unable to find MessageService!!!!", __FUNCTION__));
        return efd::SyncResult_Failure;
    }

    if (!m_spNetLib)
    {
        m_spNetLib = INetLib::CreateNetLib(m_netLibType,m_spMessageService);
    }

    // make sure that MessageService gets producer send messages
    Subscribe(kCAT_SendToProducer, m_spMessageService);
    m_spConfigList = ChannelManagerService::ReadConfig(pConfigManager);


    // initialize INetLib
    if (INetLib::StartNet() <= 0)
    {
        return efd::SyncResult_Failure;
    }

    // add concrete message classes to the factory
    RegisterMessageWrapperFactory< ServiceDiscoveryRequest, kMSGID_ServiceDiscoveryRequest >(
        m_spMessageService);
    RegisterMessageWrapperFactory< ServiceDiscoveryResponse, kMSGID_ServiceDiscoveryResponse >(
        m_spMessageService);

    RegisterMessageWrapperFactory< StreamMessage, kMSGID_ConsoleInput >(m_spMessageService);
    RegisterMessageWrapperFactory< StreamMessage, kMSGID_TestShutdown >(m_spMessageService);
    RegisterMessageWrapperFactory< StreamMessage, kMSGID_SubscribeExternal >(m_spMessageService);
    RegisterMessageWrapperFactory< StreamMessage, kMSGID_UnsubscribeExternal >(m_spMessageService);
    RegisterMessageWrapperFactory< StreamMessage, kMSGID_BeginCategoryProduction >(
        m_spMessageService);
    RegisterMessageWrapperFactory< StreamMessage, kMSGID_EndCategoryProduction >(
        m_spMessageService);
    RegisterMessageWrapperFactory< StreamMessage, kMSGID_NetServiceConnectionEstablished >(
        m_spMessageService);
    RegisterMessageWrapperFactory< StreamMessage, kMSGID_NetServiceConnectionClosed >(
        m_spMessageService);
    //testsubscriptionmessages.cpp:goodmsg
    RegisterMessageWrapperFactory< StreamMessage, 0x00001111 >(m_spMessageService);
    //testsubscriptionmessages.cpp:controlMsg
    RegisterMessageWrapperFactory< StreamMessage, 0x00002222 >(m_spMessageService);
    //testsubscriptionmessages.cpp:badMsg
    RegisterMessageWrapperFactory< StreamMessage, 0x00003333 >(m_spMessageService);
    //testsubscriptionmessages.cpp:badcontrolMsg
    RegisterMessageWrapperFactory< StreamMessage, 0x00004444 >(m_spMessageService);
    //testsubscriptionmessages.cpp:testcontrolMsg
    RegisterMessageWrapperFactory< StreamMessage, 0x00005555 >(m_spMessageService);
    //testsubscriptionmessages.cpp:finalcontrolMsg
    RegisterMessageWrapperFactory< StreamMessage, 0x00006666 >(m_spMessageService);
    // NOTE NOTE NOTE!!!!
    //
    // DO NOT ADD ANY MORE RegisterMessageWrapperFactory CALLS HERE!
    // You should be adding these registrations to the services that receive these messages.  If
    // you are a service and receive a message over the wire then in the OnInit of your service
    // you should be registering the factory.

    RegisterMessageWrapperFactory< NetMessage, kMSGID_ConnectionDataReceivedMsg >(
        m_spMessageService);
    RegisterMessageWrapperFactory< EnvelopeMessage, kMSGID_RemoteWrapper >(m_spMessageService);
    RegisterMessageWrapperFactory< EnvelopeMessage, kMSGID_SendToProducer >(m_spMessageService);
    // NOTE NOTE NOTE!!!!
    //
    // DO NOT ADD ANY MORE RegisterMessageWrapperFactory CALLS HERE!
    // You should be adding these registrations to the services that receive these messages.  If
    // you are a service and receive a message over the wire then in the OnInit of your service
    // you should be registering the factory.


    RegisterMessageWrapperFactory< NetMessage, kMSGID_Resubscribe >(m_spMessageService);
    RegisterMessageWrapperFactory< IMessage, kMSGID_ResubscribeComplete >(m_spMessageService);
    RegisterMessageWrapperFactory< RequestNetIDMessage, kMSGID_RequestNetID >(m_spMessageService);
    RegisterMessageWrapperFactory< AnnounceNetIDMessage, kMSGID_AnnounceNetID >(
        m_spMessageService);
    RegisterMessageWrapperFactory< NetMessage, kMSGID_ConnectionFailedToConnectMsg >(
        m_spMessageService);
    RegisterMessageWrapperFactory< NetMessage, kMSGID_ConnectionConnectedMsg >(m_spMessageService);
    RegisterMessageWrapperFactory< NetMessage, kMSGID_ConnectionFailedToAcceptMsg >(
        m_spMessageService);
    RegisterMessageWrapperFactory< NetMessage, kMSGID_ConnectionAcceptedMsg >(m_spMessageService);
    RegisterMessageWrapperFactory< NetMessage, kMSGID_ConnectionDisconnectedMsg >(
        m_spMessageService);
    // used by ConnectionTCP to send version info.  Cannot be registered by ConnectionTCP until 
    // MessageFactory refactor is complete
    RegisterMessageWrapperFactory< StreamMessage, kMSGID_UnreliableVersion >(m_spMessageService);

    RegisterMessageWrapperFactory< AssignNetIDMessage, kMSGID_SetNetID >(m_spMessageService);
    // NOTE NOTE NOTE!!!!
    //
    // DO NOT ADD ANY MORE FACTORY CALLS HERE!
    // You should be adding these registrations to the services that receive these messages.  If
    // you are a service and receive a message over the wire then in the OnInit of your service
    // you should be registering the factory.

    m_spNetLib->AddLocalConsumer(kCAT_NetSystem, QOS_RELIABLE, this);
    BeginCategoryProduction(kCAT_NetSystem, QOS_RELIABLE, this);

    // Since we rely on the program type being set lets assert that it has been:
    EE_ASSERT(m_pServiceManager->IsProgramType(ServiceManager::kProgType_Client) ||
        m_pServiceManager->IsProgramType(ServiceManager::kProgType_Server) ||
        m_pServiceManager->IsProgramType(ServiceManager::kProgType_Tool));

    // Only server's have ChannelManagers.  Note that we might be both a client and a server
    // in which case we still need a ChannelManager.
    if (m_pServiceManager->IsProgramType(ServiceManager::kProgType_Server))
    {
        // set our connection status to failed so that if auto reconnect is enabled a connection 
        // will automatically establish
        EE_LOG(efd::kNetMessage, efd::ILogger::kLVL1,
            ("%s> kProgType_Server: m_connectionStatus = kCS_Failed.",__FUNCTION__));
        m_connectionStatus = kCS_Failed;
    }
    else if (m_pServiceManager->IsProgramType(ServiceManager::kProgType_Client))
    {
        EE_LOG(efd::kNetMessage, efd::ILogger::kLVL1,
            ("%s> kNetID_Client",__FUNCTION__));
        // Assign the "client net id" value
        SetNetID(kNetID_Client);
    }
    else if (m_pServiceManager->IsProgramType(ServiceManager::kProgType_Tool))
    {
        EE_LOG(efd::kNetMessage, efd::ILogger::kLVL1,
            ("%s> kProgType_Tool",__FUNCTION__));

        // do not set NetID and do not auto connect
    }

    return SyncResult_Success;
}


//-------------------------------------------------------------------------------------------------
efd::AsyncResult NetService::OnInit()
{
    if (m_spNetLib->GetNetID() != kNetID_Unassigned)
    {
        EE_LOG(efd::kMessage, ILogger::kLVL0,
            ("%s> Complete. using NetID=%d", __FUNCTION__, m_spNetLib->GetNetID()));
        return efd::AsyncResult_Complete;
    }
    else
    {
        return OnTick();
    }
}


//-------------------------------------------------------------------------------------------------
efd::AsyncResult NetService::OnShutdown()
{
    m_spMessageService = NULL;

    m_spNetLib->Shutdown();
    INetLib::StopNet();

    return efd::AsyncResult_Complete;
}


//-------------------------------------------------------------------------------------------------
efd::AsyncResult NetService::OnTick()
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
            //efd::QualityOfService qualityOfService = channelSubscription.m_qualityOfService;
            if (channelSubscription.m_subscriptionType == kMSGID_SubscribeExternal)
            {
                m_spNetLib->AddRemoteConsumer(catID, cidToSubscribe);
            }
            else
            {
                m_spNetLib->RemoveRemoteConsumer(catID, cidToSubscribe);
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

    switch (m_connectionStatus)
    {
    case kCS_Failed:
        if (m_autoReconnectToChannelManager)
        {
            efd::TimeType curTime = m_pServiceManager->GetServiceManagerTime();
            if(m_reconnectTime < curTime)
            {
                m_connectionStatus = kCS_Connecting;

                EE_LOG(efd::kNetMessage, efd::ILogger::kERR2,
                    ("Connection to ChannelManager failed, retrying."));
                if (!m_channelManagerIP.empty())
                {
                    ConnectToChannelManager(
                        QOS_RELIABLE,
                        m_channelManagerIP,
                        m_channelManagerListenPort);
                }
                else
                {
                    ConnectToDefaultChannelManager();
                }
            }
        }
        break;

    case kCS_ReliableConnected:
        {
            EE_ASSERT(GetNetID() != kNetID_Unassigned);
            EE_ASSERT(GetNetID() != kNetID_Client);
            EE_ASSERT(GetChannelManagerConnectionID(QOS_RELIABLE) != kCID_INVALID);
            bool allConnected = true;
            EE_ASSERT(m_spConfigList);
            for(ChannelManagerConfigList::iterator it2 = m_spConfigList->begin();
                it2 != m_spConfigList->end();
                ++it2)
            {
                ChannelManagerConfigPtr spConfig = *it2;
                //start reliable connection right away.  
                // Other connections wait until reliable connection 
                // is established
                if (spConfig->m_qualityOfService != QOS_RELIABLE
                    && spConfig->m_connectionStatus != kCS_Connecting
                    && spConfig->m_connectionStatus != kCS_Connected)
                {
                    allConnected = false;
                    spConfig->m_connectionStatus = kCS_Connecting;
                    ConnectToChannelManager(
                        spConfig->m_qualityOfService,
                        spConfig->m_hostname,
                        spConfig->m_port);
                }
            }
            if (allConnected)
            {
                EE_LOG(efd::kNetMessage, efd::ILogger::kLVL0,
                    ("%s> All Connections to ChannelManager complete.",__FUNCTION__));
                m_connectionStatus = kCS_Connected;
            }
        }
        break;

    case kCS_Canceled:
    case kCS_Connected:
    case kCS_Connecting:
    default:
        break;
    }

    return efd::AsyncResult_Pending;
}


//-------------------------------------------------------------------------------------------------
void NetService::ConnectToDefaultChannelManager()
{
    if (!m_spConfigList)
    {
        IConfigManager* pConfigManager = m_pServiceManager->GetSystemServiceAs<IConfigManager>();
        m_spConfigList = ChannelManagerService::ReadConfig(pConfigManager);
        EE_ASSERT_MESSAGE(m_spConfigList, ("%s> Failed to read ChannelManager config information!",
            __FUNCTION__));
    }

    for(ChannelManagerConfigList::iterator it = m_spConfigList->begin();
        it != m_spConfigList->end();
        ++it)
    {
        ChannelManagerConfigPtr spConfig = *it;
        //start reliable connection right away.  Other connections wait until reliable connection 
        // is established
        if ((spConfig->m_qualityOfService == QOS_RELIABLE) 
            && (spConfig->m_connectionStatus != kCS_Connected))
        {
            ConnectToChannelManager(
                spConfig->m_qualityOfService,
                spConfig->m_hostname,
                spConfig->m_port);
        }
    }
}


//-------------------------------------------------------------------------------------------------
efd::ConnectionID NetService::ConnectToChannelManager(
    QualityOfService qualityOfService, 
    efd::utf8string cmIPAddress, 
    efd::UInt16 cmPort)
{
    DisconnectFromChannelManager(qualityOfService);

    if (qualityOfService == QOS_RELIABLE)
    {
        m_channelManagerIP = cmIPAddress;
        m_channelManagerListenPort = cmPort;
        EE_LOG(efd::kNetMessage, efd::ILogger::kLVL1,
            ("%s> m_connectionStatus = kCS_Connecting",__FUNCTION__));
        m_connectionStatus = kCS_Connecting;
    }
    else
    {
        // we can't even attempt any qos connection until the reliable connection is established
        EE_ASSERT(m_channelManagerNetID != kNetID_Unassigned);
    }

    if (m_spNetLib)
    {
        return m_spNetLib->Connect(cmIPAddress, cmPort, qualityOfService, this);
    }
    else
    {
        if (qualityOfService == QOS_RELIABLE)
        {
            EE_LOG(efd::kNetMessage, efd::ILogger::kERR2,
                ("%s> No INetLib m_connectionStatus = kCS_Failed",__FUNCTION__));
            m_connectionStatus = kCS_Failed;
        }
        return kCID_INVALID;
    }
}

//-------------------------------------------------------------------------------------------------
void NetService::DisconnectFromChannelManager()
{
    for(ChannelManagerConfigList::iterator it = m_spConfigList->begin();
        it != m_spConfigList->end();
        ++it)
    {
        ChannelManagerConfigPtr spConfig = *it;
        EE_ASSERT(spConfig);
        DisconnectFromChannelManager(spConfig->m_qualityOfService);
    }
}

//-------------------------------------------------------------------------------------------------
void NetService::DisconnectFromChannelManager(QualityOfService qualityOfService)
{
    if (qualityOfService == QOS_RELIABLE)
    {
        m_channelManagerIP = "";
        m_channelManagerListenPort = 0;
        EE_LOG(efd::kNetMessage, efd::ILogger::kLVL1,
            ("%s> m_connectionStatus = kCS_Canceled",__FUNCTION__));
        m_connectionStatus = kCS_Canceled;
    }

    ConnectionID cmConnectionID = GetChannelManagerConnectionID(qualityOfService);
    if (cmConnectionID != kCID_INVALID 
        && cmConnectionID.GetLocalPort() != 0)
    {
        // iterate through netid/qos to cid map so we can remove stale connections
        NetIDQosPairToConnectionIDMap::iterator it = m_NetIDQosLookupMap.begin();
        while (it != m_NetIDQosLookupMap.end())
        {
            if ((*it).second == cmConnectionID)
            {
                NetIDQosPairToConnectionIDMap::iterator eraseIt = it++;
                m_NetIDQosLookupMap.erase(eraseIt);
            }
            else
            {
                ++it;
            }
        }
        m_spNetLib->CloseConnection(cmConnectionID);
        EE_LOG(efd::kNetMessage, efd::ILogger::kLVL1,
            ("%s> Set ChannelManager connection ID to %s because %s closed.",
            __FUNCTION__, kCID_INVALID.ToString().c_str(),
            cmConnectionID.ToString().c_str()));
        ConnectionID emptyConnectionID(qualityOfService, 0ULL);
        SetChannelManagerConnectionID(emptyConnectionID);
        if(qualityOfService == QOS_RELIABLE)
            SetNetID(kNetID_Unassigned);
    }
}

//-------------------------------------------------------------------------------------------------
void NetService::AutoReconnectToChannelManager(bool autoReconnect)
{
    m_autoReconnectToChannelManager = autoReconnect;
}

//-------------------------------------------------------------------------------------------------
void NetService::HandleSystemMessage(EnvelopeMessage* pEnvelopeMessage)
{
    // tick off another system message occurred
    METRICS_PROBE_METRIC("NETSERVICE.SYSTEMMESSAGE.COUNT", 1);

    const IMessage* pLocalMessage = pEnvelopeMessage->GetContents(m_spMessageService);
    // if you hit this assert it means that the factory function for the message type was not
    // registerd
    EE_ASSERT(pLocalMessage);
    if (!pLocalMessage)
    {
        EE_LOG(efd::kNetMessage, efd::ILogger::kERR2,
            ("Invalid system message (type 0x%08X) received", 
            pEnvelopeMessage->GetContentsClassID()));
        return;
    }

    // switch based on the message type and process the messages
    ConnectionID senderConnectionID = pEnvelopeMessage->GetSenderConnection();
    EE_ASSERT(senderConnectionID.m_qos != QOS_INVALID);
    efd::ClassID msgClassID = pLocalMessage->GetClassID();
    switch (msgClassID)
    {
    case kMSGID_SubscribeExternal:
    case kMSGID_UnsubscribeExternal:
        {
            // we only accept subscriptions over the reliable QOS
            if (senderConnectionID.GetQualityOfService() != QOS_RELIABLE)
                break;

            const StreamMessage* pSystemMessage = 
                EE_DYNAMIC_CAST(const StreamMessage, pLocalMessage);
            if (pSystemMessage)
            {
                pSystemMessage->Reset();

                UInt8 numCats = 0;
                *pSystemMessage >> numCats;
                Category catID = 0;
                UInt32 netID = pEnvelopeMessage->GetSenderNetID();

                for (UInt8 curCat = 0; curCat < numCats; curCat++)
                {
                    SInt32 qosTemp;
                    *pSystemMessage >> catID;
                    *pSystemMessage >> qosTemp;
                    QualityOfService qualityOfService = (QualityOfService)qosTemp;

                    EE_LOG(efd::kChannelManager,
                        efd::ILogger::kLVL2,
                        ("NetService Received %s for cat %s, qos=%d, from %s NetID=%d",
                        pLocalMessage->GetDescription().c_str(),
                        catID.ToString().c_str(),
                        qualityOfService,
                        senderConnectionID.ToString().c_str(),
                        netID));

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
                                msgClassID);
                            m_pendingSubscriptions.push_back(channelSubscription);
                            return;
                        }
                    }
                    else
                    {
                        cidToSubscribe = senderConnectionID;
                    }
                    if (msgClassID == kMSGID_SubscribeExternal)
                    {
                        m_spNetLib->AddRemoteConsumer(catID, cidToSubscribe);
                    }
                    else
                    {
                        m_spNetLib->RemoveRemoteConsumer(catID, cidToSubscribe);
                    }
                }
            }
        }
        break;

    case kMSGID_Resubscribe:
        EE_LOG(efd::kNetMessage, efd::ILogger::kLVL1,
            ("NetService Received Resubscribe message from %s",
            senderConnectionID.ToString().c_str()));
        Resubscribe(senderConnectionID);
        break;

    case kMSGID_ResubscribeComplete:
        EE_LOG(efd::kNetMessage, efd::ILogger::kLVL1,
            ("NetService Received ResubscribeComplete message from %s",
            senderConnectionID.ToString().c_str()));
        // tell MessageService we are ready to go before we notify everyone it is safe to send
        Subscribe(kCAT_SendToProducer, m_spMessageService);
        m_spMessageService->SetNetService(this);
        m_spMessageService->SetSendRemote(true);
        SendConnectionUpdate<kMSGID_NetServiceConnectionEstablished>(
            m_spMessageService,
            senderConnectionID);
        break;

    case kMSGID_AnnounceNetID:
        {
            EE_ASSERT(senderConnectionID.GetQualityOfService() == QOS_RELIABLE);
            const AnnounceNetIDMessage* pNetIdAnnounceMessage = 
                EE_DYNAMIC_CAST(AnnounceNetIDMessage, pLocalMessage);
            EE_ASSERT(pNetIdAnnounceMessage);
            UInt32 senderNetID = pEnvelopeMessage->GetSenderNetID();
            ConnectionID newConnectionID = pNetIdAnnounceMessage->GetAnnouncedConnectionID();
            QualityOfService newQulaityOfService = newConnectionID.GetQualityOfService();
            ConnectionID remoteConnectionID(
                newConnectionID.GetQualityOfService(), 
                senderConnectionID.m_ipInfo.ip, 
                newConnectionID.GetRemotePort(), // swap remote and local ports
                newConnectionID.GetLocalPort());
            // make sure we have a matching connection

            EE_ASSERT(newConnectionID == senderConnectionID ||
                newQulaityOfService != QOS_RELIABLE);
            EE_ASSERT(senderConnectionID == GetChannelManagerConnectionID(QOS_RELIABLE)
                || newQulaityOfService == QOS_RELIABLE);

            NetIDQosPair netIDQos(senderNetID, newQulaityOfService);
            m_NetIDQosLookupMap[netIDQos]=remoteConnectionID;
            m_channelManagerNetID = senderNetID;
        }
        break;

    case kMSGID_SetNetID:
        {
            // ignore SetNetID message except from default QOS
            EE_ASSERT(senderConnectionID.GetQualityOfService() == QOS_RELIABLE);

            if (senderConnectionID != GetChannelManagerConnectionID(
                senderConnectionID.GetQualityOfService()))
            {
                // only the ChannelManager we connected to can set our NetID
                EE_LOG(efd::kNetMessage, efd::ILogger::kERR0,
                    ("Received SetNetID message from invalid source %s, expected %s",
                    senderConnectionID.ToString().c_str(), 
                    GetChannelManagerConnectionID(
                        senderConnectionID.GetQualityOfService()).ToString().c_str()));
                break;
            }

            EE_ASSERT(m_spMessageService);
            const AssignNetIDMessage* pAssignNetIDMessage = 
                EE_DYNAMIC_CAST(AssignNetIDMessage, pLocalMessage);
            EE_ASSERT(pAssignNetIDMessage);
            m_channelManagerNetID = pEnvelopeMessage->GetSenderNetID();
            EE_ASSERT(pAssignNetIDMessage->GetAssignedNetID() != kNetID_Unassigned);
            EE_ASSERT(pAssignNetIDMessage->GetAssignedNetID() != kNetID_Client);
            SetNetID(pAssignNetIDMessage->GetAssignedNetID());
            EE_LOG(efd::kNetMessage,
                efd::ILogger::kLVL1,
                ("%s> Received NetID, setting m_connectionStatus = kCS_ReliableConnected.",
                __FUNCTION__));
            m_connectionStatus = kCS_ReliableConnected;

            // Since we are the reliable QOS, now send our subscription information
            Resubscribe(senderConnectionID);

            // if here, ok to pass along message
            m_spMessageService->SendLocal(pAssignNetIDMessage, kCAT_LocalMessage);

            EE_LOG(efd::kNetMessage, efd::ILogger::kLVL1,
                ("Net ID configured from SetNetID message as %d.", GetNetID()));
            //printf("Net ID configured from SetNetID message as %d.\n", GetNetID());
        }
        break;

    case  kMSGID_BeginCategoryProduction:
    case  kMSGID_EndCategoryProduction:
        {
            const IMessage* pMessage = pEnvelopeMessage->GetContents(m_spMessageService);
            const StreamMessage* pSystemMessage = EE_DYNAMIC_CAST(const StreamMessage, pMessage);
            if (pSystemMessage)
            {
                pSystemMessage->Reset();

                Category catID = 0;
                efd::SInt32 qualityOfService;
                *pSystemMessage >> catID;
                *pSystemMessage >> qualityOfService;
                if(pMessage->GetClassID() == kMSGID_BeginCategoryProduction)
                {
                    m_spNetLib->ProducerAssociate(senderConnectionID, catID);
                }
                else
                {
                    m_spNetLib->ProducerDeassociate(senderConnectionID, catID);
                }
            }
        }
        break;

    default:
        EE_FAIL("Invaild system message type!");
        EE_LOG(efd::kNetMessage, efd::ILogger::kERR2,
            ("Received UNKNOWN system message type: %u (0x%08x)", msgClassID, msgClassID));
        break;
    }
}


//-------------------------------------------------------------------------------------------------
bool NetService::BeginCategoryProduction(
    const Category& cat,
    efd::QualityOfService qualityOfService,
    INetCallback* pCallback)
{
    EE_ASSERT(cat != kCAT_INVALID);
    EE_ASSERT(qualityOfService != QOS_INVALID);

    CategoryCallback categoryCallback(cat,pCallback);
    CategoryRefcount::iterator it= m_producersRefcount.find(categoryCallback);
    if (it != m_producersRefcount.end())
    {
        ++((*it).second);
    }
    else
    {
        // a Category can only be associated with a single QualityOfService
        // if you hit this assert it means that somewhere else has registered the same Category
        // with a different QOS
        EE_ASSERT(m_spNetLib->GetTransport(cat) == QOS_INVALID
            || (m_spNetLib->GetTransport(cat) == qualityOfService));

        m_spNetLib->BeginCategoryProduction(cat, qualityOfService);

        // we can't send messages yet if we have not yet received our NetID
        // category interest will automatically be communicated when connection is established
        if (m_spNetLib->GetNetID() != kNetID_Unassigned)
        {
            m_spNetLib->SendBeginCategoryProduction(cat, qualityOfService);
        }
        NonAtomicRefCount refCount(1);
        m_producersRefcount[categoryCallback] = refCount;
    }
    return true;
}


//-------------------------------------------------------------------------------------------------
bool NetService::EndCategoryProduction(const Category& cat, INetCallback* pCallback)
{
    EE_ASSERT(cat != kCAT_INVALID);
    CategoryCallback categoryCallback(cat,pCallback);
    CategoryRefcount::iterator it= m_producersRefcount.find(categoryCallback);
    if (it != m_producersRefcount.end())
    {
        --((*it).second);
        EE_ASSERT(m_spNetLib->GetTransport(cat) != QOS_INVALID);
    }
    else
    {
        EE_LOG(efd::kNetMessage, efd::ILogger::kERR2,
            ("%s> Tried to end %sno being produced by callback 0x%08X",
            __FUNCTION__, cat.ToString().c_str(),pCallback));
        return false;
    }

    if (((*it).second) <= 0)
    {
        if (m_spNetLib->GetNetID() != kNetID_Unassigned)
        {
            m_spNetLib->SendEndCategoryProduction(cat);
        }
        m_spNetLib->EndCategoryProduction(cat);
        m_producersRefcount.erase(it);
    }
    return true;
}


//-------------------------------------------------------------------------------------------------
void NetService::Resubscribe(const ConnectionID& cid)
{
    // Send the new connection the categories we are subscribed to
    m_spNetLib->SendAddLocalConsumerRequests(cid);
    m_spNetLib->SendCategoryProductionMessages(cid);
    IMessage* pMessage = EE_NEW MessageWrapper<IMessage, kMSGID_ResubscribeComplete>;
    m_spNetLib->SendRemote(pMessage, kCAT_NetSystem, cid);
}


//-------------------------------------------------------------------------------------------------
efd::UInt32 NetService::GetNetID() const
{
    return m_spNetLib->GetNetID();
}


//-------------------------------------------------------------------------------------------------
void NetService::SetNetID(efd::UInt32 netID)
{
    EE_ASSERT(m_spNetLib->GetNetID() == kNetID_Unassigned 
        || netID == kNetID_Unassigned 
        || m_spNetLib->GetNetID() == kNetID_Client);
    m_spNetLib->SetNetID(netID);
}


//-------------------------------------------------------------------------------------------------
efd::ConnectionID NetService::GetChannelManagerConnectionID(QualityOfService qualityOfService)
{
    if (m_spNetLib)
    {
        return m_spNetLib->GetChannelManagerConnectionID(qualityOfService);
    }
    else
    {
        return kCID_INVALID;
    }
}


//-------------------------------------------------------------------------------------------------
void NetService::SetChannelManagerConnectionID(const efd::ConnectionID& cmConnectionID)
{
    EE_LOG(efd::kNetMessage, efd::ILogger::kLVL1,
        ("%s> Actually setting ChannelManager connection ID to %s.",
        __FUNCTION__, cmConnectionID.ToString().c_str()));
    m_spNetLib->SetChannelManagerConnectionID(cmConnectionID);
}

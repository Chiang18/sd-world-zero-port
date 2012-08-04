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

#include <efd/DataStream.h>
#include <efd/NetMessage.h>
#include <efd/ILogger.h>
#include <efd/SystemFrameworkEnums.h>
#include <efd/INetCallback.h>
#include <efd/Metrics.h>
#include <efd/IDs.h>

#include <efdNetwork/INetLib.h>
#include <efdNetwork/HostInfo.h>
#include <efdNetwork/NetTransportTCP.h>
#include <efdNetwork/SocketErrors.h>

#if !defined(EE_PLATFORM_WII)
#include <efdNetwork/ConnectionTCP.h>
#endif

using namespace efd;

//-------------------------------------------------------------------------------------------------
NetTransportTCP::NetTransportTCP(MessageFactory* pMessageFactory, QualityOfService qos)
    : m_running(false)
    , m_qos(qos)
    , m_pMessageFactory(pMessageFactory)
    , m_lastResult(false)
{
}


//-------------------------------------------------------------------------------------------------
NetTransportTCP::~NetTransportTCP()
{
    OnShutdown();
}


//-------------------------------------------------------------------------------------------------
bool NetTransportTCP::Send(
    EnvelopeMessage *pMessage,
    ConnectionIDSet* pListenerList)
{
    bool success = false;

    // Loop through the listeners here not in OnTick()
    if (pMessage)
    {
        // Loop through the listeners list and send message to each one
        ConnectionIDSet::iterator iterListener = pListenerList->begin();

        while (pListenerList->end() != iterListener)
        {
            const ConnectionID& cid = (*iterListener);

            IConnection* pConnection = GetTransportData(cid);

            // Make sure we have a valid TCP transport data object
            if (pConnection)
            {
                success = pConnection->Send(pMessage, cid);
                if(!success)
                {
                    EE_LOG(efd::kNetMessage, efd::ILogger::kERR2,
                        ("Error: Failed to send message (0x%08X) on catagory %s "
                        "to connnection %s.  Closing connection.",
                        pMessage,
                        pMessage->GetDestinationCategory().ToString().c_str(),
                        cid.ToString().c_str()));
                    METRICS_PROBE_METRIC("NETTRANSPORTTCP.SEND.COUNT.ERROR", 1);
                    CloseConnection(cid);
                }
            }
            else
            {
                EE_LOG(efd::kNetMessage, efd::ILogger::kERR2,
                    ("QualityOfService %d does not contain IConnection %s.",
                    m_qos, cid.ToString().c_str()));
                METRICS_PROBE_METRIC("NETTRANSPORTTCP.SEND.COUNT.ERROR", 1);
            }

            // Move to the next listener
            ++iterListener;
        }
    }
    else
    {
        EE_LOG(efd::kNetMessage, efd::ILogger::kERR2,
            ("Error: Null message received from Net Service for sending."));
        METRICS_PROBE_METRIC("NETTRANSPORTTCP.SEND.COUNT.ERROR", 1);
    }
    return success;
}


//-------------------------------------------------------------------------------------------------
efd::AsyncResult NetTransportTCP::OnTick()
{
    AcceptClient();
    CheckPendingConnections();
    CloseWaitingConnections();
    ReceiveMessages();
    SendMessages();

    return efd::AsyncResult_Pending;
}


//-------------------------------------------------------------------------------------------------
efd::AsyncResult NetTransportTCP::OnInit()
{
    m_running = true;

    return efd::AsyncResult_Complete;
}


//-------------------------------------------------------------------------------------------------
efd::AsyncResult NetTransportTCP::OnShutdown()
{
    // Stop receiving messages
    m_running = false;

    // Clear the list of established connections
    CloseAllConnections();

    m_transportsToClose.clear();
    m_listenList.clear();
    m_transports.clear();
    m_connectionsPending.clear();
    m_incomingConnections.clear();

    return efd::AsyncResult_Complete;
}


//-------------------------------------------------------------------------------------------------
efd::ConnectionID NetTransportTCP::Listen(efd::UInt16 portListen, INetCallback* pCallback)
{
#if !defined(EE_PLATFORM_WII)
    ConnectionTCPPtr spTransportData = 
        ConnectionTCP::Listen(m_pMessageFactory, m_qos, portListen, m_pMessageCallback, pCallback);
    if (spTransportData)
    {
        if (m_qos & NET_UDP)
        {
            m_transports.push_back(
                spTransportData->GetRemoteConnectionID(),
                (ConnectionTCP*)spTransportData);
        }
        else
        {
            m_listenList.push_back(
                spTransportData->GetRemoteConnectionID(),
                (ConnectionTCP*)spTransportData);
        }
        return spTransportData->GetRemoteConnectionID();
    }
    else
    {
        return kCID_INVALID;
    }
#else
    return kCID_INVALID;
#endif
}


//-------------------------------------------------------------------------------------------------
ConnectionID NetTransportTCP::Connect(
    const efd::utf8string& strServerAddress,
    efd::UInt16 portServer,
    INetCallback* pCallback)
{
#if !defined(EE_PLATFORM_WII)
    ConnectionTCPPtr spTransportData = 
        ConnectionTCP::Connect(
            m_pMessageFactory, 
            m_qos, 
            strServerAddress, 
            portServer, 
            m_pMessageCallback, 
            pCallback);
    if (spTransportData)
    {
        if (!(m_qos & NET_CONNECTIONLESS))
        {
            m_connectionsPending.push_back(
                spTransportData->GetRemoteConnectionID(),
                (ConnectionTCP*)spTransportData);
            return spTransportData->GetRemoteConnectionID();
        }
        else
        {
            ConnectionID realConnectionID = spTransportData->GetRemoteConnectionID();

            HostInfo* pServerInfo = NULL;
            HostInfo::ResolveNameOrIP(strServerAddress, pServerInfo);
            EE_ASSERT(pServerInfo);

            UInt32 ipAddr = ntohl(pServerInfo->GetHostIPAddressNum());

            ConnectionID sendToConnectionID = 
                ConnectionID(m_qos,ipAddr,realConnectionID.GetLocalPort(),portServer);
            m_transports.push_back(
                spTransportData->GetRemoteConnectionID(),
                (ConnectionTCP*)spTransportData);
            return sendToConnectionID;
        }
    }
    else
    {
        efd::UInt32 remoteIP = ntohl(HostInfo::ResolveNameOrIP(strServerAddress));
        ConnectionID remoteConnection = 
            INetLib::GenerateConnectionID(m_qos, remoteIP, portServer, 0);
        NetMessagePtr spMsg = 
            EE_NEW MessageWrapper<NetMessage, kMSGID_ConnectionFailedToConnectMsg>;
        SendConnectionUpdate(remoteConnection, pCallback, spMsg);
        METRICS_PROBE_METRIC("NETTRANSPORTTCP.CONNECTION.COUNT.OUT", 1);
        EE_LOG(efd::kNetMessage,
            efd::ILogger::kERR1,
            ("Connect to %s:%d Failed!", strServerAddress.c_str(), portServer));
        return kCID_INVALID;
    }
#else
    return kCID_INVALID;
#endif
}


//-------------------------------------------------------------------------------------------------
bool NetTransportTCP::SendTo(
    const ConnectionID& sourceConnectionID,
    EnvelopeMessage* pEnvelopeMessage,
    const efd::utf8string& strServerAddress,
    efd::UInt16 portServer)
{
#if !defined(EE_PLATFORM_WII)
    IConnection* pConnection = GetTransportData(sourceConnectionID);
    if (pConnection)
    {
        efd::UInt32 remoteIP = ntohl(HostInfo::ResolveNameOrIP(strServerAddress));
        ConnectionID remoteConnectionID = 
            ConnectionID(m_qos,remoteIP,sourceConnectionID.GetLocalPort(),portServer);
        return pConnection->Send(pEnvelopeMessage,remoteConnectionID);
    }
    else
    {
        return false;
    }
#else
    return false;
#endif
}


//-------------------------------------------------------------------------------------------------
void NetTransportTCP::CheckPendingConnections()
{
    ConnectionToTransportMap::iterator it = m_connectionsPending.begin();
    while(it != m_connectionsPending.end())
    {
        IConnectionPtr pTransportData = (*it)->second;
        efd::SInt32 retVal = CompleteConnection(pTransportData);
        switch(retVal)
        {
        case EE_SOCKET_CONNECTION_IN_PROGRESS:
            ++it;
            break;
        case EE_SOCKET_CONNECTION_FAILED:
        case EE_SOCKET_CONNECTION_COMPLETE:
            {
                ConnectionID connectionToErase = (*it)->first;
                ++it;
                m_connectionsPending.erase(connectionToErase);
            }
            break;
        default:
            EE_FAIL("Unhandled return value from NetTransportTCP::Connect");
            break;
        }
    }
}


//-------------------------------------------------------------------------------------------------
efd::SInt32 NetTransportTCP::CompleteConnection(IConnection* pTransportData)
{
    INetCallback* pCallback = pTransportData->GetConnectionCallback();
    ConnectionID remoteConnection = pTransportData->GetRemoteConnectionID();

    efd::SInt32 retVal = pTransportData->CompleteConnection();

    // connect to the server.
    switch (retVal)
    {
    case EE_SOCKET_NO_DATA:
    case EE_SOCKET_CONNECTION_IN_PROGRESS:
        return EE_SOCKET_CONNECTION_IN_PROGRESS;
    case EE_SOCKET_CONNECTION_FAILED:
        {
            NetMessagePtr spNetMessage = pTransportData->GetStatusMessage();
            SendConnectionUpdate(remoteConnection, pCallback, spNetMessage);
            METRICS_PROBE_METRIC("NETTRANSPORTTCP.CONNECTION.COUNT.OUT", 1);
            EE_LOG(efd::kNetMessage,
                efd::ILogger::kERR1,
                ("Connect to %s Failed!", remoteConnection.ToString().c_str()));
        }
        return EE_SOCKET_CONNECTION_FAILED;
    case EE_SOCKET_CONNECTION_COMPLETE:
        {
            EE_LOG(efd::kNetMessage,
                efd::ILogger::kLVL1,
                ("Connected to %s!",remoteConnection.ToString().c_str()));
            if (AddListener(pTransportData))
            {
                EE_LOG(efd::kNetMessage,
                    efd::ILogger::kLVL2,
                    ("Connect %s added to listeners", remoteConnection.ToString().c_str()));
                return EE_SOCKET_CONNECTION_COMPLETE;
            }
            else
            {
                NetMessagePtr spNetMessage = pTransportData->GetStatusMessage();
                SendConnectionUpdate(remoteConnection, pCallback, spNetMessage);
                METRICS_PROBE_METRIC("NETTRANSPORTTCP.CONNECTION.COUNT.OUT", 1);
                EE_LOG(efd::kNetMessage,
                    efd::ILogger::kERR1,
                    ("Connect to %s Failed to AddListener!", 
                    remoteConnection.ToString().c_str()));
                return EE_SOCKET_CONNECTION_FAILED;
            }
        }
    case EE_SOCKET_ERROR_UNKNOWN:
    case EE_SOCKET_SHUTDOWN:
    default:
        {
            NetMessagePtr spNetMessage = pTransportData->GetStatusMessage();
            SendConnectionUpdate(remoteConnection, pCallback, spNetMessage);
            METRICS_PROBE_METRIC("NETTRANSPORTTCP.CONNECTION.COUNT.OUT", 1);
            EE_LOG(efd::kNetMessage,
                efd::ILogger::kERR1,
                ("Connect to %s Failed for unknown reason %d.", 
                remoteConnection.ToString().c_str(), retVal));
            return EE_SOCKET_CONNECTION_FAILED;
        }
    }
}

//-------------------------------------------------------------------------------------------------
bool NetTransportTCP::AcceptClient()
{
    bool newConnection = false;
    // wait to accept a client connection.
    // processing is suspended until the client connects
    for(ConnectionToTransportMap::iterator i = m_listenList.begin(); 
        i != m_listenList.end(); 
        ++i)
    {
        IConnection* pListenSocket = (*i)->second;
        IConnectionPtr spAcceptedSocket = pListenSocket->Accept();
        if (spAcceptedSocket)
        {
            EE_LOG(efd::kNetMessage,
                efd::ILogger::kLVL1,
                ("AcceptClient from %s",
                spAcceptedSocket->GetRemoteConnectionID().ToString().c_str()));
            m_connectionsPending.push_back(
                spAcceptedSocket->GetRemoteConnectionID(),
                spAcceptedSocket);
        }
    }
    return newConnection;
}


//-------------------------------------------------------------------------------------------------
bool NetTransportTCP::AddListener(IConnection* pTransportData)
{
    if (!pTransportData  || !pTransportData->IsValid())
        return false;

    ConnectionID cid = pTransportData->GetRemoteConnectionID();

    EE_ASSERT(cid != kCID_INVALID);

    ConnectionToTransportMap::NodeType* pIncomingTransport = m_incomingConnections.find(cid);

    if(!pIncomingTransport)
    {
        EE_ASSERT(!m_transports.find(cid));
        EE_LOG(efd::kNetMessage,
            efd::ILogger::kLVL1,
            ("Created A Connection %s",
            cid.ToString().c_str()));
        m_transports.push_back(cid, pTransportData);
    }
    else
    {
        EE_ASSERT((pIncomingTransport->second)->GetRemoteConnectionID() == cid);
        EE_LOG(efd::kNetMessage,
            efd::ILogger::kLVL1,
            ("Created A Connection %s",
            cid.ToString().c_str()));
    }
    NetMessagePtr spNetMessage = pTransportData->GetStatusMessage();
    SendConnectionUpdate(cid, pTransportData->GetConnectionCallback(), spNetMessage);

    return true;
}


//-------------------------------------------------------------------------------------------------
void NetTransportTCP::ReceiveMessages()
{
#if !defined(EE_PLATFORM_WII)
    int numBytes = 0;

    if (m_running && !m_transports.empty())
    {
        const int maxMessagesPerConnection = 32;
        const int maxBytesPerConnection = 3072;

        // keep track of any connection that sends too much data
        // so we can put it at the end of the list we iterate
        typedef efd::list< ConnectionToTransportMap::NodeType* > IterList;
        IterList naughtyList;

        // Loop through all the services and initialize them
        for (ConnectionToTransportMap::iterator iterData = m_transports.begin();
            iterData != m_transports.end();
            ++iterData)
        {
            IConnectionPtr spData = (*iterData)->second;
            ConnectionTCP* pConnectionToService = 
                (ConnectionTCP*)((IConnection*)(*iterData)->second);

            EE_ASSERT(spData);
            EE_ASSERT(pConnectionToService);

            // we need to pull more than one message per-frame out of the
            // socket buffers, but we don't want to get stuck here and be
            // DOS'd.  So we grab up to some max number of messages off
            // of the socket and then leave the rest for later
            int messagesPerConnection = 0;
            int bytesPerConnection = 0;

            // used to hold the ConnectionID of the connection that needs to be closed
            ConnectionID senderConnectionID;
            do
            {
                EnvelopeMessagePtr spEnvelopeMsg;
                numBytes = spData->ReceiveMessage(spEnvelopeMsg,senderConnectionID);
                if(numBytes > 0)
                {
                    // check to see if we know about this sender yet
                    if (m_qos & NET_UDP)
                    {
                        EE_ASSERT(senderConnectionID == spEnvelopeMsg->GetSenderConnection());
                        IConnection* pVirtualConnection = GetTransportData(senderConnectionID);
                        if (!pVirtualConnection)
                        {
                            EE_ASSERT(pConnectionToService->IsListening());
                            // we have not received a message yet from this socket, treat this as 
                            // a new connection
                            IConnectionPtr spConnectionTCP = EE_NEW ConnectionTCP(
                                *pConnectionToService, 
                                senderConnectionID);
                            EE_ASSERT(spConnectionTCP->GetRemoteConnectionID() != kCID_INVALID);
                            EE_ASSERT(spConnectionTCP->GetRemoteConnectionID() == 
                                senderConnectionID);
                            m_incomingConnections.push_back(senderConnectionID, spConnectionTCP);
                            if (!(m_qos & NET_CONNECTIONLESS))
                            {
                                m_connectionsPending.push_back(
                                    senderConnectionID,
                                    spConnectionTCP);
                            }
                            pVirtualConnection = spConnectionTCP;
                        }
                        if (spEnvelopeMsg->GetContentsClassID() == kMSGID_UnreliableVersion)
                        {
                            ConnectionTCP* pConnectionTCP = 
                                (ConnectionTCP*)((IConnection*)(pVirtualConnection));
                            ConnectionID versionConnectionID;
                            pConnectionTCP->ReceiveVersions(spEnvelopeMsg,versionConnectionID);
                            if (versionConnectionID != pConnectionTCP->GetRemoteConnectionID())
                            {
                                // we have just asked the wrong connection to process this 
                                // version request.  Luckily it was nice enough to tell us who
                                // the version request was actually destined for.
                                IConnection* pDestinedConnection = 
                                    GetTransportData(versionConnectionID);
                                if (pDestinedConnection)
                                {
                                    ConnectionID secondConnectionID;
                                    ConnectionTCP* pDestinedConnectionTCP = 
                                        (ConnectionTCP*)((IConnection*)(pDestinedConnection));
                                    pDestinedConnectionTCP->ReceiveVersions(
                                        spEnvelopeMsg,
                                        secondConnectionID);
                                    EE_ASSERT(pDestinedConnectionTCP->GetRemoteConnectionID() == 
                                        secondConnectionID);
                                }
                            }
                        }
                        else
                        {
                            // receive messages normally
                            ReceiveMessage(
                                pVirtualConnection->GetMessageCallback(),
                                spEnvelopeMsg);
                            bytesPerConnection += numBytes;
                        }
                    }
                    else
                    {
                        ReceiveMessage(spData->GetMessageCallback(), spEnvelopeMsg);
                        bytesPerConnection += numBytes;
                    }
                }
                ++messagesPerConnection;
                if(messagesPerConnection > maxMessagesPerConnection ||
                    bytesPerConnection > maxBytesPerConnection)
                {
                    EE_LOG(efd::kNetMessage,
                        efd::ILogger::kERR2,
                        ("Warning: %s received %d/%d bytes %d/%d messages",
                        spData->GetRemoteConnectionID().ToString().c_str(),
                        bytesPerConnection,
                        maxBytesPerConnection,
                        messagesPerConnection,
                        maxMessagesPerConnection));
                    //naughtyList.push_back(*iterData);
                    break;
                }
            } while(numBytes > 0);
/*
            messagesPerTick += messagesPerConnection;
            bytesPerTick += bytesPerConnection;

            if(messagesPerTick > maxMessagesPerTick ||
                bytesPerTick > maxBytesPerTick)
            {
                EE_LOG(efd::kNetMessage,
                    efd::ILogger::kERR2,
                    ("Warning: total received %d/%d bytes %d/%d messages",
                    bytesPerTick,
                    maxBytesPerTick,
                    messagesPerTick,
                    maxMessagesPerTick));
                // move anyone in naughty list to the end of the connection OrderedMap
                for(IterList::iterator it = naughtyList.begin();
                    it != naughtyList.end();
                    ++it)
                {
                    ConnectionToTransportMap::NodeType* node = (*it);
                    IConnectionPtr spData2 = node->second;
                    m_transports.erase(node->first);
                    m_transports.push_back(node->first, spData2);
                }
                break;
            }
*/

            if (numBytes < 0)
            {
                EE_LOG(efd::kNetMessage,
                    efd::ILogger::kERR2,
                    ("Receive error %s",
                    spData->GetRemoteConnectionID().ToString().c_str()));

                // if senderConnectionID got set, then it is the id of the connection we want to 
                // close.  If it didn't get set some other error happened and we just need to close
                // the connection we received from.
                if (senderConnectionID != kCID_INVALID)
                {
                    CloseConnection(senderConnectionID);
                }
                else
                {
                    CloseConnection(spData->GetRemoteConnectionID());
                }

                // Break to avoid problems with removing while iterating...
                break;
            }
        }
    }
#endif
}


//-------------------------------------------------------------------------------------------------
void NetTransportTCP::SendMessages()
{
    if (m_running)
    {
        ConnectionToTransportMap::iterator iterData;
        // Loop through all the services and initialize them
        for (iterData = m_transports.begin();
            iterData != m_transports.end();
            ++iterData)
        {
            IConnection* pConnection = (*iterData)->second;

            EE_ASSERT(pConnection);
            pConnection->SendMessages();
        }
        // Loop through all the services and initialize them
        for (iterData = m_incomingConnections.begin();
            iterData != m_incomingConnections.end();
            ++iterData)
        {
            IConnection* pConnection = (*iterData)->second;
            EE_ASSERT(pConnection);
            pConnection->SendMessages();
        }
    }
}


//-------------------------------------------------------------------------------------------------
void NetTransportTCP::ReceiveMessage(INetCallback* pMessageCallback,
                                                  efd::EnvelopeMessage* pEnvelopeMessage)
{
    pMessageCallback->HandleNetMessage(pEnvelopeMessage, pEnvelopeMessage->GetSenderConnection());
}


//-------------------------------------------------------------------------------------------------
void NetTransportTCP::SendConnectionUpdate(const ConnectionID& cid,
                                                       INetCallback* pCallback,
                                                       NetMessage* pStatusMessage)
{
    pStatusMessage->SetSenderConnection(cid);
    if (pCallback)
    {
        pCallback->HandleNetMessage(pStatusMessage, cid);
    }
}


//-------------------------------------------------------------------------------------------------
void NetTransportTCP::SendConnectionClosed(const ConnectionID& cid,
                                                        INetCallback* pCallback)
{
    NetMessagePtr spMsg = EE_NEW MessageWrapper< NetMessage, kMSGID_ConnectionDisconnectedMsg>;
    spMsg->SetSenderConnection(cid);
    if (pCallback)
    {
        pCallback->HandleNetMessage(spMsg, cid);
    }
}


//-------------------------------------------------------------------------------------------------
bool NetTransportTCP::Send(EnvelopeMessage* pMessage)
{
    bool success = false;
    ConnectionToTransportMap::iterator iterData;
    for (iterData = m_transports.begin();
        iterData != m_transports.end();
        ++iterData)
    {
        IConnection* pNetData = (*iterData)->second;

        success = pNetData->Send(pMessage,pNetData->GetRemoteConnectionID());
        if(!success)
        {
            CloseConnection(pNetData->GetRemoteConnectionID());
        }
    }
    for (iterData = m_incomingConnections.begin();
        iterData != m_incomingConnections.end();
        ++iterData)
    {
        IConnection* pNetData = (*iterData)->second;

        success = pNetData->Send(pMessage,pNetData->GetRemoteConnectionID());
        if(!success)
        {
            CloseConnection(pNetData->GetRemoteConnectionID());
        }
    }
    return success;
}


//-------------------------------------------------------------------------------------------------
bool NetTransportTCP::Send(EnvelopeMessage* pMessage, const ConnectionID& cid)
{
    bool success = false;
    IConnection* pNetData = NULL;

    if (cid.GetQualityOfService() & NET_CONNECTIONLESS)
    {
        ConnectionID realConnectionID = 
            ConnectionID(cid.GetQualityOfService(),0,cid.GetLocalPort(),0);
        pNetData = GetTransportData(realConnectionID);
    }
    else
    {
        pNetData = GetTransportData(cid);
    }

    if(pNetData)
    {
        success = pNetData->Send(pMessage, cid);
        if(!success)
        {
            CloseConnection(cid);
        }
    }
    else
    {
        EE_LOG(efd::kNetMessage,
            efd::ILogger::kERR1,
            ("Send could not find %s",
            cid.ToString().c_str()));
    }
    return success;
}


//-------------------------------------------------------------------------------------------------
void NetTransportTCP::CloseConnection(const ConnectionID& cid)
{
    m_transportsToClose.insert(cid);
}


//-------------------------------------------------------------------------------------------------
void NetTransportTCP::CloseWaitingConnections(bool sendNotification)
{
    if (!m_transportsToClose.empty())
    {
        ConnectionIDSet tmpList = m_transportsToClose;
        m_transportsToClose.clear();

        // Loop through all the services and initialize them
        for (ConnectionIDSet::iterator iterClose = tmpList.begin();
            iterClose != tmpList.end();
            ++iterClose)
        {
            const ConnectionID& cid = *iterClose;
            // close any pending connections
            IConnection* pNetData = GetTransportData(cid);
            if (pNetData != NULL)
            {
                EE_ASSERT(cid == pNetData->GetRemoteConnectionID());

                EE_LOG(efd::kNetMessage,
                    efd::ILogger::kLVL1,
                    ("CloseWaitingConnections Closing %s",
                    cid.ToString().c_str()));
                pNetData->Close();

                // Send a local message letting any services that care know
                // that this transport connection has been closed....
                if (sendNotification)
                {
                    SendConnectionClosed(cid, pNetData->GetConnectionCallback());
                }
                m_transports.erase(cid);
                m_incomingConnections.erase(cid);
                m_connectionsPending.erase(cid);
                m_listenList.erase(cid);
            }
            else
            {
                EE_LOG(efd::kNetMessage,
                    efd::ILogger::kERR1,
                    ("CloseWaitingConnections could not find %s",
                    cid.ToString().c_str()));
            }
        }
    }
}


//-------------------------------------------------------------------------------------------------
void NetTransportTCP::CloseAllConnections()
{
    ConnectionToTransportMap::iterator iterData;
    for (iterData = m_transports.begin();
        iterData != m_transports.end();
        ++iterData)
    {
        CloseConnection((*iterData)->first);
    }
    for (iterData = m_incomingConnections.begin();
        iterData != m_incomingConnections.end();
        ++iterData)
    {
        CloseConnection((*iterData)->first);
    }
    for (iterData = m_connectionsPending.begin();
        iterData != m_connectionsPending.end();
        ++iterData)
    {
        CloseConnection((*iterData)->first);
    }
    for (iterData = m_listenList.begin();
        iterData != m_listenList.end();
        ++iterData)
    {
        CloseConnection((*iterData)->first);
    }
    CloseWaitingConnections(false);
}


//-------------------------------------------------------------------------------------------------
IConnection* NetTransportTCP::GetTransportData(const ConnectionID& cid)
{
    IConnectionPtr pRetVal = NULL;
    ConnectionToTransportMap::NodeType* node = m_transports.find(cid);
    if(!node)
    {
        node = m_incomingConnections.find(cid);
    }
    if(!node)
    {
        node = m_listenList.find(cid);
    }
    if(!node)
    {
        node = m_connectionsPending.find(cid);
    }
    if(!node)
    {
        return NULL;
    }
    pRetVal = (*node).second;
    return pRetVal;
}


//-------------------------------------------------------------------------------------------------
void NetTransportTCP::RemoveCallback(
    INetCallback* pCallbackToRemove,
    INetCallback* pReplacementCallback)
{
    ConnectionToTransportMap::iterator iterData;
    for (iterData = m_transports.begin();
        iterData != m_transports.end();
        ++iterData)
    {
        ((*iterData)->second)->RemoveCallback(pCallbackToRemove, pReplacementCallback);
    }
    for (iterData = m_incomingConnections.begin();
        iterData != m_incomingConnections.end();
        ++iterData)
    {
        ((*iterData)->second)->RemoveCallback(pCallbackToRemove, pReplacementCallback);
    }
}

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

#include <efdNetwork/ConnectionTCP.h>
#include <efdNetwork/TCPSocket.h>
#include <efdNetwork/HostInfo.h>
#include <efd/StreamMessage.h>
#include <efdNetwork/Net.h>

//-------------------------------------------------------------------------------------------------
using namespace efd;

const utf8string ConnectionTCP::ms_pVersionName = "NetTransportTCP";
const utf8string ConnectionTCP::ms_pVersionValue = "0.1.1";
const efd::UInt32 ConnectionTCP::ms_numVersions = 2;


//-------------------------------------------------------------------------------------------------
ConnectionTCP::ConnectionTCP(const ConnectionTCP& rhs, ConnectionID remoteConnectionID)
    : m_spTCPSocket(rhs.m_spTCPSocket)
    , m_versionsValid(false)
    , m_pMessageCallback(rhs.m_pMessageCallback)
    , m_pConnectionCallback(rhs.m_pConnectionCallback)
    , m_versionsSent(false)
    , m_connectionState(EE_CONN_ACCEPTED)
    , m_isIncomingConnection(true)
    , m_qos(rhs.m_qos)
    , m_pMessageFactory(rhs.m_pMessageFactory)
    , m_remoteConnectionID(remoteConnectionID)
    , m_isListening(false)
{
    EE_ASSERT(m_spTCPSocket);
    EE_ASSERT(m_pMessageCallback);
    EE_ASSERT(m_pConnectionCallback);
    EE_ASSERT(m_remoteConnectionID != kCID_INVALID);
    EE_ASSERT(m_remoteConnectionID.GetIP() != 0);
    EE_ASSERT(m_remoteConnectionID.GetLocalPort() != 0);
    EE_ASSERT(m_remoteConnectionID.GetRemotePort() != 0);
}


//-------------------------------------------------------------------------------------------------
ConnectionTCP::ConnectionTCP(
    MessageFactory* pMessageFactory,
    QualityOfService qos,
    TCPSocket* pTCPSocket,
    INetCallback* pMessageCallback,
    bool isIncomingConnection)
    : m_spTCPSocket(pTCPSocket)
    , m_versionsValid(false)
    , m_pMessageCallback(pMessageCallback)
    , m_pConnectionCallback(NULL)
    , m_versionsSent(false)
    , m_connectionState(EE_CONN_NOT_CONNECTED)
    , m_isIncomingConnection(isIncomingConnection)
    , m_qos(qos)
    , m_pMessageFactory(pMessageFactory)
    , m_isListening(true)
{
    EE_ASSERT(m_spTCPSocket);
    EE_ASSERT(pMessageCallback);
    m_pConnectionCallback = m_spTCPSocket->GetCallback();
    EE_ASSERT(m_pConnectionCallback);
    m_remoteConnectionID = m_spTCPSocket->GetConnectionID();
}

//-------------------------------------------------------------------------------------------------
ConnectionTCP::ConnectionTCP(
    MessageFactory* pMessageFactory,
    QualityOfService qos,
    INetCallback* pMessageCallback,
    INetCallback* pConnectionCallback)
    : m_spTCPSocket(NULL)
    , m_versionsValid(false)
    , m_pMessageCallback(pMessageCallback)
    , m_pConnectionCallback(pConnectionCallback)
    , m_versionsSent(false)
    , m_connectionState(EE_CONN_NOT_CONNECTED)
    , m_isIncomingConnection(false)
    , m_qos(qos)
    , m_pMessageFactory(pMessageFactory) 
    , m_isListening(false)
{
    m_spTCPSocket = EE_NEW TCPSocket(m_qos, 0,pConnectionCallback);
    EE_ASSERT(m_spTCPSocket);
    EE_ASSERT(pMessageCallback);
    EE_ASSERT(pConnectionCallback);
}


//-------------------------------------------------------------------------------------------------
ConnectionTCP::~ConnectionTCP()
{
    Close();
    m_spTCPSocket = NULL;
}


//-------------------------------------------------------------------------------------------------
bool ConnectionTCP::IsValid()
{
    return m_versionsValid && m_spTCPSocket;
}


//-------------------------------------------------------------------------------------------------
bool ConnectionTCP::VersionChecked()
{
    return m_versionsValid;
}


//-------------------------------------------------------------------------------------------------
bool ConnectionTCP::ValidateVersions(
    DataStream* pStream,
    ConnectionID& versionConnectionID)
{
    if (!pStream)
        return false;

    UInt32 numStrings = 0;
    utf8string versionName;
    utf8string versionValue;
    bool needResponse = false;
    ConnectionID remoteConnectionID;

    pStream->Reset();

    // Stream the message into a binary stream
    (*pStream) >> numStrings;

    // Not enough data for de-serialization
    if( numStrings != ms_numVersions )
        return false;

    *pStream >> versionName;
    *pStream >> versionValue;
    if (m_qos & NET_UDP)
    {
        *pStream >> needResponse;
        *pStream >> remoteConnectionID;

        if (needResponse)
        {
            versionConnectionID = remoteConnectionID;
        }
        else
        {
            remoteConnectionID = GetRemoteConnectionID();
        }
    }

    if ((versionName == ms_pVersionName) && (versionValue == ms_pVersionValue))
    {
        m_versionsValid = true;
        if (m_qos & NET_UDP)
        {
            m_connectionState = EE_CONN_CONNECTED;
            if(needResponse)
            {
                // the other side still needs a response, so we send one
                SendVersions(false,remoteConnectionID);
            }
        }
        return true;
    }
    else
    {
        EE_LOG(efd::kNetMessage,
            efd::ILogger::kERR1,
            ("Error: Failed to validate version numbers on connection %s.   Connection Closing.",
            GetRemoteConnectionID().ToString().c_str() ));
        METRICS_PROBE_METRIC( "NETTRANSPORTTCP.RECEIVE.COUNT.ERROR", 1);
        return false;
    }
}

//-------------------------------------------------------------------------------------------------
efd::SInt32 ConnectionTCP::SendVersions(
    bool needResponse,
    const ConnectionID& responseConnectionID)
{
    EE_ASSERT( m_spTCPSocket );

    /// UDP can only have whole messages sent across
    if (m_qos & NET_UDP)
    {
        efd::StreamMessagePtr spMessage =
            EE_NEW MessageWrapper<StreamMessage, kMSGID_UnreliableVersion>;
        *spMessage << ms_numVersions;
        *spMessage << ms_pVersionName;
        *spMessage << ms_pVersionValue;
        *spMessage << needResponse;
        *spMessage << responseConnectionID;
        spMessage->Reset();
        efd::EnvelopeMessagePtr spEnvelopeMessage = 
            EE_NEW MessageWrapper< EnvelopeMessage, kMSGID_ConnectionDataReceivedMsg>;
        spEnvelopeMessage->SetDestinationCategory(kCAT_NetSystem);
        spEnvelopeMessage->SetQualityOfService(GetTransportType());
        spEnvelopeMessage->SetChild(spMessage);
        if (Send(spEnvelopeMessage, m_remoteConnectionID))
        {
            m_versionsSent = true;
            return spMessage->GetStreamSize();
        }
        else
        {
            return EE_SOCKET_ERROR_UNKNOWN;
        }
    }
    else
    {
        if (!m_spSendVersionStream)
        {
            // Stream the message into a binary stream
            m_spSendVersionStream = EE_NEW DataStream();
            *m_spSendVersionStream << ms_numVersions;
            *m_spSendVersionStream << ms_pVersionName;
            *m_spSendVersionStream << ms_pVersionValue;
            m_spSendVersionStream->Reset();
        }

        // if here, have a valid net data object, go ahead and send it...
        // Shove it through the TCP Socket
        efd::SInt32 numBytes = m_spTCPSocket->Send(m_spSendVersionStream);
        if (numBytes > 0)
            m_versionsSent = true;
        return numBytes;
    }
}


//-------------------------------------------------------------------------------------------------
efd::SInt32 ConnectionTCP::ReceiveVersions(
    efd::EnvelopeMessage* pEnvelopeMessage,
    ConnectionID& versionConnectionID)
{
    EE_ASSERT(pEnvelopeMessage);
    const IMessage* pMessage = pEnvelopeMessage->GetContents(m_pMessageFactory);
    if(!pMessage)
    {
        return EE_SOCKET_ERROR_UNKNOWN;
    }

    efd::StreamMessagePtr spStreamMessage = 
        EE_DYNAMIC_CAST(efd::StreamMessage,const_cast<IMessage*>(pMessage));

    if(!spStreamMessage)
    {
        return EE_SOCKET_ERROR_UNKNOWN;
    }

    EE_ASSERT(spStreamMessage->GetClassID() == kMSGID_UnreliableVersion);

    DataStreamPtr spDataStream;
    spStreamMessage->GetStream(spDataStream);
    if(!spDataStream)
    {
        return EE_SOCKET_ERROR_UNKNOWN;
    }

    if(!ValidateVersions(spDataStream,versionConnectionID))
    {
        return EE_SOCKET_ERROR_UNKNOWN;
    }
    return spStreamMessage->GetStreamSize();
}


//-------------------------------------------------------------------------------------------------
efd::SInt32 ConnectionTCP::ReceiveVersions(ConnectionID& senderConnectionID)
{
    /// UDP can only have whole messages sent across
    if (m_qos & NET_UDP)
    {
        static UInt32 tickCount = 0;
        if (tickCount%5 == 0)
        {
            // we are unreliable so send our versions again to make sure they got through
            SendVersions(true,GetRemoteConnectionID());
        }
        // Listening an incoming connections have data pulled off of the socket by NetTransportTCP
        if (!m_isListening && !m_isIncomingConnection)
        {
            efd::EnvelopeMessagePtr spEnvelopeMessage;
            efd::SInt32 numBytes = ReceiveMessage(spEnvelopeMessage, senderConnectionID);
            if (spEnvelopeMessage)
            {
                ConnectionID versionConnectionID;
                senderConnectionID = spEnvelopeMessage->GetSenderConnection();
                return ReceiveVersions(spEnvelopeMessage,versionConnectionID);
            }
            return numBytes;
        }
        else
        {
            return EE_SOCKET_NO_DATA;
        }
    }
    else
    {
        DataStreamPtr spStream;
        ConnectionID versionConnectionID;
        SInt32 numBytes = ReceiveStream(spStream, senderConnectionID);
        if (numBytes > 0)
        {
            if (!ValidateVersions(spStream,versionConnectionID))
            {
                EE_LOG(efd::kNetMessage,
                    efd::ILogger::kERR1,
                    ("Connect (%s) Failed to validate version information!",
                    senderConnectionID.ToString().c_str()));
                return EE_SOCKET_CONNECTION_FAILED;
            }
            else
            {
                return spStream->GetSize();
            }
        }
        else
        {
            return numBytes;
        }
    }
}

//-------------------------------------------------------------------------------------------------
efd::SInt32 ConnectionTCP::ReceiveStream(DataStreamPtr& spStream, ConnectionID& senderConnectionID)
{
    if ( !m_spTCPSocket )
    {
        return EE_SOCKET_ERROR_UNKNOWN;
    }

    // receive from the client
    efd::SInt32 numBytes = 0;

    // Attempt to read a message from the socket
    if (m_qos & NET_RELIABLE)
    {
        numBytes = m_spTCPSocket->Receive(spStream);
        senderConnectionID = GetRemoteConnectionID();
    }
    else
    {
        numBytes = m_spTCPSocket->ReceiveFrom(spStream, senderConnectionID);
    }
    if ( numBytes == EE_SOCKET_NO_DATA)
        return EE_SOCKET_NO_DATA;
    if ( numBytes == EE_SOCKET_ERROR_UNKNOWN)
    {
        EE_LOG(efd::kNetMessage,
            efd::ILogger::kERR1,
            ("Error: Failed to read from socket.  Connection Closing..."));
        METRICS_PROBE_METRIC( "NETTRANSPORTTCP.RECEIVE.COUNT.ERROR", 1);
        // The Socket is blocking so we had an error and should close the socket...
        return EE_SOCKET_ERROR_UNKNOWN;
    }
    if ( numBytes == EE_SOCKET_CONNECTION_CLOSED)
    {
        EE_LOG(efd::kNetMessage,
            efd::ILogger::kLVL1,
            ("Connection %s closed gracefully.",
            senderConnectionID.ToString().c_str() ));
        return EE_SOCKET_CONNECTION_CLOSED;
    }
    if ( numBytes == EE_SOCKET_SHUTDOWN)
    {
        EE_LOG(efd::kNetMessage,
            efd::ILogger::kLVL1,
            ("Connection %s shutdown in progress.",
            senderConnectionID.ToString().c_str() ));
        return EE_SOCKET_SHUTDOWN;
    }
    // we should be handling any error conditions
    EE_ASSERT(numBytes > 0);
    spStream->Reset();
    return numBytes;
}

//-------------------------------------------------------------------------------------------------
efd::SInt32 ConnectionTCP::ReceiveMessage(
    EnvelopeMessagePtr& spEnvelopeMessage, 
    ConnectionID& remoteConnectionID)
{
    DataStreamPtr spStream;
    efd::SInt32 numBytes = ReceiveStream(spStream, remoteConnectionID);
    if (numBytes <= 0)
    {
        return numBytes;
    }
    spStream->Reset();

    spEnvelopeMessage = EE_NEW MessageWrapper< EnvelopeMessage, kMSGID_ConnectionDataReceivedMsg >;

    // Read the data from the stream into the message
    if (!spEnvelopeMessage->FromStream(*spStream))
    {
        spEnvelopeMessage = NULL;
        return EE_SOCKET_ERROR_UNKNOWN;
    }

    EE_MESSAGE_BREAK(
        spEnvelopeMessage,
        IMessage::mdf_BreakOnNetReceive,
        ("%s| Net Receive from process %d on connection %s to category %s",
        spEnvelopeMessage->GetDescription().c_str(),
        spEnvelopeMessage->GetSenderNetID(),
        GetRemoteConnectionID().ToString().c_str(),
        spEnvelopeMessage->GetDestinationCategory().ToString().c_str()) );

    // Set the name of the transport connection that received the message
    spEnvelopeMessage->SetSenderConnection(remoteConnectionID);
    spEnvelopeMessage->SetQualityOfService(m_qos);
    // Clear the pointer to the stream because the message holds it for future use
    spStream = NULL;

    METRICS_PROBE_METRIC( "NETTRANSPORTTCP.RECEIVE.COUNT.IN", 1);
    METRICS_PROBE_METRIC( "NETTRANSPORTTCP.RECEIVE.BYTES.IN", int(spStream->GetSize()));
    return numBytes;
}


//-------------------------------------------------------------------------------------------------
efd::SInt32 ConnectionTCP::SendMessages()
{
    int maxBytesPerTick = 3072;
    int maxMessagesPerTick = 32;

    bool queueEmpty = true;
    while( (!m_messageQueue.empty()) && maxMessagesPerTick-- && maxBytesPerTick > 0)
    {
        queueEmpty = false;
        MessageToSend messageToSend;
        m_messageQueue.pop_front( messageToSend );
        int retVal = InternalSend( messageToSend );
        // if InternalSend returns 0 it means there is no socket to send to
        if( retVal == EE_SOCKET_CONNECTION_CLOSED )
        {
            EE_LOG(efd::kNetMessage,
                efd::ILogger::kERR2,
                ( "ConnectionTCP::SendMessages %d in queue, socket closed!",
                (int)m_messageQueue.size() ));
            m_messageQueue.clear();
            return EE_SOCKET_CONNECTION_CLOSED;
        }
        if(retVal == EE_SOCKET_MESSAGE_QUEUED)
        {
            m_messageQueue.push_front( messageToSend );
            return (int)m_messageQueue.size();
        }
        if(retVal < 0)
        {
            return EE_SOCKET_ERROR_UNKNOWN;
        }
        EE_LOG(efd::kNetMessage,
            efd::ILogger::kERR2,
            ( "ConnectionTCP::SendMessages %d in queue", (int)m_messageQueue.size() )
            );
        maxBytesPerTick -= retVal;
    }
    if( !queueEmpty )
    {
        EE_LOG(efd::kNetMessage,
            efd::ILogger::kERR2,
            ( "ConnectionTCP::SendMessages %d in queue", (int)m_messageQueue.size() )
            );
    }
    return (int)m_messageQueue.size();
}


//-------------------------------------------------------------------------------------------------
ConnectionID ConnectionTCP::GetRemoteConnectionID() const
{
    if (m_remoteConnectionID == kCID_INVALID)
    {
        EE_ASSERT(m_spTCPSocket);
        return m_spTCPSocket->GetConnectionID();
    }
    else
    {
        return m_remoteConnectionID;
    }
}


//-------------------------------------------------------------------------------------------------
bool ConnectionTCP::Send(EnvelopeMessage* pMessage, const ConnectionID& cid)
{
    // Stream the message into a binary stream
    DataStreamPtr spDataStreamToSend = EE_NEW DataStream;
    EE_ASSERT(spDataStreamToSend);
    pMessage->ToStream(*spDataStreamToSend);

    MessageToSend messageToSend(pMessage, spDataStreamToSend, cid);
    EE_ASSERT(messageToSend.m_spMessageToSend);
    EE_ASSERT(messageToSend.m_spDataStreamToSend);

    if (cid.GetIP() == INADDR_BROADCAST)
    {
        m_spTCPSocket->setSocketBroadcast(1);
    }

    if( m_messageQueue.empty() )
    {
        int retVal = InternalSend(messageToSend);
        if( retVal == EE_SOCKET_MESSAGE_QUEUED )
        {
            m_messageQueue.push_front( messageToSend );
            return true;
        }

        if( retVal < 0 )
        {
            return false;
        }
    }
    else
    {
        m_messageQueue.push_back( messageToSend );
    }
    return true;
}

//-------------------------------------------------------------------------------------------------
efd::SInt32 ConnectionTCP::InternalSend(MessageToSend& messageToSend)
{
    EE_ASSERT( messageToSend.m_spMessageToSend );
    EnvelopeMessagePtr pMessage = messageToSend.m_spMessageToSend;

    // Make sure the transport data has a valid socket
    if (m_spTCPSocket)
    {
        EE_MESSAGE_BREAK(
            pMessage,
            IMessage::mdf_BreakOnNetSend,
            ("%s| %s Net Send to %s on transport %s",
            pMessage->GetDescription().c_str(),
            pMessage->GetChildDescription().c_str(),
            pMessage->GetDestinationCategory().ToString().c_str(),
            GetRemoteConnectionID().ToString().c_str() ) );

        METRICS_PROBE_METRIC( "NETTRANSPORTTCP.RECEIVE.COUNT.OUT", 1);
        METRICS_PROBE_METRIC( "NETTRANSPORTTCP.RECEIVE.BYTES.OUT", int(strm.GetSize()));

        // if here, have a valid net data object, go ahead and send it...
        // Shove it through the TCP Socket
        int retVal = 0;
        if (m_qos & NET_UDP)
        {
            ConnectionID cid = GetRemoteConnectionID();
            EE_ASSERT(m_remoteConnectionID != kCID_INVALID);
            retVal = m_spTCPSocket->SendTo(
                messageToSend.m_spDataStreamToSend,
                messageToSend.m_destCid);
        }
        else
        {
            retVal = m_spTCPSocket->Send(messageToSend.m_spDataStreamToSend);
        }
        if ( retVal == EE_SOCKET_MESSAGE_QUEUED )
        {
            return EE_SOCKET_MESSAGE_QUEUED;
        }

        if ( retVal <= 0 )
        {
            EE_LOG( efd::kMessageTrace, efd::ILogger::kERR1,
                ("%s| Failed Send on connection %s",
                pMessage->GetDescription().c_str(), GetRemoteConnectionID().ToString().c_str() ) );
            return EE_SOCKET_MESSAGE_NOT_SENT;
        }
        return retVal;
    }

    EE_LOG( efd::kMessageTrace, efd::ILogger::kERR1,
        ("%s| m_spTCPSocket is NULL on transport %s",
        pMessage->GetDescription().c_str(), GetRemoteConnectionID().ToString().c_str() ) );

    return EE_SOCKET_CONNECTION_CLOSED;
}


//-------------------------------------------------------------------------------------------------
void ConnectionTCP::Close()
{
    if (!(m_qos & NET_UDP) || !m_isIncomingConnection )
    {
        if (m_spTCPSocket)
        {
            EE_ASSERT(m_spTCPSocket->GetConnectionID() == m_remoteConnectionID
                || m_remoteConnectionID == kCID_INVALID);
            m_spTCPSocket->Shutdown();
        }
    }
}


//-------------------------------------------------------------------------------------------------
void ConnectionTCP::RemoveCallback(
    INetCallback* pCallbackToRemove,
    INetCallback* pReplacementCallback)
{
    if( m_spTCPSocket && m_spTCPSocket->GetCallback() == pCallbackToRemove)
    {
        m_spTCPSocket->SetCallback( pReplacementCallback );
    }
    if( m_pMessageCallback == pCallbackToRemove )
        m_pMessageCallback = pReplacementCallback;
    if( m_pConnectionCallback == pCallbackToRemove )
        m_pConnectionCallback = pReplacementCallback;
}


//-------------------------------------------------------------------------------------------------
efd::SmartPointer< ConnectionTCP > ConnectionTCP::Listen(
    MessageFactory* pMessageFactory,
    efd::QualityOfService qos,
    efd::UInt16 portListen, 
    INetCallback* pMessageCallback, 
    INetCallback* pConnectionCallback)
{
    // open socket on the local host(server) and show its configuration
    TCPSocketPtr spTCPSocket = EE_NEW TCPSocket(qos, portListen, pConnectionCallback);

    // bind the server to the socket
    if( !spTCPSocket->Bind() )
    {
        EE_LOG(efd::kNetMessage,
            efd::ILogger::kLVL1,
            ("Server binding process failed! port = %d", portListen));
        return NULL;
    }
    EE_LOG(efd::kNetMessage,
        efd::ILogger::kLVL1,
        ("Finished the server binding process. port = %d", portListen));

    // Set the TCP Socket non-blocking
    spTCPSocket->setSocketBlocking( false);

    // server starts to wait for client calls
    if (qos & NET_TCP) // udp does not need or want listen called
    {
        spTCPSocket->Listen();
    }
    EE_LOG(efd::kNetMessage,
        efd::ILogger::kLVL1,
        ("Server now awaiting accepting client connections on port %d", portListen));

    return EE_NEW ConnectionTCP(pMessageFactory, qos, spTCPSocket, pMessageCallback, false);
}


//-------------------------------------------------------------------------------------------------
efd::SmartPointer< ConnectionTCP > ConnectionTCP::Connect(
    MessageFactory* pMessageFactory,
    efd::QualityOfService qos,
    const efd::utf8string& strServerAddress,
    efd::UInt16 portServer,
    INetCallback* pMessageCallback,
    INetCallback* pConnectionCallback)
{
    ConnectionTCPPtr spTransportData = 
        EE_NEW ConnectionTCP(pMessageFactory, qos, pMessageCallback, pConnectionCallback);

    EE_ASSERT(spTransportData);
    EE_ASSERT(spTransportData->m_spTCPSocket);

    TCPSocket* pTCPSocket = spTransportData->m_spTCPSocket;

    // connect to the server.
    efd::SInt32 retVal = EE_SOCKET_CONNECTION_COMPLETE;

    // only actually call connect if we are not in connectionless mode
    if (!(qos & NET_CONNECTIONLESS))
    {
        retVal = pTCPSocket->Connect(strServerAddress, portServer);
    }
    else
    {
        pTCPSocket->Bind();
        retVal = EE_SOCKET_CONNECTION_COMPLETE;
    }

    if (retVal == EE_SOCKET_CONNECTION_IN_PROGRESS ||
        retVal == EE_SOCKET_CONNECTION_COMPLETE)
    {
        spTransportData->m_remoteConnectionID = pTCPSocket->GetConnectionID();
        return spTransportData;
    }
    else
    {
        return NULL;
    }
}


//-------------------------------------------------------------------------------------------------
efd::SInt32 ConnectionTCP::CompleteConnection()
{
    ConnectionID remoteConnectionID = GetRemoteConnectionID();
    efd::utf8string strServerAddress = HostInfo::IPToString(remoteConnectionID.GetIP());
    efd::UInt16 portServer = remoteConnectionID.GetRemotePort();

    switch (m_connectionState)
    {
    case EE_CONN_NOT_CONNECTED:
        {
            efd::SInt32 numBytes = m_spTCPSocket->Connect(strServerAddress, portServer);
            if (numBytes == EE_SOCKET_CONNECTION_COMPLETE)
            {
                EE_LOG(efd::kNetMessage,
                    efd::ILogger::kLVL1,
                    ("Connected to %s!", GetRemoteConnectionID().ToString().c_str()));
                m_connectionState = EE_CONN_SEND_VERSIONS;
                return EE_SOCKET_CONNECTION_IN_PROGRESS;
            }
            return numBytes;
        }
    case EE_CONN_ACCEPTED:
        EE_ASSERT(m_isIncomingConnection);
        m_connectionState = EE_CONN_SEND_VERSIONS;
        // fall through
    case EE_CONN_SEND_VERSIONS:
        {
            efd::SInt32 numBytes = SendVersions(true,GetRemoteConnectionID());
            if(numBytes < 0 && numBytes != EE_SOCKET_MESSAGE_QUEUED)
            {
                METRICS_PROBE_METRIC( "NETTRANSPORTTCP.CONNECTION.COUNT.OUT", 1);
                EE_LOG(efd::kNetMessage,
                    efd::ILogger::kERR1,
                    ("Connect (%s) Failed to send version information!",
                    GetRemoteConnectionID().ToString().c_str()));
                return EE_SOCKET_CONNECTION_FAILED;
            }
            else
            {
                m_connectionState = EE_CONN_RECEIVE_VERSIONS;
                return EE_SOCKET_CONNECTION_IN_PROGRESS;
            }
        }
    case EE_CONN_RECEIVE_VERSIONS:
        {
            ConnectionID senderConnectionID;
            // Attempt to receive version strings
            efd::SInt32 numBytes = ReceiveVersions(senderConnectionID);
            if (numBytes > 0)
            {
                m_connectionState = EE_CONN_CONNECTED;
                return EE_SOCKET_CONNECTION_COMPLETE;
            }
            else
            {
                return numBytes;
            }
        }
    case EE_CONN_CONNECTED:
        return EE_SOCKET_CONNECTION_COMPLETE;
    case EE_CONN_CONNECTION_FAILED:
        EE_FAIL("ConnectionTCP::Connect called after connection failure!");
        return EE_SOCKET_CONNECTION_FAILED;
    default:
        EE_FAIL("ConnectionTCP::Connect unhandled state!");
        return EE_SOCKET_CONNECTION_FAILED;
    }
}


//-------------------------------------------------------------------------------------------------
IConnectionPtr ConnectionTCP::Accept()
{
    TCPSocketPtr spTCPSocket = m_spTCPSocket->Accept();
    if (spTCPSocket)
    {
        ConnectionTCPPtr spTCPData = 
            EE_NEW ConnectionTCP(m_pMessageFactory, m_qos, spTCPSocket, m_pMessageCallback, true);
        spTCPData->m_isIncomingConnection = true;
        spTCPData->m_connectionState = EE_CONN_ACCEPTED;
        IConnectionPtr spIConnection = (ConnectionTCP*)spTCPData;
        return spIConnection;
    }
    return NULL;
}


//-------------------------------------------------------------------------------------------------
NetMessagePtr ConnectionTCP::GetStatusMessage()
{
    if (m_isIncomingConnection)
    {
        if(m_connectionState == EE_CONN_CONNECTED)
        {
            return EE_NEW MessageWrapper<NetMessage, kMSGID_ConnectionAcceptedMsg>;
        }
        else
        {
            return EE_NEW MessageWrapper<NetMessage, kMSGID_ConnectionFailedToAcceptMsg>;
        }
    }
    else
    {
        if(m_connectionState == EE_CONN_CONNECTED)
        {
            return EE_NEW MessageWrapper<NetMessage, kMSGID_ConnectionConnectedMsg>;
        }
        else
        {
            return EE_NEW MessageWrapper<NetMessage, kMSGID_ConnectionFailedToConnectMsg>;
        }
    }
}

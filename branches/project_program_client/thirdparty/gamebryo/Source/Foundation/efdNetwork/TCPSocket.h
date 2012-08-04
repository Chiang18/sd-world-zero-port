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

#pragma once
#ifndef EE_TCPSOCKET_H
#define EE_TCPSOCKET_H

/** @file TCPSocket.h
    Platform neutral wrapper for a bsd TCP socket. Internal use only.
*/
#include <efd/OS.h>
#include <efd/UniversalTypes.h>
#include <efd/INetCallback.h>

#include <efdNetwork/Net.h>
#include <efdNetwork/SocketErrors.h>

#include <efd/utf8string.h>
#include <efd/SmartPointer.h>
#include <efd/DataStream.h>
#include <efd/ConnectionID.h>
#include <efdNetwork/INetLib.h>
#include <efdNetwork/efdNetworkLibType.h>

namespace efd
{

// so far we only consider the TCP socket, UDP will be added in later release
//const int MAX_RECV_LEN = 1048576;
const int MAX_RECV_LEN = 8096;
const int MAX_MSG_LEN = 1024;

/**
    Abstract representation of a socket connection. 
 */
class EE_EFDNETWORK_ENTRY Socket : public efd::MemObject
{
    EE_DECLARE_ABSTRACT_REFCOUNT;
public:

    /// @name Construction and Destruction
    //@{

    /// given a port number, create a socket.
    Socket(efd::UInt16 listenPort, QualityOfService qos);

    /**
      Constructor. used for creating instances dedicated to client communication.
      @param socketId socket id of the socket created by call to accept
      @param localAddr sockaddr_in for the local end of the socket
      @param remoteAddr sockaddr_in for the remote end of the socket
      @param qos Quality of service.
     */
    Socket(
        SOCKET socketId,
        struct sockaddr_in& localAddr,
        struct sockaddr_in& remoteAddr,
        QualityOfService qos);

    virtual ~Socket();
    //@}

public:
    
    /// @name Socket options: 1=On, 0=Off
    //@{
    void setDebug(int);
    void setReuseAddr(int);
    void setKeepAlive(int);
    void setLingerOnOff(bool);
    void setLingerSeconds(int);
    void setSocketBlocking(bool);
    void setSocketBroadcast(int);
    //@}

    /// @name Set the size of the send and receive buffers.
    //@{
    void setSendBufSize(int);
    void setReceiveBufSize(int);
    //@}

    /// @name Get socket options settings. 1=On, 0=Off.
    //@{
    int  getDebug();
    int  getReuseAddr();
    int  getKeepAlive();
    int  getSendBufSize();
    int  getReceiveBufSize();
    bool getSocketBlocking() { return m_blocking; }
    int  getLingerSeconds();
    bool getLingerOnOff();
    //@}

    /// Get the current socket identifier.
    SOCKET getSocketId() { return m_socketId; }

    /// Get the port of the local side of the socket connection.
    efd::UInt16 getLocalPort();

    /// Get the ip of the local side of the socket connection.
    efd::UInt32 getLocalIP();

    /// Get the port of the remote side of the socket connection.
    efd::UInt16 getRemotePort();

    /// Get the ip of the remote side of the socket connection.
    efd::UInt32 getRemoteIP();

protected:
    efd::UInt32 getError();
    efd::utf8string getErrorMessage();
    efd::utf8string getErrorMessage( efd::UInt32 errorNum );

    SOCKET m_socketId;          // Socket file descriptor
    bool m_blocking;        // Blocking flag
    struct sockaddr_in m_localAddr;     // local Address/port
    struct sockaddr_in m_remoteAddr;    // Address of the remote side of the connection
    QualityOfService m_qos; // Used to determine the type of socket created, TCP, UDP, etc

private:

    /// not used
    Socket() {}
};
typedef efd::SmartPointer<Socket> SocketPtr;


/**
    An implementation of Socket that uses TCP as the transport mechanism.
 */
class EE_EFDNETWORK_ENTRY TCPSocket : public Socket
{
    EE_DECLARE_CONCRETE_REFCOUNT;
public:

    /**
       Constructor. used for creating instances dedicated to client
       communication.
       @param qos Quality of service.
       @param socketId socket id of the socket created by call to accept
       @param localAddr sockaddr_in for the local end of the socket
       @param remoteAddr sockaddr_in for the remote end of the socket
       @param pCallback Callback pointer.
     */
    TCPSocket(
        efd::QualityOfService qos,
        SOCKET socketId,
        struct sockaddr_in& localAddr,
        struct sockaddr_in& remoteAddr,
        INetCallback* pCallback);

    /**
       Used to create a new TCP socket given a local port to listen to.
     */
    TCPSocket( efd::QualityOfService qos, efd::UInt16 listenPort, INetCallback* pCallback );

    virtual ~TCPSocket();

    /**
        Sends a message to a connected host.
        @param pMessageStream message to send
        @return: number of bytes sent or error
    */
    efd::SInt32 Send( efd::DataStream* pMessageStream );

#if defined(EE_PLATFORM_WIN32)
    /**
        Sends a raw buffer to a connected host.
        @param pData data to send
        @param size size of the data to send
        @return number of bytes sent or error
    */
    efd::SInt32 Send(const efd::Char* pData, efd::SInt32 size);


    /**
        Sends a message to a datagram socket.
        @param pData data to send
        @param size size of the data to send
        @param serverNameOrAddr hostname or ip address of peer to send to
        @param portServer port to send to
        @return number of bytes sent or error
    */
    efd::SInt32 SendTo(
        const efd::Char* pData,
        efd::SInt32 size,
        const efd::utf8string& serverNameOrAddr,
        efd::UInt16 portServer);

    /**
        Receive data and store in the supplied buffer.

        @param pData The buffer to receive data into

        @return The number of bytes received, 0 if no data available
        on a non blocking call, -1 if a socket error occurred
    */
    efd::SInt32 Receive(efd::Char* pData, efd::SInt32 size);

    /**
        Receive a message from a datagram socket
        @param spStream : The stream to fill in with the received data
        @param hostOrderIPAddr hostname or ip address remote sender
        @param hostOrderRemotePort port of remote sender

        @return int : The number of bytes in the stream, 0 if no data available
        on a non blocking call, -1 if a socket error occurred
    */
    efd::SInt32 ReceiveFrom(
        efd::Char* pData,
        efd::SInt32 size,
        efd::UInt32& hostOrderIPAddr,
        efd::UInt16& hostOrderRemotePort);

#endif // defined(EE_PLATFORM_WIN32)

    /**
        Sends a message to a datagram socket.
        @param pMessageStream message to send
        @param destinationConnectionID Destination connection ID. 
        @return: number of bytes sent or error
    */
    efd::SInt32 SendTo(efd::DataStream* pMessageStream, efd::ConnectionID destinationConnectionID);

    /**
        receive messages and stores the message in a buffer.
      
       @param spStream The stream to fill in with the received data
      
       @return int The number of bytes in the stream, 0 if no data available
                on a non blocking call, -1 if a socket error occurred
     */
    efd::SInt32 Receive( efd::DataStreamPtr& spStream );

    /**
        Receive a message from a datagram socket
        @param spStream The stream to fill in with the received data
        @param senderConnectionID The ConnectionID of the sender of the data

        @return int The number of bytes in the stream, 0 if no data available
        on a non blocking call, -1 if a socket error occurred
    */
    efd::SInt32 ReceiveFrom(efd::DataStreamPtr& spStream, efd::ConnectionID& senderConnectionID);

    /**
       Binds the socket to an address and port number
       a server call.  Returns true if the bind succeeded, false if it failed.
    */
    bool Bind();

    /**
       accepts a connecting client.  The address of the connected client
       is stored in the parameter
       a server call
    */
    TCPSocket* Accept();

    /// Listens to connecting clients, a server call
    void Listen(efd::UInt32 numPorts = 5);

    /**
        Connect to a listening server
        @param serverNameOrAddr hostname or ip address of server to connect to
        @param portServer port to connect to
    */
    efd::SInt32 Connect(const efd::utf8string& serverNameOrAddr, efd::UInt16 portServer);

    void Shutdown();

    /// @name INetCallback methods
    //@{

    /// Set the INetCallback method to call when data is recieved in this socket.
    void SetCallback( INetCallback* pCallback ) { m_pCallback = pCallback; };

    /// Get the currently registered INetCallback instance.
    INetCallback* GetCallback() { return m_pCallback; };
    //@}

    /// Get the unique connection ID for this TCP Socket.
    ConnectionID GetConnectionID() 
    {
        return INetLib::GenerateConnectionID(
            m_qos,
            getRemoteIP(),
            getRemotePort(),
            getLocalPort());
    }

protected:
    INetCallback* m_pCallback;
    efd::SInt32 m_SendOffset;
    efd::SInt32 m_ReceiveOffset;
    efd::SInt32 m_ReceiveSize;
    efd::SmartPointer< efd::DataStream > m_spSendMessage;
    efd::SmartPointer< efd::DataStream > m_spReceiveMessage;
    efd::UInt8* m_pSendBuffer;
    efd::UInt8* m_pReceiveBuffer;

};
typedef efd::SmartPointer<TCPSocket> TCPSocketPtr;

} // end namespace efd

#endif // EE_TCPSOCKET_H


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
#ifndef EE_CONNECTIONID_H
#define EE_CONNECTIONID_H

#include <efd/DataStream.h>
#include <efd/NetEnums.h>

namespace efd
{

/**
    A unique id for a connection. The ConnectionID is used throughout the networking layer to
    identify connection instances. This id encapsulates the connection IP address, the local
    port number, the remote port number, and the QualityOfService.
 */
class ConnectionID
{
public:
    
    /// @name Construction and Destruction
    //@{
    /// Construct the default ConnectionID with id 0.
    ConnectionID()
        : m_id( 0ull )
        , m_qos(efd::QOS_INVALID)
    {}

    /// Construct a new ConnectionID from the given identifier.
    ConnectionID(efd::QualityOfService i_qos, efd::UInt64 i_id)
        : m_id(i_id)
        , m_qos(i_qos)
    {}

    /// Construct a new ConnectionID from an IP address, local port number and remote port number.
    ConnectionID(efd::QualityOfService i_qos, efd::UInt32 i_ip, efd::UInt16 i_localPort, efd::UInt16 i_remotePort)
        : m_qos(i_qos)
    {
        m_ipInfo.ip = i_ip;
        m_ipInfo.localPort = i_localPort;
        m_ipInfo.remotePort = i_remotePort;
    }
    //@}

    /// Return true of the rhs ConnectionID is equal to this ID.
    bool operator==(const ConnectionID& rhs) const
    {
        return m_id == rhs.m_id && m_qos == rhs.m_qos;
    }

    /// Return true if the rhs ConnectionID is not equal to this ID.
    bool operator!=(const ConnectionID& rhs) const
    {
        return m_id != rhs.m_id || m_qos != rhs.m_qos;
    }

    /// Return true if the rhs ConnectionID is numerically greater than this ID.
    bool operator>(const ConnectionID& rhs) const
    {
        return (m_id > rhs.m_id) || ( m_id == rhs.m_id && m_qos > rhs.m_qos);
    }

    /// Return true if the rhs ConnectionID is numerically less than this ID.
    bool operator<(const ConnectionID& rhs) const
    {
        return m_id < rhs.m_id || ( m_id == rhs.m_id && m_qos < rhs.m_qos);
    }

    /// returns true if this ConnectionID is valid
    bool IsValid() const
    {
        return m_id != 0ULL;
    }

    /// Get is ID as an unsigned int64.
    efd::UInt64 GetValue() const
    {
        return m_id;
    }

    /// Get the IP address portion of this ID as a unsigned int32.
    efd::UInt32 GetIP() const
    {
        return m_ipInfo.ip;
    }

    /// Get the local port portion of this ID as an unsigned int16.
    efd::UInt16 GetLocalPort() const
    {
        return m_ipInfo.localPort;
    }

    /// Get the remote port portion of this ID as an unsigned int16.
    efd::UInt16 GetRemotePort() const
    {
        return m_ipInfo.remotePort;
    }

    /// Get the remote port portion of this ID as an unsigned int16.
    efd::QualityOfService GetQualityOfService() const
    {
        return m_qos;
    }

    /// Convert this ID into a displayable string.
    efd::utf8string ToString() const
    {
        return efd::utf8string( efd::Formatted, "<ConnID=0x%08X,%d,%d,qos=%d>",
            GetIP(), GetLocalPort(), GetRemotePort(), GetQualityOfService() );
    }

    
    /// @cond EMERGENT_INTERNAL
    // Internal contents of the ConnectionID.

    #if defined(EE_PLATFORM_WII)
    struct IPFields
    {
        efd::UInt32 ip : 32;
        efd::UInt16 localPort : 16;
        efd::UInt16 remotePort : 16;
    };
    #else
    struct IPFields
    {
        efd::UInt64 ip : 32;
        efd::UInt64 localPort : 16;
        efd::UInt64 remotePort : 16;
    };
    #endif

    union
    {
        efd::UInt64 m_id;
        IPFields m_ipInfo;
    };

    efd::QualityOfService m_qos;
    
    EE_COMPILETIME_ASSERT( sizeof(IPFields) == sizeof(efd::UInt64) );

    /// @endcond
};

typedef efd::RefCountedMemObj< efd::set< ConnectionID > > ConnectionIDSet;
typedef efd::SmartPointer< ConnectionIDSet > ConnectionIDSetPtr;

inline efd::DataStream& operator<<( efd::DataStream& i_strm, const ConnectionID& i_val );
inline const efd::DataStream& operator>>( const efd::DataStream& i_strm, ConnectionID& o_val );


static const ConnectionID kCID_INVALID; // use default constructor

} // end namespace efd


// Inline definitions:
#include <efd/ConnectionID.inl>


#endif //EE_CONNECTIONID_H

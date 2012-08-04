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
#ifndef EE_CHANNELMANAGERCONFIG_H
#define EE_CHANNELMANAGERCONFIG_H

#include "efd/ConnectionID.h"

namespace efd
{

enum ConnectionStatus
{
    kCS_Canceled = 0,
    kCS_Failed,
    kCS_Connecting,
    kCS_ReliableConnected,
    kCS_Connected,
    kCS_Disconnected
};

/// @cond EMERGENT_INTERNAL

/// struct for holding ChannelManager configuration information
class ChannelManagerConfig : public efd::MemObject
{
    EE_DECLARE_CONCRETE_REFCOUNT;
public:
    ChannelManagerConfig()
        : m_index(0)
        , m_port(0)
        , m_qualityOfService(efd::QOS_INVALID)
        , m_connectionStatus(0)
    {
    }
    ChannelManagerConfig(
        efd::UInt32 index,
        efd::utf8string hostname,
        efd::UInt16 port,
        efd::QualityOfService qualityOfService)
        : m_index(index)
        , m_hostname(hostname)
        , m_port(port)
        , m_qualityOfService(qualityOfService)
        , m_connectionStatus(0)
    {
    }

    ChannelManagerConfig(const ChannelManagerConfig& rhs)
        : m_index(rhs.m_index)
        , m_hostname(rhs.m_hostname)
        , m_port(rhs.m_port)
        , m_qualityOfService(rhs.m_qualityOfService)
        , m_connectionStatus(rhs.m_connectionStatus)
    {
    }

    ChannelManagerConfig& operator=(const ChannelManagerConfig& rhs)
    {
        m_index = rhs.m_index;
        m_hostname = rhs.m_index;
        m_port = rhs.m_port;
        m_qualityOfService = rhs.m_qualityOfService;
        m_connectionStatus = rhs.m_connectionStatus;
        return *this;
    }

    efd::UInt32 m_index;
    efd::utf8string m_hostname;
    efd::UInt16 m_port;
    efd::QualityOfService m_qualityOfService;
    efd::ConnectionID m_connectionID;
    efd::UInt32 m_connectionStatus;
};
typedef efd::SmartPointer< ChannelManagerConfig > ChannelManagerConfigPtr;

typedef efd::RefCountedMemObj< efd::list< ChannelManagerConfigPtr > > ChannelManagerConfigList;
typedef efd::SmartPointer< ChannelManagerConfigList > ChannelManagerConfigListPtr;

/// @endcond

} //end namespace efd

#endif //EE_CHANNELMANAGERCONFIG_H

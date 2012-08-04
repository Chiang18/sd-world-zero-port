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
#ifndef EE_CHANNELMANAGERSERVICE_H
#define EE_CHANNELMANAGERSERVICE_H

#include <efd/ISystemService.h>
#include <efd/MessageService.h>
#include <efd/StreamMessage.h>

#include <efdNetwork/INetLib.h>
#include <efdNetwork/efdNetworkLibType.h>

#include <efd/ChannelManagerConfig.h>
#include <efd/ChannelSubscriptionList.h>

namespace efd
{
class EnvelopeMessage;
class ConnectionCallback;

/**
    \brief ChannelManagerService receives messages from connected NetServices and routes those
    messages to all other connected NetServices based on Subscription to Categories.
    
    The Network System uses Category subscription to route messages between processes. All 
    processes that wish to communicate must connect to a central ChannelManagerService.
    The ChannelManagerService can exist in a process separate from all clients (client/server)
    or in the same process as a client (peer to peer).  
           
*/
class EE_EFDNETWORK_ENTRY ChannelManagerService : public ISystemService
{
    /// @cond EMERGENT_INTERNAL
    EE_DECLARE_CLASS1(ChannelManagerService, efd::kCLASSID_ChannelManager, ISystemService);
    EE_DECLARE_CONCRETE_REFCOUNT
    /// @endcond
public:

    /// @name Config strings used by this service
    //@{
    //! The default section name for retrieving configuration variables
    const static char* kConfigSection;
    //! ip address that ChannelManagerService listens on
    const static char* kListenIP;
    //! port that gateway should listen for client connections on
    const static char* kListenPort;
    //! hostname that ChannelManagerService listens on
    const static char* kHostname;
    //! port that ChannelManagerService listens on
    const static char* kPort;
    //! Quality of Service that ChannelManagerService listens on and allowed values
    const static char* kQualityOfService;
    const static char* kQOS_Reliable;
    const static char* kQOS_Unreliable;
    const static char* kQOS_Connectionless;
    const static char* kQOS_Invalid;
    //! default numeric port if no configuration is provided
    const static efd::UInt16 defaultListenPort;
    //@}

public:
    /// @name Construction and Destruction
    //@{
    /// Default constructor.
    ChannelManagerService();

    /// Virtual destructor.
    virtual ~ChannelManagerService();
    //@}

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual SyncResult OnPreInit();
    virtual AsyncResult OnInit();
    virtual AsyncResult OnTick();
    virtual AsyncResult OnShutdown();
    const char* GetDisplayName() const
    {
        return "ChannelManager";
    }

     /**
         Receives messages from all connected NetServices.
         @param pEnvelopeMessage Message to handle
    */
    void HandleMessage( const efd::EnvelopeMessage* pEnvelopeMessage );

    /**
        Handle a new incoming connection.
        @param acceptedConnectionID New ConnectionID to add 
    */
    void AddConnection( const efd::ConnectionID& acceptedConnectionID );

    /**
        Handle a disconnection.
        @param acceptedConnectionID Previously added ConnectionID to remove
    */
    void RemoveConnection( const efd::ConnectionID& acceptedConnectionID );

    /**
    Uses the NameResolutionService to announce the ChannelManager on the network

    @param name A name used to identify this particular ChannelManager
    */
    void AnnounceService (const efd::utf8string& name);

    /**
        Returns a pointer to the ChannelManagerConfigList.
    */
    ChannelManagerConfigList* GetConfigList() { return m_spConfigList; }

    /**
        Reads ChannelManager config section and returns a list of ChannelManagerConfig objects
        each describing a ChannelManager/QualityOfService to connect to.
        Allocates a new list that can be used to keep track of connection status.
    */
    static ChannelManagerConfigListPtr ReadConfig(IConfigManager* pConfigManager);

protected:
    /**
        Convert a string read from config file to QualityOfService.  Returns QOS_INVLIAD if
        the string cannot be converted into a QualityOfService.
    */
    static efd::QualityOfService StringToQualityOfService(efd::utf8string stringName);

    /**
        Convert a QualityOfService value to the string equivalent.
    */
    static efd::utf8string QualityOfServiceToString(efd::QualityOfService qos);

    /**
        Assigns a Server/Peer NetID.
    */
    efd::UInt32 AssignNetID();

    /**
        Assigns a Client NetID.
    */
    efd::UInt32 AssignClientNetID();

    efd::INetLibPtr m_spNetLib;
    ConnectionCallback* m_pConnectionCallback;
    /// true if ChannelManager has successfully bound to the m_listenPort
    bool m_listening;
    /// next Server/Peer NetID to be assigned
    efd::UInt32 m_curNetID;
    /// next Client NetID to be assigned
    efd::UInt32 m_curClientNetID;
    efd::MessageServicePtr m_spMessageService;
    efd::utf8string m_netLibType;
    /// Keeps track of config and status information for quality of service connections
    ChannelManagerConfigListPtr m_spConfigList;
    /// keeps track of which NetID/QOS maps to which ConnectionID
    NetIDQosPairToConnectionIDMap m_NetIDQosLookupMap;
    /// list of subscriptions to connections of a QoS that has not yet been established
    ChannelSubscriptionList m_pendingSubscriptions;
};

typedef efd::SmartPointer<ChannelManagerService> ChannelManagerServicePtr;

} // end namespace efd

#endif // EE_CHANNELMANAGERSERVICE_H

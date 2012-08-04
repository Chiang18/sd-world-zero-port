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
#ifndef EE_NETSERVICE_H
#define EE_NETSERVICE_H

#include <efd/IBase.h>
#include <efd/ISystemService.h>
#include <efd/MessageService.h>
#include <efd/CriticalSection.h>
#include <efd/INetListener.h>
#include <efd/StreamMessage.h>
#include <efd/INetCallback.h>
#include <efd/NetMessage.h>
#include <efd/INetService.h>

#include <efdNetwork/INetLib.h>
#include <efdNetwork/NetCategory.h>
#include <efd/ChannelSubscriptionList.h>
#include <efd/GenericFactory.h>

namespace efd
{
class EnvelopeMessage;
class INetTransport;


/**
    \brief NetService connects to a ChannelManagerService.
    
    The NetService is the service that connects to and communicates with the 
    ChannelManagerService. Each client process must have a NetService. When a process wants to
    express interest in aCategory it tells NetService to Subscribe to that Category. NetService
    notifies the ChannelManagerService it is connected to that it is interested in the Category.
    From then on whenever a message is sent from a NetService on that Category the 
    ChannelManagerService routes the message to any process that has expressed interest.
*/
class EE_EFDNETWORK_ENTRY NetService : public INetService
{

    /// @cond EMERGENT_INTERNAL
    // Dont call EE_DECLARE_CLASS1 here -- use the interface class info directly.
    EE_DECLARE_CONCRETE_REFCOUNT;
    /// @endcond

public:

    /// @name Construction and Destruction
    //@{
    /// Default constructor.
    NetService(efd::utf8string netLibType = "");

    /// Virtual destructor.
    virtual ~NetService();
    //@}

    /**
        Subscribe tells NetService that the INetCallback specified by
        pCallback is interested in receiving messages on the Category cat.
        Messages delivered to this Category will be delivered to the INetCallback specified.
        @param cat Category to subscribe to
        @param pCallback pointer to callback class to handle messages
        @param qualityOfService Quality of service.
    */
    virtual void Subscribe(
        const Category& cat,
        INetCallback* pCallback,
        QualityOfService qualityOfService = QOS_RELIABLE);


    /**
        Unsubscribe tells NetService that the INetCallback specified by
        pCallback is no longer interested in receiving messages on the Category cat.
        Messages delivered to the Category specified will no longer be  delivered to the 
        INetCallback specified.
        @param cat Category to subscribe to
        @param pCallback pointer to callback class to handle messages
    */
    virtual void Unsubscribe(const Category& cat, INetCallback* pCallback);

    /**
    * Unsubscribe the specified callback from all Categories.
    * Used in cases where the callback object pCallback is going to be deleted,
    * or is otherwise no longer interested in receiving any more callbacks
    *   @param pCallback pointer to callback object to Unsubscribe
    */
    virtual void Unsubscribe(INetCallback* pCallback);

    /**
        Send an IMessage to every process except this one (Send to every remote process)
        @param pMessage Pointer to message for delivery
        @param cat Category to send with
        @param defaultQOS quality of service to send message on
    */
    virtual void SendRemote(
        IMessage* pMessage,
        const Category &cat,
        efd::QualityOfService defaultQOS = QOS_INVALID);

    /**
        Send an IMessage the remote process specified by cid.
        @note: This function ignores subscription data and sends directly to the connection.
        @param pMessage Pointer to message for delivery
        @param cat Category to send with
        @param cid the ConnectionID to send message to
    */
    virtual void SendRemote(IMessage* pMessage, const Category &cat, const ConnectionID& cid);

    /**
        Send a message to producers of specified category
    
        @param pMessage Pointer to message for delivery
        @param categoryProduced send message to anyone who produces this category
        @param defaultQOS Quality of service.
    */
    virtual void ProducerSendRemote(
        IMessage* pMessage,
        const Category& categoryProduced,
        QualityOfService defaultQOS = QOS_INVALID);

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual efd::AsyncResult OnTick();
    virtual efd::SyncResult OnPreInit();
    virtual efd::AsyncResult OnInit();
    virtual efd::AsyncResult OnShutdown();
    const char* GetDisplayName() const;

    /// @name Accessors for NetID
    //@{
    virtual efd::UInt32 GetNetID() const;
    virtual void SetNetID(efd::UInt32 netID);
    //@}

    /**
        BeginCategoryProduction assigns a quality of service to a category
        and notifies the CategoryManager of the intention of this NetService to
        produce messages destined for the specified category
        @param cat The category specified "match" category
        @param qualityOfService Quality of service for this category
        @param pCallback the callback that will receive producer messages for this category
    */
    virtual bool BeginCategoryProduction(
        const Category& cat,
        QualityOfService qualityOfService,
        INetCallback* pCallback);

    /**
        EndCategoryProduction notifies the CategoryManager of the intention of
        this NetService to no longer produce messages destined for the specified
        category
        @param cat The category specified "match" category
        @param pCallback the callback that will no longer receive producer messages for this 
            category
    */
    virtual bool EndCategoryProduction(const Category& cat, INetCallback* pCallback);

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual void HandleNetMessage(
        const IMessage* pIncomingMessage,
        const ConnectionID& senderConnectionID);

    /// Return the connection private category of the ChannelManager
    virtual efd::ConnectionID GetChannelManagerConnectionID(QualityOfService qualityOfService);

    /// Return the NetID of the ChannelManager we are connected to
    virtual efd::UInt32 GetChannelManagerNetID();

    /**
        Sets the desired ip and port of the ChannelManager based on whatever default settings
        are specified in the Config.ini.
        @note: Applications with ProgramType ProgType_Server will automatically connect to the
        ChannelManager using this method, so they should not need to call this.
    */
    virtual void ConnectToDefaultChannelManager();

    /**
        Sets the desired ip and port of the ChannelManager
    
        @note: Applications with ProgramType ProgType_Server will automatically connect to the
        ChannelManager using ConnectToDefaultChannelManager, so they should not need to call this.
    */
    virtual efd::ConnectionID ConnectToChannelManager(
        QualityOfService qualityOfService,
        efd::utf8string cmIPAddress, 
        efd::UInt16 cmPort);

    /// Tell NetService to disconnect from the ChannelManager it is currently connected to.
    virtual void DisconnectFromChannelManager();

    /**
        Tell NetService to auto-reconnect or not. Default is to auto-reconnect
        to ChannelManager
        @param autoReconnect true if auto reconnection is desired
    */
    virtual void AutoReconnectToChannelManager(bool autoReconnect);

    /// Accessor for contained INetLib*
    virtual INetLib* GetNetLib();

    /**
        Used to send Connection and Disconnection notifications.
        @param pMessageService pointer to the MessageService instance to use to send the update
        @param cid ConnectionID of the connection that has just connected or disconnected
    */
    template< efd::UInt32 MessageID >
    static void SendConnectionUpdate(
        efd::MessageService* pMessageService,
        const ConnectionID& cid);

    /**
        Resends all subscription messages to specified connection
        @param cid id of the connection to send subscription messages to
    */
    virtual void Resubscribe(const ConnectionID& cid);

    /// Return internal ConnectionStatus
    virtual ConnectionStatus GetConnectionStatus();

    /// Set the MessageService this NetService will use.  Will no longer be necessary once full
    /// virtual processes are implemented
    virtual void SetMessageService(MessageService* pMessageService);

    /**
        Create our INetLib object based on the string passed in
    */
    virtual void CreateNetLib(efd::utf8string netLibType);

    /// @name Config strings used by this service
    //@{
    const static char* kConfigSection;
    const static char* kNetLibType;
    const static char* kNetLibTCP;
    const static char* kWaitBeforeReconnect;
    //@}

protected:

    /// Tell NetService to disconnect from all ChannelManagers of a specific QualityOfService
    virtual void DisconnectFromChannelManager(QualityOfService qualityOfService);

    /// Set the ConnectionID of the ChannelManager we are currently connected to.
    virtual void SetChannelManagerConnectionID(const efd::ConnectionID& cmConnectionID);

    /// Unsubscribe all callbacks from everything.
    virtual void UnsubscribeAll();

    /**
    * This method handles system messages received by the NetService.  Examples
    * of system messages are SubscribeExternal and UnsubscribeExternal messages
    * used to set up communication between two connected processes.
    *
    * @param pMessage The system message to handle
    */
    virtual void HandleSystemMessage(EnvelopeMessage* pMessage);

protected:
    /**
    * The pointer to the local message service in this framework.
    * Used to send messages from the Net Service to the Local Message Service
    */
    MessageServicePtr    m_spMessageService;

    /// keeps track of which NetID/QOS maps to which ConnectionID
    NetIDQosPairToConnectionIDMap m_NetIDQosLookupMap;
    /// list of configurations of ChannelManagers to connect to
    ChannelManagerConfigListPtr m_spConfigList;
    /// list of subscriptions to connections of a QoS that has not yet been established
    ChannelSubscriptionList m_pendingSubscriptions;

    efd::SmartPointer< INetLib > m_spNetLib;
    /// port of the ChannelManager we connect to.
    efd::UInt16 m_channelManagerListenPort;
    /// ip address of the ChannelManager we connect to.
    efd::utf8string m_channelManagerIP;
    /// NetID of the ChannelManager
    efd::UInt32 m_channelManagerNetID;
    bool m_autoReconnectToChannelManager;
    ConnectionStatus m_connectionStatus;
    efd::TimeType m_reconnectTime;
    efd::TimeType m_reconnectWaitTime;
    efd::utf8string m_netLibType;
    bool m_readNetLibConfig;

    typedef EE_STL_NAMESPACE::pair<efd::Category, INetCallback*> CategoryCallback;
    typedef efd::map<CategoryCallback, NonAtomicRefCount> CategoryRefcount;
    CategoryRefcount m_subscriberRefcount;
    CategoryRefcount m_producersRefcount;
};

typedef efd::SmartPointer<NetService> NetServicePtr;

} // end namespace efd

#include <efdNetwork/NetService.inl>

#endif // EE_NETSERVICE_H

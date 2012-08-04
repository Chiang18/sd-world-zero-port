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
#ifndef EE_INETSERVICE_H
#define EE_INETSERVICE_H

#include <efd/IBase.h>
#include <efd/ISystemService.h>
#include <efd/CriticalSection.h>
#include <efd/INetListener.h>
#include <efd/StreamMessage.h>
#include <efd/INetCallback.h>
#include <efd/ChannelManagerConfig.h>

namespace efd
{
class EnvelopeMessage;
class INetTransport;
class INetLib;
class MessageService;

class EE_EFD_ENTRY INetService : public efd::INetCallback, public efd::ISystemService
{
    /// @cond EMERGENT_INTERNAL

    EE_DECLARE_CLASS1(INetService, efd::kCLASSID_NetService, efd::ISystemService);
    EE_DECLARE_ABSTRACT_REFCOUNT;

    /// @endcond

public:

    /**
        Subscribe tells NetService that the INetCallback specified by
        pCallback is interested in receiving messages on the Category cat.
        Messages delivered to this Category will be delivered to the INetCallback specified.
        @param cat Category to subscribe to
        @param pCallback pointer to callback class to handle messages
        @param qualityOfService Quality of service, defaulted to QOS_RELIABLE.
    */
    virtual void Subscribe(const Category &cat, INetCallback* pCallback, QualityOfService qualityOfService = QOS_RELIABLE)=0;


    /**
        Unsubscribe tells NetService that the INetCallback specified by
        pCallback is no longer interested in receiving messages on the Category cat.
        Messages delivered to the Category specified will no longer be  delivered to the 
        INetCallback specified.
        @param cat Category to subscribe to
        @param pCallback pointer to callback class to handle messages
    */
    virtual void Unsubscribe(const Category &cat, INetCallback* pCallback)=0;

    /**
        Unsubscribe the specified callback from all Categories.
        Used in cases where the callback object pCallback is going to be deleted,
        or is otherwise no longer interested in receiving any more callbacks
        @param pCallback pointer to callback object to Unsubscribe
    */
    virtual void Unsubscribe(INetCallback* pCallback)=0;

    /**
        Send an IMessage to every process except this one ( Send to every remote process )
        @param pMessage Pointer to message for delivery
        @param cat Category to send with
        @param defaultQOS quality of service to send message on
    */
    virtual void SendRemote(IMessage* pMessage, const Category &cat,
        efd::QualityOfService defaultQOS = QOS_INVALID)=0;

    /**
        Send an IMessage the remote process specified by cid.
        @note: This function ignores subscription data and sends directly to the connection.
        @param pMessage Pointer to message for delivery
        @param cat Category to send with
        @param cid the ConnectionID to send message to
    */
    virtual void SendRemote(IMessage* pMessage, const Category &cat, const ConnectionID& cid)=0;

    /**
        Send a message to producers of specified category
    
        @param pMessage Pointer to message for delivery
        @param categoryProduced send message to anyone who produces this category
        @param qualityOfService The quality of service.
    */
    virtual void ProducerSendRemote(IMessage* pMessage, const Category& categoryProduced,
        QualityOfService qualityOfService)=0;

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual efd::AsyncResult OnTick()=0;
    virtual efd::SyncResult OnPreInit()=0;
    virtual efd::AsyncResult OnInit()=0;
    virtual efd::AsyncResult OnShutdown()=0;
    virtual const char* GetDisplayName() const=0;

    /// @name Accessors for NetID
    //@{
    virtual efd::UInt32 GetNetID() const=0;
    virtual void SetNetID(efd::UInt32 netID)=0;
    //@}

    /**
        BeginCategoryProduction assigns a quality of service to a category
        and notifies the CategoryManager of the intention of this NetService to
        produce messages destined for the specified category
        @param cat The category specified "match" category
        @param qualityOfService Quality of service for this category
        @param pCallback The callback that will receive producer messages for this category
    */
    virtual bool BeginCategoryProduction(
        const Category& cat,
        QualityOfService qualityOfService,
        INetCallback* pCallback)=0;

    /**
        EndCategoryProduction notifies the CategoryManager of the intention of
        this NetService to no longer produce messages destined for the specified
        category
        @param cat The category specified "match" category
        @param pCallback The callback that will no longer receive producer messages for this 
        category
    */
    virtual bool EndCategoryProduction(const Category& cat, INetCallback* pCallback)=0;

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual void HandleNetMessage(
        const IMessage* pIncomingMessage,
        const ConnectionID& senderConnectionID)=0;

    /// Return the connection private category of the ChannelManager
    virtual efd::ConnectionID GetChannelManagerConnectionID(QualityOfService qualityOfService)=0;

    /// Return the NetID of the ChannelManager we are connected to
    virtual efd::UInt32 GetChannelManagerNetID()=0;

    /**
        Sets the desired ip and port of the ChannelManager based on whatever default settings
        are specified in the Config.ini.
        @note: Applications with ProgramType ProgType_Server will automatically connect to the
        ChannelManager using this method, so they should not need to call this.
    */
    virtual void ConnectToDefaultChannelManager()=0;

    /**
        Sets the desired ip and port of the ChannelManager
    
        @note: Applications with ProgramType ProgType_Server will automatically connect to the
        ChannelManager using ConnectToDefaultChannelManager, so they should not need to call this.
    */
    virtual efd::ConnectionID ConnectToChannelManager(
        QualityOfService qualityOfService,
        efd::utf8string cmIPAddress, 
        efd::UInt16 cmPort)=0;

    /// Tell NetService to disconnect from all ChannelManagers it is currently connected to.
    virtual void DisconnectFromChannelManager()=0;

    /**
        Tell NetService to auto-reconnect or not. Default is to auto-reconnect
        to ChannelManager
        @param autoReconnect true if auto reconnection is desired
    */
    virtual void AutoReconnectToChannelManager(bool autoReconnect)=0;

    /// Accessor for contained INetLib*
    virtual INetLib* GetNetLib()=0;

    /**
        Resends all subscription messages to specified connection
        @param cid id of the connection to send subscription messages to
    */
    virtual void Resubscribe(const ConnectionID& cid)=0;

    /**
        Gets the status of the connection to the ChannelManager
    */
    virtual ConnectionStatus GetConnectionStatus()=0;

    /// Set the MessageService this NetService will use.  Will no longer be necessary once full
    /// virtual processes are implemented
    virtual void SetMessageService(MessageService* pMessageService)=0;

    /// Used to create the INetLib instance this NetService uses
    virtual void CreateNetLib(efd::utf8string netLibType)=0;

protected:
    /// Set the ConnectionID of the ChannelManager we are currently connected to.
    virtual void SetChannelManagerConnectionID(const efd::ConnectionID& cmConnectionID)=0;

    /// Unsubscribe all callbacks from everything.
    virtual void UnsubscribeAll()=0;

    /**
    * This method handles system messages received by the NetService.  Examples
    * of system messages are SubscribeExternal and UnsubscribeExternal messages
    * used to set up communication between two connected processes.
    *
    * @param pMessage The system message to handle
    */
    virtual void HandleSystemMessage(EnvelopeMessage* pMessage)=0;
};


typedef efd::SmartPointer<INetService> INetServicePtr;

} // end namespace efd

#endif // EE_INETSERVICE_H

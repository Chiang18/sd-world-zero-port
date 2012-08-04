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
#ifndef EE_MESSAGESERVICE_H
#define EE_MESSAGESERVICE_H

#include <efd/ISystemService.h>
#include <efd/CriticalSection.h>
#include <efd/StdContainers.h>
#include <efd/IMessage.h>
#include <efd/Category.h>
#include <efd/INetCallback.h>
#include <efd/NetEnums.h>
#include <efd/MessageFactory.h>
#include <efd/INetService.h>
#include <efd/MessageHandlerBase.h>

namespace efd
{

/**
    \brief MessageService handles sending messages and registering callbacks for delivery through
    Subscribe.
*/
class EE_EFD_ENTRY MessageService : public ISystemService, public INetCallback, public MessageFactory
{
    EE_DECLARE_CLASS1(MessageService, kCLASSID_MessageService, ISystemService);
    EE_DECLARE_CONCRETE_REFCOUNT;

public:

    /// Default Constructor
    MessageService();

    /// Virtual Destructor
    virtual ~MessageService();

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual efd::SyncResult OnPreInit();

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual efd::AsyncResult OnInit();

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual efd::AsyncResult OnTick();

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual efd::AsyncResult OnShutdown();

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    inline virtual const char* GetDisplayName() const;

    /**
        Causes message pMessage to be handled immediately (on the MessageService's thread) by all
        targets currently registered for its type and subscribed to targetChannel.
        This function is called internally by MessageService::OnTick to deliver messages that have
        been queued by MessageService::Send or MessageService::SendLocal.

        @param pMessage Pointer to the message to send
        @param targetChannel Category to deliver message on
        @param producerMessage if true, pMessage will be delivered to callback that have 
            registered with BeginCategoryProduction instead of Subscribe

        @note A SmartPointer to pMessage is held until all handlers have been called and/or the
            IMessage has been sent remotely.
            The MessageService assumes ownership of the message, and it must not be modified any 
            time after calling any MessageService Send method.

        @warning This method is not thread safe and can only be safely called from the same thread
            as the MessageService instance it is called on.
    */
    virtual void SendImmediate(
        const IMessage* pMessage,
        Category targetChannel,
        bool producerMessage = false);

    /**
        Causes message pMessage to be handled immediately (on the MessageService's thread) by all
        targets currently registered for its type and subscribed to 
        pMessage->GetDestinationCategory.
        This function is called internally by MessageService::OnTick to deliver messages that have
        been queued by MessageService::Send or MessageService::SendLocal.

        @param pMessage Pointer to the message to send

        @note A SmartPointer to pMessage is held until all handlers have been called and/or the
            IMessage has been sent remotely.
            The MessageService assumes ownership of the message, and it must not be modified any 
            time after calling any MessageService Send method.

        @warning This method is not thread safe and can only be safely called from the same thread
            as the MessageService instance it is called on.
    */
    inline void SendImmediate(const IMessage* pMessage);

    /**
        Causes message pMessage to be handled the next time this service is ticked by all targets
        registered for its type and subscribed to targetChannel at that time.

        @param pMessage Pointer to the message to post
        @param targetChannel Category to deliver message on
        @param producerMessage if true this message is for producers of targetChannel not 
        consumers

        @note Threading: This method is thread-safe, and is the preferred way for threads to
            interact with each other.

        @note A SmartPointer to pMessage is held until all handlers have been called and/or the
            IMessage has been sent remotely.
            The MessageService assumes ownership of the message, and it must not be modified any 
            time after calling any MessageService Send method.
    */
    virtual void SendLocal(
        const IMessage* pMessage, 
        Category targetChannel, 
        bool producerMessage = false);

    /**
        Causes message pMessage to be handled the next time this service is ticked by all targets
        registered for its type and subscribed to pMessage->GetDestinationCategory at that time.

        @param pMessage Pointer to the message to post

        @note Threading: This method is thread-safe, and is the preferred way for threads to
            interact with each other.

        @note A SmartPointer to pMessage is held until all handlers have been called and/or the
            IMessage has been sent remotely.
            The MessageService assumes ownership of the message, and it must not be modified any 
            time after calling any MessageService Send method.
    */
    inline void SendLocal(const IMessage* pMessage);

    /**
        Causes message pMessage to be handled the next time this service is ticked by all targets
        registered for its type and subscribed to targetChannel at that time, 
        after the given time has elapsed.

        @param pMessage Pointer to the message to post for later delivery
        @param fDelay Time in seconds to delay the message delivery
        @param targetChannel Category to deliver message on

        @note Threading: This method is thread-safe, and is the preferred way for threads to
            interact with each other.

        @note A SmartPointer to pMessage is held until all handlers have been called and/or the
            IMessage has been sent remotely.
            The MessageService assumes ownership of the message, and it must not be modified any 
            time after calling any MessageService Send method.
    */
    virtual void SendLocalDelayed(
        const IMessage* pMessage, 
        efd::TimeType fDelay,
        Category targetChannel);

    /**
        Causes message pMessage to be handled the next time this service is ticked by all targets
        registered for its type and subscribed to pMessage->GetDestinationCategory at that time, 
        after the given time has elapsed.

        @param pMessage Pointer to the message to post for later delivery
        @param fDelay Time in seconds to delay the message delivery

        @note Threading: This method is thread-safe, and is the preferred way for threads to
            interact with each other.

        @note A SmartPointer to pMessage is held until all handlers have been called and/or the
            IMessage has been sent remotely.
            The MessageService assumes ownership of the message, and it must not be modified any 
            time after calling any MessageService Send method.
    */
    inline void SendLocalDelayed(const IMessage* pMessage, efd::TimeType fDelay);

    /**
        BeginCategoryProduction assigns a quality of service to a category and notifies the
        ChannelManager of the intention of this MessageService to produce messages destined for
        the specified category.  Every call to BeginCategoryProduction should be matched with an
        eventual call to EndCategoryProduction once you no longer produce messages onto the
        target category.
        
        If you wish to respond to SendToProducer messages then you must also provide a callback
        to which these messages will be delivered.  If a callback is specified in the Begin call
        then the same callback must be passed to the matching End call.  If the same callback is
        registered multiple times it will still be called no more than one time for any arriving
        message.  Regardless every call to Begin needs a matching call to End.
        
        @param cat The category onto which we will be producing messages
        @param qualityOfService Quality of service for this category
        @param pProducerCallback The callback to be invoked if someone uses
            SendToProducers on this category.

        @note BeginCategoryProduction/EndCategoryProduction is not needed to send messages unless
            you need to receive producer message callbacks.  If a you need to send a message do not
            need producer messages in response pass a QualityOfSerice to Send/SendRemote.
    */
    virtual void BeginCategoryProduction(
        const Category& cat,
        QualityOfService qualityOfService,
        IBase* pProducerCallback);

    /**
        EndCategoryProduction notifies the ChannelManager of the intention of this MessageService
        to no longer produce messages destined for the specified category.  If a producer callback
        was specified when beginning production that same callback must be specified in the end 
        call.
        
        @param cat The category for which we are no longer producing messages.
        @param pProducerCallback [optional] : The callback used when calling 
            BeginCategoryProduction.

        @note BeginCategoryProduction/EndCategoryProduction is not needed to send messages unless
            you need to receive producer message callbacks.  If a you need to send a message do not
            need producer messages in response pass a QualityOfSerice to Send/SendRemote.
    */
    virtual void EndCategoryProduction(
        const Category& cat, 
        IBase* pProducerCallback = NULL);

    /// @name Send methods
    //@{

    /**
        Send a message remotely only.  Sends to any interested remote connection.  Calls 
        NetService::SendRemote() internally.

        @param pMessage Pointer to message for delivery
        @param cat Category to send with
        @param defaultQOS default QOS to send to if Category is not already mapped to an
            existing QOS.

        @note A SmartPointer to pMessage is held until all handlers have been called and/or the
            IMessage has been sent remotely.
            The MessageService assumes ownership of the message, and it must not be modified any 
            time after calling any MessageService Send method.
        @note If Category is already mapped to an existing QOS that QOS will
            will be used instead of defaultQOS and an error will be logged.
    */
    virtual void SendRemote(
        IMessage* pMessage,
        const Category &cat,
        QualityOfService defaultQOS = QOS_RELIABLE);

    /**
        Send a message remotely only.  Sends to any interested remote connection.  Calls 
        NetService::SendRemote() internally. Messages are sent using pMessages destination
        category using the default quality of service.

        @param pMessage Pointer to message for delivery

        @note A SmartPointer to pMessage is held until all handlers have been called and/or the
            IMessage has been sent remotely.
            The MessageService assumes ownership of the message, and it must not be modified any 
            time after calling any MessageService Send method.
        @note If Category is already mapped to an existing QOS that QOS will
            will be used instead of defaultQOS and an error will be logged.
    */
    inline void SendRemote(IMessage* pMessage);

    /**
        Send a message remotely only.  Sends to the specified remote connection regardless of 
        interest.  Calls NetService::SendRemote() internally.

        @param pMessage Pointer to message for delivery
        @param cat Category to send with
        @param cid ConnectionID of the remote connection to send to

        @note A SmartPointer to pMessage is held until all handlers have been called and/or the
            IMessage has been sent remotely.
            The MessageService assumes ownership of the message, and it must not be modified any 
            time after calling any MessageService Send method.
    */
    virtual void SendRemote(IMessage* pMessage, const Category &cat, const ConnectionID& cid);

    /**
        Delivers to local producers and calls NetService::ProducerSendRemote().  Used to send a 
        message to any target that has registered by passing a producer callback to 
        BeginCategoryProduction.

        @param pMessage Pointer to message for delivery
        @param categoryProduced send message to anyone who produces this category
        @param qualityOfService the quality of service to use when sending.
    */
    virtual void ProducerSend(
        IMessage* pMessage,
        const Category& categoryProduced,
        QualityOfService qualityOfService = QOS_RELIABLE);

    /**
        Calls NetService::ProducerSendRemote().   Used to send a  message to any target that has 
        registered by passing a producer callback to BeginCategoryProduction.

        @param pMessage Pointer to message for delivery
        @param categoryProduced send message to anyone who produces this category
        @param qualityOfService the quality of service to use when sending.
    */
    virtual void ProducerSendRemote(
        IMessage* pMessage,
        const Category& categoryProduced,
        QualityOfService qualityOfService = QOS_RELIABLE);

    /**
        Calls MessageService::SendRemote and MessageService::SendLocal

        @param pMessage Pointer to message for delivery.
        @param cat Category to send on.
        @param defaultQOS default QOS to send to if Category is not already mapped to an
            existing QOS.

        @note A SmartPointer to pMessage is held until all handlers have been called and/or the
            IMessage has been sent remotely.
            The MessageService assumes ownership of the message, and it must not be modified any 
            time after calling any MessageService Send method.
        @note If Category is already mapped to an existing QOS that QOS will
            will be used instead of defaultQOS and an error will be logged.
    */
    virtual void Send(
        const IMessage* pMessage,
        const Category &cat,
        QualityOfService defaultQOS = QOS_RELIABLE);

    /**
        Calls MessageService::SendRemote and MessageService::SendLocal. Messages are delivered to
        the pMessages destination category using the default quality of service.

        @param pMessage Pointer to message for delivery.

        @note A SmartPointer to pMessage is held until all handlers have been called and/or the
            IMessage has been sent remotely.
            The MessageService assumes ownership of the message, and it must not be modified any 
            time after calling any MessageService Send method.
        @note If Category is already mapped to an existing QOS that QOS will
            will be used instead of defaultQOS and an error will be logged.
    */
    inline void Send(const IMessage* pMessage);

    /**
         Get the NetID for this MessageService/ServiceManager.
         @note there should only be one MessageService and ServiceManager per thread.
         @note This function is not valid to be called until after an IMessage of type 
            kMSGID_SetNetID on kCAT_LocalMessage has been received.
    */
    virtual efd::UInt32 GetNetID() const;

    /**
         Get the VirtualProcessID for this MessageService/ServiceManager.
         @note there should only be one MessageService and ServiceManager per thread.
         @note This function is not valid to be called until after an IMessage of type 
             kMSGID_SetNetID on kCAT_LocalMessage has been received.
    */
    virtual efd::UInt32 GetVirtualProcessID() const;

    /**
        Subscribe to receive any messages that are on the given Category. They will be delivered
        to pTarget and associated method for the given pTarget's class only if one
        exists for an arriving message's classID.

        @param pTarget Pointer to the object the message will be delivered to.
        @param consumeFrom Category the message will be sent on.
        @param qualityOfService the quality of service to use when sending.
        @param producerSubscribe True indicates a producer subscribe message type.

        @note Subscribe/Unsubscribe should be called in matching pairs for a given IBase and 
            Category.  An IBase can be subscribed multiple times to the same Category but the 
            handler method will only be called once per message.  Messages will continue being 
            delivered until the same number of Unsubscribes has been called.
        @note There is no support for subscribing to all message classes that derive from one 
            with a given classID.
    */
    virtual void Subscribe(
        IBase* pTarget,
        const Category& consumeFrom,
        QualityOfService qualityOfService = QOS_RELIABLE,
        bool producerSubscribe = false);

    /**
        Unsubscribe to stop receiving any messages that are on the given Category.

        @param pTarget Pointer to the object the message will be delivered to.
        @param consumeFrom Category the message will be sent on.
        @param producerSubscribe True indicates a producer subscribe message type.

        @note Subscribe/Unsubscribe should be called in matching pairs for a given IBase and 
            Category.  An IBase can be subscribed multiple times to the same Category but the 
            handler method will only be called once per message.  Messages will continue being 
            delivered until the same number of Unsubscribes has been called.
        @note There is no support for subscribing to all message classes that derive from one 
            with a given classID.
    */
    virtual void Unsubscribe(
        IBase* pTarget,
        const Category& consumeFrom,
        bool producerSubscribe = false);

    /**
        Accessors for m_sendRemote
    */
    virtual void SetSendRemote(bool sendRemote);
    virtual bool GetSendRemote();

    /// Set the NetService this MessageService will use.
    virtual void SetNetService(efd::INetService* pNetService);


protected:
    /**
        Internal method used to do actual work of Subscribe
    */
    void InternalSubscribe(
        IBase* pTarget,
        const Category& consumeFrom,
        bool producerSubscribe);

    /**
        Internal method used to do actual work of Unsubscribe
    */
    void InternalUnsubscribe(
        IBase* pTarget,
        const Category& consumeFrom,
        bool producerSubscribe);

    /**
        Helper for messages received on the SendToProducers channel.  Invokes all the producer
        callbacks for anyone producing the target channel who registered a callback.
    */
    void DeliverToProducers( const IMessage* pMessage, Category targetChannel );

    /**
        Handles messages that originate from the NetService
    */
    virtual void HandleNetMessage(
        const IMessage* pIncomingMessage,
        const ConnectionID& senderConnectionID );

    /**
        SubscribeUnsubscribe loops through the list of message targets to subscribe
        and unsubscribe and then calls the internal subscriptions methods to do
        the work.
    */
    virtual void SubscribeUnsubscribe();

    /**
        Iterates through the posted and post delayed messages and sends them
        if it is time.
    */
    virtual void ProcessMessages( );

    /// Insert static handler into map.
    /// There is no support for subscribing to all message classes that derive from one 
    /// with a given classID.
    /// @note this method is not expected to be called directly. Use the MessageHandler template,
    ///   which auto-registers.
    void RegisterMessageHandler(BaseMessageHandler *h);

private:
    /// Scan statically declared handlers for calling arbitrary methods. Think RPC/RMI.
    void RegisterMessageHandlers();

protected:
    /// @cond EMERGENT_INTERNAL
    /**
        A class that encapsulates the message and the time it is to be delivered.
    */
    class EE_EFD_ENTRY DelayedMessage
    {
    public:
        DelayedMessage(
            const IMessage* pMessage, 
            Category targetCategory, 
            efd::TimeType sendTime, 
            bool isProducerMessage = false)
            : m_spMessage(pMessage)
            , m_targetCategory(targetCategory)
            , m_sendTime(sendTime)
            , m_isProducerMessage( isProducerMessage )
        {}

        //! Copy constructor
        DelayedMessage( const DelayedMessage& delayedMessage)
            : m_spMessage(delayedMessage.m_spMessage)
            , m_targetCategory(delayedMessage.m_targetCategory)
            , m_sendTime(delayedMessage.m_sendTime)
            , m_isProducerMessage(delayedMessage.m_isProducerMessage)
        {}

        ~DelayedMessage()
        {
            // Ensure the smart pointer gets freed
            m_spMessage = NULL;
        }
        bool operator == (const DelayedMessage& delayedMessage)
        {
            return (m_spMessage == delayedMessage.m_spMessage)
                && (m_targetCategory == delayedMessage.m_targetCategory)
                && (m_sendTime == delayedMessage.m_sendTime)
                && (m_isProducerMessage == delayedMessage.m_isProducerMessage);
        }
        bool operator != (const DelayedMessage& delayedMessage)
        {
            return (m_spMessage != delayedMessage.m_spMessage)
                || (m_targetCategory != delayedMessage.m_targetCategory)
                || (m_sendTime != delayedMessage.m_sendTime)
                || (m_isProducerMessage == delayedMessage.m_isProducerMessage);
        }
        IMessageConstPtr m_spMessage;
        Category m_targetCategory;
        efd::TimeType m_sendTime;
        bool m_isProducerMessage;
    };


    class EE_EFD_ENTRY MessageWithCategory
    {
    public:
        // default parameters so that we can create a default instance to populate from pop_front
        MessageWithCategory(
            const IMessage* pMessage = NULL,
            Category targetCategory = kCAT_INVALID, 
            bool isProducerMessage = false)
            : m_spMessage(pMessage)
            , m_targetCategory(targetCategory)
            , m_isProducerMessage( isProducerMessage )
        {}

        //! Copy constructor
        MessageWithCategory( const MessageWithCategory& messageWithCategory)
            : m_spMessage(messageWithCategory.m_spMessage)
            , m_targetCategory(messageWithCategory.m_targetCategory)
            , m_isProducerMessage(messageWithCategory.m_isProducerMessage)
        {}

        ~MessageWithCategory()
        {
            // Ensure the smart pointer gets freed
            m_spMessage = NULL;
        }
        bool operator == (const MessageWithCategory& messageWithCategory)
        {
            return (m_spMessage == messageWithCategory.m_spMessage)
                && (m_targetCategory == messageWithCategory.m_targetCategory)
                && (m_isProducerMessage == messageWithCategory.m_isProducerMessage);
        }
        bool operator != (const MessageWithCategory& messageWithCategory)
        {
            return (m_spMessage != messageWithCategory.m_spMessage)
                || (m_targetCategory != messageWithCategory.m_targetCategory)
                || (m_isProducerMessage != messageWithCategory.m_isProducerMessage);
        }
        IMessageConstPtr m_spMessage;
        Category m_targetCategory;
        bool m_isProducerMessage;
    };


    //! A class that encapsulates the efd::ClassID and the Message Handler
    class EE_EFD_ENTRY TargetSubscription
    {
    public:
        enum SUBSCRIBE_TYPE
        {
            SUBSCRIBE = 0,
            UNSUBSCRIBE,
            PRODUCER_SUBSCRIBE,
            PRODUCER_UNSUBSCRIBE
        };
        // default parameters so that we can create a default instance to populate from pop_front
        TargetSubscription(
            efd::ClassID msgClassID = k_invalidMessageClassID,
            IBase* pIAppTarget = NULL,
            Category targetChannel = kCAT_INVALID,
            QualityOfService qualityOfService = QOS_INVALID,
            SUBSCRIBE_TYPE subscribe = UNSUBSCRIBE)
            : m_subscribe(subscribe)
            , m_typeID(msgClassID)
            , m_targetChannel(targetChannel)
            , m_pIAppTarget(pIAppTarget)
            , m_qualityOfService(qualityOfService)
        {}

        //! Copy constructor
        TargetSubscription( const TargetSubscription& HandlerSub )
            : m_subscribe(HandlerSub.m_subscribe)
            , m_typeID(HandlerSub.m_typeID)
            , m_targetChannel( HandlerSub.m_targetChannel )
            , m_pIAppTarget(HandlerSub.m_pIAppTarget)
            , m_qualityOfService(HandlerSub.m_qualityOfService)
        {}

        bool operator==(const TargetSubscription &HandlerSub)
        {
            return (m_typeID == HandlerSub.m_typeID)
                && (m_subscribe == HandlerSub.m_subscribe)
                && (m_pIAppTarget == HandlerSub.m_pIAppTarget)
                && (m_targetChannel == HandlerSub.m_targetChannel)
                && (m_qualityOfService == HandlerSub.m_qualityOfService);
        }
        bool operator!=(const TargetSubscription &HandlerSub)
        {
            return (m_typeID != HandlerSub.m_typeID)
                || (m_subscribe != HandlerSub.m_subscribe)
                || (m_pIAppTarget != HandlerSub.m_pIAppTarget)
                || (m_targetChannel != HandlerSub.m_targetChannel)
                || (m_qualityOfService != HandlerSub.m_qualityOfService);
        }

        //! Is to determine if a target is to be subscribed or unsubscribed.
        //! It is set to true for subscribe or false to unsubscribe.
        SUBSCRIBE_TYPE m_subscribe;
        efd::ClassID m_typeID;
        Category m_targetChannel;
        IBase* m_pIAppTarget;
        QualityOfService m_qualityOfService;
    };

    /**
        A lock to make the public Subscribe() and Unsubscribe() thread safe.
    */
    efd::CriticalSection m_subUnsubLock;

    typedef efd::list< TargetSubscription > TargetSubscriptionList;
    /**
        The list of targets to subscribe or unsubscribe.  It is used to proxy the
        requests between threads.

        @attention All access to the m_subUnsubTargetList must be wrapped with
                  calls to lock and unlock the m_subUnsubTargetListLock
    */
    TargetSubscriptionList m_subUnsubTargetList;

    /**
        List of subscriptions that have been processed locally but not yet sent to the NetService.
    */
    TargetSubscriptionList m_pendingNetSubscriptions;

    /**
        A lock to protect the m_pMessageList and make the public
        SendLocal() method thread safe.
    */
    efd::CriticalSection m_messageListLock;
    /**
        The list of messages received in the SendLocal() method.  It is used to proxy
        the messages between threads.  The messages are pulled off and processed
        in the OnTick() method.

        @attention All access to the m_pMessageList must be wrapped with calls to
        lock and unlock the m_messageListLock
    */
    typedef efd::RefCountedMemObj< efd::list< MessageWithCategory > > MessageList;
    MessageList* m_pMessageList;

    /**
        This is the internal list of messages.  It is swapped with the
        m_pMessageList during each OnTick() call so that it can be processed
        without holding the lock and so it will not process any messages posted
        while a message is being processed (causing an infinite loop).
    */
    MessageList* m_pMessageListInternal;

    /**
        A lock to protect the m_delayedMessageList and make the public
        SendLocalDelayed() method thread safe.
    */
    efd::CriticalSection m_delayedMessageListLock;

    /**
        The list of messages received in the SendLocalDelayed() method.  It is used to
        proxy the messages between threads.  The messages are pulled off and
        processed in the OnTick() method after they have been delayed the proper
        amount of time.

        @attention All access to the m_delayedMessageList must be wrapped with calls to
        lock and unlock the m_delayedMessageListLock
    */
    typedef efd::list<DelayedMessage> DelayedMsgList;
    DelayedMsgList m_delayedMessageList;

    /// pointer to NetService.  Used for Subscription of non-local categories
    efd::SmartPointer<INetService> m_spNetService;

    /// true while OnTick is in progress
    SInt32 m_ticking;

    /// true if OnInit has completed
    bool m_initialized;

    /// true if we should try to send messages remotely
    bool m_sendRemote;

    /// true if a subscription happened while m_ticking is true
    bool m_subscriptionsDirty;

    /**
        Each element of this map is indexed by a Category. Each element contains a list of
        message targets. Each target in one of those lists is subscribed to the Category that
        holds its list. A target can be in many lists.
    */
    typedef efd::map<efd::IBasePtr, efd::UInt32> TargetsList;
    typedef efd::map<efd::Category, TargetsList> TargetsListsByCategory;
    TargetsListsByCategory m_targetsListsByCategory;

    /**
        Same as m_targetsListsByCategory but for producer messages
    */
    TargetsListsByCategory m_producerListsByCategory;

    /**
        Each element of this map is indexed by the ClassID of a message. Each element contains a list
        of static/RPC handlers that have been registered to expect that message class. We do not expect
        that a handler will be in more than one list, but will support that in case a user decides
        to implement a generic handler for multiple message class (and then use a switch 
        or if/then/else). Note that the expected and more efficient use is register a separate handler
        per message class.
    */
    typedef efd::list<efd::BaseMessageHandler*> MessageHandlersList;
    typedef efd::map<efd::ClassID, MessageHandlersList> HandlersListsByMessageClass;
    HandlersListsByMessageClass m_handlersListsByMessageClass;

    /// Pointer to the last handler this MessageService instance has registered
    BaseMessageHandler* m_lastHandlerProcessed;
    /// @endcond

};
/// A SmartPointer for the MessageService class
typedef efd::SmartPointer<MessageService> MessageServicePtr;

} //  namespace efd

#include <efd/MessageService.inl>

#endif // EE_MESSAGESERVICE_H

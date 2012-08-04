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
#ifndef EE_BASEMESSAGEHANDLER_H
#define EE_BASEMESSAGEHANDLER_H

#include <efd/IMessage.h>
#include <efd/Category.h>
#include <efd/efdLibType.h>
#include <efd/MessageFactory.h>

namespace efd
{

/**
    Class used to register message handler objects. This class is not meant to be used directly.
    It is intended to be used by the EE_HANDLER, EE_HANDLER_SUBCLASS, EE_HANDLER_WRAP, and
    EE_HANDLER_SUBCLASS_WRAPPER macros. In cases where these macros are not sufficient the
    MessageHandlerImpl, MessageSubclassHandler or MessageWrapperSubclassHandler should be used
    instead.
*/
class EE_EFD_ENTRY BaseMessageHandler
{
public:
    virtual ~BaseMessageHandler() {}

    /**
        Callback that handles a generic IMessage. Child classes can override this method to cast 
        the IMessage to the appropriate type and call another handler function with a different
        signature.
        @param pTargetObject Target object that subscribed to the targetCategory
        @param pIMessage The message being delivered
        @param targetChannel the Category the message was sent on
    */
    virtual void HandleMessage(
        efd::IBase* pTargetObject,
        const efd::IMessage* pMessage,
        efd::Category targetChannel)=0;

    /// Next object in linked list of message handlers registered statically.
    BaseMessageHandler* m_nextMessageHandler; 

    /// Collects all statically declared handlers w/o using init-time allocation.
    static BaseMessageHandler* s_firstMessageHandler;

    /// The class of message this handler consumes
    efd::ClassID m_messageClassID;

    /// The class of the target that this method will call.
    efd::ClassID m_targetClassID;

    /**
        Registers a factory for the message type used by this handler.
        @param pMessageFactory Pointer to MessageFactory responsible for creating all messages.
    */
    virtual void RegisterMessageFactory(MessageFactory* pMessageFactory)=0;
};

/**
    Class used to declare a message handler object.
    @note Assumes that TMessage is an IMessage, and that TMessage has a CLASS_ID,
    which is usually provided by DECLARE_CLASS().
*/
template <class Target, class TMessage> 
class MessageHandlerImpl : public BaseMessageHandler
{
public:
    /// Typedef for the signature the class method must have
    typedef void (Target::*HandlerMethod)(const TMessage*, Category);

    /**
        Constructor
        @param pMethod Class method to pass the messages to
    */
    MessageHandlerImpl(HandlerMethod pMethod)
    {
        m_pMethod = pMethod;
        m_nextMessageHandler = s_firstMessageHandler;
        s_firstMessageHandler = this;

        m_messageClassID = TMessage::CLASS_ID;
        m_targetClassID = Target::CLASS_ID;
    }

    /**
        HandleMessage dynamically casts the message to the custom type TMessage
        and then calls the class handler if the cast is successful

        @param t Class type that this handler will interface with
        @param pMsg Pointer to the message to pass onto the class method
        @param targetChannel The category the message will be on.
    */
    virtual void HandleMessage(
        efd::IBase* t,
        const efd::IMessage *pMsg,
        efd::Category targetChannel)
    {
        const TMessage *pCustom = NULL;

        // If the class id's match exactly we can hard-cast. 
        // This allows us to use MessageWrapper as-is.
        if (pMsg->GetClassID() == TMessage::CLASS_ID) 
        {
            pCustom = static_cast<const TMessage*>(pMsg);
        }
        else 
        {
            pCustom = EE_DYNAMIC_CAST( const TMessage, pMsg);
        }
        // Check if this handler is mis-registered.
        // Note that a message is only allowed one usable ClassID. If you want a handler method to take a base
        // class message type, use MessageSubclassHandler. We would hit this handler if the message has a classID
        // it cannot be dynamically cast to, which is illegal, and indicates a bad application message definition.
        EE_ASSERT_MESSAGE(pCustom != NULL, ("HandleMessage called with a mis-declared message"));

        Target *tgt = EE_DYNAMIC_CAST(Target, t);
        // Can have targets consuming on this Category that are not of the target type for this handler.
        // No way to avoid the dynamic cast, because targets can be derived and have multiple ClassID's

        if ( pCustom && tgt)
        {
            EE_MESSAGE_TRACE(
                pCustom,
                efd::kMessageTrace,
                efd::ILogger::kLVL0,
                efd::ILogger::kLVL2,
                ("%s| delivery on %s to %p",
                pCustom->GetDescription().c_str(),
                targetChannel.ToString().c_str(),
                tgt));
        
            /// streams can be read in a const safe way
            (tgt->*m_pMethod)(pCustom,targetChannel);
        }
    }

    /**
        Registers a factory for the message type used by this handler.
        @param pMessageFactory Pointer to MessageFactory responsible for creating all messages.
    */
    virtual void RegisterMessageFactory(MessageFactory* pMessageFactory)
    {
        pMessageFactory->RegisterFactoryMethod(
            m_messageClassID,
            &MessageFactory::GenerateFactoryMethod<TMessage>);
    }

private:
    /// a reference to the method to invoke when the message arrives.
    HandlerMethod m_pMethod;
};

/**
    Class used to declare a message handler object where the type of the handler function
    in the handler object does not match the actual type of the message being sent.
    @note: This template is used to subscribe to a specific message class, but to use a handler
        that expects a base class (e.g. StreamMessage, or IMessage).
*/
template <class Target, class TBaseMessage, class TMessage> 
class MessageSubclassHandler : public MessageHandlerImpl<Target, TBaseMessage>
{
public:
    // Working around some cross-platform compiler issues. These handler method signatures should 
    // be identical.
    /// Typedef for function signature
    typedef void (Target::*SubHandlerMethod)(const TBaseMessage*, Category);

    /// Constructor
    /// @param pMethod Pointer to callback funtion.
    MessageSubclassHandler(SubHandlerMethod pMethod):
        MessageHandlerImpl<Target, TBaseMessage>(pMethod)
    {
        // Another cross-platform compiler issue.
        this->BaseMessageHandler::m_messageClassID = TMessage::CLASS_ID;
    }

    /**
        Registers a factory for the message type used by this handler.
        @param pMessageFactory Pointer to MessageFactory responsible for creating all messages.
    */
    virtual void RegisterMessageFactory(MessageFactory* pMessageFactory)
    {
      pMessageFactory->RegisterFactoryMethod(
          this->BaseMessageHandler::m_messageClassID,
          &MessageFactory::GenerateFactoryMethod<TMessage>);
    }
};

/** 
    This template is used to simplify subscribing a Handler and registering an MessageWrapper
    derived class factory.
*/
template <class Target, class TBaseMessage, class TMessage, efd::UInt32 TMessageType> 
class MessageWrapperSubclassHandler : 
    public MessageSubclassHandler<Target, TBaseMessage, TMessage >
{
public:
    // Working around some cross-platform compiler issues. These handler method signatures should 
    // be identical.
    /// Typedef for function signature
    typedef void (Target::*SubHandlerMethod)(const TBaseMessage*, Category);

    /// Constructor
    /// @param pMethod Pointer to callback funtion.
    MessageWrapperSubclassHandler(SubHandlerMethod pMethod):
        MessageSubclassHandler<Target, TBaseMessage, TMessage>(pMethod)
    {
        // Another cross-platform compiler issue.
        this->BaseMessageHandler::m_messageClassID = TMessageType;
    }

    /**
        Registers a factory for the message type used by this handler.
        @param pMessageFactory Pointer to MessageFactory responsible for creating all messages.
    */
    virtual void RegisterMessageFactory(MessageFactory* pMessageFactory)
    {
        RegisterMessageWrapperFactory<TMessage,TMessageType>(pMessageFactory);
    }
};

/// Macro for creating a static auto-registering object for setting up mapping of message to method
#define EE_HANDLER(CLASS,METHOD,MESSAGE) \
    static efd::MessageHandlerImpl< \
    CLASS, \
    MESSAGE> \
    EE_MAKE_UNIQUE_NAME(HandlerObject)(&CLASS::METHOD);

/// Macro for creating a static auto-registering object for setting up mapping of message to method
/// Used in cases where a parent class of the actual message is in the handler signature.
#define EE_HANDLER_SUBCLASS(CLASS,METHOD,BASEMESSAGE,MESSAGE) \
    static efd::MessageSubclassHandler< \
    CLASS, \
    BASEMESSAGE, \
    MESSAGE> \
    EE_MAKE_UNIQUE_NAME(HandlerObject)(&CLASS::METHOD);

/// Macro for creating a static auto-registering object for setting up mapping of message to method
/// Used in cases where MessageWrapper is used to change classid of a message.
#define EE_HANDLER_WRAP(CLASS,METHOD,BASEMESSAGE,MESSAGETYPE) \
    static efd::MessageWrapperSubclassHandler< \
    CLASS, \
    BASEMESSAGE, \
    BASEMESSAGE, \
    MESSAGETYPE > \
    EE_MAKE_UNIQUE_NAME(HandlerObject)(&CLASS::METHOD);

/// Macro for creating a static auto-registering object for setting up mapping of message to method
/// Used in cases where a parent class of the actual message is in the handler signature, and
/// where MessageWrapper is used to change classid of the message
#define EE_HANDLER_SUBCLASS_WRAPPER(CLASS,METHOD,BASEMESSAGE,MESSAGE,MESSAGETYPE) \
    static efd::MessageWrapperSubclassHandler< \
    CLASS, \
    BASEMESSAGE, \
    MESSAGE, \
    MESSAGETYPE> \
    EE_MAKE_UNIQUE_NAME(HandlerObject)(&CLASS::METHOD);


} // end namespace efd

#endif // EE_BASEMESSAGEHANDLER_H

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
#ifndef EE_NETMESSAGE_H
#define EE_NETMESSAGE_H

#include <efd/IMessage.h>
#include <efd/DataStream.h>
#include <efd/NetEnums.h>

#include <efd/ILogger.h>
#include <efd/efdMessageIDs.h>
#include <efd/Category.h>
#include <efd/ConnectionID.h>

#include <efd/MessageFactory.h>
#include <efd/efdClassIDs.h>


namespace efd
{

/**
    Supports network messaging.
*/
class EE_EFD_ENTRY NetMessage : public DirectedMessage
{
    /// @cond EMERGENT_INTERNAL
    EE_DECLARE_CLASS1(NetMessage, efd::kCLASSID_NetMessage, IMessage);
    EE_DECLARE_CONCRETE_REFCOUNT;
    /// @endcond

public:
    /// @name Construction and Destruction
    //@{

    /// Default NetMessage Constructor
    NetMessage();

    /** 
        Construct a NetMessage from attributes.

        @param cid The connection ID.
        @param senderNetID The sender's network ID.
        @param cat The message category. @see Category. 
    */
    NetMessage( const ConnectionID& cid, efd::UInt32 senderNetID = 0, Category cat = kCAT_INVALID);

    /// virtual Destructor
    virtual ~NetMessage();
    //@}

    /**
        Method to convert this class to a stream.
    
        @param strm Stream to write this class contents into.
    */
    virtual void ToStream( efd::DataStream &strm ) const;

    /**
        Method to read the contents of an envelope message from a stream.
    
        @param strm Stream to read the from.
    */
    virtual bool FromStream( const efd::DataStream &strm );

    /// @name Accessors for SenderConnection
    //@{
    inline void SetSenderConnection( const ConnectionID& cid );

    inline const ConnectionID& GetSenderConnection() const;
    //@}

    /// @name Accessors for SenderNetID
    //@{
    inline void SetSenderNetID( efd::UInt32 senderNetID );

    inline efd::UInt32 GetSenderNetID() const;
    //@}

protected:
    /// Private category ID of the sender (most recent hop) of this message
    ConnectionID m_senderConnection;

    /// NetID of the original sender of this message
    efd::UInt32 m_senderNetID;
};

///SmartPointer for the NetMessage class.
typedef efd::SmartPointer<NetMessage> NetMessagePtr;

/**
    Implements the network message envelope.
*/
class EE_EFD_ENTRY EnvelopeMessage : public NetMessage
{
    /// @cond EMERGENT_INTERNAL
    EE_DECLARE_CLASS1(EnvelopeMessage, efd::kCLASSID_EnvelopeMessage, NetMessage);
    /// @endcond

public:
    /// @name Construction and Destruction
    //@{

    /// Constructor creates an empty envelope message that can be filled from a stream
    EnvelopeMessage();

    /**
        Constructor which creates an envelope message class containing the given payload message
          for sending to the specified category.
         
        @param pMessage The message to add to the envelope message
        @param cat The category the message will be sent on
        @param senderNetID [optional] Only INetLib should ever use this parameter.
            NetID for the sending process.
        @param sender [optional] Only INetLib should ever use this parameter.
            Connection on which envelope was received.
    */
    EnvelopeMessage( IMessage *pMessage, const Category& cat, efd::UInt32 senderNetID = 0,
                     const ConnectionID& sender = kCID_INVALID );

public:

    /**
        Constructor which creates an envelope message class containing an already streamed
        message of the given type for sending to the specified category.
     
        @param idChildType The message type for the message in the DataStream
        @param pChildStreamedContents The streamed message data resulting from IMessage::ToStream
        @param cbChildStreamOffset The offset within the stream where the message begins.  
            Typically this is 0.
        @param cbChildStreamSize The size of the serialized message within the data stream.
        @param cat The category the message will be sent on.
    */
    EnvelopeMessage( 
        efd::ClassID idChildType, 
        const efd::DataStream* pChildStreamedContents,
        size_t cbChildStreamOffset,
        efd::UInt32 cbChildStreamSize,
        const Category &cat );

public:
    /// Destructor
    virtual ~EnvelopeMessage();
    //@}   

    static const int kDESTINATION_OFFSET;
    static const int kSIZE_BEFORE_MESSAGE_OFFSET;

    /**
        Get the contents stream.

        @param pOutStream The output stream.
    */
    void GetContentsStream(efd::DataStream* pOutStream) const;

    /**
        Returns the message contents.
        @param pMessageFactory A pointer to the MessageFactory that will be used to create the 
        message.
    */
    virtual const IMessage* GetContents(MessageFactory* pMessageFactory) const;
    
    /// Returns the message contents class ID.
    virtual efd::ClassID GetContentsClassID() const;

    /// Returns the message contents unique identifier.
    virtual efd::UInt32 GetContentsUniqueIdentifier() const;

    /// Returns the child description.
    virtual efd::utf8string GetChildDescription() const;

    /**
        Set the child of the message.

        @param pMessage The child Message of this Envelope.
    */
    inline void SetChild(IMessage *pMessage);

    /// Returns true if the envelope contains a message.
    inline bool IsInflated() const;

    /**
        Converts this class to a stream.
     
        @param strm Stream to write this class contents into.
    */
    virtual void ToStream( efd::DataStream &strm ) const;
    
    /**
        Reads the contents of an envelope message from a stream.
     
        @param strm Stream to read this object from.
    */
    virtual bool FromStream( const efd::DataStream &strm );

    /// Sets the contents type.
    inline void SetContentsType(efd::ClassID type);

    /// @name Accessors for Quality of Service
    //@{
    inline efd::QualityOfService GetQualityOfService() const;
    inline void SetQualityOfService( efd::QualityOfService qos );
    //@}

private:
    /**
        Streams a message out of m_spStream.
        
        This method is const because it does not change the logical contents of the envelope.
    */
    virtual void InflateContents(MessageFactory* pMessageFactory) const;

    /// Pointer to message.
    mutable IMessagePtr m_spMessage; 

    /// msgClassID for the message contained in this message.
    efd::ClassID m_contentsClassID;

    /// Identity of contained message for logging non-inflated messages.
    efd::UInt32 m_childIdentity;

    /// In order to hold reference to a common stream we need three bits of data, the stream, the
    /// offset within that stream to begin, and the size of the data we can read at that offset.
    //@{
    /// Pointer to stream that contains the contents of the child.
    SmartPointer<const efd::DataStream> m_spStream;
    /// The position in the stream when first set, so it can be reset with a seek.
    size_t m_strmPos;  
    /// Size of the data to be read.
    efd::UInt32 m_SubMessageSize;
    //@}
    /// Quality of Service.
    efd::QualityOfService m_qos;
};

///A smart pointer (reference counting, self deleting pointer) for the EnvelopeMessage class.
typedef efd::SmartPointer<EnvelopeMessage> EnvelopeMessagePtr;

//-------------------------------------------------------------------------------------------------
/**
    Request AssignNetIDMessage be sent.
*/
class EE_EFD_ENTRY RequestNetIDMessage : public NetMessage
{
    /// @cond EMERGENT_INTERNAL
    EE_DECLARE_CLASS1(RequestNetIDMessage, kMSGID_RequestNetID, NetMessage);
    /// @endcond

public:
    /// @name Construction and Destruction
    //@{
    /// Default Constructor
    RequestNetIDMessage();

    /// Constructor
    RequestNetIDMessage( const ConnectionID& cid,
        efd::UInt32 senderNetID,
        efd::Category privateResponseChannel );
    //@}

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual void ToStream( efd::DataStream &strm ) const;
    virtual bool FromStream( const efd::DataStream &strm );

    /// @name Accessors for AssignedNetID
    //@{
    inline void SetPrivateResponseChannel( efd::Category privateResponseChannel );

    inline efd::Category GetPrivateResponseChannel() const;
    //@}

protected:
    /// The private Category to send AssignNetIDMessage on
    efd::Category m_privateResponseChannel;
};

/// SmartPointer for the RequestNetIDMessage class
typedef efd::SmartPointer<RequestNetIDMessage> RequestNetIDMessagePtr;


//-------------------------------------------------------------------------------------------------
/**
    Assign NetID Messages.
*/
class EE_EFD_ENTRY AssignNetIDMessage : public NetMessage
{
    /// @cond EMERGENT_INTERNAL
    EE_DECLARE_CLASS1(AssignNetIDMessage, kMSGID_SetNetID, NetMessage);
    /// @endcond

    /// @name Construction and Destruction
    //@{
public:
    /// Default constructor.
    AssignNetIDMessage();
    /** 
        Construct an AssignNetID message from attributes.
        @param cid The connection ID.
        @param senderNetID The sender's network ID.
        @param assignedNetID The message category. @see Category. 
    */
    AssignNetIDMessage( const ConnectionID& cid, efd::UInt32 senderNetID, 
        efd::UInt32 assignedNetID );
    //@}

public:
    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual void ToStream( efd::DataStream &strm ) const;
    virtual bool FromStream( const efd::DataStream &strm );

    /// @name Accessors for AssignedNetID
    //@{
    inline void SetAssignedNetID( efd::UInt32 assignedNetID );

    inline efd::UInt32 GetAssignedNetID() const;
    //@}

protected:
    /// Assigned network ID.
    efd::UInt32 m_assignedNetID;
};

/// A smart pointer (reference counting, self deleting pointer) for the AssignNetIDMessage class.
typedef efd::SmartPointer<AssignNetIDMessage> AssignNetIDMessagePtr;


//-------------------------------------------------------------------------------------------------
/**
    Messages used to announce a NetID 
*/
class EE_EFD_ENTRY AnnounceNetIDMessage : public NetMessage
{
    /// @cond EMERGENT_INTERNAL
    EE_DECLARE_CLASS1(AnnounceNetIDMessage, kMSGID_AnnounceNetID, NetMessage);
    /// @endcond

    /// @name Construction and Destruction
    //@{
public:
    /// Default constructor.
    AnnounceNetIDMessage();
    /** 
    Construct an AssignNetID message from attributes.
    @param cid The connection ID.
    @param senderNetID The sender's network ID.
    @param announcedConnectionID The announced connection ID.
    */
    AnnounceNetIDMessage(
        const ConnectionID& cid,
        efd::UInt32 senderNetID,
        const ConnectionID& announcedConnectionID);
    //@}

public:
    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual void ToStream(efd::DataStream &strm) const;
    virtual bool FromStream(const efd::DataStream &strm);

    /// @name Accessors for AssignedNetID
    //@{
    inline void SetAnnouncedConnectionID(efd::ConnectionID announcedConnectionID);

    inline efd::ConnectionID GetAnnouncedConnectionID() const;
    //@}

protected:
    /// Announced connection ID.
    efd::ConnectionID m_announcedConnectionID;
};

///A smart pointer (reference counting, self deleting pointer) for the AssignNetIDMessage class.
typedef efd::SmartPointer<AnnounceNetIDMessage> AnnounceNetIDMessagePtr;

} // end namespace efd

#include <efd/NetMessage.inl>

#endif //EE_NETMESSAGE


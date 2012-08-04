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


#include "efdPCH.h"

#include <efd/NetMessage.h>
#include <efd/efdLogIDs.h>


//-------------------------------------------------------------------------------------------------
using namespace efd;


//-------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO( efd::EnvelopeMessage );
EE_IMPLEMENT_CONCRETE_CLASS_INFO( efd::NetMessage );
EE_IMPLEMENT_CONCRETE_CLASS_INFO( efd::AssignNetIDMessage );
EE_IMPLEMENT_CONCRETE_CLASS_INFO( efd::RequestNetIDMessage );
EE_IMPLEMENT_CONCRETE_CLASS_INFO( efd::AnnounceNetIDMessage );

const int EnvelopeMessage::kDESTINATION_OFFSET = 0;
const int EnvelopeMessage::kSIZE_BEFORE_MESSAGE_OFFSET = 24;

//-------------------------------------------------------------------------------------------------
EnvelopeMessage::EnvelopeMessage(
    IMessage *pMessage,
    const Category &cat,
    efd::UInt32 senderNetID,
    const ConnectionID& sender )
    : NetMessage( sender, senderNetID, cat )
    , m_spMessage(pMessage)
    , m_contentsClassID( efd::k_invalidMessageClassID )
    , m_childIdentity( 0 )
    , m_spStream(NULL)
    , m_strmPos(static_cast<size_t>(-1))
    , m_SubMessageSize( 0 )
{
    if ( pMessage )
    {
        m_contentsClassID = pMessage->GetClassID();
        m_childIdentity = pMessage->GetUniqueIdentifier();

        // Transfer debug flags from the message to the envelope. 
        // Many of the debug flags only apply to the INetLib envelope that always gets created.
        SetDebugFlags(GetDebugFlags() | pMessage->GetDebugFlags());

        EE_MESSAGE_TRACE(
            this,
            efd::kMessageTrace,
            efd::ILogger::kLVL1,
            efd::ILogger::kLVL2,
            ("%s| placed into envelope 0x%08X",
            pMessage->GetDescription().c_str(), GetUniqueIdentifier()) );
    }
}


//-------------------------------------------------------------------------------------------------
EnvelopeMessage::EnvelopeMessage(
    efd::ClassID idChildType,
    const efd::DataStream* pChildStreamedContents,
    size_t cbChildStreamOffset,
    efd::UInt32 cbChildStreamSize,
    const Category &cat )
    : NetMessage( kCID_INVALID, 0, cat )
    , m_spMessage()
    , m_contentsClassID( idChildType )
    , m_childIdentity( 0 )
    , m_spStream( pChildStreamedContents )
    , m_strmPos( cbChildStreamOffset )
    , m_SubMessageSize( cbChildStreamSize )
    , m_qos( QOS_INVALID )
{
    // You must specify a valid content type, stream, and size.
    EE_ASSERT( m_contentsClassID );
    EE_ASSERT( m_spStream );
    EE_ASSERT( m_SubMessageSize );

    EE_MESSAGE_TRACE(
        this,
        efd::kMessageTrace,
        efd::ILogger::kLVL1,
        efd::ILogger::kLVL2,
        ("%s| created with pre-streamed payload of type %s",
        GetDescription().c_str(), IMessage::ClassIDToString(m_SubMessageSize).c_str()) );
}


//-------------------------------------------------------------------------------------------------
EnvelopeMessage::EnvelopeMessage()
    : NetMessage()
    , m_spMessage(NULL)
    , m_contentsClassID(0)
    , m_childIdentity( 0 )
    , m_spStream(NULL)
    , m_strmPos(static_cast<size_t>(-1))
    , m_SubMessageSize(0)
    , m_qos( QOS_INVALID )
{}


//-------------------------------------------------------------------------------------------------
EnvelopeMessage::~EnvelopeMessage()
{
    m_spMessage = NULL;
    m_spStream = NULL;
}


//-------------------------------------------------------------------------------------------------
void EnvelopeMessage::InflateContents(MessageFactory* pMessageFactory) const
{
    // only inflate if the child message is currently NULL
    if (m_spMessage)
    {
        return;
    }

    if (!m_spStream)
    {
        EE_LOG(efd::kNetMessage, efd::ILogger::kERR1,
            ("Error: Attempted to call InflateContents on an envelope with no stream."));
        return;
    }

    m_spStream->Seek(m_strmPos);

    efd::ClassID cid;
    *m_spStream >> cid;

    if (cid != INVALID_CLASSID)
    {
        IBasePtr spBase = pMessageFactory->CreateObject(cid);
        m_spMessage = EE_DYNAMIC_CAST(IMessage,spBase);

        if ( !m_spMessage )
        {
            EE_LOG( efd::kMessageTrace, efd::ILogger::kERR1,
                ("%s| failed to factory child 0x%08X of type %s",
                GetDescription().c_str(),
                m_childIdentity,
                IMessage::ClassIDToString(m_contentsClassID).c_str()));
        }
        else
        {
            // fill the message from stream
            *m_spStream >> *m_spMessage;
            EE_MESSAGE_TRACE(
                this,
                efd::kMessageTrace,
                efd::ILogger::kLVL1, efd::ILogger::kLVL2,
                ("%s: 0x%08X| inflating %s sender NetID=%d",
                IMessage::ClassIDToString(GetClassID()).c_str(),
                GetUniqueIdentifier(),
                m_spMessage->GetDescription().c_str(),
                GetSenderNetID()
                ) );
        }
    }
    else
    {
        EE_LOG( efd::kMessageTrace, efd::ILogger::kERR1,
            ("%s| 0x%08X Invalid child class id",
            GetDescription().c_str(),
            m_childIdentity));
    }
}


//-------------------------------------------------------------------------------------------------
void EnvelopeMessage::GetContentsStream(efd::DataStream* pOutStream) const
{
    if (!pOutStream)
    {
        EE_LOG(efd::kNetMessage, efd::ILogger::kERR1,
            ("Error: Attempted to call GetContentsStream on an envelope with no stream."));
        return;
    }

    pOutStream->Write(*pOutStream, m_strmPos, pOutStream->GetSize() - m_strmPos);
}


//-------------------------------------------------------------------------------------------------
const IMessage* EnvelopeMessage::GetContents(MessageFactory* pMessageFactory) const
{
    if ( !m_spMessage )
    {
        InflateContents(pMessageFactory);
    }
    return m_spMessage;
}


//-------------------------------------------------------------------------------------------------
efd::ClassID EnvelopeMessage::GetContentsClassID() const
{
    return m_contentsClassID;
}

//-------------------------------------------------------------------------------------------------
efd::UInt32 EnvelopeMessage::GetContentsUniqueIdentifier() const
{
    //!< Identity of contained message for logging non-inflated messages
    return m_childIdentity;
}


//-------------------------------------------------------------------------------------------------
efd::utf8string EnvelopeMessage::GetChildDescription() const
{
    efd::utf8string description( efd::Formatted, "%s: 0x%08X",
        IMessage::ClassIDToString(GetContentsClassID()).c_str(), GetContentsUniqueIdentifier() );
    return description;
}


//-------------------------------------------------------------------------------------------------
void EnvelopeMessage::ToStream(DataStream &strm) const
{
    if ( !GetDestinationCategory().IsValid() )
    {
        EE_LOG(efd::kNetMessage, efd::ILogger::kERR1,
            ("Error: Envelope ToStream invalid category, %s.",
            GetDestinationCategory().ToString().c_str()));
    }

    // ************ HUGE NOTE HERE!!! *****************
    // if you change the ordering or location of m_destination, be sure to update 
    // kDESTINATION_OFFSET
    // this can be used by specific INetLib implementations for the routing of the packets. 
    // Also, don't put any "ifdef" or unknown size calls between here and the 
    // strm << *m_spMessage; line.
    strm << GetDestinationCategory();  // writes 8 bytes
    strm << m_senderNetID;  // writes 4 bytes
    strm << m_contentsClassID; // writes 4 bytes

    if (m_spMessage)
    {
        size_t markerPos = strm.GetPos();
        UInt32 size = 0;
        // stream a placeholder value for the size of the message data.  Later we will right
        // over this with the real value.
        strm << size; // writes 4 bytes

        UInt32 startPos = (UInt32)strm.GetPos();
        strm << m_spMessage->GetClassID(); // writes 4 bytes
        strm << *m_spMessage;
        UInt32 endPos = (UInt32)strm.GetPos();

        // Try to ensure no one is doing bad things inside their IMessage::ToStream like reseting
        // the stream position.
        EE_ASSERT( endPos > startPos );

        size = endPos - startPos;
        strm.Seek( markerPos );
        strm << size;   // now write the real size over top

        // Finally return to the end position in case anything further needs to be written to
        // the same stream
        strm.Seek( endPos );
    }
    else if (m_spStream)
    {
        strm << m_SubMessageSize;

        EE_ASSERT(m_strmPos >= 0);

        strm.Write( *m_spStream, m_strmPos, m_SubMessageSize );
    }
    else
    {
        strm << 4;  // size of a class ID
        strm << INVALID_CLASSID;
    }

    strm << m_childIdentity;

    IMessage::ToStream( strm ); // moved down here due to the GetDestinationCategory() note above.


}


//-------------------------------------------------------------------------------------------------
bool EnvelopeMessage::FromStream(const DataStream &strm)
{
    efd::Category dest;
    strm >> dest;
    SetDestinationCategory(dest);

    //EE_ASSERT( GetDestinationCategory().IsValid() );
    if ( !GetDestinationCategory().IsValid() )
    {
        EE_LOG(efd::kNetMessage, efd::ILogger::kERR1,
            ("Error: Envelope FromStream invalid category, %s.",
            GetDestinationCategory().ToString().c_str()));
    }

    strm >> m_senderNetID;
    strm >> m_contentsClassID;

    strm >> m_SubMessageSize;

    // save a reference to the stream with the class to inflate the contents later.
    m_spStream = &strm;
    m_strmPos = strm.GetPos();

    // Skip the bytes for the sub-message
    strm.Seek( strm.GetPos() + m_SubMessageSize );

    strm >> m_childIdentity;

    IMessage::FromStream( strm );

    return true;
}


//-------------------------------------------------------------------------------------------------
NetMessage::NetMessage():
    m_senderConnection(kCID_INVALID),
    m_senderNetID(0)
{
}


//-------------------------------------------------------------------------------------------------
NetMessage::NetMessage(const ConnectionID& cid, efd::UInt32 senderNetID, Category cat):
    m_senderConnection(cid),
    m_senderNetID(senderNetID)
{
    SetDestinationCategory(cat);
}


//-------------------------------------------------------------------------------------------------
NetMessage::~NetMessage()
{
}


//-------------------------------------------------------------------------------------------------
void NetMessage::ToStream(DataStream &strm) const
{
    IMessage::ToStream( strm );
    strm << GetDestinationCategory();
    strm << m_senderConnection; 
    strm << m_senderNetID;
}


//-------------------------------------------------------------------------------------------------
bool NetMessage::FromStream(const DataStream &strm)
{
    IMessage::FromStream( strm );

    efd::Category dest;
    strm >> dest;
    SetDestinationCategory(dest);

    strm >> m_senderConnection;
    strm >> m_senderNetID;
    return true;
}


//-------------------------------------------------------------------------------------------------
RequestNetIDMessage::RequestNetIDMessage()
{
}


//-------------------------------------------------------------------------------------------------
RequestNetIDMessage::RequestNetIDMessage( const ConnectionID& connectionPrivateCat,
                                        efd::UInt32 senderNetID, 
                                        efd::Category privateResponseChannel )
    : NetMessage( connectionPrivateCat, senderNetID )
    , m_privateResponseChannel(privateResponseChannel)
{
}


//-------------------------------------------------------------------------------------------------
void RequestNetIDMessage::ToStream( efd::DataStream &strm ) const
{
    NetMessage::ToStream( strm );
    strm << m_privateResponseChannel;
}


//-------------------------------------------------------------------------------------------------
bool RequestNetIDMessage::FromStream( const efd::DataStream &strm )
{
    bool retVal = NetMessage::FromStream( strm );
    if( !retVal )
        return false;
    strm >> m_privateResponseChannel;
    return true;
}

//-------------------------------------------------------------------------------------------------
AssignNetIDMessage::AssignNetIDMessage()
: m_assignedNetID(kNetID_Unassigned)
{
}


//-------------------------------------------------------------------------------------------------
AssignNetIDMessage::AssignNetIDMessage( const ConnectionID& connectionPrivateCat,
                                       efd::UInt32 senderNetID, efd::UInt32 assignedNetID )
                                       : NetMessage( connectionPrivateCat, senderNetID )
                                       , m_assignedNetID(assignedNetID)
{
}


//-------------------------------------------------------------------------------------------------
void AssignNetIDMessage::ToStream( efd::DataStream &strm ) const
{
    NetMessage::ToStream( strm );
    strm << m_assignedNetID;
}


//-------------------------------------------------------------------------------------------------
bool AssignNetIDMessage::FromStream( const efd::DataStream &strm )
{
    bool retVal = NetMessage::FromStream( strm );
    if( !retVal )
        return false;
    strm >> m_assignedNetID;
    return true;
}

//-------------------------------------------------------------------------------------------------
AnnounceNetIDMessage::AnnounceNetIDMessage()
{
}


//-------------------------------------------------------------------------------------------------
AnnounceNetIDMessage::AnnounceNetIDMessage( 
    const ConnectionID& cid,
    efd::UInt32 senderNetID, 
    const ConnectionID& announcedConnectionID )
    : NetMessage(cid, senderNetID)
    , m_announcedConnectionID(announcedConnectionID)
{
}


//-------------------------------------------------------------------------------------------------
void AnnounceNetIDMessage::ToStream(efd::DataStream &strm) const
{
    NetMessage::ToStream(strm);
    strm << m_announcedConnectionID;
}


//-------------------------------------------------------------------------------------------------
bool AnnounceNetIDMessage::FromStream(const efd::DataStream &strm)
{
    bool retVal = NetMessage::FromStream(strm);
    if( !retVal )
        return false;
    strm >> m_announcedConnectionID;
    return true;
}


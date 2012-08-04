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

#include <efd/StreamMessage.h>
#include <efd/DataStream.h>
#include <efd/ILogger.h>
#include <efd/efdLogIDs.h>

//-------------------------------------------------------------------------------------------------
using namespace efd;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(StreamMessage);


//-------------------------------------------------------------------------------------------------
StreamMessage::StreamMessage()
: IMessage()
, m_spStream(NULL)
{
    m_spStream = EE_NEW DataStream ();
    if (!m_spStream)
    {
        EE_LOG(efd::kServiceManager, efd::ILogger::kERR0,
            ("MsgStream Error: Failed to allocate DataStream to StreamMessage, NULL pointer"));
    }
}





//-------------------------------------------------------------------------------------------------
StreamMessage::~StreamMessage()
{
    m_spStream = NULL;
}


//-------------------------------------------------------------------------------------------------
StreamMessage::operator DataStream &()
{
    return *m_spStream;
}


//-------------------------------------------------------------------------------------------------
StreamMessage::operator const DataStream&() const
{
    return *m_spStream;
}


//-------------------------------------------------------------------------------------------------
efd::SmartPointer<const efd::DataStream> StreamMessage::GetParameters() const
{
    return (efd::DataStream*)m_spStream;
}


//-------------------------------------------------------------------------------------------------
bool StreamMessage::FromStream(const DataStream &strm)
{
    IMessage::FromStream( strm );

    // stream off the stream
    strm >> *m_spStream;

    return true;
}


//-------------------------------------------------------------------------------------------------
void StreamMessage::ToStream(DataStream &strm) const
{
    IMessage::ToStream( strm );

    // stream the data
    strm << *m_spStream;
}


//-------------------------------------------------------------------------------------------------
void StreamMessage::Reset() const
{
    if (m_spStream)
    {
        m_spStream->Reset();
    }
}


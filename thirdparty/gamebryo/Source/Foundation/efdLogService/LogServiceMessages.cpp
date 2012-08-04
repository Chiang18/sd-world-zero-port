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
// Emergent Game Technologies, Calabasas, California 91302
// http://www.emergent.net

#include "efdLogServicePCH.h"

#include <efdLogService/LogServiceMessages.h>


//-------------------------------------------------------------------------------------------------
using namespace efd;


EE_IMPLEMENT_CONCRETE_CLASS_INFO(LogServiceRequest);
EE_IMPLEMENT_CONCRETE_CLASS_INFO(GetModulesResponse);
EE_IMPLEMENT_CONCRETE_CLASS_INFO(GetLogLevelResponse);
EE_IMPLEMENT_CONCRETE_CLASS_INFO(GetMsgLogLevelResponse);



//-------------------------------------------------------------------------------------------------
void LogServiceRequest::Set(
    efd::UInt32 netid,
    const efd::Category& i_callback)
{
    m_netid = netid;
    m_callback = i_callback;
}


//-------------------------------------------------------------------------------------------------
LogServiceRequest::~LogServiceRequest()
{
}


//-------------------------------------------------------------------------------------------------
void LogServiceRequest::ToStream(efd::DataStream& strm) const
{
    IMessage::ToStream(strm);
    strm << m_netid;
    strm << m_callback;
}


//-------------------------------------------------------------------------------------------------
bool LogServiceRequest::FromStream(const efd::DataStream& strm)
{
    IMessage::FromStream(strm);
    strm >> m_netid;
    strm >> m_callback;
    return true;
}



//-------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(LogServiceResponse);


//-------------------------------------------------------------------------------------------------
LogServiceResponse::LogServiceResponse()
    : m_netid(0)
    , m_result(lsr_Unknown)
{
}


//-------------------------------------------------------------------------------------------------
void LogServiceResponse::Set(
    efd::UInt32 i_netid,
    LogServiceResponse::Result i_result)
{
    m_netid = i_netid;
    m_result = i_result;
}


//-------------------------------------------------------------------------------------------------
LogServiceResponse::~LogServiceResponse()
{
}


//-------------------------------------------------------------------------------------------------
// convert this class into a stream of atomic types.
void LogServiceResponse::ToStream(efd::DataStream& strm) const
{
    StreamMessage::ToStream(strm);
    strm.Write(m_netid);
    strm.Write((UInt32)m_result);
}

//-------------------------------------------------------------------------------------------------
// populate this class with data from the stream
bool LogServiceResponse::FromStream(const efd::DataStream& strm)
{
    StreamMessage::FromStream(strm);
    strm.Read(m_netid);
    efd::UInt32 tmp;
    strm.Read(tmp);
    m_result = static_cast<efd::LogServiceResponse::Result>(tmp);
    return true;
}


//-------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(LogEntriesMessage);


//-------------------------------------------------------------------------------------------------
LogEntriesMessage::LogEntriesMessage()
{
}


//-------------------------------------------------------------------------------------------------
LogEntriesMessage::~LogEntriesMessage()
{
}


//-------------------------------------------------------------------------------------------------
LogEntriesMessage::LogEntriesMessage(efd::UInt32 netId,
                                     efd::UInt64 pid,
                                     const efd::vector< efd::utf8string > &entries)
    : m_netID(netId)
    , m_pid(pid)
    , m_entries(entries)
{
}


//-------------------------------------------------------------------------------------------------
void LogEntriesMessage::ToStream(efd::DataStream& strm) const
{
    IMessage::ToStream(strm);

    strm << m_netID;
    strm << m_pid;

    strm << (efd::UInt32)m_entries.size();
    for (LogEntries::const_iterator it = m_entries.begin(); it != m_entries.end(); ++it)
    {
        strm << *it;
    }
}


//-------------------------------------------------------------------------------------------------
bool LogEntriesMessage::FromStream(const efd::DataStream& strm)
{
    IMessage::FromStream(strm);

    strm >> m_netID;
    strm >> m_pid;

    efd::UInt32 numEntries;
    strm >> numEntries;
    for (efd::UInt32 i = 0; i < numEntries; ++i)
    {
        utf8string tmp;
        strm >> tmp;

        m_entries.push_back(tmp);
    }
    return true;
}


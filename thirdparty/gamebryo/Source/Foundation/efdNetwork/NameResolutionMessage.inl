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

namespace efd
{

//-------------------------------------------------------------------------------------------------
inline NameResolutionMessage::NameResolutionMessage()
    : m_classID(0)
    //, m_name
    , m_port(0)
    //, m_hostname;
    //, m_responseCategory;
    , m_responsePort(0)
    , m_responseIP(0)
    , m_timestamp(0.0f)
    , m_lastSend(0.0f)
    , m_delay(0.0f)
    , m_requestNumber(0)
{
}

//-------------------------------------------------------------------------------------------------
inline NameResolutionMessage::~NameResolutionMessage()
{
}

//-------------------------------------------------------------------------------------------------
inline efd::UInt32 NameResolutionMessage::GetServiceClassID()
{
    return m_classID;
}

//-------------------------------------------------------------------------------------------------
inline void NameResolutionMessage::SetServiceClassID(efd::UInt32 classID)
{
    m_classID = classID;
}

//-------------------------------------------------------------------------------------------------
inline efd::utf8string NameResolutionMessage::GetName()
{
    return m_name;
}

//-------------------------------------------------------------------------------------------------
inline void NameResolutionMessage::SetName(efd::utf8string name)
{
    m_name = name;
}

//-------------------------------------------------------------------------------------------------
inline efd::utf8string NameResolutionMessage::GetHostname()
{
    return m_hostname;
}

//-------------------------------------------------------------------------------------------------
inline void NameResolutionMessage::SetHostname(efd::utf8string hostname)
{
    m_hostname = hostname;
}

//-------------------------------------------------------------------------------------------------
inline efd::UInt16 NameResolutionMessage::GetPort()
{
    return m_port;
}

//-------------------------------------------------------------------------------------------------
inline void NameResolutionMessage::SetPort(efd::UInt16 port)
{
    m_port = port;
}

//-------------------------------------------------------------------------------------------------
inline efd::Category NameResolutionMessage::GetResponseCategory()
{
    return m_responseCategory;
}

//-------------------------------------------------------------------------------------------------
inline void NameResolutionMessage::SetResponseCategory(efd::Category responseCategory)
{
    m_responseCategory = responseCategory;
}

//-------------------------------------------------------------------------------------------------
inline efd::UInt32 NameResolutionMessage::GetResponseIP()
{
    return m_responseIP;
}

//-------------------------------------------------------------------------------------------------
inline void NameResolutionMessage::SetResponseIP(efd::UInt32 ip)
{
    m_responseIP = ip;
}

//-------------------------------------------------------------------------------------------------
inline efd::UInt16 NameResolutionMessage::GetResponsePort()
{
    return m_responsePort;
}

//-------------------------------------------------------------------------------------------------
inline void NameResolutionMessage::SetResponsePort(efd::UInt16 port)
{
    m_responsePort = port;
}

//-------------------------------------------------------------------------------------------------
inline efd::TimeType NameResolutionMessage::GetTimestamp()
{
    return m_timestamp;
}

//-------------------------------------------------------------------------------------------------
inline void NameResolutionMessage::SetTimestamp(efd::TimeType timestamp)
{
    m_timestamp = timestamp;
}

//-------------------------------------------------------------------------------------------------
inline efd::TimeType NameResolutionMessage::GetRequestTimeout()
{
    return m_request_timeout;
}

//-------------------------------------------------------------------------------------------------
inline void NameResolutionMessage::SetRequestTimeout(efd::TimeType tmo)
{
    m_request_timeout = tmo;
}

//-------------------------------------------------------------------------------------------------
inline void NameResolutionMessage::ToStream(efd::DataStream &strm) const
{
    IMessage::ToStream(strm);
    strm << m_classID;
    strm << m_name;
    strm << m_port;
    strm << m_hostname;
    strm << m_responseCategory;
    strm << m_responseIP;
    strm << m_responsePort;
    strm << m_timestamp;
    strm << m_requestNumber;
}

//-------------------------------------------------------------------------------------------------
inline bool NameResolutionMessage::FromStream(const efd::DataStream &strm)
{
    IMessage::FromStream(strm);
    strm >> m_classID;
    strm >> m_name;
    strm >> m_port;
    strm >> m_hostname;
    strm >> m_responseCategory;
    strm >> m_responseIP;
    strm >> m_responsePort;
    strm >> m_timestamp;
    strm >> m_requestNumber;
    return true;
}

//-------------------------------------------------------------------------------------------------
inline bool NameResolutionMessage::operator==(const NameResolutionMessage& a) const
{
    return (m_classID == a.m_classID
        && m_name == a.m_name
        && m_port == a.m_port
        && m_hostname == a.m_hostname
        // ignore response category as it can change
        //&& m_responseCategory == a.m_responseCategory
        && m_responsePort == a.m_responsePort
        && m_responseIP == a.m_responseIP
        && m_timestamp == a.m_timestamp);
}

//-------------------------------------------------------------------------------------------------
inline bool NameResolutionMessage::operator<(const NameResolutionMessage& a) const
{
    if(m_classID > a.m_classID)
        return false;
    if(m_name > a.m_name)
        return false;
    if(m_port > a.m_port)
        return false;
    if(m_hostname > a.m_hostname)
        return false;
    // ignore response category as it can change
    //if(m_responseCategory > a.m_responseCategory)
    //    return false;
    if(m_responsePort > a.m_responsePort)
        return false;
    if(m_responseIP > a.m_responseIP)
        return false;
    if(m_timestamp > a.m_timestamp)
        return false;
    if((*this) == a)
        return false;
    return true;
}

//-------------------------------------------------------------------------------------------------
inline bool NameResolutionMessagePtrLess::operator()(
    const NameResolutionMessagePtr& a, 
    const NameResolutionMessagePtr& b) const
{
    return (*a < *b);
}

} // end namespace efd

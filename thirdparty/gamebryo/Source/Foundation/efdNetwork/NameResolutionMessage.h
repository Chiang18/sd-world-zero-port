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
#ifndef EE_NAMERESOLUTIONMESSAGE_H
#define EE_NAMERESOLUTIONMESSAGE_H

#include <efd/ISystemService.h>
#include <efd/INetCallback.h>
#include <efd/SmartPointer.h>
#include <efd/IMessage.h>
#include <efd/efdClassIDs.h>

namespace efd
{

/**
    Message used by the NameResolutionService to send Requests and Resopnses
*/
class EE_EFDNETWORK_ENTRY NameResolutionMessage : public IMessage
{
    EE_DECLARE_CLASS1(NameResolutionMessage, kCLASSID_NameResolutionMessage, IMessage);
public:

    /// @name Construction and Destruction
    //@{
    /// Default constructor.
    NameResolutionMessage();

    /// Virtual destructor.
    virtual ~NameResolutionMessage();
    //@}

    /// @name Accessors
    //@{
    efd::UInt32 GetServiceClassID();
    void SetServiceClassID(efd::UInt32 classID);
    efd::utf8string GetName();
    void SetName(efd::utf8string name);
    efd::utf8string GetHostname();
    void SetHostname(efd::utf8string hostname);
    efd::UInt16 GetPort();
    void SetPort(efd::UInt16 port);
    efd::Category GetResponseCategory();
    void SetResponseCategory(efd::Category responseCategory);
    efd::UInt32 GetResponseIP();
    void SetResponseIP(efd::UInt32 ip);
    efd::UInt16 GetResponsePort();
    void SetResponsePort(efd::UInt16 port);
    efd::TimeType GetTimestamp();
    void SetTimestamp(efd::TimeType timestamp);
    efd::TimeType GetRequestTimeout();
    void SetRequestTimeout(efd::TimeType tmo);
    //@}

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual void ToStream(efd::DataStream &strm) const;
    virtual bool FromStream(const efd::DataStream &strm);

    /// @name Operators
    //@{
    bool operator==(const NameResolutionMessage& a) const;
    bool operator<(const NameResolutionMessage& a) const;
    //@}

protected:
    efd::UInt32 m_classID;
    efd::utf8string m_name;
    efd::UInt16 m_port;
    efd::utf8string m_hostname;
    efd::Category m_responseCategory;
    efd::UInt16 m_responsePort;
    efd::UInt32 m_responseIP;
    efd::TimeType m_timestamp;
    efd::TimeType m_request_timeout;
public:
    efd::TimeType m_lastSend;
    efd::TimeType m_delay;
    efd::UInt16 m_requestNumber;
};
typedef efd::SmartPointer< NameResolutionMessage > NameResolutionMessagePtr;

/**
    Comparison operator for comparing two NameResolutionMessage instances.
    We use this below when defining a Set of NameResolutionMessage smart pointers.
*/
class NameResolutionMessagePtrLess
{
public:
    bool operator()(const NameResolutionMessagePtr& a, const NameResolutionMessagePtr& b) const;
};

typedef efd::set< NameResolutionMessagePtr, NameResolutionMessagePtrLess > 
    NameResolutionMessageSet;
typedef efd::SmartPointer< efd::RefCountedMemObj<NameResolutionMessageSet> > 
    NameResolutionMessageSetPtr;

typedef efd::list< NameResolutionMessagePtr > NameResolutionMessageList;
typedef efd::SmartPointer< efd::RefCountedMemObj< NameResolutionMessageList > > 
    NameResolutionMessageListPtr;

} // end namespace efd

#include <efdNetwork/NameResolutionMessage.inl>

#endif //EE_NAMERESOLUTIONMESSAGE_H



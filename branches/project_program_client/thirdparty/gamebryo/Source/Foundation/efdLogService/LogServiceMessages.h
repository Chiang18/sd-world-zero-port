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

#pragma once
#ifndef LOGSERVICEMESSAGES_H
#define LOGSERVICEMESSAGES_H

#include <efd/StreamMessage.h>
#include <efd/Category.h>
#include <efd/efdMessageIDs.h>
#include <efdNetwork/ServerToClientMessage.h>
#include <efdLogService/efdLogServiceLibType.h>

namespace efd
{
/**
    Base log service request class. All log service request messages contain
    an optional callback category. This category is used by the target to
    send a response back to the caller.
*/
class EE_EFD_LOGSERVICE_ENTRY LogServiceRequest : public efd::StreamMessage
{
    EE_DECLARE_CLASS1(LogServiceRequest, efd::kMSGID_LogServiceRequest, efd::StreamMessage);
    EE_DECLARE_CONCRETE_REFCOUNT;

public:
    LogServiceRequest(){};
    /**
        @param netId The net ID we want to service the request, or 0 for all log services
        @param callback Category used to send responses, if needed.
    */
    void Set(efd::UInt32 netId, const efd::Category& callback);

    virtual ~LogServiceRequest();

    // convert this class into a stream of atomic types.
    virtual void ToStream(efd::DataStream& strm) const;

    // populate this class with data from the stream
    virtual bool FromStream(const efd::DataStream& strm);

    // category to send the response to.
    efd::UInt32 m_netid;
    efd::Category m_callback;
};

typedef efd::SmartPointer<LogServiceRequest> LogServiceRequestPtr;

/// @name MessageWrappers from LogServiceMessages.h
//@{
typedef MessageWrapper<LogServiceRequest,efd::kMSGID_GetLogDestinationsRequest> GetLogDestinationsRequest;
typedef MessageWrapper<LogServiceRequest,efd::kMSGID_GetModulesRequest> GetModulesRequest;
//@}

/**
    Base log service request class for getting / setting specific log levels.
    We add an optional log destination to to the base tLogServiceRequest. If this
    destination is the empty string then the message applies to the default destination.
*/
class EE_EFD_LOGSERVICE_ENTRY LogLevelRequest: public LogServiceRequest
{
public:
    LogLevelRequest() {}
    virtual ~LogLevelRequest() {}

    /**
        @param netId Target identity for the message. e.g. net ID for server log services.
        @param callback Callback category for responses.
        @param dest Log destination
    */
    void Set(efd::UInt32 netId,
        const efd::Category& callback,
        const efd::utf8string& dest)
    {
        LogServiceRequest::Set(netId, callback);
        m_destination = dest;
    }

    // convert this class into a stream of atomic types.
    virtual void ToStream(efd::DataStream& strm) const
    {
        LogServiceRequest::ToStream(strm);
        strm << m_destination;
    }

    // populate this class with data from the stream
    virtual bool FromStream(const efd::DataStream& strm)
    {
        LogServiceRequest::FromStream(strm);
        strm >> m_destination;
        return true;
    }

    efd::utf8string m_destination;
};

/// @name MessageWrappers from LogServiceMessages.h
//@{

typedef MessageWrapper<LogLevelRequest, efd::kMSGID_StopLogStreamRequest> StopLogStreamRequest;
//@}

/**
    Helper class for requesting a module-based log level. We add the moduleId to
    the base tLogLevelRequest to identify which module the caller is interested in.
*/
class EE_EFD_LOGSERVICE_ENTRY BaseGetLogLevelRequest : public LogLevelRequest
{
public:
    BaseGetLogLevelRequest() {}

    virtual ~BaseGetLogLevelRequest() { }

    /**
        @param moduleId The module id in question
        @param netId Net ID of the log service we want to respond. 0 for all.
        @param callback Callback category for responses.
        @param dest Log destination
    */
    void Set(efd::UInt16 moduleId,
        efd::UInt32 netId,
        const efd::Category& callback,
        const efd::utf8string& dest="")
    {
        LogLevelRequest::Set(netId, callback, dest);
        m_moduleId = moduleId;
    }

    // convert this class into a stream of atomic types.
    virtual void ToStream(efd::DataStream& strm) const
    {
        LogLevelRequest::ToStream(strm);
        strm << m_moduleId;
    }

    // populate this class with data from the stream
    virtual bool FromStream(const efd::DataStream& strm)
    {
        LogLevelRequest::FromStream(strm);
        strm >> m_moduleId;
        return true;
    }

    efd::UInt16 m_moduleId;
};

/// @name MessageWrappers from LogServiceMessages.h
//@{

typedef MessageWrapper<BaseGetLogLevelRequest, efd::kMSGID_GetLogLevelRequest> GetLogLevelRequest;
//@}

typedef efd::SmartPointer<GetLogLevelRequest> GetLogLevelRequestPtr;


/**
    Helper class for requesting a category / message type based log level. We add both
    a category and message type id to the base tLogLevelRequest to identify the category
    and / or message type the caller is interested in.
*/
class EE_EFD_LOGSERVICE_ENTRY BaseGetMsgLogLevelRequest: public LogLevelRequest
{
public:
    BaseGetMsgLogLevelRequest() {}

    virtual ~BaseGetMsgLogLevelRequest() { }

    /**
        @param category The category in question, or 0 if not interested.
        @param msgClassID The message type ID in question, or 0 if not interested.
        @param netId Net ID of the log service we want to respond. 0 for all.
        @param callback Callback category for responses.
        @param dest Log destination
    */
    void Set(const efd::Category& category,
        efd::ClassID msgClassID,
        efd::UInt32 netId,
        const efd::Category& callback,
        const efd::utf8string& dest="")
    {
        LogLevelRequest::Set(netId, callback, dest);
        m_category = category;
        m_msgClassID = msgClassID;
    }

    // convert this class into a stream of atomic types.
    virtual void ToStream(efd::DataStream& strm) const
    {
        LogLevelRequest::ToStream(strm);
        strm << m_category;
        strm << m_msgClassID;
    }

    // populate this class with data from the stream
    virtual bool FromStream(const efd::DataStream& strm)
    {
        LogLevelRequest::FromStream(strm);
        strm >> m_category;
        strm >> m_msgClassID;
        return true;
    }

    efd::Category m_category;
    efd::ClassID m_msgClassID;
};

/// @name MessageWrappers from LogServiceMessages.h
//@{
typedef MessageWrapper<BaseGetMsgLogLevelRequest, efd::kMSGID_GetMsgLogLevelRequest> GetMsgLogLevelRequest;

//@}
typedef efd::SmartPointer<GetMsgLogLevelRequest> GetMsgLogLevelRequestPtr;

/**
    Helper class for setting a module-based log level. We add the moduleId and levelId
    to the base tLogLevelRequest to indicate the module and log level to set.
*/
class EE_EFD_LOGSERVICE_ENTRY BaseSetLogLevelRequest : public LogLevelRequest
{
public:
    BaseSetLogLevelRequest() {}

    virtual ~BaseSetLogLevelRequest() { }

    /**
        @param moduleId The module id in question
        @param levelId The log level to set.
        @param netId Net ID of the log service we want to respond. 0 for all.
        @param callback Callback category for responses.
        @param dest Log destination
    */
    void Set(efd::UInt16 moduleId,
        efd::UInt8 levelId,
        efd::UInt32 netId,
        const efd::Category& callback=efd::kCAT_INVALID,
        const efd::utf8string& dest="")
    {
        LogLevelRequest::Set(netId, callback, dest);
        m_moduleId = moduleId;
        m_levelId = levelId;
    }

    // convert this class into a stream of atomic types.
    virtual void ToStream(efd::DataStream& strm) const
    {
        LogLevelRequest::ToStream(strm);
        strm << m_moduleId;
        strm << m_levelId;
    }

    // populate this class with data from the stream
    virtual bool FromStream(const efd::DataStream& strm)
    {
        LogLevelRequest::FromStream(strm);
        strm >> m_moduleId;
        strm >> m_levelId;
        return true;
    }

    efd::UInt16 m_moduleId;
    efd::UInt8 m_levelId;
};

/// @name MessageWrappers from LogServiceMessages.h
//@{

typedef MessageWrapper<BaseSetLogLevelRequest, efd::kMSGID_SetLogLevelRequest> SetLogLevelRequest;
//@}

typedef efd::SmartPointer<SetLogLevelRequest> SetLogLevelRequestPtr;

/**
    Helper class for setting a module-based log level. We add the moduleId and levelId
    to the base tLogLevelRequest to indicate the module and log level to set.
*/
class EE_EFD_LOGSERVICE_ENTRY BaseStartLogStreamRequest : public LogLevelRequest
{
public:
    BaseStartLogStreamRequest() {}

    virtual ~BaseStartLogStreamRequest() { }

    /**
        @param moduleId The module id in question
        @param levelId The log level to set.
        @param netId Net ID of the log service we want to respond.
        @param streamName A unique name for the new stream.
        @param callback Callback category for responses.
    */
    void Set(efd::UInt16 moduleId,
        efd::UInt8 levelId,
        efd::UInt32 netId,
        const efd::utf8string& streamName,
        const efd::Category& callback=efd::kCAT_INVALID)
    {
        LogLevelRequest::Set(netId, callback, streamName);
        m_moduleId = moduleId;
        m_levelId = levelId;
    }

    // convert this class into a stream of atomic types.
    virtual void ToStream(efd::DataStream& strm) const
    {
        LogLevelRequest::ToStream(strm);
        strm << m_moduleId;
        strm << m_levelId;
    }

    // populate this class with data from the stream
    virtual bool FromStream(const efd::DataStream& strm)
    {
        LogLevelRequest::FromStream(strm);
        strm >> m_moduleId;
        strm >> m_levelId;
        return true;
    }

    efd::UInt16 m_moduleId;
    efd::UInt8 m_levelId;
};
/// @name MessageWrappers from LogServiceMessages.h
//@{
typedef MessageWrapper<BaseStartLogStreamRequest, efd::kMSGID_StartLogStreamRequest> StartLogStreamRequest;
//@}

typedef efd::SmartPointer<StartLogStreamRequest> StartLogStreamRequestPtr;


/**
    Helper class for setting a category / message type based log level. We add the
    category, message type id, and log level to the base tLogLevelRequest to identify
    which category and/or message type to set.
*/
class EE_EFD_LOGSERVICE_ENTRY BaseSetMsgLogLevelRequest : public LogLevelRequest
{
public:
    BaseSetMsgLogLevelRequest() {}

    virtual ~BaseSetMsgLogLevelRequest() { }

    /**
        @param category Category we want to set, or kCAT_INVALID if N/A.
        @param msgClassID Message type id we want to set, or 0 if N/A.
        @param levelId The log level to set.
        @param netId Net ID of the log service we want to respond. 0 for all.
        @param callback Callback category for responses.
        @param dest Log destination
    */
    void Set(const efd::Category& category,
        efd::ClassID msgClassID,
        efd::UInt8 levelId,
        efd::UInt32 netId,
        const efd::Category& callback=efd::kCAT_INVALID,
        const efd::utf8string& dest="")
    {
        LogLevelRequest::Set(netId, callback, dest);
        m_msgClassID = msgClassID;
        m_levelId = levelId;
        m_category = category;
    }

    // convert this class into a stream of atomic types.
    virtual void ToStream(efd::DataStream& strm) const
    {
        LogLevelRequest::ToStream(strm);
        strm << m_category;
        strm << m_msgClassID;
        strm << m_levelId;
    }

    // populate this class with data from the stream
    virtual bool FromStream(const efd::DataStream& strm)
    {
        LogLevelRequest::FromStream(strm);
        strm >> m_category;
        strm >> m_msgClassID;
        strm >> m_levelId;
        return true;
    }

    efd::Category m_category;
    efd::ClassID m_msgClassID;
    efd::UInt8 m_levelId;
};
/// @name MessageWrappers from LogServiceMessages.h
//@{

typedef MessageWrapper<BaseSetMsgLogLevelRequest, efd::kMSGID_SetMsgLogLevelRequest> SetMsgLogLevelRequest;
//@}

typedef efd::SmartPointer<SetMsgLogLevelRequest> SetMsgLogLevelRequestPtr;

/**
    Base class for all responses to log service request messages. This class defines
    the result codes for operations executed against the Log Service. It also contains
    the netId of the respondent.
*/
class EE_EFD_LOGSERVICE_ENTRY LogServiceResponse : public efd::StreamMessage
{
    EE_DECLARE_CLASS1(LogServiceResponse, efd::kMSGID_LogServiceResponse, efd::StreamMessage);
    EE_DECLARE_CONCRETE_REFCOUNT;

public:
    enum Result
    {
        lsr_Unknown = 0,
        lsr_Success = 1,
        lsr_Failure = 2
    };

    LogServiceResponse();

    virtual ~LogServiceResponse();

    /**
        @param netId Net ID of the respondent.
        @param result Result of the operation
    */
    void Set(efd::UInt32 netId, Result result);

    // convert this class into a stream of atomic types.
    virtual void ToStream(efd::DataStream& strm) const;

    // populate this class with data from the stream
    virtual bool FromStream(const efd::DataStream& strm);

    efd::UInt32 m_netid;
    Result m_result;
};
typedef efd::SmartPointer<LogServiceResponse> LogServiceResponsePtr;


/**
    Base class for log service responses that contain a set of names. e.g.
    Get Destinations. This adds a efd::set<efd::utf8string> set of names
    to the LogServiceResponse base class.
*/
class EE_EFD_LOGSERVICE_ENTRY NameSetResponse : public LogServiceResponse
{
public:
    typedef efd::set<efd::utf8string> NameSet;

    NameSetResponse() {}
    virtual ~NameSetResponse() {}

    /**
        @param names Set of names
        @param netId Net ID of the respondent
        @param result Result of the operation.
    */
    void Set(
        const NameSet& names,
        efd::UInt32 netId,
        LogServiceResponse::Result result)
    {
        LogServiceResponse::Set(netId, result);
        m_names = names;
    }


    // convert this class into a stream of atomic types.
    virtual void ToStream(efd::DataStream& strm) const
    {
        LogServiceResponse::ToStream(strm);
        strm << (efd::UInt32)m_names.size();
        NameSet::const_iterator it = m_names.begin();
        for(; it != m_names.end(); ++it)
        {
            strm << (*it);
        }
    }

    // populate this class with data from the stream
    virtual bool FromStream(const efd::DataStream& strm)
    {
        LogServiceResponse::FromStream(strm);
        efd::UInt32 count;
        strm >> count;
        for (efd::UInt32 i=0; i < count; ++i)
        {
            efd::utf8string tmp;
            strm >> tmp;
            m_names.insert(tmp);
        }
        return true;
    }

    NameSet m_names;
};
/// @name MessageWrappers from LogServiceMessages.h
//@{

typedef MessageWrapper<NameSetResponse, efd::kMSGID_GetLogDestinationsResponse> GetLogDestinationsResponse;
//@}

/**
    Base class for log service responses that contain a map of names. e.g.
    Get Destinations. This adds a efd::map<efd::UInt16, efd::utf8string> map of names
    to the LogServiceResponse base class.
*/
class EE_EFD_LOGSERVICE_ENTRY GetModulesResponse : public MessageWrapper<LogServiceResponse, efd::kMSGID_GetModulesResponse>
{
    EE_DECLARE_CLASS1(GetModulesResponse, efd::kMSGID_GetModulesResponse, efd::LogServiceResponse);
    EE_DECLARE_CONCRETE_REFCOUNT;

public:
    typedef efd::map<efd::UInt16, efd::utf8string> NameMap;

    GetModulesResponse() {}
    virtual ~GetModulesResponse() {}

    /**
        @param names Map of UInt32 ids to names
        @param netId Net ID of the respondent
        @param result Result of the operation.
    */
    void Set(
        const NameMap& names,
        efd::UInt32 netId,
        LogServiceResponse::Result result)
    {
        LogServiceResponse::Set(netId, result);
        m_names = names;
    }


    // convert this class into a stream of atomic types.
    virtual void ToStream(efd::DataStream& strm) const
    {
        LogServiceResponse::ToStream(strm);
        strm << (efd::UInt32)m_names.size();
        NameMap::const_iterator it = m_names.begin();
        for(; it != m_names.end(); ++it)
        {
            strm << it->first;
            strm << it->second;
        }
    }

    // populate this class with data from the stream
    virtual bool FromStream(const efd::DataStream& strm)
    {
        LogServiceResponse::FromStream(strm);
        efd::UInt32 count;
        strm >> count;
        for (efd::UInt32 i=0; i < count; ++i)
        {
            efd::UInt16 id;
            strm >> id;

            efd::utf8string tmp;
            strm >> tmp;

            m_names[id] = tmp;
        }
        return true;
    }

    NameMap m_names;
};

/**
    Response to a get log level request. It adds the module id and level id to the
    base LogServiceResponse.
*/
class EE_EFD_LOGSERVICE_ENTRY GetLogLevelResponse : public MessageWrapper<LogServiceResponse, kMSGID_GetLogLevelResponse>
{
    EE_DECLARE_CLASS1(GetLogLevelResponse, efd::kMSGID_GetLogLevelResponse, efd::LogServiceResponse);
    EE_DECLARE_CONCRETE_REFCOUNT;

public:
    GetLogLevelResponse() {}

    virtual ~GetLogLevelResponse() { }

    /**
        @param moduleId The module id in question
        @param levelId The log level to set.
        @param netId Net ID of the log service we want to respond. 0 for all.
        @param result result of the operation
    */
    void Set(
        efd::UInt16 moduleId,
        efd::UInt8 levelId,
        efd::UInt32 netId,
        LogServiceResponse::Result result)
    {
        LogServiceResponse::Set(netId, result);
        m_moduleId = moduleId;
        m_levelId = levelId;
    }

    // convert this class into a stream of atomic types.
    virtual void ToStream(efd::DataStream& strm) const
    {
        LogServiceResponse::ToStream(strm);
        strm << m_moduleId;
        strm << m_levelId;
    }

    // populate this class with data from the stream
    virtual bool FromStream(const efd::DataStream& strm)
    {
        LogServiceResponse::FromStream(strm);
        strm >> m_moduleId;
        strm >> m_levelId;
        return true;
    }
    efd::UInt32 m_moduleId;
    efd::UInt8 m_levelId;
};

/**
    Response to a get msg log level request message. It adds the category,
    message type id, and log level id to the base LogServiceResponse.
*/
class EE_EFD_LOGSERVICE_ENTRY GetMsgLogLevelResponse : public MessageWrapper<LogServiceResponse, kMSGID_GetMsgLogLevelResponse>
{
    EE_DECLARE_CLASS1(GetMsgLogLevelResponse, efd::kMSGID_GetMsgLogLevelResponse, efd::LogServiceResponse);
    EE_DECLARE_CONCRETE_REFCOUNT;

public:
    GetMsgLogLevelResponse() {}

    virtual ~GetMsgLogLevelResponse() { }

    /**
        @param category the category of the message.
        @param msgClassID Message type id.
        @param levelId Log level.
        @param netId Net ID of the log service we want to respond. 0 for all.
        @param result The result of the operation.
    */
    GetMsgLogLevelResponse(const efd::Category& category,
        efd::ClassID msgClassID,
        efd::UInt8 levelId,
        efd::UInt32 netId,
        LogServiceResponse::Result result)
        : m_category(category)
        , m_msgClassID(msgClassID)
        , m_levelId(levelId)
    {
        Set(netId, result);
    }

    // convert this class into a stream of atomic types.
    virtual void ToStream(efd::DataStream& strm) const
    {
        LogServiceResponse::ToStream(strm);
        strm << m_category;
        strm << m_msgClassID;
        strm << m_levelId;
    }

    // populate this class with data from the stream
    virtual bool FromStream(const efd::DataStream& strm)
    {
        LogServiceResponse::FromStream(strm);
        strm >> m_category;
        strm >> m_msgClassID;
        strm >> m_levelId;
        return true;
    }

    efd::Category m_category;
    efd::ClassID m_msgClassID;
    efd::UInt8 m_levelId;
};

/// @name MessageWrappers from LogServiceMessages.h
//@{

typedef MessageWrapper<LogServiceResponse, efd::kMSGID_SetLogLevelResponse> SetLogLevelResponse;
typedef MessageWrapper<LogServiceResponse, efd::kMSGID_SetMsgLogLevelResponse> SetMsgLogLevelResponse;
typedef MessageWrapper<LogServiceResponse, efd::kMSGID_StartLogStreamResponse> StartLogStreamResponse;
typedef MessageWrapper<LogServiceResponse, efd::kMSGID_StopLogStreamResponse> StopLogStreamResponse;
//@}

/**
    A message containing one or more log entries.
*/
class EE_EFD_LOGSERVICE_ENTRY LogEntriesMessage : public efd::StreamMessage
{
    EE_DECLARE_CLASS1(LogEntriesMessage, efd::kMSGID_LogEntriesMessage, efd::StreamMessage);
    EE_DECLARE_CONCRETE_REFCOUNT;

public:
    typedef efd::vector< efd::utf8string > LogEntries;

    LogEntriesMessage();

    ~LogEntriesMessage();

    /**
        Create a new request to the log service.

        @param netID      The NetId of the producer
        @param PID        The producers process ID
        @param entries    A vector of log entries.
    */
    LogEntriesMessage(
        efd::UInt32 netID,
        efd::UInt64 PID,
        const efd::vector< efd::utf8string >& entries);

    // convert this class into a stream of atomic types.
    virtual void ToStream( efd::DataStream& strm ) const;

    // populate this class with data from the stream
    virtual bool FromStream( const efd::DataStream& strm );

    efd::UInt32 m_netID;
    efd::UInt64 m_pid;
    LogEntries m_entries;
};


typedef efd::SmartPointer<LogEntriesMessage> LogEntriesMessagePtr;

/// @name MessageWrappers from LogServiceMessages.h
//@{

typedef efd::MessageWrapper<ServerToClientMessage,efd::kMSGID_ClientLogServiceRequest> 
    ClientLogServiceRequest;
//@}

} // end namespace efd

#endif // LOGSERVICEMESSAGES_H

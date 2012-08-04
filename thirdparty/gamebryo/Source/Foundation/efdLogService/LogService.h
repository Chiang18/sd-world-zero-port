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
#ifndef LOGSERVICE_H
#define LOGSERVICE_H

#include <efdLogService/LogServiceMessages.h>
#include <efd/MessageService.h>
#include <efdLogService/NetDestination.h>
#include <efd/FileDestination.h>

namespace efd
{

/**
   Log Service class - responsible for handling log configuration calls /
   messages, tracking log destinations, and log files.

   This is a light weight control for an instance of the ILogger interface
   that runs as a SystemService. This allows the LogService to register for
   Category / Messages that can change the behavior of the underlying logger.
 */
class EE_EFD_LOGSERVICE_ENTRY LogService : public efd::ISystemService
{
    EE_DECLARE_CLASS1(LogService, efd::kCLASSID_LogService, efd::ISystemService);
    EE_DECLARE_CONCRETE_REFCOUNT;

public:

    /// log destination name to net destination pointer
    typedef efd::map< efd::utf8string, efd::NetDestinationPtr > LogDestinationMap;

    /**
        Construct a new LogManager to manage the given logger.

        @param sendLocal True to send the responses locally.
     */
    LogService();

    // See ISystemService::OnPreInit()
    virtual efd::SyncResult OnPreInit();

    // See ISystemService::OnInit()
    virtual efd::AsyncResult OnInit();

    // See ISystemService::OnTick()
    virtual efd::AsyncResult OnTick();

    // See ISystemService::OnShutdown()
    virtual efd::AsyncResult OnShutdown();

    // See ISystemService::GetDisplayName() const
    const char* GetDisplayName() const
    {
        return "LogService";
    }

    /**
        Handle incoming request to get the log destinations.

        @param pRequest Pointer to the log service request to handle.
        @param targetChannel The channel the message arrived on.
    */
    void HandleGetLogDestinationsRequest(
        const efd::LogServiceRequest* pRequest,
        efd::Category targetChannel);

    /**
        Handle incoming request to get the log modules.

        @param pRequest Pointer to the log service request to handle.
        @param targetChannel The channel the message arrived on.
    */
    void HandleGetModulesRequest(
        const efd::LogServiceRequest *pRequest,
        efd::Category targetChannel);

    /**
        Handle incoming request to get the log level for a module.

        @param pRequest Pointer to the log service request to handle.
        @param targetChannel The channel the message arrived on.
    */
    void HandleGetLogLevelRequest(
        const efd::BaseGetLogLevelRequest *pRequest,
        efd::Category targetChannel);

    /**
        Handle incoming request to get the log level for a message class.

        @param pRequest Pointer to the log service request to handle.
        @param targetChannel The channel the message arrived on.
    */
    void HandleGetMsgLogLevelRequest(
        const efd::BaseGetMsgLogLevelRequest *pRequest,
        efd::Category targetChannel);

    /**
        Handle incoming request to set the log level for a module.
        This messages changes the behavior of the logger.

        @param pRequest Pointer to the log service request to handle.
        @param targetChannel The channel the message arrived on.
    */
    void HandleSetLogLevelRequest(
        const efd::BaseSetLogLevelRequest *pRequest,
        efd::Category targetChannel);

    /**
        Handle incoming request to set the log level for a message.
        This messages changes the behavior of the logger.

        @param pRequest Pointer to the log service request to handle.
        @param targetChannel The channel the message arrived on.
    */
    void HandleSetMsgLogLevelRequest(
        const efd::BaseSetMsgLogLevelRequest *pRequest,
        efd::Category targetChannel);

    /**
        Handle incoming request to start streaming log message to a specific destination.
        This messages changes the behavior of the logger.

        @param pRequest Pointer to the log service request to handle.
        @param targetChannel The channel the message arrived on.
    */
    void HandleStartLogStreamRequest(
        const efd::BaseStartLogStreamRequest *pRequest,
        efd::Category targetChannel);

    /**
        Handle incoming request to stop streaming messages to a destination.
        This messages changes the behavior of the logger.

        @param pRequest Pointer to the log service request to handle.
        @param targetChannel The channel the message arrived on.
    */
    void HandleStopLogStreamRequest(
        const efd::LogLevelRequest *pRequest,
        efd::Category targetChannel);

private:
    /// Helper to ensure the Logger has been configured
    void LoadConfig();

    /// Cached pointer to the message service
    MessageServicePtr m_spMessageService;

    /// map of stream name to net destination instances
    LogDestinationMap m_netDestinations;

    /// netid of this server
    efd::UInt32 m_netid;

    /// cached pointer to the logger
    efd::ILogger* m_pLogger;
};

/*! \typedef LogServicePtr
 * A smart pointer (reference counting, self deleting pointer) for the
 * LogService class
 */
typedef efd::SmartPointer<LogService> LogServicePtr;


} // end namespace efd

#endif // LOGSERVICE_H

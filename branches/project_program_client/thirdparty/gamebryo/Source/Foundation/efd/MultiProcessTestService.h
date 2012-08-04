/******************************************************************************
*
* EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
*
* This software is supplied under the terms of a license agreement or
* nondisclosure agreement with Emergent Game Technologies and may not
* be copied or disclosed except in accordance with the terms of that
* agreement.
*
*      Copyright (c) 1996-2009 Emergent Game Technologies.
*      All Rights Reserved.
*
* Emergent Game Technologies, Calabasas, CA 91302
* http://www.emergent.net
*
\******************************************************************************/

#pragma once
#ifndef MULTIPROCESSTESTSERVICE_H
#define MULTIPROCESSTESTSERVICE_H

#include <efd/IMultiProcessTestService.h>
#include <efd/ITest.h>
#include <efd/UniversalTypes.h>
#include <efd/StdContainers.h>
#include <efd/TimeType.h>
#include <efd/StreamMessage.h>
#include <efd/MessageHandlerBase.h>
#include <efd/IDs.h>

/// @cond EMERGENT_INTERNAL

namespace efd
{


// Forward reference
class ISection;

class EE_EFD_ENTRY MultiProcessTestService : public IMultiProcessTestService
{
    EE_DECLARE_CLASS1(
        MultiProcessTestService, 
        efd::kCLASSID_MultiProcessTestService,
        IMultiProcessTestService);
    EE_DECLARE_CONCRETE_REFCOUNT;

public:

    /// We initialize the exit code to this value to indicate that we have yet to determine
    /// whether the test has succeeded or failed.  If the test times out this is typically
    /// the value that will be returned.
    static const efd::UInt32 kTestPendingExitCode = 0xFDFDFDFD;
    static const efd::UInt32 kTestTimeoutCode = 0xDFFDDFFD;
    static const efd::UInt32 kTestSuccessCode = 0x0;
    static const efd::UInt32 kTestFailCode = 0x1;

    /**
        Constructor.  The value of the runtimeSecs
        parameter will be overridden by the configuration variable TestTime in
        the Testing section of the configuration.  It is the recommended method
        for setting the value.

        @param runtimeSecs The maximum time, in seconds, the process should run.
            By default it is set to 0 which will allow the process to run
            indefinitely.
    */
    MultiProcessTestService(efd::Float32 runtimeSecs = 0.0f);

    /// Destructor
    virtual ~MultiProcessTestService();

    // Overridden virtual functions inherit base documentation and thus
    // are not documented here.
    virtual AsyncResult OnInit();
    virtual AsyncResult OnTick();
    virtual AsyncResult OnShutdown();
    const char* GetDisplayName() const
    {
        return "MPTestService";
    }

    virtual void ReportTestStatus(
        tTestResult result,
        const char* pFile,
        efd::UInt32 line,
        const char* pMessage);

    virtual void Trace(
        const char* pFile,
        efd::UInt32 line,
        const char* pMessage);

    virtual efd::UInt32 GetExitCode(bool justChecking = false)
    {
        EE_UNUSED_ARG(justChecking);
        return m_exitCode;
    }

    virtual efd::UInt32 InTestMode() { return m_shutdownTimeSecs > 0; }

    virtual void TestFinished();

    /// Receives the delayed message to shutdown the application
    void HandleMessage(const efd::StreamMessage* pMessage, efd::Category targetChannel);

protected:

    /**
        Function to output the formatted status and trace mechanisms

        @param pMessage The formatted test to output
        @param messageLvl The log level for the output message
    */
    void WriteOutput(const char* pMessage, int messageLvl = efd::ILogger::kLVL1);

    /// The time the process should be shutdown to prevent deadlock
    efd::TimeType m_shutdownTimeSecs;

    /// The exit code
    efd::UInt32 m_exitCode;

    /// The testing section of the configuration file
    const ISection* m_pTestingSection;

    bool m_bFinished;
};

/// Smart pointer for the IMultiProcessTestService class
typedef efd::SmartPointer<MultiProcessTestService> MultiProcessTestServicePtr;

} // end namespace efd

/// @endcond

#endif // MULTIPROCESSTESTSERVICE_H

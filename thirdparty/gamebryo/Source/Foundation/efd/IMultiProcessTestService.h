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
#ifndef IMULTIPROCESSTESTSERVICE_H
#define IMULTIPROCESSTESTSERVICE_H

#include <efd/ISystemService.h>
#include <efd/efdLibType.h>
#include <efd/ITestBase.h>

namespace efd
{

/// @cond EMERGENT_INTERNAL 

/**
    @class IMultiProcessTestService
    @brief A system service to run multiprocess tests.
    It reads a configuration section "Testing" and looks for the entry
    "TestTime".  It uses the value there (time in seconds) to set a time
    limit for the process.  When that time limit is exceeded, it calls the
    framework to shutdown the process.
*/
class EE_EFD_ENTRY IMultiProcessTestService : public ISystemService
{
public:
    EE_DECLARE_INTERFACE1(
        IMultiProcessTestService, 
        efd::kCLASSID_IMultiProcessTestService, 
        ISystemService);

    /**
        Report the status of a test.  Unlike the unit test service, derived
        tests may call this method directly to report test status.

        @param result The result of the test.
        @param file The filename that reported the status __FILE__
        @param line The line number where the status was reported __LINE__
        @param message A message describing the test and status
    */
    virtual void ReportTestStatus(
        tTestResult result,
        const char* file,
        efd::UInt32 line,
        const char* message) = 0;

    /**
        Writes a message to an output mechanism.

        @param pFile The filename that reported the status __FILE__
        @param line The line number where the status was reported __LINE__
        @param pMessage A message describing the test and status
    */
    virtual void Trace(
        const char* pFile,
        efd::UInt32 line,
        const char* pMessage) = 0;

    /**
        Return an exit code that should be returned from main().

        @param justChecking passing true allows you to query if the test would pass, but not 
            actually cause the test to fail.  Passing false causes the test to fail if immediately
            if the exit code is non-zero.

        @return A code indicating the success/failure test process.
    */
    virtual efd::UInt32 GetExitCode(bool justChecking = false) = 0;

    /**
        Called when the test is finished, but before shutdown is called.
    */
    virtual void TestFinished() = 0;

    // Overridden virtual functions inherit base documentation and thus
    // are not documented here.
    const char* GetDisplayName() const
    {
        return "IMPTestService";
    }

};

/// Smart pointer for the IMultiProcessTestService class
typedef efd::SmartPointer<IMultiProcessTestService> IMultiProcessTestServicePtr;

/// @endcond

} // end namespace efd

#endif // IMULTIPROCESSTESTSERVICE_H

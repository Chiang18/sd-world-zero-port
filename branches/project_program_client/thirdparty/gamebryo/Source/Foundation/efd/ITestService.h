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
#ifndef EE_ITESTSERVICE_H
#define EE_ITESTSERVICE_H

#include <efd/ISystemService.h>
#include <efd/ITestBase.h>
#include <efd/TimeType.h>

/// @cond EMERGENT_INTERNAL

namespace efd
{
/// Forward reference.
class ITest;

/// A system service to run unit tests.
class EE_EFD_ENTRY ITestService : public ISystemService
{
public:
    /// @cond EMERGENT_INTERNAL
    EE_DECLARE_INTERFACE1(ITestService, efd::kCLASSID_ITestService, ISystemService);
    /// @endcond

    /// @cond EMERGENT_INTERNAL

    /** 
        Checks is the result for the test is successful.

        @return True if the result is a success.
    */
    bool IsSuccess(tTestResult result) const {return (result > kResultTimedOut);}

    /**
        Reports the status of a test.
        This method is called via the ASSERT, etc. macros.  Implementors of tests should not call
        this method directly.
        
        @param pTest [in] The test to report status on
        @param result[in]  The result of the last test.
        @param file[in]  File for logging results.
        @param line[in]  Line number.
        @param message[in]  Name of the test.
    */
    virtual void ReportTestStatus(
        tTestResult result,
        const char* file,
        efd::UInt32 line,
        const char* message,
        ITest* pTest = NULL) = 0;

    /// Signals the test service to shutdown the current test class and let the next one run.
    /// @param force if true causes the currently running test to shutdown even if there are
    ///     still outstanding child tests running.
    virtual void ShutdownCurrentTest(bool force = false) = 0;

    /// @endcond

    /**
        Pushes a string of context information onto a stack.

        The current top of stack will be printed whenever ReportTestStatus is called with a result
        of false.

        @param state String of context information representing current state.
    */
    virtual void PushState(const char* state) = 0;

    /**
        Pops a string of context information from a stack.

        The current top of stack will be printed whenever ReportTestStatus is called with a result
        of false.

        @param state String of context information representing current state.
    */
    virtual void PopState(const char* state) = 0;

    /**
        Allows for ITests to spawn other ITests.  Each ITest including the parent test that holds
        all of the child tests puts itself into the concurrent test list.  If one of the tests
        passes ITestService knows to keep running the other tests until they all pass.
    */
    virtual void StartConcurrentTest(ITest* pTest) = 0;

    /**
        Called when a concurrent test completes.  Parent test continues to run until all child 
        tests complete or TS_ASSERT
    */
    virtual void PassConcurrentTest(ITest* pTest) = 0;

    /**
        Writes a message to some output mechanism.

        @param pTest the test that is tracing
        @param pFile Name of the file emitting the trace statement.
        @param line Line number of the file emitting the trace statement.
        @param pMessage Message to write.
    */
    virtual void Trace(
        const char* pFile,
        efd::UInt32 line,
        const char* pMessage,
        ITest* pTest = NULL) = 0;

    /**
        Registers a test with the testing service.
        
        The test service will call the tests SetTestService method, saving a pointer to itself in
        the test.

        @param pTest Pointer to the test being registered.
    */
    virtual void RegisterTest(ITest* pTest) = 0;

    /**
        Resets the query deadlock timer.
        
        This step is useful for complex tests that pass many messages back and forth and that
        expect to take a bit of time to complete.  Calling this method in message handlers that
        receive messages should guarantee enough time is left to receive a subsequent message
        before deadlock timeout happens.
    */
    virtual void ResetDeadlockTimer() = 0;

    /**
        Return an exit code that should be returned from main().

        @return A code indicating the success/failure of all tests that were run.
    */
    virtual efd::UInt32 GetExitCode() = 0;

    /// Unregisters all tests with the testing service.
    virtual void UnregisterTests() = 0;

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    inline virtual const char* GetDisplayName() const;


    /**
        Writes the results from a test to a data file (XML) for later display.

        @param result [in] The result of the current test.
        @param pFile [in] The filename where the result was reported.
        @param line [in] Line number where the result was reported.
        @param pName [in] The name of the test that is reporting the result.
        @param pDescription [in] The description for the test.
        @param pFunctionArea [in] The functional area covered by the test.
        @param startTime [in] The start time of the test.
        @param endTime [in] The end time of the test.
        @param pMessage [in] A message about why the result is the given value and is usually 
            reserved for failures.
    */
    virtual void WriteResultData(        
        tTestResult result,
        const char* pFile,
        efd::UInt32 line,
        const char* pName,
        const char* pDescription,
        const char* pFunctionArea,
        efd::TimeType startTime,
        efd::TimeType endTime,
        const char* pMessage) = 0;

    /// Get the currently running test pointer
    virtual ITest* GetCurrentTest() = 0;

};

typedef efd::SmartPointer<ITestService> ITestServicePtr;

}   // End namespace efd.

#include <efd/ITestService.inl>

/// @endcond

#endif  // EE_ITESTSERVICE_H

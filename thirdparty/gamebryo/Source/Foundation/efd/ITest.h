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
#ifndef EE_ITEST_H
#define EE_ITEST_H

#include <efd/efdLibType.h>
#include <efd/IBase.h>
#include <efd/utf8string.h>
#include <efd/IDs.h>
#include <efd/Helpers.h>
#include <efd/TimeType.h>
#include <efd/ITestBase.h>
#include <efd/efdSystemServiceIDs.h>

/// @cond EMERGENT_INTERNAL

/// Macro to trace.
#define TS_TRACE(m) efd::g_pTestService->Trace(__FILE__, __LINE__, m)

/// Macro to trace with a printf-style formated message. The message and arguments must be
/// enclosed in an extra set of parentheses.
#define TS_TRACE_MESSAGE(m) efd::g_pTestService->Trace(__FILE__, __LINE__, efd::PrintfHelper m )

/**
    Macro to assert, given file and line.

    If !e, then report the failure to the test service and return out of the procedure.
*/
#define _TS_ASSERT(f, l, e)                                         \
    if (!(e))                                                       \
    {                                                               \
        efd::g_pTestService->ReportTestStatus(kResultFailed, f, l, #e);     \
        return;                                                     \
    }

/**
    Macro to assert, with file and line implicit.

    If !e, then report the failure to the test service and return out of the procedure.
*/
#define TS_ASSERT(e)                                                            \
    if (!(e))                                                                   \
    {                                                                           \
        efd::g_pTestService->ReportTestStatus(kResultFailed, __FILE__, __LINE__, #e);   \
        return;                                                                 \
    }

/**
    Macro to assert and return a specific value, with file and line implicit.

    If !e, then report the failure to the test service and return out of the procedure.
*/
#define TS_ASSERT_RET(e, ret)                                                   \
    if (!(e))                                                                   \
    {                                                                           \
        efd::g_pTestService->ReportTestStatus(kResultFailed, __FILE__, __LINE__, #e);   \
        return ret;                                                             \
    }

/**
Macro to assert with a printf-style formated message, with file and line implicit.
The message and arguments must be enclosed in parentheses.

If !e, then report the failure to the test service and return out of the procedure.
*/
#define TS_ASSERT_MESSAGE(e, m)                                                                 \
    if (!(e))                                                                                   \
    {                                                                                           \
        efd::g_pTestService->ReportTestStatus(                                                  \
            kResultFailed,                                                                      \
            __FILE__,                                                                           \
            __LINE__,                                                                           \
            efd::PrintfHelper m);                                                               \
        return;                                                                                 \
    }

/**
Macro to assert with a printf-style formated message, with file and line implicit.
The message and arguments must be enclosed in parentheses.

If !e, then report the failure to the test service and return out of the procedure.
*/
#define TS_ASSERT_MESSAGE_RET(e, m, ret)                                                        \
    if (!(e))                                                                                   \
    {                                                                                           \
        efd::g_pTestService->ReportTestStatus(                                                  \
            kResultFailed,                                                                      \
            __FILE__,                                                                           \
            __LINE__,                                                                           \
            efd::PrintfHelper m);                                                               \
        return ret;                                                                             \
    }

/**
    Macro to report a test failure, with file and line implicit.
*/
#define TS_FAIL(m)                                                              \
    {                                                                           \
        efd::g_pTestService->ReportTestStatus(kResultFailed, __FILE__, __LINE__, m, this);    \
        UpdateTestCaseStatus();                                                 \
        return;                                                                 \
    }

/**
    Macro to report a test failure with a printf-style formated message, with file and line
    implicit.  The message and arguments must be enclosed in parentheses.
*/
#define TS_FAIL_MESSAGE(m)                                                                      \
    {                                                                                           \
        efd::g_pTestService->ReportTestStatus(                                                  \
            kResultFailed,                                                                      \
            __FILE__,                                                                           \
            __LINE__,                                                                           \
            efd::PrintfHelper m,                                                                \
            this);                                                                              \
        return;                                                                                 \
    }

/// Macro to pass a test.
#define TS_PASS_TEST                                                            \
    {                                                                           \
        efd::g_pTestService->PassConcurrentTest(this);                          \
                                                                                \
        efd::g_pTestService->ReportTestStatus(kResultPassed, __FILE__, __LINE__, "",this);    \
        UpdateTestCaseStatus();                                                 \
    }

/// Macro to initialize a test case.  It must be called at least one in each test class but may be
/// called more than once to handle multiple test cases in a test class.  In addition to saving
/// the name and description for the test, it also resets/starts the test timer.
#define TS_TEST_CASE(name, description) \
    SetTest(name, description); \
    efd::g_pTestService->StartConcurrentTest(this)

/// Macro to register a test case.  Test files will be parsed for TS_TEST_CASE macros, these
/// will be turned into TS_REGISTER_TEST calls to generate a vector of test cases which will 
/// then be modified as tests complete so a known list of skipped tests can be generated at the end
#define TS_REGISTER_TEST_CASE(name, description) RegisterTestCase(name, description)

/// Macro to add a statement into the testing results that a test is needed.  The result for the
/// test will be marked as skipped (which is considered a non-error state).
#define TS_NEED_TEST(name, description)                                                 \
{                                                                                       \
    SetTest(name, description);                                                         \
    efd::g_pTestService->PassConcurrentTest(this);                                      \
    efd::g_pTestService->ReportTestStatus(                                              \
        kResultNeeded,                                                                  \
        __FILE__,                                                                       \
        __LINE__,                                                                       \
        "Need To Implement Test");                                                      \
}

/// Macro to add a statement into the testing results that a test has been skipped.  The 
/// result for the test will be marked as skipped (which is considered a non-error state).
#define TS_SKIPPED_TEST(name, description)                                              \
{                                                                                       \
    SetTest(name, description);                                                         \
    efd::g_pTestService->PassConcurrentTest(this);                                      \
    efd::g_pTestService->ReportTestStatus(                                              \
        kResultSkipped,                                                                 \
        __FILE__,                                                                       \
        __LINE__,                                                                       \
        "Test Skipped");                                                                \
}

/// Macro to tell the test service to shutdown the current test class and starting running the 
/// next class if there is one.
#define TS_SHUTDOWN() efd::g_pTestService->ShutdownCurrentTest()

/// Macro to push state.
#define TS_PUSH_STATE(e) efd::g_pTestService->PushState(e)

/// Macro to pop state.
#define TS_POP_STATE(e) efd::g_pTestService->PopState(e)

namespace efd
{

/// Forward reference.
class ITestService;

/**
    Global test service pointer that can be seen anywhere.
    
    The application must set this pointer.
*/
extern EE_EFD_ENTRY ITestService* g_pTestService;

/// Interface for all unit/functional tests.
class EE_EFD_ENTRY ITest : public efd::IBase
{
    /// @cond EMERGENT_INTERNAL

    EE_DECLARE_INTERFACE1(ITest,efd::kCLASSID_ITest,IBase);

    /// @endcond

public:

    /**
        Setup the test before run is called.

        This method may be overridden by derived tests.  The framework is ticked at least once
        before and after this method is called, prior to calling the tests run method.
    */
    virtual void Setup() = 0;

    /**
        Run the test.
        
        This method is implemented by derived tests.  Tests must call either PassTest() or
        FailTest() (or from a message handler invoked, as a result of the Run()).
    */
    virtual void Run() = 0;

    /**
        Receives ticks while the test is running after run completes.

        This method may be overridden by derived tests.
    */
    virtual void Tick() = 0;

    /**
        Tear down the test after run it called and the test completes.
    
        This method may be overridden by derived tests.
    */
    virtual void TearDown() = 0;

    /**
        Sets the test name and clears that flag that a result has been reported.  It also sets the
        description to 'Unknown' because it is called by the extern function emitted form the 
        python script which is not given a description.

        @param testName [in] Name of the test.
    */
    virtual void SetName(const efd::utf8string& testName) = 0;

    /**
        Sets the test name and description and clears that flag that a result has been reported.

        @param testName [in] Name of the test.
        @param descrip [in] The description for the test.
    */
    virtual void SetTest(const efd::utf8string& testName, const efd::utf8string& descrip) = 0;

    /**
        Returns the test name.

        @return Name of the test.
    */
    virtual const efd::utf8string& GetName() const = 0;

    /**
        Pure virtual function that returns a description of the test.  This method must be 
        implemented on each test that is derived from this class so that the description
        can be accessed by this method for inclusion in build status reports.

        @return A string description of the test.
    */
    virtual const efd::utf8string& GetDescription() const = 0;

    /**
        Sets the functional area for the test.

        @param funcArea [in] Functional area of the test.
    */
    virtual void SetFunctionalArea(const efd::utf8string& funcArea) = 0;

    /**
        Returns a the functional area covered by the test.

        @return A string version of the functional area covered by this test.
    */
    virtual const efd::utf8string& GetFunctionalArea() const = 0;

    /** 
        Gets the start time for the current test case.

        @return The start time.
    */
    virtual efd::TimeType GetStartTime() const = 0;

    /** 
        Gets the end time for the current test case.

        @return The end time.
    */
    virtual efd::TimeType GetEndTime() const = 0;

    /**
        Sets the result reported flag.

        @param flag [in] The new value for the flag
    */
    virtual void SetReported(efd::Bool flag) = 0;

    /**
        Retrieves the result reported flag.

        @return True if the result has been reported.
    */
    virtual bool IsReported() const = 0;

    /**
        Sets a pointer to the test service that is running this test.
        
        This step happens during registration with the test service.

        @param testService Pointer to the ITestService running the test.
    */
    virtual void SetTestService(ITestService* testService) = 0;

    /**
        Registers the test case name and description and clears that flag that a result has 
        been reported.
        Used in tracking which tests have been skipped due to other tests not completing.

        @param testName [in] Name of the test.
        @param descrip [in] The description for the test.
    */
    virtual void RegisterTestCase(
        const efd::utf8string& testName, 
        const efd::utf8string& descrip) = 0;

    /**
    *   Used to manage the vector of test cases, current test will be removed from the vector
    */
    virtual void UpdateTestCaseStatus() = 0;

    /**
    *   Output information about any skipped tests via the TestService log.
    */
    virtual void ReportSkippedTestCases() = 0;


};

/// Declare a smart pointer for ITest.
EE_DECLARE_SMART_POINTER(ITest);

}   // End namespace efd.

/// @endcond

#endif  // EE_ITEST_H

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
#ifndef TESTSERVICE_H
#define TESTSERVICE_H

#include <efd/ITestService.h>
#include <efd/ITest.h>
#include <efd/UniversalTypes.h>
#include <efd/StdContainers.h>
#include <efd/efdLibType.h>
#include <efd/File.h>

/// @cond EMERGENT_INTERNAL

namespace efd
{

class EE_EFD_ENTRY TestService : public ITestService
{
    EE_DECLARE_CLASS1(TestService, efd::kCLASSID_TestService, ITestService);
    EE_DECLARE_CONCRETE_REFCOUNT;

public:

    TestService(efd::UInt32 ticksBeforeDeadlock = 0x0010000);
    virtual ~TestService();

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual AsyncResult OnInit();

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual AsyncResult OnTick();

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual AsyncResult OnShutdown();

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    const char* GetDisplayName() const
    {
        return "TestService";
    }

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual void ReportTestStatus(
        tTestResult result,
        const char* file,
        efd::UInt32 line,
        const char* message,
        ITest* pTest = NULL);

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual void ShutdownCurrentTest(bool force = false);

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual void PushState( const char* state );

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual void PopState( const char* state );

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual void StartConcurrentTest(ITest* pTest);

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual void PassConcurrentTest(ITest* pTest);

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual void Trace(
        const char* file,
        efd::UInt32 line,
        const char* message,
        ITest* pTest = NULL);

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual void RegisterTest(ITest* pTest);
    virtual void UnregisterTests();

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual void ResetDeadlockTimer()
        { m_deadlockCounter = 0; }

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual efd::UInt32 GetExitCode() { return m_exitCode; }

    void WriteOutput(const char* message, int messageLvl);

    /**
        Creates the results file and initializes it with a header.

        @param solution [in] The name of this solution.
        @param config [in] The name of the solution configuration currently running.
    */
    bool CreateDataFile(
        efd::utf8string& solution, 
        efd::utf8string& config);

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
    void WriteResultData(        
        tTestResult result,
        const char* pFile,
        efd::UInt32 line,
        const char* pName,
        const char* pDescription,
        const char* pFunctionArea,
        efd::TimeType startTime,
        efd::TimeType endTime,
        const char* pMessage);

    /// Closes the data structure and data file.
    void CloseDataFile();

    virtual ITest* GetCurrentTest(){ return m_spRunningTest; }

protected:
    /// Name of the test project
    efd::utf8string m_project;

    /// Name of the script engine that was used while running this test
    efd::utf8string m_scriptEngine;

    /// ID of this process - Used for multi-process tests
    efd::utf8string m_procID;

    /// Queue of tests to run
    efd::list<ITestPtr> m_tests;

    /// list of test smart pointers to keep tests alive until the end of the process
    efd::list<ITestPtr> m_testsKeepAlive;

    /// Tick counter to prevent deadlock
    efd::UInt32 m_deadlockCounter;

    /// The currently running test
    ITestPtr m_spRunningTest;

    /// The number of ticks before deadlock is considered
    efd::UInt32 m_ticksBeforeDeadlock;

    /// The exit code
    efd::UInt32 m_exitCode;

    /// The state context stack
    efd::list< efd::utf8string > m_contextStack;

    /// The concurrent tests we are waiting for to complete
    efd::list< efd::ITest* > m_concurrentTests;

    /// Pointer to the results data file.
    efd::File* m_pResultsFile;

    /// True when we are running a test inside of OnTick.  Used to prevent recursion.
    bool m_inTick;

    /// If the currently running test fails while it's inside of OnTick this gets set so that
    /// OnTick knows to fail the current test as soon as it returns.
    bool m_currentTestFailed;
};

typedef efd::SmartPointer<TestService> TestServicePtr;

class TSSmartState
{
public:
    TSSmartState( const char* string ):
      m_string(string)
      {
          TS_PUSH_STATE( string );
      }
      ~TSSmartState()
      {
          TS_POP_STATE( m_string.c_str() );
      }
private:
    TSSmartState(){}
    efd::utf8string m_string;
};

} // end namespace efd

/// @endcond

#endif // TESTSERVICE_H

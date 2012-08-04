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

#include "efdPCH.h"

#include <efd/TestService.h>
#include <efd/ServiceManager.h>
#include <efd/ILogger.h>
#include <efd/MessageService.h>
#include <efd/ConfigManager.h>
#include <efd/TestBase.inl>
#include <efd/efdLogIDs.h>

//-------------------------------------------------------------------------------------------------
using namespace efd;


/*
    The parameters for the test can easily be passed to the executable on the command-line
    using the configuration manager.  Simply add the following for Visual Studio projects:
        "TestService.Solution=$(SolutionName)" "TestService.Config=$(ConfigurationName)" 
        "TestService.Project=$(ProjectName)" "TestService.Script=<Name>" "TestService.ProcID=<ID>"
*/

/// Initial value of the exit code - Indicates no value set yet
static const UInt32 kInitialValue = 0xFDFDFDFD;


EE_IMPLEMENT_CONCRETE_CLASS_INFO(TestService);

//-------------------------------------------------------------------------------------------------
TestService::TestService(
    efd::UInt32 ticksBeforeDeadlock /*= 0x0010000*/)
    : m_project("")
    , m_scriptEngine("")
    , m_procID("")
//  , m_tests
//  , m_testsKeepAlive
    , m_deadlockCounter(0)
    , m_spRunningTest(NULL)
    , m_ticksBeforeDeadlock(0)
    , m_exitCode(kInitialValue)
//  , m_contextStack
    , m_pResultsFile(NULL)
    , m_inTick(false)
    , m_currentTestFailed(false)
{
    m_tests.clear();
    m_testsKeepAlive.clear();
    m_contextStack.clear();

    if (ticksBeforeDeadlock <= 0)
    {
        // Don't allow 0 or less deadlock ticks, that would be bad
        EE_LOG(
            efd::kTesting,
            efd::ILogger::kERR2,
            ("Invalid deadlock tick count: '%d' detected when creating "
             "test service.  Using default of 64k instead.",
             ticksBeforeDeadlock));

        m_ticksBeforeDeadlock = 0x0010000;
    }
    else
    {
        m_ticksBeforeDeadlock = ticksBeforeDeadlock;
    }

    ResetDeadlockTimer();
}


//-------------------------------------------------------------------------------------------------
TestService::~TestService()
{
}


//-------------------------------------------------------------------------------------------------
/* virtual */
efd::AsyncResult TestService::OnInit()
{
    efd::utf8string solution;
    efd::utf8string config;

    // Find the configuration manager
    IConfigManager* pConfigManager = m_pServiceManager->GetSystemServiceAs<IConfigManager>();
    if (pConfigManager)
    {
        // Find the test service section
        const ISection *pTestSection = pConfigManager->GetConfiguration()->FindSection(kConfigHeader);
        if ( pTestSection )
        {
            // Read each of the values we need for the results file
            solution = pTestSection->FindValue(kConfigSolution);
            config = pTestSection->FindValue(kConfigConfiguration);
            m_project = pTestSection->FindValue(kConfigProject);
            m_scriptEngine = pTestSection->FindValue(kConfigScript);
            m_procID = pTestSection->FindValue(kConfigProcID);
            efd::utf8string temp = pTestSection->FindValue(kConfigExtraName);
            if (temp != efd::utf8string::NullString())
                m_project += "+" + temp;
        }
    }

    // Open the default results file
    CreateDataFile(solution, config);

    return efd::AsyncResult_Complete;
}

//-------------------------------------------------------------------------------------------------
/* virtual */
efd::AsyncResult TestService::OnTick()
{
    if (m_spRunningTest != NULL)
    {
        // a test is currently running that was started during a prior tick
        // if the deadlock counter exceeds the set threshold, then abort
        m_deadlockCounter++;

        // Check to see if the test has exceeded the number of ticks allowed.
        if (m_deadlockCounter <= m_ticksBeforeDeadlock)
        {
            m_inTick = true;

            if (m_deadlockCounter != 1)
            {
                // Test is up and running so tick the test
                m_spRunningTest->Tick();
            }
            else
            {
                // This test just started so "run" it
                m_spRunningTest->Run();
            }

            m_inTick = false;
            if (m_currentTestFailed)
            {
                // A test failed so shutdown
                ShutdownCurrentTest(true);
            }
        }
        else
        {
            // abort test - it has exceed the maximum # of ticks allowed.

            // First allow it to tear itself down
            m_spRunningTest->TearDown();
            utf8string message(
                Formatted, 
                "Deadlock timer tripped, test service aborting test. "
                "m_deadlockCounter=%d m_ticksBeforeDeadlock=%d",
                m_deadlockCounter,
                m_ticksBeforeDeadlock);
            ReportTestStatus(
                kResultTimedOut,
                __FILE__,
                __LINE__,
                message.c_str());

            m_spRunningTest = NULL;
        }
    }
    else
    {
        // remove the first test from the list and prepare to run it
        if ( m_tests.pop_front(m_spRunningTest) )
        {
            ResetDeadlockTimer();
            WriteOutput(".", efd::ILogger::kLVL1);
            m_spRunningTest->Setup();
        }
        else
        {
            // If there are no more tests, shutdown the framework
            m_pServiceManager->Shutdown();
        }
    }
    return efd::AsyncResult_Pending;
}


//-------------------------------------------------------------------------------------------------
/* virtual */
efd::AsyncResult TestService::OnShutdown()
{
    m_tests.clear();
    //m_testsKeepAlive.clear();
    UnregisterTests();

    // Close the data file
    CloseDataFile();

    return efd::AsyncResult_Complete;
}


//-------------------------------------------------------------------------------------------------
/* virtual */
void TestService::ReportTestStatus(
    tTestResult result,
    const char* file,
    efd::UInt32 line,
    const char* message,
    ITest* pTest)
{
    if (!pTest)
        pTest = m_spRunningTest;
    if (pTest)
    {
#ifndef ENABLE_TRACE
        if (!IsSuccess(result))
        {
#endif
            efd::utf8string contextStr = "";
            if( !m_contextStack.empty() )
                contextStr = m_contextStack.front();

            // format an output message and write it out
            efd::utf8string output( efd::Formatted,
                                    "%s(%d): %s:%s %s (%s)",
                                    file,
                                    line,
                                    pTest->GetName().c_str(),
                                    contextStr.c_str(),
                                    GetResultString(result),
                                    message);
            WriteOutput(output.c_str(), efd::ILogger::kERR1);

#ifndef ENABLE_TRACE
        }
#endif

        // Write the results to the results data file
        WriteResultData(
            result,
            file, 
            line, 
            pTest->GetName().c_str(),
            pTest->GetDescription().c_str(),
            pTest->GetFunctionalArea().c_str(),
            pTest->GetStartTime(),
            pTest->GetEndTime(),
            message);

        if (!IsSuccess(result))
        {
            // If we fail while inside OnTick lets unwind the stack before we call shutdown.
            // Otherwise we are deleting the very test that is still running.
            if (m_inTick && pTest == m_spRunningTest)
            {
                m_currentTestFailed = true;
            }
            else
            {
                // A test failed so shutdown
                ShutdownCurrentTest(true);
            }
        }
    }

    if ( m_exitCode == kInitialValue)
    {
        m_exitCode = 0;
    }

    // if the test failed, decrement the result code by 1 to indicate there was
    // a failure.  this will be queried at the end of all tests to determine
    // how many failures there were
    if (!IsSuccess(result))
    {
        m_exitCode--;
    }
}

//-------------------------------------------------------------------------------------------------
/* virtual */
void TestService::ShutdownCurrentTest(bool force)
{
    if (m_concurrentTests.empty() || force)
    {
        if (m_spRunningTest)
        {
            ITestPtr spRunningTest = m_spRunningTest;
            m_spRunningTest = NULL;

            spRunningTest->ReportSkippedTestCases();
            // Allow the test to tear itself down
            spRunningTest->TearDown();
        }

        // clear the running test so the next tick will invoke a new test
        m_concurrentTests.clear();
    }
}

//-------------------------------------------------------------------------------------------------
/* virtual */
void TestService::PushState( const char* state )
{
    m_contextStack.push_back( state );
}

//-------------------------------------------------------------------------------------------------
/* virtual */
void TestService::PopState( const char* state )
{
    efd::utf8string curHead = m_contextStack.front();
    if( curHead != state )
    {
        efd::utf8string output( efd::Formatted,
            "Popping stack with bogus value %s expected %s",
            state,
            curHead.c_str());
        TS_TRACE( output.c_str() );
        TS_ASSERT( curHead == state );
    }
    m_contextStack.pop_front();
}


//-------------------------------------------------------------------------------------------------
void TestService::StartConcurrentTest(ITest* pTest)
{
    m_concurrentTests.push_back(pTest);
}


//-------------------------------------------------------------------------------------------------
void TestService::PassConcurrentTest(ITest* pTest)
{
    for (efd::list< efd::ITest* >::iterator it = m_concurrentTests.begin();
        it != m_concurrentTests.end();
        )
    {
        if (*it == pTest)
        {
            // remove test
            efd::list< efd::ITest* >::iterator eraseIt = it++;
            m_concurrentTests.erase(eraseIt);
        }
        else
        {
            ++it;
        }
    }
}

//-------------------------------------------------------------------------------------------------
/* virtual */
void TestService::Trace(
    const char* file,
    efd::UInt32 line,
    const char* message,
    ITest* pTest)
{
    if (!pTest)
        pTest = m_spRunningTest;
    // format an output message and write it out
    efd::utf8string output( efd::Formatted,
                            "%s(%d): %s: Trace (%s)",
                            file,
                            line,
                            pTest->GetName().c_str(),
                            message);
    WriteOutput( output.c_str(), efd::ILogger::kLVL1 );

}
//-------------------------------------------------------------------------------------------------
/* virtual */
void TestService::RegisterTest(ITest* pTest)
{
    // add to the end of the test list
    m_tests.push_back(pTest);
    // keep a pointer to the test so that it doesn't get deleted until we're done
    m_testsKeepAlive.push_back(pTest);
    pTest->SetTestService(this);
}

void TestService::UnregisterTests()
{
    for( efd::list<ITestPtr>::iterator i = m_testsKeepAlive.begin();
        i != m_testsKeepAlive.end();
        ++i )
    {
        (*i)->SetTestService( NULL );
        (*i) = NULL;
    }
    m_testsKeepAlive.clear();
}

//-------------------------------------------------------------------------------------------------
void TestService::WriteOutput( const char* message, int messageLvl )
{
    printf("%s\n", message);
    EE_LOG(efd::kTesting, static_cast<efd::UInt8>(messageLvl), ("%s", message));
    EE_UNUSED_ARG( messageLvl );
}

//-------------------------------------------------------------------------------------------------
bool TestService::CreateDataFile(
    efd::utf8string& solution, 
    efd::utf8string& config)
{
    // Use the base function to actually create and initialize the file
    m_pResultsFile = BaseCreateDataFile(solution, config, m_project, m_scriptEngine, m_procID);

    // Check for an open file
    if ( !m_pResultsFile )
    {
        EE_LOG(
            efd::kTesting, 
            efd::ILogger::kERR2,
            ("Error: Could not open results data file"));
        return false;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
void TestService::WriteResultData(        
    tTestResult result,
    const char* pFile,
    efd::UInt32 line,
    const char* pName,
    const char* pDescription,
    const char* pFunctionArea,
    efd::TimeType startTime,
    efd::TimeType endTime,
    const char* pMessage)
{
    // Make sure the results file is still open
    if (!m_pResultsFile)
        return;

    EE_ASSERT(pName);
    if (!pName || (strlen(pName) == 0))
        pName = "Unknown";

    // If a result has already been reported then don't report this result.
    if (m_spRunningTest && m_spRunningTest->IsReported())
    {
        EE_LOG(
            efd::kTesting, 
            efd::ILogger::kLVL3,
            ("%s(%d): Is trying to report another result (%s) - Ignored.",
            pFile,
            line,
            GetResultString(result)));
        return;
    }

    // Use the base function to actually write the result
    BaseWriteResultData(
        m_pResultsFile,
        result,
        pFile,
        line,
        pName,
        pDescription,
        pFunctionArea,
        m_project,
        m_scriptEngine,
        m_procID,
        startTime,
        endTime,
        pMessage);

    // Mark the test as reported so that it is not reported twice
    if (m_spRunningTest)
    {
        m_spRunningTest->SetReported(true);
    }
}

//-------------------------------------------------------------------------------------------------
void TestService::CloseDataFile()
{
    // Use the base function to close the results file
    BaseCloseDataFile(m_pResultsFile);
    m_pResultsFile = NULL;
}

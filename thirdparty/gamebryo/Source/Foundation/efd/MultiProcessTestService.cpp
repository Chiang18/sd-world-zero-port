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

#include <efd/MultiProcessTestService.h>
#include <efd/ConfigManager.h>
#include <efd/ServiceManager.h>
#include <efd/MessageService.h>
#include <efd/IDs.h>
#include <efd/ILogger.h>
#include <efd/StreamMessage.h>
#include <efd/efdLogIDs.h>

//-------------------------------------------------------------------------------------------------
using namespace efd;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(MultiProcessTestService);

EE_HANDLER_WRAP(MultiProcessTestService, HandleMessage, StreamMessage, kMSGID_TestShutdown);

//-------------------------------------------------------------------------------------------------
MultiProcessTestService::MultiProcessTestService(efd::Float32 runtimeSecs /*= 0.0f*/)
: m_shutdownTimeSecs(0.0f)
, m_exitCode(kTestPendingExitCode)
, m_pTestingSection(NULL)
, m_bFinished( false )
{
    // If runtime is greater than 0 use it, otherwise assume we are not in
    // test mode
    if (runtimeSecs > 0.0f)
    {
        m_shutdownTimeSecs = runtimeSecs;
    }
}


//-------------------------------------------------------------------------------------------------
MultiProcessTestService::~MultiProcessTestService()
{
    // You must call TestFinished at some point prior to exit:
    EE_ASSERT( m_bFinished );
}


//-------------------------------------------------------------------------------------------------
efd::AsyncResult MultiProcessTestService::OnInit()
{
    // Subscribe to the shutdown message
    MessageService* pMsg = m_pServiceManager->GetSystemServiceAs<MessageService>();
    if (pMsg)
    {
        pMsg->Subscribe(this, kCAT_LocalMessage);
    }
    else
    {
        return efd::AsyncResult_Failure;
    }

    IConfigManager* pConfigManager = m_pServiceManager->GetSystemServiceAs<IConfigManager>();
    if (!pConfigManager)
    {
        return efd::AsyncResult_Failure;
    }

    m_pTestingSection = pConfigManager->GetConfiguration()->FindSection("Testing");
    if (m_pTestingSection)
    {
        efd::utf8string strRole = m_pTestingSection->FindValue("TestTime");
        efd::TimeType testTime = atof(strRole.c_str());

        // Add the current time to the runtime amount set at creation if > 0
        if (testTime > 0)
        {
            efd::TimeType curTime = m_pServiceManager->GetServiceManagerTime();
            m_shutdownTimeSecs = testTime + curTime;
            // format an output message and write it out
            efd::utf8string output( 
                efd::Formatted, 
                "Current time is %.2f, test time %.2f, stopping at %.2f",
                curTime, 
                testTime, 
                m_shutdownTimeSecs );
            WriteOutput(output.c_str());
        }
        else
        {
            efd::utf8string output( 
                efd::Formatted, 
                "Current time is %.2f not ever stopping", 
                efd::GetCurrentTimeInSec() );
            WriteOutput(output.c_str());
        }
    }
    else
    {
        // We are not testing so set the return value to 0 (success)
        m_exitCode = kTestSuccessCode;
    }

    return efd::AsyncResult_Complete;
}


//-------------------------------------------------------------------------------------------------
efd::AsyncResult MultiProcessTestService::OnTick()
{
    if (InTestMode())
    {
        if (m_shutdownTimeSecs < efd::GetCurrentTimeInSec())
        {
            // Reset the shutdown time so we don't keep calling shutdown
            m_shutdownTimeSecs = efd::GetCurrentTimeInSec() + 1.0f;
            efd::utf8string output( 
                efd::Formatted, 
                "MultiProcessTestService::OnTick test time is up, failing" );
            WriteOutput(output.c_str());

            TestFinished();
            m_exitCode = kTestTimeoutCode;
        }

        // If we have the expected test conditions satisfied, then shutdown
        if ( m_bFinished )
        {
            return efd::AsyncResult_Complete;
        }
    }
    else
    {
        // Since the process is not in test mode, return 0 as the return code
        m_exitCode = kTestSuccessCode;
        m_bFinished = true;

        efd::utf8string output( 
            efd::Formatted, 
            "MultiProcessTestService::OnTick not in test mode" );
        WriteOutput(output.c_str());

        // We aren't testing so return complete so we don't get anymore ticks
        return efd::AsyncResult_Complete;
    }

    return efd::AsyncResult_Pending;
}


//-------------------------------------------------------------------------------------------------
efd::AsyncResult MultiProcessTestService::OnShutdown()
{
    m_pTestingSection = NULL;
    // Subscribe to the shutdown message
    MessageService* pMsg = m_pServiceManager->GetSystemServiceAs<MessageService>();
    if (pMsg)
    {
        pMsg->Unsubscribe(this, kCAT_LocalMessage);
    }

    efd::TimeType curTime = m_pServiceManager->GetServiceManagerTime();
    efd::utf8string output( 
        efd::Formatted, 
        "MultiProcessTestService::OnShutdown called, Current time is %.2f, shutdown Time %.2f", 
        curTime, 
        m_shutdownTimeSecs );
    WriteOutput(output.c_str());

    // Try to ensure that TestFinished is always called:
    if ( !m_bFinished )
    {
        TestFinished();
    }

    return efd::AsyncResult_Complete;
}


//-------------------------------------------------------------------------------------------------
void MultiProcessTestService::ReportTestStatus(
    tTestResult result,
    const char* pFile,
    efd::UInt32 line,
    const char* pMessage)
{
    // format an output message and write it out
    efd::utf8string output( efd::Formatted,
                            "%s(%d): %s (%s)",
                            pFile,
                            line,
                            (result ? "Passed" : "Failed"),
                            pMessage);
    WriteOutput( output.c_str(), result ? efd::ILogger::kLVL1 : efd::ILogger::kERR1 );

    // if the test failed, set the result code to -1 to indicate there was
    // a failure.  this will be queried at the end of all tests to determine
    // if there was an error or not
    if (result)
    {
        // In case ReportTestStatus is called multiple times do not let a "success" result
        // override a previous failure result, only accept it if we are in the initial pending
        // state to start with.
        if ( kTestPendingExitCode == m_exitCode )
        {
            m_exitCode = kTestSuccessCode;
        }
    }
    else
    {
        // We keep a count of the number of tests that failed:
        if ( kTestPendingExitCode == m_exitCode )
        {
            m_exitCode = kTestFailCode;
        }
        else
        {
            ++m_exitCode;
        }
    }
}


//-------------------------------------------------------------------------------------------------
void MultiProcessTestService::Trace(const char* pFile,
                        efd::UInt32 line,
                        const char* pMessage)
{
    // format an output message and write it out
    efd::utf8string output( efd::Formatted,
                            "%s(%d): Trace (%s)",
                            pFile,
                            line,
                            pMessage);
    WriteOutput( output.c_str(), efd::ILogger::kLVL1 );
}


//-------------------------------------------------------------------------------------------------
void MultiProcessTestService::WriteOutput(const char* pMessage, int messageLvl)
{
    printf("%s:%s\n", m_pServiceManager->GetVirtualProcessName().c_str(), pMessage);
    EE_LOG( efd::kTesting, static_cast<efd::UInt8>(messageLvl), ("%s", pMessage));
    EE_UNUSED_ARG( messageLvl );
}


//-------------------------------------------------------------------------------------------------
void MultiProcessTestService::TestFinished()
{
    // Do nothing.  This is really to allow applications to logout and such
    // before calling shutdown
    m_bFinished = true;

    // Don't shutdown right away, instead send a shutdown message
    MessageService* pMsgSvc = m_pServiceManager->GetSystemServiceAs<MessageService>();
    EE_ASSERT( pMsgSvc );
    if ( pMsgSvc )
    {
        StreamMessagePtr spMsg = EE_NEW MessageWrapper< StreamMessage, kMSGID_TestShutdown >;
        EE_ASSERT( spMsg );
        // SendLocal the message with a delay to allow any queued
        // messages to go out before shutting down
        pMsgSvc->SendLocalDelayed(spMsg, 0.5f, kCAT_LocalMessage);
    }
}


//-------------------------------------------------------------------------------------------------
void MultiProcessTestService::HandleMessage(
    const efd::StreamMessage* pMessage, 
    Category targetChannel)
{
    // runtime has been exceeded so shutdown
    m_pServiceManager->Shutdown();

    EE_UNUSED_ARG(pMessage);
    EE_UNUSED_ARG(targetChannel);
}


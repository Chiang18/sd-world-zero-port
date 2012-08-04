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

#include <efd/Test.h>
#include <efd/ServiceManager.h>
//-------------------------------------------------------------------------------------------------
using namespace efd;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(efd::Test);

//-------------------------------------------------------------------------------------------------
ITestService* efd::g_pTestService = NULL;
//-------------------------------------------------------------------------------------------------
Test::Test( ServiceManager* pServiceManager )
    : m_resultReported(false)
    , m_spTestService(NULL)
    , m_testName("Unknown")
    , m_description("No Description")
    , m_funcArea("Unknown Functional Area")
    , m_startTime(0.0)
    , m_pServiceManager(pServiceManager)
{
    EE_ASSERT(pServiceManager);
}

//-------------------------------------------------------------------------------------------------
/* virtual */
Test::~Test()
{
    m_testcaseMap.clear();
}

//-------------------------------------------------------------------------------------------------
/* virtual */
void Test::SetName(const efd::utf8string& testName) 
{
    m_testName = testName; 
    m_description = "Unknown"; 
    m_startTime = GetCurrentTimeInSec(); 
}

//-------------------------------------------------------------------------------------------------
/* virtual */
void Test::SetTest(const efd::utf8string& testName, const efd::utf8string& descrip) 
{
    m_testName = testName; 
    m_description = descrip; 
    m_resultReported = false; 
    m_startTime = GetCurrentTimeInSec();
}

//-------------------------------------------------------------------------------------------------
/* virtual */
efd::TimeType Test::GetStartTime() const
{
    return m_startTime;
}
//-------------------------------------------------------------------------------------------------
/* virtual */
efd::TimeType Test::GetEndTime() const
{
    return GetCurrentTimeInSec();
}
//-------------------------------------------------------------------------------------------------
/* virtual */
void Test::RegisterTestCase(const efd::utf8string& testName, const efd::utf8string& descrip) 
{
    //store testName in map of pending tests
    m_testcaseMap[testName] = descrip;
}

//-------------------------------------------------------------------------------------------------
/* virtual */
void Test::UpdateTestCaseStatus() 
{
    efd::map<efd::utf8string, efd::utf8string>::iterator pIterToRemove = 
        m_testcaseMap.find(m_testName);

    if(pIterToRemove != m_testcaseMap.end())
    {
        m_testcaseMap.erase(pIterToRemove);
    }
    
}

//-------------------------------------------------------------------------------------------------
void Test::ReportSkippedTestCases()
{
    efd::map<efd::utf8string, efd::utf8string>::iterator pIter;

    for(pIter = m_testcaseMap.begin(); pIter != m_testcaseMap.end(); pIter++)
    {
        efd::utf8string output( efd::Formatted,
                                "%s: test case %s skipped",
                                m_testName.c_str(),
                                pIter->first.c_str());
        //m_spTestService->Trace(__FILE__, __LINE__, output.c_str());


        m_spTestService->WriteResultData(
                          kResultSkipped,
                          __FILE__,
                          __LINE__,
                          pIter->first.c_str(),
                          pIter->second.c_str(),
                          m_funcArea.c_str(),
                          0.0,
                          0.0,
                          output.c_str());
    }

}

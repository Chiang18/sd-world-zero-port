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
#ifndef TEST_H
#define TEST_H

#include <efd/ITest.h>
#include <efd/ITestService.h>

/// @cond EMERGENT_INTERNAL

namespace efd
{
    class ServiceManager;

    class EE_EFD_ENTRY Test : public ITest
    {
        EE_DECLARE_CONCRETE_REFCOUNT;
        EE_DECLARE_CLASS1(Test,efd::kCLASSID_Test,ITest);
    public:
        Test( ServiceManager* pServiceManager );
        virtual ~Test();

        // Overridden virtual functions inherit base documentation and thus
        // are not documented here.
        virtual void Setup() {}

        virtual void Tick() {}

        virtual void TearDown() {}

        virtual void SetName(const efd::utf8string& testName);

        virtual void SetTest(const efd::utf8string& testName, const efd::utf8string& descrip);

        virtual const efd::utf8string& GetName() const {return m_testName;}

        virtual const efd::utf8string& GetDescription() const {return m_description;}

        virtual void SetFunctionalArea(const efd::utf8string& funcArea) {m_funcArea = funcArea;}

        virtual const efd::utf8string& GetFunctionalArea() const {return m_funcArea;}

        virtual efd::TimeType GetStartTime() const;

        virtual efd::TimeType GetEndTime() const;

        virtual void SetReported(efd::Bool flag) {m_resultReported = flag;} 

        virtual bool IsReported() const {return m_resultReported;}

        virtual void SetTestService(ITestService* pTestService) {m_spTestService = pTestService;}

        virtual void RegisterTestCase(
            const efd::utf8string& testName, 
            const efd::utf8string& descrip);

        virtual void UpdateTestCaseStatus();
        
        virtual void ReportSkippedTestCases();

    protected:
        bool m_resultReported;
        ITestServicePtr m_spTestService;
        efd::utf8string m_testName;
        efd::utf8string m_description;
        efd::utf8string m_funcArea;
        efd::TimeType m_startTime;
        ServiceManager* m_pServiceManager;
        efd::map<efd::utf8string, efd::utf8string> m_testcaseMap;
    };

} // end namespace efd

/// @endcond

#endif // TEST_H

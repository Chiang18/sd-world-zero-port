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

#include <efd/ServiceManager.h>
#include <efd/IConfigManager.h>
#include <efd/Metrics.h>
#include <efd/ILogger.h>
#include <efd/BitUtils.h>
#include <efd/StdContainers.h>
#include <efd/ILogger.h>
#include <efd/MessageService.h>
#include <efd/efdLogIDs.h>


using namespace efd;

//-------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(ServiceManager);


//-------------------------------------------------------------------------------------------------
const /*static*/ char* ServiceManager::kConfigFramerate = "ServiceManager";
const /*static*/ char* ServiceManager::kSleepKey = "Sleep";
const /*static*/ char* ServiceManager::kDeactivatedSleepKey = "DeactivatedSleep";
/*static*/ ServiceManager* ServiceManager::sm_pServiceManager = NULL;

// The root name for the metrics counter used for collecting service timing.  The display name
// of the service is added to this to get the complete name.
const char kServiceTickRoot[] = "FRAMEWORK.SERVICES.TICK_TIME.";

// no need to catch ctrl-C on PS3
#if !defined(EE_PLATFORM_PS3)
#include <signal.h>
#endif

//-------------------------------------------------------------------------------------------------
void ServiceManager::HandleSignal(efd::SInt32 sig)
{
    EE_LOG(efd::kServiceManager, efd::ILogger::kLVL0,
        ("Caught Signal: %d, shutting down framework",
        sig));
    EE_UNUSED_ARG(sig);

    // flush any outstanding log messages.
    efd::GetLogger()->Flush();

    sm_pServiceManager->Shutdown();
}

//-------------------------------------------------------------------------------------------------
// This should be called after any Scripting Runtimes have
void ServiceManager::RegisterSignalHandlers()
{
    // no need to catch ctrl-C on PS3
#if !defined(EE_PLATFORM_PS3)
    signal(SIGINT, ServiceManager::HandleSignal);
#if defined(EE_PLATFORM_WIN32) || defined(EE_PLATFORM_XBOX360)
    signal(SIGBREAK, ServiceManager::HandleSignal);
#elif defined(EE_PLATFORM_LINUX)
    signal(SIGQUIT, ServiceManager::HandleSignal);
#endif //defined(EE_PLATFORM_WIN32) || defined(EE_PLATFORM_XBOX360)
#endif //!defined(EE_PLATFORM_PS3)
}


//-------------------------------------------------------------------------------------------------
void ServiceManager::ConfigureFramerate()
{
    // Try to find the configuration manager
    IConfigManagerPtr spConfigManager = GetSystemServiceAs<IConfigManager>();

    if (spConfigManager)
    {
        // Find the logging section in the configuration file
        const ISection *pFramerateSection =
            spConfigManager->GetConfiguration()->FindSection(kConfigFramerate);

        if (pFramerateSection)
        {
            // Read the sleep times if they exist
            efd::utf8string val;
            if (pFramerateSection->FindValue(kSleepKey, val))
            {
                if (!val.empty())
                    m_sleepTime = atoi(val.c_str());
            }
            if (pFramerateSection->FindValue(kDeactivatedSleepKey, val))
            {
                if (!val.empty())
                    m_deactivatedSleepTime = atoi(val.c_str());
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
ServiceManager::ServiceManager()
: m_state(kSysServState_PreInit)
, m_bShutdownRequested(false)
, m_active(false)
, m_programTypes(kProgType_Invalid)
, m_tickTimeMetricTail(NULL)
, m_nextTimingDump(0)
, m_sleepTime(0) // rapid iteration on Wii requires a sleep call
, m_deactivatedSleepTime(100)
, m_useDeactivatedSleepTime(false)
, m_kTimingDumpInterval(20)
, m_kMaxAcceptableTickTime(100) // Dump a log if a frame takes > 100 ms
, m_bLogPeriodicServiceTiming(true)
, m_inRunOnce(false)
, m_virtualProcessID(kNetID_Unassigned)
{
    EE_LOG(efd::kServiceManager, efd::ILogger::kLVL1,
        ("Creating a ServiceManager instance 0x%08X", this));

    // Create the metric base name
    strcpy( m_tickTimeMetric, kServiceTickRoot);
    // Point the tail at the NULL terminate of the base so we can easily append
    // the SSID of the service later (over and over again).
    m_tickTimeMetricTail = m_tickTimeMetric + strlen(m_tickTimeMetric);

    // By default, all timing logs are active:
    SetServiceTimeStatTypes( SERVICETIME_LOG | SERVICETIME_ACCUMULATE );

    m_spStartTickMsg = EE_NEW efd::MessageWrapper<efd::IMessage, kMSGID_StartTick >;

    sm_pServiceManager = this;
}


//-------------------------------------------------------------------------------------------------
/*virtual*/ ServiceManager::~ServiceManager()
{
    EE_LOG(efd::kServiceManager, efd::ILogger::kLVL1,
        ("Destroying a ServiceManager instance 0x%08X", this));

    m_spStartTickMsg = NULL;

    sm_pServiceManager = NULL;

    m_regSysSerList.clear();
    m_sysSerList.clear();

    // empty out the clocks list.
    for ( ClockList::iterator iter = m_clocks.begin();
        iter != m_clocks.end();
        ++iter )
    {
        IClock* pClock = iter->second;
        EE_DELETE pClock;
    }
    m_clocks.clear();
}


//-------------------------------------------------------------------------------------------------
AsyncResult ServiceManager::RegisterSystemService(SSID idSysSer, ISystemService* pService)
{
    return RegisterSystemService(idSysSer, pService, pService->GetDefaultPriority());
}


//-------------------------------------------------------------------------------------------------
AsyncResult ServiceManager::RegisterSystemService(SSID idSysSer,
    ISystemService *pSysSer,
    int iPriority)
{
    // If you hit this assert it means that you did not declare your ISystemService CLASS_ID 
    // with the EE_DECLARE_SYSTEMSERVICE_ID macro and in the required range.
    EE_ASSERT_SERVICEID_RANGE(idSysSer);

    // Check for valid system service information
    if (!pSysSer || (idSysSer == INVALID_SSID))
    {
        EE_LOG(efd::kServiceManager, efd::ILogger::kERR1,
            ("%s - Failed to register system service %i priority %i",
            !pSysSer ? "NULL pointer" : "Invalid SSID", idSysSer, iPriority));
        return AsyncResult_Failure;
    }

    if (NULL != GetSystemService(idSysSer))
    {
        EE_LOG(efd::kServiceManager, efd::ILogger::kERR1,
            ("Service id 0x%08X is already registered.",
            idSysSer));
        return AsyncResult_Failure;
    }

    pSysSer->SetServiceManager(this);

    // Create the system service structure to add to the list
    CSysSer sysSer;
    sysSer.m_idSysSer = idSysSer;
    sysSer.m_register = true;
    sysSer.m_state = kSysServState_PreInit;
    sysSer.m_priority = iPriority;
    sysSer.m_spSysSer = pSysSer;

    if (IsActiveState())
    {
        // Add it to the list of services to register and unregister
        EE_LOG(efd::kServiceManager, efd::ILogger::kLVL0,
            ("Scheduling to register a new system service 0x%08X priority %i",
            idSysSer, iPriority));

        efd::AsyncResult ar = sysSer.CallPreInit();
        if (ar == AsyncResult_Failure)
        {
            return AsyncResult_Failure;
        }
        sysSer.m_state = kSysServState_Initializing;
        m_regSysSerList.push_back(sysSer);
        return AsyncResult_Pending;
    }

    return RegisterSystemService(sysSer);
}


//-------------------------------------------------------------------------------------------------
/*virtual*/ void ServiceManager::UnregisterSystemService(ISystemService *pSysSer)
{
    // Check for valid system service information
    if (!pSysSer)
    {
        EE_LOG(efd::kServiceManager, efd::ILogger::kERR1,
            ("NULL pointer - Failed to unregister system service"));
        return;
    }

    // Allocate the system service structure
    EE_LOG(efd::kServiceManager, efd::ILogger::kLVL0,
        ("Scheduling to unregister a new system service 0x%08X",
        pSysSer));

    // Create the system service structure to add to the list
    CSysSer sysSer;
    sysSer.m_register = false;
    sysSer.m_spSysSer = pSysSer;

    // Add it to the list of services to register and unregister
    //if (IsActiveState() )
    //{
        m_regSysSerList.push_back( sysSer );
    //}
    //else
    //{
    //    UnregisterSystemService( sysSer );
    //}
}


//-------------------------------------------------------------------------------------------------
/*virtual*/ void ServiceManager::UnregisterSystemService(SSID idSysSer)
{
    // Check for valid system service information
    if (idSysSer == INVALID_SSID)
    {
        EE_LOG(efd::kServiceManager, efd::ILogger::kERR1,
            ("Invalid SSID - Failed to unregister system service"));
        return;
    }

    EE_LOG(efd::kServiceManager, efd::ILogger::kLVL0,
        ("Scheduling to unregister a new system service 0x%08X",
        idSysSer));

    // Create the system service structure to add to the list
    CSysSer sysSer;
    sysSer.m_register = false;
    sysSer.m_idSysSer = idSysSer;

    // Add it to the list of services to register and unregister
    //if (IsActiveState() )
    //{
        m_regSysSerList.push_back( sysSer );
    //}
    //else
    //{
    //    UnregisterSystemService( sysSer );
    //}
}


//-------------------------------------------------------------------------------------------------
/*virtual*/ ISystemService* ServiceManager::GetSystemService(SSID idSysSer) const
{
    // Loop through the system service list and find the service requested
    for ( SystemServiceList::const_iterator iterSysSer = m_sysSerList.begin();
          m_sysSerList.end() != iterSysSer;
          ++iterSysSer )
    {
        const CSysSer& ptr = *iterSysSer;
        if (ptr.m_idSysSer == idSysSer)
        {
            // If the service was found, return a pointer to it
            return ptr.m_spSysSer;
        }
    }

    // Loop through the pending system service list and find the service requested
    for ( SystemServiceList::const_iterator iterSysSer = m_regSysSerList.begin();
        m_regSysSerList.end() != iterSysSer;
        ++iterSysSer )
    {
        const CSysSer& ptr = *iterSysSer;
        if (ptr.m_idSysSer == idSysSer && ptr.m_register)
        {
            // If the service was found, return a pointer to it
            return ptr.m_spSysSer;
        }
    }

    // This is not necessarily an error condition
    //EE_LOG(efd::kServiceManager, efd::ILogger::kERR2,
    //    ("GetSystemService could not find system service %i", idSysSer));
    return NULL;
}

//-------------------------------------------------------------------------------------------------
ServiceManager::ServiceState ServiceManager::CheckSystemServiceState( SSID idSysSer )
{
    // Loop through the system service list and find the service requested
    for ( SystemServiceList::iterator iterSysSer = m_sysSerList.begin();
          m_sysSerList.end() != iterSysSer;
          ++iterSysSer )
    {
        CSysSer& ptr = *iterSysSer;
        if (ptr.m_idSysSer == idSysSer)
        {
            // If the service was found, return a pointer to it
            return ptr.m_state;
        }
    }

    EE_LOG(efd::kServiceManager, efd::ILogger::kERR2,
        ("CheckSystemServiceState could not find system service 0x%08X", idSysSer));
    return kSysServState_Invalid;
}

//-------------------------------------------------------------------------------------------------
bool ServiceManager::RegisterClock(IClock* pClock)
{
    EE_ASSERT(pClock);
    ClockList::const_iterator iter = m_clocks.find(pClock->GetClockClassID());
    if (iter != m_clocks.end())
    {
        return false;
    }

    m_clocks[pClock->GetClockClassID()] = pClock;
    return true;
}

//-------------------------------------------------------------------------------------------------
bool ServiceManager::UnregisterClock(IClock* pClock)
{
    EE_ASSERT(pClock);
    ClockList::iterator iter = m_clocks.find(pClock->GetClockClassID());
    if (iter == m_clocks.end() || iter->second != pClock)
    {
        return false;
    }
    m_clocks.erase(iter);
    return true;
}

//-------------------------------------------------------------------------------------------------
TimeType ServiceManager::GetTime(efd::ClassID id) const
{
    IClock* internalClock = GetClock(id);
    if (internalClock)
    {
        return internalClock->GetCurrentTime();
    }
    else
    {
        return 0.0f;
    }
}

//-------------------------------------------------------------------------------------------------  
IClock* ServiceManager::GetClock(efd::ClassID id) const
{
    ClockList::const_iterator iter = m_clocks.find(id);
    if (iter != m_clocks.end())
    {
        return iter->second;
    }
    return NULL;
}

//-------------------------------------------------------------------------------------------------
void ServiceManager::Run()
{
    EE_LOG(efd::kServiceManager, efd::ILogger::kLVL0,
        ("Starting to run the ServiceManager 0x%08X", this));

    while ( m_state != kSysServState_Complete )
    {
        // Tick all the services once
        RunOnce();

        // Yield some CPU to other applications, if desired
        // A sleep time of kNoSleep means don't sleep at all
        if (m_useDeactivatedSleepTime)
        {
            if (m_deactivatedSleepTime != kNoSleep)
                efd::Sleep(m_deactivatedSleepTime);
        }
        else
        {
            if (m_sleepTime != kNoSleep)
                efd::Sleep(m_sleepTime);
        }
    }

    EE_LOG(efd::kServiceManager, efd::ILogger::kLVL0,
        ("Finished running the ServiceManager 0x%08X", this));
}

//-------------------------------------------------------------------------------------------------
/*virtual*/ void ServiceManager::RunOnce()
{
    // ServiceManager is not re-entrant, so make sure we don't call RunOnce from RunOnce
    EE_ASSERT( !m_inRunOnce );
    m_inRunOnce = true;

    // ServiceManager pClock is updated once per main loop:
    m_clock.Update();

    // Update the internal clocks
    for ( ClockList::iterator iter = m_clocks.begin();
        iter != m_clocks.end();
        ++iter )
    {
        IClock* pClock = iter->second;
        if (pClock)
        {
            pClock->Update(m_clock.GetCurrentTime());
        }
    }

    // Grab the current time
#ifndef EE_DISABLE_LOGGING
    TimeType cycleStartTime = GetServiceManagerTime();

    EE_LOG(efd::kTime, efd::ILogger::kLVL3, ("New frame at time %.4f", cycleStartTime));
#endif


    // once we arrive here we know its time to start doing real work so if we haven't already
    // then move into the Init state.
    if (m_state < kSysServState_PreInit )
    {
        m_state = kSysServState_PreInit;
    }

    // Tick all the services.  This will either call OnInit, OnTick, or OnShutdown depending
    // on the state of the ServiceManager and of all the services.
    TickServices();

    // Check for new services to register or existing services to unregister
    RegUnRegSystemServices();

#ifndef EE_DISABLE_LOGGING
    TimeType cycleEndTime = m_clock.ComputeCurrentTime();
    // compute the delta in milliseconds:
    TimeType delta = 1000.0 * (cycleEndTime - cycleStartTime);

    if (delta > m_kMaxAcceptableTickTime )
    {
        EE_LOG(efd::kServiceManager, efd::ILogger::kLVL3, ("Slow frame: %.4fms", delta));
        LogServiceTiming();
    }
#endif

    // Send the time it took to tick the service to the metrics system
    METRICS_PROBE_METRIC( "FRAMEWORK.CYCLE_TIME", delta );
    METRICS_TICK();

    m_inRunOnce = false;
}


//-------------------------------------------------------------------------------------------------
void ServiceManager::InitializeServices()
{
    if (m_state < kSysServState_PreInit )
    {
        m_state = kSysServState_PreInit;
    }

    bool initComplete = false;
    while ( !initComplete )
    {
        initComplete = true;
        RunOnce();

        // Forward iterate through the services if we are initializing and running
        for ( SystemServiceList::iterator iterSysSer = m_sysSerList.begin();
            iterSysSer != m_sysSerList.end();
            ++iterSysSer )
        {
            CSysSer& ptr = *iterSysSer;
            if( ptr.m_state <= kSysServState_Initializing )
            {
                initComplete = false;
            }
        }
    }
}


//-------------------------------------------------------------------------------------------------
void ServiceManager::InitializeService(SSID idSysSer)
{
    if (m_state < kSysServState_PreInit )
    {
        m_state = kSysServState_PreInit;
    }

    bool initComplete = false;
    while ( !initComplete )
    {
        initComplete = true;
        RunOnce();

        // Forward iterate through the services if we are initializing and running
        for ( SystemServiceList::iterator iterSysSer = m_sysSerList.begin();
            iterSysSer != m_sysSerList.end();
            ++iterSysSer )
        {
            CSysSer& ptr = *iterSysSer;
            if( ptr.m_idSysSer == idSysSer && ptr.m_state <= kSysServState_Initializing )
            {
                initComplete = false;
            }
        }
    }
}


//-------------------------------------------------------------------------------------------------
void ServiceManager::ShutdownServices()
{
    Shutdown();
    bool shutdownComplete = false;
    while ( !shutdownComplete )
    {
        shutdownComplete = true;
        // Forward iterate through the services if we are initializing and running
        for ( SystemServiceList::iterator iterSysSer = m_sysSerList.begin();
            iterSysSer != m_sysSerList.end();
            ++iterSysSer )
        {
            CSysSer& ptr = *iterSysSer;
            if( ptr.m_state < kSysServState_Complete )
            {
                shutdownComplete = false;
            }
        }
        RunOnce();
    }

    // flush any outstanding log messages.
    efd::GetLogger()->Flush();
}


//-------------------------------------------------------------------------------------------------
// Requests a shutdown. OnShutdown actually does the shutting down.
void ServiceManager::Shutdown( bool i_Immediate /*=false*/ )
{
    DumpAccumulatedServiceTimes();

    EE_LOG(efd::kServiceManager, efd::ILogger::kLVL0,
        ("Signaling the ServiceManager 0x%08X to shutdown", this));

    m_bShutdownRequested = true;

    // If the shutdown was requested by someone running outside of the ServiceManager::TickServices
    // function then we should immediately go to the shutting down state.  Otherwise we will
    // wait until the current TickServices call completes before changing states.
    if (!IsActiveState() &&
         m_state < kSysServState_ShuttingDown )
    {
        EE_LOG(efd::kServiceManager, efd::ILogger::kLVL1,
            ("ServiceManager immediately entering the ShuttingDown state."));
        m_state = kSysServState_ShuttingDown;
    }

    // flush any outstanding log messages.
    efd::GetLogger()->Flush();

    EE_UNUSED_ARG(i_Immediate);
}


//-------------------------------------------------------------------------------------------------
bool ServiceManager::GetShutdown()
{
    return m_bShutdownRequested;
}


//-------------------------------------------------------------------------------------------------
// The "real" registration happens here. The other call is just a "request"
AsyncResult ServiceManager::RegisterSystemService( CSysSer& sysSer )
{
    SystemServiceList::iterator iterInsertSysSer = m_sysSerList.end();

    // Loop through all the services to make sure there is not a duplicate
    // service ID registered already.  Also look for the insertion point for
    // this new service.
    for ( SystemServiceList::iterator iterSysSer = m_sysSerList.begin();
          m_sysSerList.end() != iterSysSer;
          ++iterSysSer )
    {
        CSysSer& ptr = *iterSysSer;

        // Check for duplicate IDs, which is strictly verbotten
        if (ptr.m_idSysSer == sysSer.m_idSysSer )
        {
            EE_LOG(efd::kServiceManager, efd::ILogger::kERR0,
                ("Duplicate SSID - Failed to actually register a new system service "
                "%s (0x%08X) priority %i",
                sysSer.m_spSysSer->GetDisplayName(), sysSer.m_idSysSer, sysSer.m_priority));

            return AsyncResult_Failure;
        }

        // If the current item is a lower priority save the iterator so we can
        // insert at that point
        if ((m_sysSerList.end() == iterInsertSysSer) && (ptr.m_priority < sysSer.m_priority) )
        {
            iterInsertSysSer = iterSysSer;
        }
    }

    EE_LOG(efd::kServiceManager, efd::ILogger::kLVL1,
        ("Actually registering a new system service %s (id=0x%08X, this=0x%08X) priority %i",
        sysSer.m_spSysSer->GetDisplayName(), sysSer.m_idSysSer,
        (efd::ISystemService*)sysSer.m_spSysSer, sysSer.m_priority));

    sysSer.m_register = true;

    if (m_sysSerList.end() != iterInsertSysSer )
    {
        // If we found a lower priority insert the service before it
        m_sysSerList.insert( iterInsertSysSer, sysSer );
    }
    else
    {
        // If did not find a lower priority insert it at the end of the list
        m_sysSerList.push_back( sysSer );
    }
    return AsyncResult_Complete;
}


//-------------------------------------------------------------------------------------------------
AsyncResult ServiceManager::UnregisterSystemService( CSysSer& sysSer )
{
    // Loop through the system service list and find the service to unregister
    SystemServiceList::iterator iterSysSer = m_sysSerList.begin();

    while ( m_sysSerList.end() != iterSysSer
            && ((*iterSysSer).m_idSysSer != sysSer.m_idSysSer)
            && ((*iterSysSer).m_spSysSer != sysSer.m_spSysSer))
    {
        ++iterSysSer;
    }

    // If the service was found remove it
    if (m_sysSerList.end() != iterSysSer )
    {
        EE_LOG(efd::kServiceManager, efd::ILogger::kLVL1,
            ("Begin shutdown on system service %i (0x%08X) priority %i",
            (*iterSysSer).m_idSysSer,
            (efd::ISystemService*)((*iterSysSer).m_spSysSer),
            (*iterSysSer).m_priority));

        // Update the service pointer to the service to actually shutdown
        sysSer.m_spSysSer = (*iterSysSer).m_spSysSer;

        if (kSysServState_Complete == (*iterSysSer).m_state )
        {
            m_sysSerList.erase( iterSysSer );
            return AsyncResult_Complete;
        }
        else
        {
            (*iterSysSer).m_state = kSysServState_ShuttingDown;
            return AsyncResult_Pending;
        }
    }
    else
    {
        EE_LOG(efd::kServiceManager, efd::ILogger::kERR1,
            ("Not Found - Failed to unregister a system service %s (id=0x%08X, this=0x%08X)",
            sysSer.m_spSysSer->GetDisplayName(), sysSer.m_idSysSer,
            (efd::ISystemService*)(sysSer.m_spSysSer)));

        return AsyncResult_Failure;
    }
}


//-------------------------------------------------------------------------------------------------
/*virtual*/ void ServiceManager::RegUnRegSystemServices()
{
    // Loop through the list of services to register and unregister and do it
    // Grab and hold the lock during the entire process because it is not only
    // easier, but will ensure the ServiceManager will only have to wait for the lock
    // once per cycle.

    SystemServiceList::iterator iterRegSysSer = m_regSysSerList.begin();
    while ( m_regSysSerList.end() != iterRegSysSer )
    {
        CSysSer sysSer = (*iterRegSysSer);

        AsyncResult result;

        if (sysSer.m_register )
        {
            result = RegisterSystemService( sysSer );
        }
        else
        {
            result = UnregisterSystemService( sysSer );
        }

        // warning: iterator advance must occur before pop_front to avoid invalidating it
        ++iterRegSysSer;

        if (result != AsyncResult_Pending)
        {
            m_regSysSerList.pop_front();
        }
    }
}


//-------------------------------------------------------------------------------------------------
/*virtual*/ void ServiceManager::UnregAllSystemServices()
{
    EE_LOG(efd::kServiceManager, efd::ILogger::kLVL1,
        ("Unregistering all the system services in ServiceManager 0x%08X", this));

    // Loop through the list of services to register and unregister and do it
    while ( !m_sysSerList.empty() )
    {
        // Grab the first service to unregister
        CSysSer& sysSer = m_sysSerList.back();

        // Unregister the service immediately
        UnregisterSystemService( sysSer );
    }
}


//-------------------------------------------------------------------------------------------------
/*virtual*/ void ServiceManager::TickServices()
{
    EE_LOG(efd::kServiceManager, efd::ILogger::kLVL3,
        ("Beginning a new tick cycle"));
    m_active = true;

    efd::UInt32 cActiveServices = 0;

    // Figure out if we should forward or reverse iterator through the list
    // We need to cache this in a local variable because we don't want to change
    // iteration directions in the middle of a cycle in case shutdown is called
    // while cycling through the services.
    bool fwdIterate = m_state != kSysServState_ShuttingDown;

    // If the system is initializing then attempt to configure the ServiceManager
    if (m_state == kSysServState_Initializing)
    {
        // immediately move to running state
        m_state = kSysServState_Running;
        ConfigureFramerate();
    }

    // Loop through the system service list and tick them in turn
    if (fwdIterate )
    {
        // Foward iterate through the services if we are initializing and running
        for ( SystemServiceList::iterator iterSysSer = m_sysSerList.begin();
              iterSysSer != m_sysSerList.end();
              ++iterSysSer )
        {
            CSysSer& service = *iterSysSer;
            cActiveServices += TickService( service );
        }
    }
    else
    {
        // Reverse iterate through the service if we are shutting down
        for ( SystemServiceList::reverse_iterator iterSysSer = m_sysSerList.rbegin();
              iterSysSer != m_sysSerList.rend();
              ++iterSysSer )
        {
            CSysSer& service = *iterSysSer;
            cActiveServices += TickService( service );
        }
    }

#ifndef EE_DISABLE_LOGGING
    if (m_bLogPeriodicServiceTiming && GetServiceManagerTime() > m_nextTimingDump )
    {
        LogServiceTiming();
    }
#endif

    // Check to see if we have completed the initialization or shutdown states.  We never
    // automatically advance out of the running state because its valid for a completely event
    // driven program to not use OnTick at all.
    if (kSysServState_PreInit == m_state )
    {
        // the Pre-Init state is a one shot deal.
        m_state = kSysServState_Initializing;
    }
    else if (0 == cActiveServices )
    {
        switch ( m_state )
        {
        default:
            break;
        case kSysServState_Initializing:
#ifndef EE_DISABLE_LOGGING
            EE_LOG(efd::kServiceManager, efd::ILogger::kLVL1,
                ("ServiceManager Initialization complete, entering the Running state."));
            LogServiceTiming();
#endif
            m_state = kSysServState_Running;
            break;

        case kSysServState_ShuttingDown:
#ifndef EE_DISABLE_LOGGING
            EE_LOG(efd::kServiceManager, efd::ILogger::kLVL1,
                ("ServiceManager Shutdown complete, process will now exit."));
            LogServiceTiming();
#endif
            m_state = kSysServState_Complete;
            break;
        }
    }

    // If a shutdown was requested this loop, process it:
    if (GetShutdown() &&
         m_state < kSysServState_ShuttingDown )
    {
#ifndef EE_DISABLE_LOGGING
        EE_LOG(efd::kServiceManager, efd::ILogger::kLVL1,
            ("ServiceManager entering the ShuttingDown state."));
        LogServiceTiming();
#endif
        m_state = kSysServState_ShuttingDown;
    }

    m_active = false;
}


//-------------------------------------------------------------------------------------------------
efd::UInt32 ServiceManager::TickService( ServiceManager::CSysSer& currentService )
{
    efd::UInt32 result = 0;

    // We should not be in an unitialized state.
    EE_ASSERT( currentService.m_state >= kSysServState_PreInit );

    // Grab the current time
    TimeType startTime = 0.0;
    if (m_uiServiceTimeStatTypes )
    {
        startTime = m_clock.ComputeCurrentTime();
    }


    // Tick the service
    AsyncResult ar = AsyncResult_Pending;

    switch (m_state)
    {
    case kSysServState_PreInit:
        if (kSysServState_PreInit == currentService.m_state)
        {
            ar = currentService.CallPreInit();
            EE_ASSERT( ar != AsyncResult_Pending );
        }
        break;
    case kSysServState_Initializing:
        switch (currentService.m_state)
        {
        default:
            break;
        case kSysServState_PreInit:
            ar = currentService.CallPreInit();
            if (AsyncResult_Failure == ar)
            {
                break;
            }
            currentService.m_state = kSysServState_Initializing;
            // fall through...

        case kSysServState_Initializing:
            ar = currentService.m_spSysSer->OnInit();
            if (AsyncResult_Pending == ar)
            {
                result = 1;
            }
            break;
        }
        break;

    case kSysServState_Running:
        switch (currentService.m_state)
        {
        default:
            break;
        case kSysServState_PreInit:
            ar = currentService.CallPreInit();
            if (AsyncResult_Failure == ar)
            {
                break;
            }
            currentService.m_state = kSysServState_Initializing;
            // fall through...

        case kSysServState_Initializing:
            ar = currentService.m_spSysSer->OnInit();
            break;

        case kSysServState_WaitingToRun:
            currentService.m_state = kSysServState_Running;
            // fall through...

        case kSysServState_Running:
            ar = currentService.m_spSysSer->OnTick();
            break;

        case kSysServState_ShuttingDown:
            // Services can enter shutdown during the ServiceManager running state by calling
            // UnregisterService.
            ar = currentService.m_spSysSer->OnShutdown();
        }
        break;

    case kSysServState_ShuttingDown:
        switch (currentService.m_state)
        {
        case kSysServState_PreInit:
        case kSysServState_Initializing:
        case kSysServState_WaitingToRun:
        case kSysServState_Running:
        case kSysServState_WaitingToShutdown:
            // abort current state and go to shutdown state:
            currentService.m_state = kSysServState_ShuttingDown;
            // fall through ...

        case kSysServState_ShuttingDown:
            ar = currentService.m_spSysSer->OnShutdown();
            if (AsyncResult_Pending == ar)
            {
                result = 1;
            }
            break;

        case kSysServState_Complete:
            // do nothing
            break;

        default:
            EE_FAIL("Unexpected service state");
            break;
        }
        break;
    case kSysServState_Complete:
        break;
    default:
        EE_FAIL("Unexpected ServiceManager state");
        break;
    }

    switch (ar)
    {
    default:
        break;
    case AsyncResult_Complete:
        // just go to the next state when we're done.  The various "WaitingFor" states
        // provide all the gates we need to wait for the ServiceManager state to advance.
        currentService.m_state = (ServiceState)((efd::SInt32)currentService.m_state + 1);
        if (kSysServState_Complete == currentService.m_state )
        {
            currentService.m_spSysSer->SetServiceManager( NULL );
        }

        m_serviceTickResult = ar;
        break;

    case AsyncResult_Failure:
        EE_LOG(efd::kServiceManager, efd::ILogger::kERR0,
            ("Service %s(0x%08X) critically failed, entering the ShuttingDown state.",
            currentService.m_spSysSer->GetDisplayName(),
            currentService.m_idSysSer ));
        Shutdown();
        m_serviceTickResult = ar;
        break;
    }

    if (m_uiServiceTimeStatTypes)
    {
        TimeType endTime = m_clock.ComputeCurrentTime();
        TimeType delta = endTime - startTime;
        currentService.StoreLastTickTime( delta );
    }

    // Create the metric name by appending the SSID to the base name
    METRICS_ONLY(efd::Snprintf( m_tickTimeMetricTail,
        EE_ARRAYSIZEOF(m_tickTimeMetric) - EE_ARRAYSIZEOF(kServiceTickRoot) - 1,
        EE_TRUNCATE,
        "%s", currentService.m_spSysSer->GetDisplayName() ));

    // Send the time it took to tick the service to the metrics system
    METRICS_PROBE_METRIC( m_tickTimeMetric, delta);

    return result;
}


//-------------------------------------------------------------------------------------------------
/*static*/ efd::utf8string ServiceManager::IDToString(efd::ClassID typeID)
{
    efd::utf8string ccstr( efd::Formatted, "0x%08X", typeID);
    return ccstr;
}


//-------------------------------------------------------------------------------------------------
void ServiceManager::SetProgramType( ProgramType i_pt, bool i_on )
{
    m_programTypes = BitUtils::SetBitsOnOrOff( m_programTypes, (UInt32)i_pt, i_on );
}


//-------------------------------------------------------------------------------------------------
bool ServiceManager::IsProgramType( ProgramType i_pt )
{
    return BitUtils::AllBitsAreSet( m_programTypes, (UInt32)i_pt );
}


//-------------------------------------------------------------------------------------------------
void ServiceManager::SetServiceTimeStatTypes(unsigned int uiTypes)
{
    m_uiServiceTimeStatTypes = uiTypes;
#ifdef EE_METRICS_ENABLED
    m_uiServiceTimeStatTypes |= SERVICETIME_METRICS;
#endif
}

//-------------------------------------------------------------------------------------------------
unsigned int ServiceManager::GetServiceTimeStatTypes()
{
    return m_uiServiceTimeStatTypes;
}

//-------------------------------------------------------------------------------------------------
const efd::map<SSID, TimeType>& ServiceManager::GetAccumulatedServiceTimes()
{
    return m_mapAccumulatedServiceTimes;
}

//-------------------------------------------------------------------------------------------------
void ServiceManager::ResetAccumulatedServiceTimes()
{
    m_mapAccumulatedServiceTimes.clear();
}

//-------------------------------------------------------------------------------------------------
TimeType ServiceManager::GetServiceLastTickTime(SSID idSysSer)
{
    for ( SystemServiceList::iterator iterSysSer = m_sysSerList.begin();
        iterSysSer != m_sysSerList.end();
        ++iterSysSer )
    {
        CSysSer& service = *iterSysSer;
        if (service.m_idSysSer == idSysSer)
            return service.GetLastTickTime();
    }

    return 0.0f;
}

//-------------------------------------------------------------------------------------------------
void ServiceManager::LogServiceTiming()
{
    if (m_uiServiceTimeStatTypes & SERVICETIME_ACCUMULATE)
    {
        for ( SystemServiceList::iterator iterSysSer = m_sysSerList.begin();
              iterSysSer != m_sysSerList.end();
              ++iterSysSer )
        {
            CSysSer& service = *iterSysSer;
            TimeType LastTickTime = service.GetLastTickTime();
            // We use INVALID_SSID as a slot to store the total time for all services
            // This lets us pass that value in the map since no service will have that ID
            m_mapAccumulatedServiceTimes[INVALID_SSID] += LastTickTime;
            m_mapAccumulatedServiceTimes[service.m_idSysSer] += LastTickTime;
        }
    }
    if (m_uiServiceTimeStatTypes & SERVICETIME_LOG)
    {
        // No sense doing all the work if the logging is disabled
        if (!efd::GetLogger()->IsLogging(efd::kServiceManager, efd::ILogger::kLVL2))
        {
            return;
        }

        efd::priority_queue< ServiceSorter > sorter;
        TimeType totalActiveTime = 0.0;

        for ( SystemServiceList::iterator iterSysSer = m_sysSerList.begin();
              iterSysSer != m_sysSerList.end();
              ++iterSysSer )
        {
            CSysSer& service = *iterSysSer;
            totalActiveTime += service.GetLastTickTime();
            sorter.push( ServiceSorter(service) );
        }

        static utf8string timingMessage;
        timingMessage.sprintf( "Service Timing:\n%20s %10s %10s %10s %10s %5s notes",
            "Service Name", "SSID", "LAT(ms)", "RAT(ms)", "LTT(ms)", "%time" );
        while ( !sorter.empty() )
        {
            const ServiceSorter& ss = sorter.top();
            CSysSer& service = *ss.m_pService;

            TimeType lastTick = service.GetLastTickTime();
            TimeType recentAverage = service.ComputeRecentAverageSPF();

            utf8string notes;

            // If a single service takes more than 10ms, its slow!
            if (lastTick >= 10.0 )
            {
                notes.append( " SLOW!" );
            }

            // If a service is more than 50% of the total time, its a hog!
            if (lastTick > 0.5 * totalActiveTime )
            {
                notes.append( " Hog!" );
            }

            // If a tick takes twice as long as average, its a spike!
            if (lastTick > 2.0 * recentAverage )
            {
                notes.append( " Spike!" );
            }

            timingMessage.sprintf_append("\n%20s 0x%08X %10.6f %10.6f %10.6f %5.2f%s",
                service.m_spSysSer->GetDisplayName(), service.m_idSysSer,
                ss.m_time, recentAverage, lastTick, 100.0 * lastTick / totalActiveTime,
                notes.c_str() );

            sorter.pop();
        }
        EE_LOG(efd::kServiceManager, efd::ILogger::kLVL2, ("%s", timingMessage.c_str() ));
    }
    m_nextTimingDump = GetServiceManagerTime() + m_kTimingDumpInterval;
}

//-------------------------------------------------------------------------------------------------
void ServiceManager::DumpAccumulatedServiceTimes()
{
#if !defined(EE_DISABLE_LOGGING)
    // No sense doing all the work if the logging or time accumulation is disabled
    if (!(m_uiServiceTimeStatTypes & SERVICETIME_ACCUMULATE))
        return;

    if (!efd::GetLogger()->IsLogging(efd::kServiceManager, efd::ILogger::kLVL1))
    {
        return;
    }

    EE_LOG(efd::kServiceManager, efd::ILogger::kLVL1, ("Accumulated Service Timing:"));

    // First get the total time for all services.  It's stored in the map
    // under INVALID_SSID.
    efd::TimeType totalTime = 1.0;

    efd::map<efd::SSID, efd::TimeType>::const_iterator iter =
        m_mapAccumulatedServiceTimes.find(INVALID_SSID);
    if (iter != m_mapAccumulatedServiceTimes.end())
    {
        totalTime = iter->second;
        EE_LOG(efd::kServiceManager, efd::ILogger::kLVL1,
            ("TOTAL TIME (All Services): %g", totalTime));
    }

    // Now iterate through the valid services and print each one's share.
    EE_LOG(efd::kServiceManager, efd::ILogger::kLVL1,
        ("%20s %10s %10s %10s",
        "Service Name", "SSID", "ACC(ms)", "%time"));
    for (iter = m_mapAccumulatedServiceTimes.begin();
         iter != m_mapAccumulatedServiceTimes.end();
         ++iter)
    {
        efd::SSID ssid = iter->first;
        if (ssid != INVALID_SSID)
        {
            efd::ISystemService* pService = GetSystemService(ssid);
            const char *pcServiceName = pService ? pService->GetDisplayName() : "NULL_SERVICE_PTR";
            efd::TimeType timeType = iter->second;

            EE_LOG(efd::kServiceManager, efd::ILogger::kLVL1, 
                ("%20s 0x%08X %10.6f %10.6f", 
                pcServiceName, ssid, timeType, (timeType/totalTime)*100.0));
        }
    }
#endif
}

//-------------------------------------------------------------------------------------------------
efd::UInt32 ServiceManager::GetVirtualProcessID() const
{
    return m_virtualProcessID;
}

//-------------------------------------------------------------------------------------------------
void ServiceManager::SetVirtualProcessID( efd::UInt32 procID )
{
    m_virtualProcessID = procID;
}

//-------------------------------------------------------------------------------------------------
const efd::utf8string ServiceManager::GetVirtualProcessName() const
{
    return m_virtualProcessName;
}

//-------------------------------------------------------------------------------------------------
efd::Bool ServiceManager::IsRunning() const
{
    return m_inRunOnce;
}

//-------------------------------------------------------------------------------------------------
void ServiceManager::SetVirtualProcessName( const efd::utf8string procName )
{
    // it is only valid to set the virtual process name once
    EE_ASSERT( m_virtualProcessName.empty() );
    EE_LOG( efd::kServiceManager, efd::ILogger::kLVL0,
        ("*******************************************************************************" ));
    EE_LOG( efd::kServiceManager, efd::ILogger::kLVL0,
        ("* VirtualProcess: %s", procName.c_str() ));
    EE_LOG( efd::kServiceManager, efd::ILogger::kLVL0,
        ("*******************************************************************************" ));
    m_virtualProcessName = procName;
}


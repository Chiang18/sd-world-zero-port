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
#ifndef EE_SERVICEMANAGER_H
#define EE_SERVICEMANAGER_H

#include <efd/ISystemService.h>
#include <efd/IBase.h>
#include <efd/CriticalSection.h>
#include <efd/utf8string.h>
#include <efd/StdContainers.h>
#include <efd/TimeType.h>
#include <efd/efdClassIDs.h>
#include <efd/IMessage.h>


namespace efd
{
/// Use this in services where you want to wait for a service to be initialized.
#define EE_DEPENDS_ON_SERVICE(serviceClass)                                                       \
    switch (m_pServiceManager->CheckSystemServiceState(serviceClass::CLASS_ID))                   \
    {                                                                                             \
    case efd::ServiceManager::kSysServState_Invalid:                                              \
        EE_LOG(efd::kServiceManager, efd::ILogger::kERR0,                                         \
            ("The dependent service (classID: %d) was not registered with the ServiceManager.",   \
            serviceClass::CLASS_ID));                                                             \
        EE_FAIL_MESSAGE(("Service (classID: %d) not registered with ServiceManager.",             \
            serviceClass::CLASS_ID));                                                             \
        return efd::AsyncResult_Failure;                                                          \
    case efd::ServiceManager::kSysServState_PreInit:                                              \
    case efd::ServiceManager::kSysServState_Initializing:                                         \
        return efd::AsyncResult_Pending;                                                          \
    default:                                                                                      \
        break;                                                                                    \
    }

/**
    A cache of ISystemService instances that are managed to control preinit, initialization,
    ticking, and shutdown. The ServiceManager is a central repository for ISystemService instances.
    It maintains a prioritized list of ISystemServices.
 */
class EE_EFD_ENTRY ServiceManager : public efd::IBase
{
    EE_DECLARE_CLASS1(ServiceManager, efd::kCLASSID_ServiceManager, efd::IBase);
    EE_DECLARE_CONCRETE_REFCOUNT;

public:


    /**
        The combination of the ServiceManager state and the service's current state determines
        which ISystemService method will be called.  The ServiceManager maintains the state of
        each service.
     */
    enum ServiceState
    {
        kSysServState_Invalid,
        kSysServState_PreInit,
        kSysServState_Initializing,
        kSysServState_WaitingToRun,
        kSysServState_Running,
        kSysServState_WaitingToShutdown,
        kSysServState_ShuttingDown,
        kSysServState_Complete,
    };

public:

    /// @name Construction and Destruction
    //@{
    ServiceManager();
    virtual ~ServiceManager();
    //@}

    /**
        RegisterSystemService is used to register a service to be executed inside
        the ServiceManager.

        You may register a system service at any time.  If you register a service during the
        processing of the current service list then the actual registration will be delayed
        until the end of that processing.  Once registered the service will enter the
        "Initializing" state.  Once all services are initialized then the service will enter the
        "Running" state.

        @note It is an error to register the same system service twice.
        @note The priority used to execute the service will be the default priority
              reported by that service (pSysServ->GetDefaultPriority())

        @param idSysSer The globally unique ID of the system service
        @param pService A pointer to the service to register

        @return AsyncResult Success if the service was immediately registered, pending if
                  registration was queued, and failure if the request is invalid.
    */
    virtual AsyncResult RegisterSystemService(
        SSID idSysSer,
        ISystemService* pService);

    /**
        RegisterSystemService is used to register a service to be executed inside
        the ServiceManager.

        You may register a system service at any time.  If you register a service during the
        processing of the current service list then the actual registration will be delayed
        until the end of that processing.  Once registered the service will enter the
        "Initializing" state.  Once all services are initialized then the service will enter the
        "Running" state.

        @note It is an error to register the same system service twice.

        @param idSysSer The globally unique ID of the system service
        @param pSysSer A pointer to the service to register
        @param iPriority The priority at which to execute the service

        @return AsyncResult Success if the service was immediately registered, pending if
                  registration was queued, and failure if the request is invalid.
    */
    virtual AsyncResult RegisterSystemService(
        SSID idSysSer,
        ISystemService* pSysSer,
        int iPriority);

    /**
        UnregisterSystemService is used to unregister an existing system service by pointer.

        A service may be unregistered at any time.  Unregistering a service places the service
        into the shutdown state.  Once a service completes shutdown it will be removed from the
        ServiceManager completely.

        @param pSysSer A pointer to the service to unregister
    */
    virtual void UnregisterSystemService(ISystemService *pSysSer);


    /**
        UnregisterSystemService is used to unregister an existing system service by ID

        A service may be unregistered at any time.  Unregistering a service places the service
        into the shutdown state.  Once a service completes shutdown it will be removed from the
        ServiceManager completely.

        @param idSysSer The ID of the service to unregister
    */
    virtual void UnregisterSystemService(SSID idSysSer);


    /**
        GetSystemService is used to find a system service by the ID it used to register itself.
        Only services that have entered the "Initializing" state and not completed the "Shutdown"
        state can be retrieved.

        @param idSysSer The ID of the service to find.

        @return ISystemService Pointer to the system service if found,
                                   otherwise it returns NULL.

        \note This method is not thread safe.  It should only be called
        from thread that is executing the Run method or in a services tick call.
    */
    virtual ISystemService* GetSystemService(SSID idSysSer) const;

    /// Helper template that can cast the system service identified by idSysSer into the
    /// appropriate type represented by T.
    template< typename T >
    T* GetSystemServiceAs(SSID idSysSer) const;

    /// Helper template that can cast the system service into the
    /// appropriate type represented by T.
    template< typename T >
    T* GetSystemServiceAs() const;

    /**
        Run is used to start ticking each of the registered services until the Shutdown method
        is called.  This is the main loop of a ServiceManager controlled application.  It first
        performs initialization, then runs until shutdown is requested, and then controls the
        shutdown process.  Applications that call Run should not call OnInit, RunOnce, or
        Shutdown directly as the Run function will perform those tasks.
    */
    virtual void Run();


    /**
        InitializeServices is used to Initialize the ServiceManager in situations where the
        application calls RunOnce() directly.  The InitializeServices function will block until
        all services have completed their initialization state.
    */
    virtual void InitializeServices();

    /**
    InitializeService is used to Initialize the ServiceManager in situations where the
    application calls RunOnce() directly.  The InitializeService function will block until
    the specified service has completed its initialization state.
    */
    virtual void InitializeService(SSID idSysSer);


    /**
        RunOnce is used to tick each of the registered services once. This is useful for
        integrating the ServiceManager with other application ServiceManagers.  Using Run() is the
        recommended approach. You must first call InitializeServices() before calling this method.
    */
    virtual void RunOnce();

    /**
        ShutdownServices is used to shut down the ServiceManager in situations where the
        application calls RunOnce() directly.  The ShutdownServices function will request a
        shutdown and then block until all services have completed their shutdown state.
    */
    virtual void ShutdownServices();


    /**
        Shutdown is used to request a shutdown of the ServiceManager.  By default the current main
        loop will complete and then the ServiceManager will go into the "shutting down" state.  If
        an immediate shutdown is requested then the main loop is aborted and we go directly to
        the shutting down state.

        @param i_Immediate If true, this shutdown is being requested due to a critical error
              and the stability of the process is in question.  Normal clean shutdown will
              be by-passed when passing this flag.
    */
    virtual void Shutdown(bool i_Immediate = false);

    /**
         GetShutdown is used to check to see if a shutdown has been requested by using the
         Shutdown() method.  Note that this becomes true the second the request is made even
         though we do not enter the "shutting down" state until the next main loop.

         @return bool True if a ServiceManager shutdown has been requested
    */
    bool GetShutdown();

    /**
        Deterimine if the initialization phase of process startup has completed.
        @return bool True if OnInit has already executed
    */
    inline bool IsInitialized() const;


    /**
         Returns the state of the ServiceManager itself.  Be aware that individual services might
         be in a different state than the ServiceManager (for example, a service added while the
         ServiceManager is already running will be in the initializing state).  The only valid
         ServiceManager states are:
          - kSysServState_PreInit : ServiceManager is allocated but has not begun to run
          - kSysServState_Initializing : ServiceManager is in the process of calling OnInit on all
                system services and will continue to do so until they all complete initialization.
          - kSysServState_Running : ServiceManager is in the process of calling OnTick on all
                system and will continue to do so until a Shutdown request is received.
          - kSysServState_ShuttingDown : ServiceManager is in the process of calling OnShutdown on
                all system services and will continue to do so until they all complete the
                shutdown state.  Note that this state is entered on the next loop after a
                Shutdown() request is received and not immediately upon receiving that request.
          - kSysServState_Complete : ServiceManager main loop has completed.

         @return ServiceState Current state of the ServiceManager.
    */
    inline ServiceState GetCurrentState() const;


    /**
        The state of each service is maintained by the ServiceManager, not by the services
        themselves. To check the state of a service you call this method.  If needed, services may
        implement their own concept of state, this is just the "initialized verse running verse
        shutting down" state and should not be overloaded for other purposes.

        @param idSysSer The ID of the service to check

        @return ServiceState Current state of the service or kSysServState_Invalid if the service
                              was not found.
    */
    ServiceState CheckSystemServiceState( SSID idSysSer );


    /**
         Translates the message type ID or ClassID into a human readable string.
          (mostly for use in log messages so can relate to real IDs)

         @param typeID the type or class to be translated.
         @return efd::utf8string containing the string for the ID specified in human readable hex
            form.
     */
    static efd::utf8string IDToString(efd::ClassID typeID);


    /**
         The ServiceManagerTime is a "stepped real time" clock.  By stepped it means that it only
         increments once at the beginning of each "main loop" of the ServiceManager.  By real time
         it means that it advances at a rate of one second per real-world second.  The
         ServiceManager time will be synchronized between all servers and all clients.
     */
    inline efd::TimeType GetServiceManagerTime() const;


    /**
        Register a clock with the service manager. This clock will be updated every tick. The
        clock will be deleted when the ServiceManager is destroyed unless unregistered.

        @param clock the clock to be registered.
        @return bool returns whether or not the clock was properly registered. If
         a clock by that id already exists, the return value is false.
    */
    bool RegisterClock(IClock* clock);

    /**
        Remove a clock from the service manager. The clock is not deleted, it is assumed
        that the caller will delete the clock.

        @param clock the clock to be unregistered.
        @return bool returns whether or not the clock was found.
    */
    bool UnregisterClock(IClock* clock);

    /**
        Get the time for the specified clock.

        @param id The clock to query; this is the IClock::GetClockClassID value for a clock
            previously registered with the RegisterClock method.
        @return efd::TimeType with the current time of the clock. If no clock by that id
            exists, the return value is 0.0f.
    */
    TimeType GetTime(efd::ClassID id) const;

    /**
        Get the clock for the specified identifier.

        @param id the clock to query; this is the IClock::GetClockClassID value for a clock
            previously registered with the RegisterClock method.
        @return An efd::IClock pointer for the requested clock, if found (NULL otherwise).
    */
    IClock* GetClock(efd::ClassID id) const;


    /**
        Specifies if the ServiceManager should periodically log a detailed time break-down of
        the running services. By default the service manager will log this information. Note:
        Even if service time logging is disabled, the ServiceManager will still log if any
        services take longer than the specified max acceptable tick time.
    */
    inline void SetPeriodicLogServiceTiming(efd::Bool bEnableLogging);

    /// Determine if periodic log timings are enabled.
    /// @return true if periodic log timings are enabled.
    inline efd::Bool GetPeriodicPeriodicLogServiceTiming() const;

    /**
        Set how often we log a detailed time break-down of the running services. The
        ServiceManager defaults to a 20 second log interval.
    */
    inline void SetLogTimingDumpInterval(efd::TimeType kTimingDumpInterval);

    /// Get how often we log a detailed time break-down of the running services.
    inline efd::TimeType GetLogTimingDumpInterval() const;

    /**
        Set the maximum number of Milliseconds that we expect a single Tick to use.  If a tick
        takes longer then we will generate a slow-frame warning and log timing data. By default
        the ServiceManager uses a 5 millisecond max acceptable tick time.
    */
    inline void SetMaxAcceptableTickTime(efd::TimeType kMaxAcceptableTickTime);

    /// Get the maximum number of Milliseconds that we expect a single Tick to use.
    inline efd::TimeType GetMaxAcceptableTickTime() const;

    /**
        A special value which can be passed to ServiceManager::SetSleepTime to avoid sleeping at all.
        @note Skipping the sleep behaves very differently from using a sleep interval of zero.
        Sleeping for zero milliseconds will still yeild the CPU which can limit the rate of your
        program.
    */
    static const efd::UInt32 kNoSleep = 0xFFFFFFFF;

    /**
        Gets the value for the milliseconds to sleep on every tick.
        @return Number of milliseconds to sleep
    */
    inline efd::UInt32 GetSleepTime() const;

    /**
        Sets the amount of time in milliseconds to sleep on every tick.  This limits the
        maximum framerate of the application.  A value of ServiceManager::kNoSleep will result
        in skipping the sleep call completely.

        @param sleeptime Number of milliseconds to sleep on every tick
    */
    inline void SetSleepTime(efd::UInt32 sleeptime);

    /**
        Gets the value for the milliseconds to sleep on every tick if using DeactivatedSleepTime.
        @return Number of milliseconds to sleep
    */
    inline efd::UInt32 GetDeactivatedSleepTime() const;

    /**
        Sets the amount of time in milliseconds to sleep on every tick if using
        DeactivatedSleepTime.  This limits the maximum framerate of the application.  A value of
        ServiceManager::kNoSleep will result in skipping the sleep call completely.

        @param sleeptime Number of milliseconds to sleep on every tick when deactivated
    */
    inline void SetDeactivatedSleepTime(efd::UInt32 sleeptime);

    /**
        Instructs the ServiceManager to start or stop using DeactivatedSleepTime each frame, rather
        than the usual SleepTime.  This is called when the app should yield CPU to other processes
        on the same machine, such as rapid iteration tools.

        @param deactivated True if DeactivatedSleepTime should be used to sleep each frame
    */
    inline void UseDeactivatedSleepTime(bool deactivated);

    /**
        The ProgramType identifies the intent of the application running the ServiceManager.
        By assigning it here we make it available throughout the application.
     */
    enum ProgramType
    {
        kProgType_Invalid,
        kProgType_Client        = 0x00000001,
        kProgType_Server        = 0x00000002,
        kProgType_Tool          = 0x00000004,
    };

    /**
        Set the program type associated with this ServiceManager.  You may set multiple program
        types if your application fills multiple roles.
    */
    void SetProgramType(ProgramType i_pt, bool i_on = true);

    /**
        Return true if this program type is the same is i_pt.
        @note Be aware that multiple program types can be set.
    */
    bool IsProgramType(ProgramType i_pt);

    /**
        Get the VirtualProcessID for this instance of ServiceManager. The VirtualProcessID is
        uniquely identifies this instance of the service manager.

        This function is not valid to be called before SetVirtualProcessID has been called
    */
    efd::UInt32 GetVirtualProcessID() const;

    /**
        Set the VirtualProcessID for this instance of ServiceManager.
    */
    void SetVirtualProcessID( efd::UInt32 procID );

    /**
       Get the VirtualProcessName for this instance of ServiceManager.
       Will return an empty string before SetVirtualProcessID has been called
    */
    const efd::utf8string GetVirtualProcessName() const;

    /**
       Set the VirtualProcessName for this instance of ServiceManager.
    */
    void SetVirtualProcessName( const efd::utf8string procName );

    /**
       Gets a value indicating if the service manager is currently performing a tick.
    */
    efd::Bool IsRunning() const;

    /**
       Registers platform appropriate signal handlers for ctrl-C
       should be called after any scripting runtimes have been registered
    */
    virtual void RegisterSignalHandlers();

protected:

    /// @cond EMERGENT_INTERNAL

    /**
        Create a class that encapsulates the SSID and the smart pointer for the service so that
        we can use a list and the memory and smart pointer will be handled automatically by the
        template list.
    */
    class CSysSer
    {
    public:
        /// Constructor
        inline CSysSer();

        /// Copy constructor
        inline CSysSer( const CSysSer &sysSer);

        /// Destructor
        inline ~CSysSer();

        inline bool operator == (const CSysSer &sysSer);
        inline bool operator != (const CSysSer &sysSer);

        /// Helper to convert OnPreInit from SyncResult to AsyncResult
        inline AsyncResult CallPreInit();

        /// @name Service data
        //@{
        SSID m_idSysSer;
        ServiceState m_state;
        int m_priority;
        ISystemServicePtr m_spSysSer;
        //@}

        /// Used to determine if a service is to be registered or unregistered and it is set
        /// to true register or false to unregister.   It is not used in the list of registered
        /// services.
        bool m_register;

        /// @name Timing data
        //@{
        efd::TimeType m_totalTimeSpent;
        efd::UInt32 m_totalTicks;
        static const efd::UInt32 kTickHistorySize = 20;
        efd::TimeType m_recentTickTimes[kTickHistorySize];
        efd::UInt32 m_insertPoint;
        //@}

        /// Set all data to initial condintion
        inline void ResetTiming();

        /// Store the time taken by the last tick
        inline void StoreLastTickTime( efd::TimeType time );

        /// Retreive the time taken by the last tick
        inline efd::TimeType GetLastTickTime() const;

        /// Calculate the Lifetime Average Seconds Pre Frame.  This is fairly cheap to compute.
        inline efd::TimeType ComputeLifetimeAverageSPF() const;

        /// Calculate the Recent Average Seconds Pre Frame.  This is fairly expensive to compute.
        inline efd::TimeType ComputeRecentAverageSPF() const;
    };

    // Helper class used in LogServiceTiming.  Defined here since local functions are apparently
    // not supported in gcc.  None the less this should be treated as a private function scoped to
    // the LogServiceTiming method.
    class ServiceSorter
    {
    public:
        /// Constructor
        inline ServiceSorter( CSysSer& service );

        /// Operator used to sort the CSysSer instances by lifetime average time per tick
        inline bool operator<( const ServiceSorter& rhs ) const;

        CSysSer* m_pService;
        efd::TimeType m_time;
    };

    /// @endcond

    /**
        RegisterSystemService actually registers the service by adding it to the
        list of registered services and calling its init method.

        @param spSysSer A pointer to a system service structure containing the
                       information required to register a system service.
    */
    virtual AsyncResult RegisterSystemService(CSysSer& spSysSer);

    /**
        UnregisterSystemService actually unregisters the service by removing it
        from the list of registered services and calling its shutdown method.

        @param spSysSer A pointer to a system service structure containing the
                       information required to find the service to unregister.
    */
    AsyncResult UnregisterSystemService(CSysSer& spSysSer);

    /**
        RegUnRegSystemServices loops through the list of services to register
        or unregister and performs the task.
    */
    virtual void RegUnRegSystemServices();

    /**
        UnregAllSystemServices loops through all the existing services and unregisters them.
    */
    virtual void UnregAllSystemServices();

    /**
        TickServices loops through the list of registered services and ticks
        each service in turn from the highest priority to the lowest priority.
    */
    virtual void TickServices();


    /**
        See if this ServiceManager is active.
        @return bool True if we are currently inside of TickServices.
    */
    inline bool IsActiveState() const;

    /**
        Returns the AsyncResult value that triggered ServiceManager shutdown
     */
    inline efd::AsyncResult GetRunResult() const;

    /// Helper for TickServices.
    /// @return UInt32 Returns 1 if the service is active, or 0 if it is not.
    efd::UInt32 TickService( CSysSer& ptr );

    /**
        Helper for outputting service timing data to the logs.  Writes the following information
        for each registered service:

        * Name - name of the service
        * LifeTime Average Time (LTT): overall average time per tick for the life of the program
        * Recent Average Time (RAT): average time per tick for the last 20 ticks
        * Last Tick Time (LTT): time used on the most recent completed tick
        * Percent Time (%time): percent of total tick time for all services used by this service
        @note These are indicators of possible problems, but might be expected conditions for
            some services.  Indicators currently include:
            - Slow: this service took more than 10ms to tick
            - Hog: this service is more than 50% of total time
            - Spike: this tick took twice as long as average
    */
    void LogServiceTiming();

public:

    /**
    Get the last tick time of a service.
    */
    TimeType GetServiceLastTickTime(SSID idSysSer);

    /**
        Get the map of services SSIDs versus accumulated time in each
        The map is only valid if the ServiceTimeStatType includes SERVICETIME_ACCUMULATE
    */
    const efd::map<SSID, TimeType>& GetAccumulatedServiceTimes();

    /// Reset the map of services SSIDs versus accumulated time in each
    void ResetAccumulatedServiceTimes();

    /// Dump the accumulated service times to the log
    void DumpAccumulatedServiceTimes();

    /// Possible bit values used for SetServiceTimeStatTypes.
    enum ServiceTimeStatType
    {
        SERVICETIME_NONE = 0,
        SERVICETIME_LOG = 1<<0,
        SERVICETIME_ACCUMULATE = 1<<1,
        SERVICETIME_METRICS = 1<<2,
    };

    /**
        Set the types of logging we are doing for services.  Use a logical OR of the values in
        the ServiceTimeStatType enum.
        @note The SERVICETIME_METRICS bit will always be added whenever EE_METRICS_ENABLED is
            defined.
     */
    void SetServiceTimeStatTypes( efd::UInt32 types );

    /// Get the types of logging we are doing for service timing.
    efd::UInt32 GetServiceTimeStatTypes();

protected:
    /// The map of service SSIDs versus accumulated time spent
    efd::map<SSID, TimeType> m_mapAccumulatedServiceTimes;

    /// The section name for retrieving frame rate configuration variables
    const static char* kConfigFramerate;
    /// The key name for the sleep time value
    const static char* kSleepKey;
    /// The key name for the sleep time value when deactivated
    const static char* kDeactivatedSleepKey;

    /**
        Checks the configuration manager for frame rate settings for the ServiceManager.
    */
    void ConfigureFramerate();

private:

    /// @cond EMERGENT_INTERNAL

    /** Pointer used in the HandleSignal method.

        @note This cannot be a smart pointer because it would prevent the ServiceManager from
        ever being freed.  It must be cleared in the destructor of the class so that it is not
        returned again.
        @deprecated this member will be removed once all code that relies on ServiceManager being
         a singleton has been removed
    */
    static ServiceManager* sm_pServiceManager;

    /// A helper used for ctrl-C handling on some platforms
    static void HandleSignal(efd::SInt32 sig);

protected:
    ServiceState m_state;
    bool m_bShutdownRequested;
    bool m_active;

    efd::UInt32 m_programTypes;

    /// @name Used to generate metrics names for per-service tick timing data
    //@{
    char m_tickTimeMetric[256];
    char *m_tickTimeMetricTail;
    //@}

    /// When GetServiceManagerTime exceeds this we know its time to perform a periodic dump of the
    /// service timing information to the logs.
    efd::TimeType m_nextTimingDump;

    typedef efd::list< CSysSer > SystemServiceList;
    SystemServiceList m_regSysSerList;
    SystemServiceList m_sysSerList;

    typedef efd::map<efd::ClassID, IClock*> ClockList;
    ClockList m_clocks;

    efd::StepHighPrecisionClock m_clock;

    AsyncResult m_serviceTickResult;

    efd::UInt32 m_sleepTime;
    efd::UInt32 m_deactivatedSleepTime;
    bool m_useDeactivatedSleepTime;

    /// This is how often we log a detailed time break-down of the running services.
    efd::TimeType m_kTimingDumpInterval;

    /**
        This is the maximum number of Milliseconds that we expect a single Tick to use.
        If a tick takes longer then we will generate a slow-frame warning and log timing data.
    */
    efd::TimeType m_kMaxAcceptableTickTime;

    efd::Bool m_bLogPeriodicServiceTiming;

    /// keeps track of if we are already in a RunOnce call
    bool m_inRunOnce;

    efd::UInt32 m_virtualProcessID;
    efd::utf8string m_virtualProcessName;

    /// The current service time statistics being generated - a logical OR of the
    /// values in the ServiceTimeStatType enum
    efd::UInt32 m_uiServiceTimeStatTypes;

    typedef efd::MessageWrapper< efd::IMessage, kMSGID_StartTick > StartTickMessage;
    efd::SmartPointer< StartTickMessage > m_spStartTickMsg;

    /// @endcond
};

/// A smart pointer for the ServiceManager class
typedef efd::SmartPointer<ServiceManager> ServiceManagerPtr;

} //  namespace efd

#include <efd/ServiceManager.inl>

#endif // EE_SERVICEMANAGER_H

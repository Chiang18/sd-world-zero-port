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
#ifndef EE_ISYSTEMSERVICE_H
#define EE_ISYSTEMSERVICE_H

#include <efd/efdLibType.h>
#include <efd/IBase.h>
#include <efd/Utilities.h>
#include <efd/utf8string.h>
#include <efd/SystemFrameworkEnums.h>
#include <efd/IDs.h>
#include <efd/efdSystemServiceIDs.h>

namespace efd
{

class ServiceManager;

/**

    \brief This is the interface for all system services that the ServiceManager
    will service by allowing it to run once during each framework cycle.
*/
class EE_EFD_ENTRY ISystemService : public efd::IBase
{
    /// @cond EMERGENT_INTERNAL

    EE_DECLARE_INTERFACE1( ISystemService, efd::kCLASSID_ISystemService, efd::IBase );

    /// @endcond

public:

    /// @name Construction and Destruction
    //@{
    /// Default constructor.
    ISystemService();

    /// Virtual destructor.
    virtual ~ISystemService();
    //@}

    /// @name Accessors for ServiceManager
    //@{
    inline ServiceManager* GetServiceManager();
    inline const ServiceManager* GetServiceManager() const;
    inline void SetServiceManager(ServiceManager* pServiceManager);
    //@}

    /// @name Accessors for default priority
    //@{
    inline int GetDefaultPriority() const;
    //@}

    /**
        Called by the framework after the service has been registered and before the service
        receives the call to OnInit.  This allows the service to perform any synchronous setup.
        Although you may query for the existence of other services, for example in order to cache
        a smart pointer to a tightly coupled service, at this point you should not call any methods
        on other services unless the method explicitly states it is safe to call during OnPreInit.
        If the method does not indicate it is safe to call from OnPreInit you should not call
        it since they may not have had a change to setup yet.  Wait until OnInit
        before you rely on cross-service interaction. 
 
        @return SyncResult the service's progress through the initialization state, one of:
                       - SyncResult_Success : The PreInit phase succeeded and this service
                               is ready to be moved to the OnInit queue.
                       - SyncResult_Failure : The PreInit phase failed, this will cause
                               framework to begin shutdown and unloading of all services.
    */
    virtual SyncResult OnPreInit();

    /**
        Called by the framework after the service has had OnPreInit called and before the service
        receives its first OnTick.  This allows the service to perform any initialization that
        it requires.  During the OnInit phase it should be safe or call methods on other services
        since all services will have had a chance to be pre-initialized already.  This phase is
        primarily intended for establishing cross-service relationships such as the registering
        of message handlers.  The OnInit phase is asynchronous meaning that you can stay in this
        phase for multiple ticks.  Once a service returns AsyncResult_Complete from OnInit on the
        next tick OnTick will be called even if not all of the other services have completed
        their OnInit.
        @return AsyncResult the service's progress through the initialization state, one of:
                      - AsyncResult_Pending : More initialization work is required.  OnInit will
                              be called again until it completes or fails.
                      - AsyncResult_Failure : Initialization has critically failed.  The entire
                              framework will be shut down and the process will exit.
                      - AsyncResult_Complete : Initialization has completed for this service.
                              It will receive no more OnInit calls.
    */
    virtual AsyncResult OnInit();


    /**
        Called by the framework every cycle to allow the service to do its work.  The service
        should perform a small slice of work and return as quickly as possible.  This is where
        your application will spend the vast majority of its lifetime.
        @return AsyncResult the service's progress through the running state, one of:
                      - AsyncResult_Pending : This is the normal result, your service is still
                              running and will continue to receive future OnTick calls.
                      - AsyncResult_Failure : The service has critically failed.  The entire
                              framework will be shut down and the process will exit.
                      - AsyncResult_Complete : The service no longer requires OnTick calls.
                              It will remain registered and might still have things like message
                              handlers that are active but it will not receive future OnTick calls.
    */
    virtual AsyncResult OnTick();


    /**
        Called by the framework after the framework shutdown has been requested.  This allows
        the service to perform any clean up that it requires.  The service should cleanly shutdown
        which might involve persisting various state.  If the service is unregistered while the
        framework is running it should be careful not to spend too much time in a single block
        and should instead do a small amount of work on each call while returning "pending" to
        request additional calls.
        @return AsyncResult the service's progress through the initialization state, one of:
                      - AsyncResult_Pending : More shutdown work is required.  OnShutdown will
                              be called again until it completes or fails.
                      - AsyncResult_Failure : Shutdown has critically failed.  The framework
                              shutdown state will be aborted and the process will exit immediately.
                      - AsyncResult_Complete : Shutdown has completed for this service.
                              It will receive no more OnShutdown calls.
    */
    virtual AsyncResult OnShutdown();

    /**
        Returns the name of this service.  This name is used only for improving the readability
        of various log statements and has no impact on functionality.  Overriding this method
        in derived services is completely optional but highly recommended.

        @return const char* Display name for the current service.
    */
    virtual const char* GetDisplayName() const;

protected:
    /// Pointer to the ServiceManager this ISystemService is being run from
    ServiceManager* m_pServiceManager;

    /// Default priority for the service (typically set by the constructor of derived classes)
    int m_defaultPriority;
};

/// A SmartPointer for the ISystemService class
typedef efd::SmartPointer<ISystemService> ISystemServicePtr;

} //  end namespace efd

#include <efd/ISystemService.inl>

#endif // EE_ISYSTEMSERVICE_H

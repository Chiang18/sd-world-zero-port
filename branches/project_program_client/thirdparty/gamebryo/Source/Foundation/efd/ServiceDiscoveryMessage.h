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
#ifndef __ServiceDiscoveryMessage_h__
#define __ServiceDiscoveryMessage_h__

#include <efd/IMessage.h>
#include <efd/efdMessageIDs.h>
#include <efd/Category.h>


namespace efd
{

    /*! \class ServiceDiscoveryRequest
    *
    * \brief Sent to a category that the service is know to subscribe to (i.e. a global well known
    * channel) in order to discover the private channels of the individual services.
    */
    class EE_EFD_ENTRY ServiceDiscoveryRequest : public IMessage
    {
        EE_DECLARE_CLASS1( ServiceDiscoveryRequest, kMSGID_ServiceDiscoveryRequest, IMessage );
        EE_DECLARE_CONCRETE_REFCOUNT;

    public:
        ServiceDiscoveryRequest();

        /**
            Create a service discovery request.  Send this to a category that one or more services
            are expected to be listening on and everyone who receives the message will reply with
            their service ClassID and a private channel that can be used to communicate with that
            particular instance of the service.

            @note: There is no way to tell if no services of that type exist since our message
                system can't tell you if message delivery fails due to no target subscribers.
                The best you can do is to wait for a while and assume that if no one has replied
                then no services of that type exist.

            @param i_userData Arbitrary data that will be returned in the response message
            @param i_callback The category to which responses should be sent
        */
        ServiceDiscoveryRequest( efd::UInt32 i_userData, efd::Category i_callback );

        efd::UInt32 GetContext() const
        {
            return m_context;
        }

        efd::Category GetCallbackChannel() const
        {
            return m_respondTo;
        }

        //@{
        /// to/from stream functions defined in IStreamable
        virtual void ToStream( efd::DataStream &strm ) const;
        virtual bool FromStream( const efd::DataStream &strm );
        //@}

    protected:
        efd::UInt32 m_context;
        efd::Category m_respondTo;
    };

    /*! \typedef ServiceDiscoveryRequestPtr
    * A smart pointer (reference counting, self deleting pointer) for the ServiceDiscoveryRequest class
    */
    typedef efd::SmartPointer<ServiceDiscoveryRequest> ServiceDiscoveryRequestPtr;


    /*! \class ServiceDiscoveryResponse
    *
    * \brief Response to the ServiceDiscoveryRequest message which identifies the message sender's
    * private channel.
    */
    class EE_EFD_ENTRY ServiceDiscoveryResponse : public IMessage
    {
        EE_DECLARE_CLASS1( ServiceDiscoveryResponse, kMSGID_ServiceDiscoveryResponse, IMessage );
        EE_DECLARE_CONCRETE_REFCOUNT;

    public:
        ServiceDiscoveryResponse();

        ServiceDiscoveryResponse( efd::SSID i_discoveredService,
            efd::UInt32 i_context,
            efd::Category i_discoveredChannel );

        efd::SSID GetServiceType() const
        {
            return m_discoveredService;
        }

        efd::UInt32 GetContext() const
        {
            return m_context;
        }

        efd::Category GetServicePrivateChannel() const
        {
            return m_servicePrivateChannel;
        }

        // to/from stream functions defined in IStreamable
        virtual void ToStream( efd::DataStream &strm ) const;

        virtual bool FromStream( const efd::DataStream &strm );

    protected:
        efd::SSID m_discoveredService;
        efd::UInt32 m_context;
        efd::Category m_servicePrivateChannel;
    };

    /*! \typedef ServiceDiscoveryResponsePtr
    * A smart pointer (reference counting, self deleting pointer) for the ServiceDiscoveryResponse class
    */
    typedef efd::SmartPointer<ServiceDiscoveryResponse> ServiceDiscoveryResponsePtr;



} // end namespace efd

#endif // __ServiceDiscoveryMessage_h__

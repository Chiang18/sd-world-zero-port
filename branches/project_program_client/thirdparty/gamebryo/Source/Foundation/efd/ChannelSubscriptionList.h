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
#ifndef EE_CHANNELSUBSCRIPTIONLIST_H
#define EE_CHANNELSUBSCRIPTIONLIST_H
#include <efd/Category.h>
#include <efd/NetEnums.h>
#include <efd/efdMessageIDs.h>

namespace efd
{

/// @cond EMERGENT_INTERNAL

/// struct for holding ChannelManager configuration information
class ChannelSubscription : public efd::MemObject
{
    EE_DECLARE_CONCRETE_REFCOUNT;
public:
    ChannelSubscription(
        efd::Category category = kCAT_INVALID,
        efd::UInt32 netID = kNetID_Unassigned,
        efd::QualityOfService qualityOfService = efd::QOS_INVALID,
        efd::UInt32 subscriptionType = efd::kMSGID_SubscribeExternal)
        : m_category(category)
        , m_netID(netID)
        , m_qualityOfService(qualityOfService)
        , m_subscriptionType(subscriptionType)
    {
    }

    ChannelSubscription(const ChannelSubscription& rhs)
        : m_category(rhs.m_category)
        , m_netID(rhs.m_netID)
        , m_qualityOfService(rhs.m_qualityOfService)
        , m_subscriptionType(rhs.m_subscriptionType)
    {
    }

    ChannelSubscription& operator=(const ChannelSubscription& rhs)
    {
        m_category = rhs.m_category;
        m_netID = rhs.m_netID;
        m_qualityOfService = rhs.m_qualityOfService;
        m_subscriptionType = rhs.m_subscriptionType;
        return *this;
    }

    efd::Category m_category;
    efd::UInt32 m_netID;
    efd::QualityOfService m_qualityOfService;
    efd::UInt32 m_subscriptionType;
};

typedef efd::RefCountedMemObj< efd::list< ChannelSubscription > > ChannelSubscriptionList;
typedef efd::SmartPointer< ChannelSubscriptionList > ChannelSubscriptionListPtr;

/// @endcond

} //end namespace efd

#endif //EE_CHANNELSUBSCRIPTIONLIST_H

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
// Emergent Game Technologies, Calabasas, California 91302
// http://www.emergent.net

#pragma once
#ifndef EE_SERVERTOCLIENTMESSAGE_H
#define EE_SERVERTOCLIENTMESSAGE_H

#include <efd/StreamMessage.h>
#include <efd/NetMessage.h>
#include <efd/Category.h>
#include <efd/efdMessageIDs.h>

#include <efdNetwork/efdNetworkLibType.h>

namespace efd
{
/**
    A wrapper envelope message that can be passed to the client.
    The intermediary (e.g. the Gateway) doesn't really care about the payload
    in this instance. We envelope the payload and set the username as the 'address'
    of the recipient. The gateway passes the payload directly to the client
    with the given address.
 */

class EE_EFDNETWORK_ENTRY ServerToClientMessage : public efd::EnvelopeMessage
{
    EE_DECLARE_CLASS1(
        ServerToClientMessage, 
        efd::kMSGID_ServerToClientMessage, 
        efd::EnvelopeMessage);
    EE_DECLARE_CONCRETE_REFCOUNT;

public:
    ServerToClientMessage();
    virtual ~ServerToClientMessage();

    /**
        @param username the name of the user who's client we want to forward the message.
        @param payload going to the client.
        @param targetCategory the target category for this message. (e.g. the gateway category)
        @param callback the callback category used to send a response to the caller.
     */
    ServerToClientMessage(const efd::utf8string& username,
                          efd::IMessagePtr payload,
                          const efd::Category& targetCategory,
                          const efd::Category& callback = efd::kCAT_INVALID);

    // convert this class into a stream of atomic types.
    virtual void ToStream(efd::DataStream& strm) const;

    // populate this class with data from the stream
    virtual bool FromStream(const efd::DataStream& strm);

    efd::utf8string m_username;
    efd::Category m_callback;
    efd::Category m_target;

};

} // end namespace efd

#endif // EE_SERVERTOCLIENTMESSAGE_H

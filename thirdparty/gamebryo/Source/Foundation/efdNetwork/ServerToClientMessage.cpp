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

#include "efdNetworkPCH.h"

#include <efdNetwork/ServerToClientMessage.h>


//-------------------------------------------------------------------------------------------------
using namespace efd;

//-------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(ServerToClientMessage);
//-------------------------------------------------------------------------------------------------


//-------------------------------------------------------------------------------------------------
ServerToClientMessage::ServerToClientMessage()
   : efd::EnvelopeMessage()
   , m_username("")
   , m_callback(efd::kCAT_INVALID)
   , m_target(efd::kCAT_INVALID)
{
}


//-------------------------------------------------------------------------------------------------
ServerToClientMessage::ServerToClientMessage(
    const efd::utf8string& username,
    efd::IMessagePtr payload,
    const efd::Category& targetCategory,
    const efd::Category& callback)
    : efd::EnvelopeMessage(payload, targetCategory)
    , m_username(username)
    , m_callback(callback)
    , m_target(targetCategory)
{
}


//-------------------------------------------------------------------------------------------------
ServerToClientMessage::~ServerToClientMessage()
{
}


//-------------------------------------------------------------------------------------------------
void ServerToClientMessage::ToStream(efd::DataStream& strm) const
{
    EnvelopeMessage::ToStream(strm);
    strm << m_target;
    strm << m_username;
    strm << m_callback;
}


//-------------------------------------------------------------------------------------------------
bool ServerToClientMessage::FromStream(const efd::DataStream& strm)
{
    EnvelopeMessage::FromStream(strm);
    strm >> m_target;
    strm >> m_username;
    strm >> m_callback;
    return true;
}


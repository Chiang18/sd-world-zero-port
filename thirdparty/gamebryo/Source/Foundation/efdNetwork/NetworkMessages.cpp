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

#include "efdNetwork/NetworkMessages.h"


//-------------------------------------------------------------------------------------------------
using namespace efd;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(NetworkStatus);

//-------------------------------------------------------------------------------------------------
NetworkStatus::~NetworkStatus()
{
}
//-------------------------------------------------------------------------------------------------
void NetworkStatus::ToStream(efd::DataStream& strm) const
{
    efd::UInt32 qos = m_qos;
    IMessage::ToStream(strm);
    strm << qos;
    strm << m_cid;
    strm << m_netId;
}
//-------------------------------------------------------------------------------------------------
bool NetworkStatus::FromStream(const efd::DataStream& strm)
{
    efd::UInt32 qos = m_qos;
    IMessage::FromStream(strm);
    strm >> qos;
    strm >> m_cid;
    strm >> m_netId;
    m_qos = (efd::QualityOfService)qos;
    return true;
}

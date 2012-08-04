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

namespace efd
{
//-------------------------------------------------------------------------------------------------
inline const char* NetService::GetDisplayName() const
{
    return "NetService";
}
//-------------------------------------------------------------------------------------------------
inline efd::UInt32 NetService::GetChannelManagerNetID()
{ 
    return m_channelManagerNetID;
}
//-------------------------------------------------------------------------------------------------
inline ConnectionStatus NetService::GetConnectionStatus()
{
    return m_connectionStatus;
}
//-------------------------------------------------------------------------------------------------
inline void NetService::SetMessageService(MessageService* pMessageService) 
{
    m_spMessageService = pMessageService;
}
//-------------------------------------------------------------------------------------------------
inline INetLib* NetService::GetNetLib()
{
    return m_spNetLib;
}
//-------------------------------------------------------------------------------------------------
template< efd::UInt32 MessageID >
inline void NetService::SendConnectionUpdate(
    efd::MessageService* pMessageService,
    const ConnectionID& cid)
{
    EE_ASSERT(pMessageService);
    StreamMessagePtr spMsg = EE_NEW MessageWrapper< StreamMessage, MessageID >;
    EE_ASSERT(spMsg);
    *spMsg << cid;

    EE_LOG(efd::kNetMessage, efd::ILogger::kLVL1,
        ("NetService notifying connection of %s", cid.ToString().c_str()));
    pMessageService->SendLocal(spMsg, kCAT_LocalMessage);
}

}   // End namespace efd

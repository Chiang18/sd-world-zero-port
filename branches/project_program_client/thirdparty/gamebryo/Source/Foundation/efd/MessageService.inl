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
inline const char* MessageService::GetDisplayName() const
{
    return "MessageService";
}

//-------------------------------------------------------------------------------------------------
inline void MessageService::SendImmediate(const IMessage* pMessage)
{
   SendImmediate(pMessage, pMessage->GetDestinationCategory());
}

//-------------------------------------------------------------------------------------------------
inline void MessageService::SendLocal(const IMessage* pMessage)
{
    SendLocal(pMessage, pMessage->GetDestinationCategory());
}

//-------------------------------------------------------------------------------------------------
inline void MessageService::SendLocalDelayed(const IMessage* pMessage, efd::TimeType fDelay)
{
    SendLocalDelayed(pMessage, fDelay, pMessage->GetDestinationCategory());
}

//-------------------------------------------------------------------------------------------------
inline void MessageService::SendRemote(IMessage* pMessage)
{
    SendRemote(pMessage, pMessage->GetDestinationCategory());
}

//-------------------------------------------------------------------------------------------------
inline void MessageService::Send(const IMessage* pMessage)
{
    Send(pMessage,pMessage->GetDestinationCategory());
}

//-------------------------------------------------------------------------------------------------

}; // namespace efd


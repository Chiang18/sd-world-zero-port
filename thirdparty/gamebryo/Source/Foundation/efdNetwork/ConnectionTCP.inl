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
inline void ConnectionTCP::SetConnectionCallback(INetCallback* pConnectionCallback)
{
    m_pConnectionCallback = pConnectionCallback;
    m_spTCPSocket->SetCallback( pConnectionCallback );
}

//-------------------------------------------------------------------------------------------------
inline INetCallback* ConnectionTCP::GetConnectionCallback()
{
    return m_pConnectionCallback;
}

//-------------------------------------------------------------------------------------------------
inline void ConnectionTCP::SetMessageCallback( INetCallback* pMessageCallback )
{
    m_pMessageCallback = pMessageCallback;
}

//-------------------------------------------------------------------------------------------------
inline INetCallback* ConnectionTCP::GetMessageCallback()
{
    return m_pMessageCallback;
}


} //End namespace efd

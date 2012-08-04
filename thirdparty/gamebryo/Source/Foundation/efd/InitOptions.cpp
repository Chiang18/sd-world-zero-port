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

#include "efdPCH.h"

#include <efd/InitOptions.h>

using namespace efd;
//-------------------------------------------------------------------------------------------------
InitOptions::InitOptions()
{
#if defined (EE_PLATFORM_PS3)

#if defined(EE_EFD_CONFIG_SHIPPING)
    m_initSpuPrintServer = false;
#else
    m_initSpuPrintServer = true;
#endif

    m_pSpursInstance = NULL;
#endif
}
//-------------------------------------------------------------------------------------------------
InitOptions::~InitOptions()
{
}
//-------------------------------------------------------------------------------------------------
#if defined (EE_PLATFORM_PS3)
bool InitOptions::GetInitSpuPrintServer() const
{
    return m_initSpuPrintServer;
}
//-------------------------------------------------------------------------------------------------
void InitOptions::SetInitSpuPrintServer(bool initSpuPrintServer)
{
    m_initSpuPrintServer = initSpuPrintServer;
}
//-------------------------------------------------------------------------------------------------
SpursInstance* InitOptions::GetSpursInstance() const
{
    return m_pSpursInstance;
}
//-------------------------------------------------------------------------------------------------
SpursInstance* InitOptions::SetSpursInstance(SpursInstance* pInstance)
{
    SpursInstance* pOldInstance = m_pSpursInstance;
    m_pSpursInstance = pInstance;
    return pOldInstance;
}
//-------------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------------

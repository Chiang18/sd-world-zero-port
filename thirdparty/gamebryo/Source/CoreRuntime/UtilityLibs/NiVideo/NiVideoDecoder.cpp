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
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net


#include "NiVideoPCH.h" // Precompiled Header
#include "NiVideoDecoder.h"

//---------------------------------------------------------------------------
NiVideoDecoder::NiVideoDecoder(NiRenderer*, Method eMethod,
    unsigned int uiFlags, char* pszVideoFile, bool bPreLoadAll)
{
    m_eMethod = eMethod;
    m_uiFlags = uiFlags;

    if (pszVideoFile)
    {
        size_t stLen = strlen(pszVideoFile) + 1;
        m_pszVideoFile = NiAlloc(char, stLen);
        NIASSERT(m_pszVideoFile);
        NiStrcpy(m_pszVideoFile, stLen, pszVideoFile);
    }
    else
    {
        m_pszVideoFile = 0;
    }

    m_bPreLoadAll = bPreLoadAll;
    
    m_pkCallback = 0;
    m_eStatus = UNKNOWN;
    m_fLastUpdate = 0.0f;
    m_bFrameSync = true;
    m_uiCurrentFrame = 0;
    m_uiTotalFrames = 0;
    m_eLastError = ERR_OK;
}
//---------------------------------------------------------------------------
NiVideoDecoder::~NiVideoDecoder()
{
    NiFree(m_pszVideoFile);
    m_pszVideoFile = 0;
}
//---------------------------------------------------------------------------
NiVideoDecoder* NiVideoDecoder::Create(NiRenderer* pkRenderer, 
    Method eMethod, unsigned int uiFlags, char* pszVideoFile, bool bPreLoadAll)
{
    NiVideoDecoder* pkVideoDecoder = NiNew NiVideoDecoder(pkRenderer, eMethod, 
        uiFlags, pszVideoFile, bPreLoadAll);

    return pkVideoDecoder;
}
//---------------------------------------------------------------------------
bool NiVideoDecoder::ProcessVideoFrame(float, NiTexture*)
{
    return false;   // Base class defaults to an error state.
}
//---------------------------------------------------------------------------
NiTexture* NiVideoDecoder::CreateVideoTexture(NiTexture::FormatPrefs&)
{
    return 0;   // Base class does nothing.
}
//---------------------------------------------------------------------------
unsigned int NiVideoDecoder::SetupFlags(bool bUseOverlay, 
    bool bUseBorderColor, bool bThreaded, bool bUseScaling, 
    unsigned int uiScaleFactor)
{
    unsigned int uiFlags = 0;

    if (bUseOverlay)
        uiFlags |= USE_OVERLAY;
    if (bUseBorderColor)
        uiFlags |= USE_BORDERCOLOR;
    if (bThreaded)
        uiFlags |= THREADED;

    if (bUseScaling)
    {
        uiFlags |= SCALED;

        if (uiScaleFactor > 100)
            uiScaleFactor = 100;
        if (uiScaleFactor == 0)
            uiScaleFactor = 1;
        
        uiFlags |= uiScaleFactor;
    }

    return uiFlags;
}
//---------------------------------------------------------------------------
bool NiVideoDecoder::IsOverlayAvailable(NiRenderer*)
{
    return false;
}
//---------------------------------------------------------------------------

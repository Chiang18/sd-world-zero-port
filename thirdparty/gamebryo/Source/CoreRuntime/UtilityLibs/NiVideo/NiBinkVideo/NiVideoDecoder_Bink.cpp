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
#include <NiDynamicTexture.h>
#include <NiRenderedTexture.h>
#include <NiRenderTargetGroup.h>
#include "NiVideoDecoder_Bink.h"

//---------------------------------------------------------------------------
NiVideoDecoder_Bink::NiVideoDecoder_Bink(NiRenderer* pkRenderer, 
    Method eMethod, unsigned int uiFlags, char* pszVideoFile,
    bool bPreLoadAll) :
    NiVideoDecoder(pkRenderer, eMethod, uiFlags, pszVideoFile, bPreLoadAll)
{
    m_hBink = 0;

#if defined(_XENON)
    XenonInit();
#elif defined(_PS3)
    PS3Init();
#elif defined(_WII)
    WiiInit();
#endif

    if (!OpenBinkFile(bPreLoadAll))
    {
        m_eLastError = ERR_INVALIDFILE;
        NIASSERT(!"Failed to open bink file!");
    }
}
//---------------------------------------------------------------------------
NiVideoDecoder_Bink::~NiVideoDecoder_Bink()
{
#if defined(_PS3) || defined(_WII)
    NiDelete m_pkRenderTargetGroup;
#endif
    CloseBinkFile();
}
//---------------------------------------------------------------------------
NiVideoDecoder* NiVideoDecoder_Bink::Create(NiRenderer* pkRenderer, 
    Method eMethod, unsigned int uiFlags, char* pszVideoFile, bool bPreLoadAll)
{
    NiVideoDecoder_Bink* pkBinkDecoder = NiNew NiVideoDecoder_Bink(pkRenderer, 
        eMethod, uiFlags, pszVideoFile, bPreLoadAll);

    if (pkBinkDecoder)
    {
        NIASSERT(eMethod == ASTEXTURE);   // The app must request a texture.
    }

    return (NiVideoDecoder*)pkBinkDecoder;
}
//---------------------------------------------------------------------------
bool NiVideoDecoder_Bink::ProcessVideoFrame(float fTime,
    NiTexture* pkVideoTexture)
{
    // Support video frame playback as a texture only.
    NIASSERT(m_eMethod == ASTEXTURE);

    NIASSERT(m_hBink != NULL);   // Check the handle.

    m_eStatus = PLAYING;
    m_fLastUpdate = fTime;

    // Wait, so the rendered frame will be 'synced' to the video frame rate.
    if (m_bFrameSync && BinkWait(m_hBink) == 1)
            return false;

#if defined(_XENON)
    // RenderResolveBinkFrame_Xenon encapsulates BinkDoFrame because we must
    // first potentially stall on the GPU.
    RenderResolveBinkFrame_Xenon(pkVideoTexture);
#elif defined(_PS3)
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);
    pkRenderer->BeginOffScreenFrame();
    pkRenderer->BeginUsingRenderTargetGroup(m_pkRenderTargetGroup, 
        NiRenderer::CLEAR_ALL); 

    RenderBinkFrame_PS3(pkVideoTexture);

    pkRenderer->EndUsingRenderTargetGroup();
    pkRenderer->EndOffScreenFrame();
#elif defined(_WII)
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);
    pkRenderer->BeginOffScreenFrame();
    pkRenderer->BeginUsingRenderTargetGroup(
        m_pkRenderTargetGroup, 
        NiRenderer::CLEAR_ALL); 

    RenderBinkFrame_Wii();

    pkRenderer->EndUsingRenderTargetGroup();
    pkRenderer->EndOffScreenFrame();
#else
    NiDynamicTexture* pkDynVideoTexture = (NiDynamicTexture*)pkVideoTexture;

    if (BinkDoFrame(m_hBink) == 0)  // Decompress a Bink frame.
    {
        // The frame was not skipped, so lock the dynamic texture's buffer so
        // the decompressed frame may be copied into it.
        int iPitch = 0;
        void* pvBuffer = (pkDynVideoTexture->Lock(iPitch));

        if (pvBuffer != NULL)
        {
            // Copy the decompressed frame into the video texture's buffer.
            BinkCopyToBuffer(m_hBink, pvBuffer, iPitch,
                pkDynVideoTexture->GetHeight(), 0, 0,
                BINKSURFACE32 | BINKCOPYALL);

            // Unlock the dynamic texture's buffer.
            pkDynVideoTexture->UnLock();   
        }
    }
#endif

    // Advance to the next frame, even if last processed frame was skipped.
    m_uiCurrentFrame++;
    BinkNextFrame(m_hBink);

    // Adjust current frame count if video loops.
    if ((m_hBink->Frames == m_uiCurrentFrame) && (m_uiFlags & LOOPING))
        m_uiCurrentFrame = 0;

    return true;
}
//---------------------------------------------------------------------------
NiTexture* NiVideoDecoder_Bink::CreateVideoTexture(
    NiTexture::FormatPrefs& kPrefs)
{
    if (m_hBink == 0)   // Make sure that the Bink handle is valid.
        return NULL;

    // A 'video texture' is not available for ASMOVIE mode.
    if ((m_eMethod == ASMOVIE) && (m_uiFlags & THREADED))
        return NULL;

    // Use the video width and height to create the texture.
    unsigned int uiBinkWidth = m_hBink->Width;
    unsigned int uiBinkHeight = m_hBink->Height;

#if defined(_PS3) || (_WII)
    // Allocate rendered texture
     NiRenderedTexture* pkVideoTexture = NiRenderedTexture::Create(
        uiBinkWidth, uiBinkHeight, NiRenderer::GetRenderer());
    NIASSERT(pkVideoTexture && "Failed to create video texture.");

    // Create a render target group and assign texture
    m_pkRenderTargetGroup = NiRenderTargetGroup::Create(
        pkVideoTexture->GetBuffer(), NiRenderer::GetRenderer(), 
        true, true);
    NIASSERT(m_pkRenderTargetGroup && "Failed to create render target group.");
#else
    // Try to get a format that matches the source for top speed.
    NiDynamicTexture* pkVideoTexture =
        NiDynamicTexture::Create(uiBinkWidth, uiBinkHeight, kPrefs
#if defined(_XENON) || defined(_WII)
        // If this is Xenon or Wii then we need a tiled texture for GPU usage.
        , true
#endif
        );
#endif
    return pkVideoTexture;
}
//---------------------------------------------------------------------------
bool NiVideoDecoder_Bink::OpenBinkFile(bool bPreLoadAll)
{
    if (!m_pszVideoFile || (strcmp(m_pszVideoFile, "") == 0))
    {
        // There was not a valid file!
        NiOutputDebugString("OpenBinkFile> No valid video file name!\n");
        return false;
    }

#if defined(_PS3)
    // Tell Bink to play all of the 7.1 tracks
    U32 TrackIDsToPlay[ 5 ] = { 0, 1, 2, 3, 4 };
    BinkSetSoundTrack( 5, TrackIDsToPlay );
#endif    
    // Open the Bink file and hold onto the HBINK handle.  
    // Preload the entire animation into memory, if specified.
    unsigned int uiBinkFlags = (bPreLoadAll) ? 
        (BINKPRELOADALL | BINKNOSKIP) : (BINKNOSKIP);
#if defined(_XENON)
    uiBinkFlags |= BINKNOFRAMEBUFFERS;
#elif defined(_PS3)
    uiBinkFlags |= BINKSNDTRACK;
#elif defined(_WII)
    uiBinkFlags |= BINKNOFRAMEBUFFERS;
#endif
    m_hBink = BinkOpen(m_pszVideoFile, uiBinkFlags);
    if (m_hBink == 0)   // If Bink is unable to preload the animation into
        return false;   // memory, then the open will fail.

#if defined(_XENON)
    AllocateBinkResources_Xenon();
#elif defined(_PS3)
    AllocateBinkResources_PS3();
#elif defined(_WII)
    AllocateBinkResources_Wii();
#endif
    m_uiTotalFrames = m_hBink->Frames;
    m_eStatus = READYTOPLAY;

    return true;
}
//---------------------------------------------------------------------------
bool NiVideoDecoder_Bink::CloseBinkFile()
{
    BinkClose(m_hBink);

#if defined(_XENON)
    ReleaseBinkResources_Xenon();
#elif defined(_PS3)
    ReleaseBinkResources_PS3();
#elif defined(_WII)
    ReleaseBinkResources_Wii();
#endif

    return true;
}
//---------------------------------------------------------------------------
void* NiVideoDecoder_Bink::NiMallocProxy(unsigned int uiSizeInBytes)
{
    return NiMalloc2(uiSizeInBytes,
        NiMemHint::IS_BULK | NiMemHint::USAGE_VIDEO);
}
//---------------------------------------------------------------------------
void NiVideoDecoder_Bink::NiFreeProxy(void* pvMem)
{
    NiFree(pvMem);
}
//---------------------------------------------------------------------------

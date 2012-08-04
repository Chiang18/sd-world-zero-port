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


#ifndef NIVIDEODECODER_BINK_H
#define NIVIDEODECODER_BINK_H


#include "NiVideoDecoder.h"
#if defined (_PS3) || defined(_WII)
#include <NiMeshScreenElements.h>
#include <NiTexturingProperty.h>
#include <NiDynamicTexture.h>
#endif

#if defined (_WII)
#include <NiWiiShader.h>
#endif

#if defined (_PS3)
#include <NiPS3GPUResourceManager.h>

class NiPS3FragmentShader;
class NiRenderTargetGroup;
#endif

#define RAD_NO_LOWERCASE_TYPES
#include <Bink.h>

class NIVIDEO_ENTRY NiVideoDecoder_Bink : public NiVideoDecoder
{
protected:
    NiVideoDecoder_Bink(NiRenderer* pkRenderer, Method eMethod, 
        unsigned int uiFlags, char* pszVideoFile, bool bPreLoadAll);

public:
    virtual ~NiVideoDecoder_Bink();

    static NiVideoDecoder* Create(NiRenderer* pkRenderer, 
        Method eMethod, unsigned int uiFlags, 
        char* pszVideoFile, bool bPreLoadAll);

    // Texture-based playback
    virtual bool ProcessVideoFrame(float fTime,
        NiTexture* pkVideoTexture);
    virtual NiTexture* CreateVideoTexture(
        NiTexture::FormatPrefs& kPrefs);

protected:
    bool OpenBinkFile(bool bPreLoadAll);
    bool CloseBinkFile();

#if defined(_XENON)
    void RenderResolveBinkFrame_Xenon(NiTexture* pkTexture);
    bool AllocateBinkResources_Xenon();
    void ReleaseBinkResources_Xenon();

    bool AllocateTextures_Xenon();
    bool AllocateShaders_Xenon();
    bool AllocateSurface_Xenon(NiTexture* pkTexture);

    void XenonInit();

    bool MakeTexture(unsigned int uiWidth, unsigned int uiHeight,
        D3DFORMAT format, IDirect3DTexture9** ppkTexture, void ** ppvBuffer, 
        unsigned int* uiOutPitch, unsigned int* uiOutSize);

    void WaitForTextures();
    void FlushTextures();
    void DrawAndResolve(NiTexture* pkTexture);
    void CacheFlush(void* pvBase, unsigned int uiLength);

    BINKFRAMEBUFFERS* m_pkBinkFrameBuffers;

    IDirect3DVertexShader9* m_pkVertexShader;
    IDirect3DPixelShader9* m_pkPixelShader;

    IDirect3DSurface9* m_pkSurface;

    unsigned int m_uiYSize;
    unsigned int m_uiCSize;

    IDirect3DTexture9* m_apkYTextures[BINKMAXFRAMEBUFFERS];
    IDirect3DTexture9* m_apkCbTextures[BINKMAXFRAMEBUFFERS];
    IDirect3DTexture9* m_apkCrTextures[BINKMAXFRAMEBUFFERS];

    IDirect3DVertexDeclaration9* m_pkDecl;

#elif defined(_PS3)

    // Initializes PS data
    void PS3Init();

    // Allocates resources
    bool AllocateBinkResources_PS3();

    // Release resources
    void ReleaseBinkResources_PS3();

    // Allocates textures for video rendering
    bool AllocateTextures_PS3();

    // Allocates shaders for video rendering 
    bool AllocateShaders_PS3();
  
    // Makes a texture from input parameters
    NiDynamicTexture* MakeTexture(unsigned int uiWidth,
        unsigned int uiHeight);

    // Renders a frame of video to the specified texture
    void RenderBinkFrame_PS3(NiTexture* pkTexture);
    
    // Draws a frame of video to the texture
    void Draw(NiTexture* pkTexture);
    
    // Waits for rendering to complete
    void WaitForTextures();

    // Flushes the textures
    void FlushTextures();

    // Flushes a buffer
    void CacheFlush(void* pvBase, unsigned int uiLength);
    
    // Initializes audio on the cell
    void InitAudio();

    // Members
    NiRenderTargetGroup*    m_pkRenderTargetGroup;  // Offscreen buffer

    // this is the Bink info on the Y Cr Cb planes
    BINKFRAMEBUFFERS        m_kBinkFrameBuffers;

    // Frame results
    NiDynamicTexturePtr m_aspYTextures[BINKMAXFRAMEBUFFERS];
    NiDynamicTexturePtr m_aspCbTextures[BINKMAXFRAMEBUFFERS];
    NiDynamicTexturePtr m_aspCrTextures[BINKMAXFRAMEBUFFERS];

    // Synchronization labels
    NiPS3GPULabelPtr m_aspGPULabels[BINKMAXFRAMEBUFFERS];

    // Full screen quad used for color conversion
    NiMeshScreenElementsPtr m_spScreenPoly;
    NiTexturingPropertyPtr m_spTextureProperty;

    // Color conversion shader and parameters
    NiPS3FragmentShader* m_pkColorConversionShader;

    float m_afAdjustValues[4];
    float m_fAlphaPassthrough;

#elif defined(_WII)

    // Initializes Wii data
    void WiiInit();

    // Allocates resources
    bool AllocateBinkResources_Wii();

    // Release resources
    void ReleaseBinkResources_Wii();

    // Allocates textures for video rendering
    bool AllocateTextures_Wii();

    // Allocates Wii shaders for video rendering 
    bool AllocateShaders_Wii();

    // Makes a texture from input parameters
    NiDynamicTexture* MakeTexture(
        const unsigned int uiWidth,
        const unsigned int uiHeight,
        const bool bSwizzler);

    // Renders a frame of video to the current render target
    void RenderBinkFrame_Wii();
    
    // Draws a frame of video to the current render target
    void Draw();
    
    // Waits for rendering to complete
    void WaitForTextures();

    // Flushes the textures
    void FlushTextures();

    // Initializes audio on the cell
    void InitAudio();

    // Create a swizzler texture
    void CreateSwizzleTexture(
        NiDynamicTexturePtr& spDynTexture,
        const unsigned int uiWidth);

    // Calculates the width of the swizzler textures
    unsigned int CalcSwizzlerWidth(
        const unsigned int iSourceWidth);

    // Fill the offset buffer with the lookup values
    void FillOffsetBuffer(
        unsigned char* pucBuffer,
        const unsigned int uiSourceWidth, 
        const unsigned int uiSwizzWidth);

    // Swizzle the offset buffer 
    void SwizzleBuffer(
        unsigned char* pucSwizzData, 
        unsigned char const* pucBuffer, 
        const unsigned int uiSourceWidth,
        const unsigned int uiSourceHeight, 
        const unsigned int uiBytesPerPixel);

    // Members
    NiRenderTargetGroup* m_pkRenderTargetGroup;  // Offscreen buffer

    // this is the Bink info on the Y Cr Cb A planes
    BINKFRAMEBUFFERS m_kBinkFrameBuffers;

    // Frame results
    NiDynamicTexturePtr m_aspYTextures[BINKMAXFRAMEBUFFERS];
    NiDynamicTexturePtr m_aspCbTextures[BINKMAXFRAMEBUFFERS];
    NiDynamicTexturePtr m_aspCrTextures[BINKMAXFRAMEBUFFERS];
    NiDynamicTexturePtr m_aspATextures[BINKMAXFRAMEBUFFERS];

    // Deswizzle textures
    NiDynamicTexturePtr m_spYASwizzleTexture;
    NiDynamicTexturePtr m_spCrCbSwizzleTexture;

    // Full screen quad used for color conversion
    NiMeshScreenElementsPtr m_spScreenPoly;
    NiTexturingPropertyPtr m_spTextureProperty;

    // Color conversion Wii shader
    NiWiiShaderPtr m_pkColorConversionShader;
#endif

    // Bink requires function pointers if we want to use Gamebryo
    // memory management, but NiMalloc and NiFree are macros.
    // We proxy the macros here so we have something to pass to Bink.
    static void* NiMallocProxy(unsigned int uiSizeInBytes);
    static void NiFreeProxy(void* pvMem);

    HBINK m_hBink;      // Handle to the bink file
};

#endif  //#ifndef NIVIDEODECODER_BINK_H

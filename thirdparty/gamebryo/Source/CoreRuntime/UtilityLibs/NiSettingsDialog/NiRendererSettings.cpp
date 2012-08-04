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

#include "NiSettingsDialogPCH.h"
#include "NiRendererSettings.h"

#include <efd/IConfigManager.h>
#include <efd/IConfigSection.h>
#include <efd/ParseHelper.h>

using namespace efd;

const char* NiRendererSettings::ms_pcSectionName = "Renderer.Win32";
const char* NiRendererSettings::ms_pcScreenWidth = "ScreenWidth";
const char* NiRendererSettings::ms_pcScreenHeight = "ScreenHeight";
const char* NiRendererSettings::ms_pcMinScreenWidth = "MinScreenWidth";
const char* NiRendererSettings::ms_pcMinScreenHeight = "MinScreenHeight";
const char* NiRendererSettings::ms_pcVertexProcessing = "VertexProcessing";
const char* NiRendererSettings::ms_pcDX9RenderTargetMode = "DX9RenderTargetMode";
const char* NiRendererSettings::ms_pcDX9DepthSurfaceMode = "DX9DepthSurfaceMode";
const char* NiRendererSettings::ms_pcDX9FrameBufferMode = "DX9FrameBufferMode";
const char* NiRendererSettings::ms_pcD3D10OutputIdx = "D3D10OutputIdx";
const char* NiRendererSettings::ms_pcD3D10MultisampleCount = "D3D10MultisampleCount";
const char* NiRendererSettings::ms_pcD3D10MultisampleQuality = "D3D10MultisampleQuality";
const char* NiRendererSettings::ms_pcD3D10DSFormat = "D3D10DSFormat";
const char* NiRendererSettings::ms_pcD3D10RTFormat = "D3D10RTFormat";
const char* NiRendererSettings::ms_pcD3D10Renderer = "UseD3D10Renderer";
const char* NiRendererSettings::ms_pcFullscreen = "Fullscreen";
const char* NiRendererSettings::ms_pcNVPerfHUD = "NVPerfHUD";
const char* NiRendererSettings::ms_pcRefRast = "RefRast";
const char* NiRendererSettings::ms_pcUse16Bit = "Use16Bit";
const char* NiRendererSettings::ms_pcPureDevice = "PureDevice";
const char* NiRendererSettings::ms_pcVSync = "VSync";
const char* NiRendererSettings::ms_pcMultiThread = "MultiThread";
const char* NiRendererSettings::ms_pcRendererDialog = "RendererDialog";
const char* NiRendererSettings::ms_pcSaveSettings = "SaveSettings";

//---------------------------------------------------------------------------
// Constructor - here all settings are initialized to default values
//---------------------------------------------------------------------------
NiRendererSettings::NiRendererSettings() :
    // Default settings. Saved settings and command line parameters
    // will override them.
    m_uiScreenWidth(0),
    m_uiScreenHeight(0),
    m_uiMinScreenWidth(640),
    m_uiMinScreenHeight(480),
    m_bD3D10Renderer(false),
    m_bFullscreen(false),
    m_bNVPerfHUD(false),
    m_bRefRast(false),
    m_bUse16Bit(false),
    m_bPureDevice(true),
    m_bVSync(false),
    m_bStencil(false),
    m_bMultiThread(false),
    m_bRendererDialog(true),
    m_bSaveSettings(false),
    m_uiAdapterIdx(0),
    m_uiNVPerfHUDAdapterIdx(0),
    m_eVertexProcessing(VERTEX_HARDWARE),
    m_eDX9RTFormat(NiDX9Renderer::FBFMT_X8R8G8B8),
    m_eDX9DSFormat(NiDX9Renderer::DSFMT_D24S8),
    m_eDX9FBFormat(NiDX9Renderer::FBMODE_DEFAULT),
    m_uiD3D10OutputIdx(0),
    m_uiD3D10MultisampleCount(1),
    m_uiD3D10MultisampleQuality(0),
    m_eD3D10DSFormat(DXGI_FORMAT_D24_UNORM_S8_UINT),
    m_eD3D10RTFormat(DXGI_FORMAT_R8G8B8A8_UNORM)
{

}
//---------------------------------------------------------------------------
// Settings loading / saving functions
//---------------------------------------------------------------------------
bool NiRendererSettings::ReadConfig(
    const ISection* pkSection,
    const char* pcValueName,
    unsigned int& uiVal)
{
    NIASSERT(pkSection && pcValueName);

    const efd::utf8string& kValue = pkSection->FindValue(pcValueName);
    UInt32 uiTemp = 0;
    if (!efd::ParseHelper<efd::UInt32>::FromString(kValue, uiTemp))
        return false;

    uiVal = uiTemp;
    return true;
}
//---------------------------------------------------------------------------
bool NiRendererSettings::ReadConfig(
    const ISection* pkSection,
    const char* pcValueName,
    bool& bVal)
{
    NIASSERT(pkSection && pcValueName);

    const efd::utf8string& kValue = pkSection->FindValue(pcValueName);
    bool bTemp = false;

    if (!efd::ParseHelper<efd::Bool>::FromString(kValue, bTemp))
        return false;

    bVal = bTemp;
    return true;
}
//---------------------------------------------------------------------------
void NiRendererSettings::LoadSettings(const char* pcFileName)
{
    ReadUInt(pcFileName, ms_pcScreenWidth, m_uiScreenWidth);
    ReadUInt(pcFileName, ms_pcScreenHeight, m_uiScreenHeight);
    ReadUInt(pcFileName, ms_pcMinScreenWidth, m_uiMinScreenWidth);
    ReadUInt(pcFileName, ms_pcMinScreenHeight, m_uiMinScreenHeight);
    ReadUInt(pcFileName, 
        ms_pcVertexProcessing,
        (unsigned int&)(m_eVertexProcessing));
    ReadUInt(pcFileName, 
        ms_pcDX9RenderTargetMode,
        (unsigned int&)(m_eDX9RTFormat));
    ReadUInt(pcFileName, 
        ms_pcDX9DepthSurfaceMode,
        (unsigned int&)(m_eDX9DSFormat));
    ReadUInt(pcFileName, 
        ms_pcDX9FrameBufferMode,
        (unsigned int&)(m_eDX9FBFormat));
    ReadUInt(pcFileName, ms_pcD3D10OutputIdx,
        m_uiD3D10OutputIdx);
    ReadUInt(pcFileName, 
        ms_pcD3D10MultisampleCount,
        m_uiD3D10MultisampleCount);
    ReadUInt(pcFileName, 
        ms_pcD3D10MultisampleQuality,
        m_uiD3D10MultisampleQuality);
    ReadUInt(pcFileName, 
        ms_pcD3D10DSFormat,
        (unsigned int&)(m_eD3D10DSFormat));
    ReadUInt(pcFileName, 
        ms_pcD3D10RTFormat,
        (unsigned int&)(m_eD3D10RTFormat));

    ReadBool(pcFileName, ms_pcD3D10Renderer, m_bD3D10Renderer);
    ReadBool(pcFileName, ms_pcFullscreen, m_bFullscreen);
    ReadBool(pcFileName, ms_pcNVPerfHUD, m_bNVPerfHUD);
    ReadBool(pcFileName, ms_pcRefRast, m_bRefRast);
    ReadBool(pcFileName, ms_pcUse16Bit, m_bUse16Bit);
    ReadBool(pcFileName, ms_pcPureDevice, m_bPureDevice);
    ReadBool(pcFileName, ms_pcVSync, m_bVSync);
    ReadBool(pcFileName, ms_pcMultiThread, m_bMultiThread);
    ReadBool(pcFileName, ms_pcRendererDialog, m_bRendererDialog);
    // Don't read SaveSettings from file - it always defaults to false.
    m_bSaveSettings = false;
}
//---------------------------------------------------------------------------
void NiRendererSettings::SaveSettings(const char* pcFileName)
{
    WriteUInt(pcFileName, ms_pcScreenWidth, m_uiScreenWidth);
    WriteUInt(pcFileName, ms_pcScreenHeight, m_uiScreenHeight);
    WriteUInt(pcFileName, ms_pcMinScreenWidth, m_uiMinScreenWidth);
    WriteUInt(pcFileName, ms_pcMinScreenHeight, m_uiMinScreenHeight);
    WriteUInt(pcFileName, ms_pcVertexProcessing, m_eVertexProcessing);
    WriteUInt(pcFileName, ms_pcDX9RenderTargetMode, m_eDX9RTFormat);
    WriteUInt(pcFileName, ms_pcDX9DepthSurfaceMode, m_eDX9DSFormat);
    WriteUInt(pcFileName, ms_pcDX9FrameBufferMode, m_eDX9FBFormat);
    WriteUInt(pcFileName, ms_pcD3D10OutputIdx, m_uiD3D10OutputIdx);
    WriteUInt(pcFileName, 
        ms_pcD3D10MultisampleCount,
        m_uiD3D10MultisampleCount);
    WriteUInt(pcFileName, 
        ms_pcD3D10MultisampleQuality,
        m_uiD3D10MultisampleQuality);
    WriteUInt(pcFileName, ms_pcD3D10DSFormat, m_eD3D10DSFormat);
    WriteUInt(pcFileName, ms_pcD3D10RTFormat, m_eD3D10RTFormat);

    WriteBool(pcFileName, ms_pcD3D10Renderer, m_bD3D10Renderer);
    WriteBool(pcFileName, ms_pcFullscreen, m_bFullscreen);
    WriteBool(pcFileName, ms_pcNVPerfHUD, m_bNVPerfHUD);
    WriteBool(pcFileName, ms_pcRefRast, m_bRefRast);
    WriteBool(pcFileName, ms_pcUse16Bit, m_bUse16Bit);
    WriteBool(pcFileName, ms_pcPureDevice, m_bPureDevice);
    WriteBool(pcFileName, ms_pcVSync, m_bVSync);
    WriteBool(pcFileName, ms_pcMultiThread, m_bMultiThread);
    WriteBool(pcFileName, ms_pcRendererDialog, m_bRendererDialog);
}
//---------------------------------------------------------------------------
void NiRendererSettings::LoadFromConfigManager(IConfigManager* pkConfigManager)
{
    NIASSERT(pkConfigManager);
    if (!pkConfigManager)
        return;

    const ISection* pkSection = 
        pkConfigManager->GetConfiguration()->FindSection(ms_pcSectionName);
    if (!pkSection)
        return;

    ReadConfig(pkSection, ms_pcScreenWidth, m_uiScreenWidth);
    ReadConfig(pkSection, ms_pcScreenHeight, m_uiScreenHeight);
    ReadConfig(pkSection, ms_pcMinScreenWidth, m_uiMinScreenWidth);
    ReadConfig(pkSection, ms_pcMinScreenHeight, m_uiMinScreenHeight);
    ReadConfig(pkSection, 
        ms_pcVertexProcessing,
        (unsigned int&)(m_eVertexProcessing));
    ReadConfig(pkSection, 
        ms_pcDX9RenderTargetMode,
        (unsigned int&)(m_eDX9RTFormat));
    ReadConfig(pkSection, 
        ms_pcDX9DepthSurfaceMode,
        (unsigned int&)(m_eDX9DSFormat));
    ReadConfig(pkSection, 
        ms_pcDX9FrameBufferMode,
        (unsigned int&)(m_eDX9FBFormat));
    ReadConfig(pkSection, ms_pcD3D10OutputIdx,
        m_uiD3D10OutputIdx);
    ReadConfig(pkSection, 
        ms_pcD3D10MultisampleCount,
        m_uiD3D10MultisampleCount);
    ReadConfig(pkSection, 
        ms_pcD3D10MultisampleQuality,
        m_uiD3D10MultisampleQuality);
    ReadConfig(pkSection, 
        ms_pcD3D10DSFormat,
        (unsigned int&)(m_eD3D10DSFormat));
    ReadConfig(pkSection, 
        ms_pcD3D10RTFormat,
        (unsigned int&)(m_eD3D10RTFormat));

    ReadConfig(pkSection, ms_pcD3D10Renderer, m_bD3D10Renderer);
    ReadConfig(pkSection, ms_pcFullscreen, m_bFullscreen);
    ReadConfig(pkSection, ms_pcNVPerfHUD, m_bNVPerfHUD);
    ReadConfig(pkSection, ms_pcRefRast, m_bRefRast);
    ReadConfig(pkSection, ms_pcUse16Bit, m_bUse16Bit);
    ReadConfig(pkSection, ms_pcPureDevice, m_bPureDevice);
    ReadConfig(pkSection, ms_pcVSync, m_bVSync);
    ReadConfig(pkSection, ms_pcMultiThread, m_bMultiThread);
    ReadConfig(pkSection, ms_pcRendererDialog, m_bRendererDialog);
    // Don't read SaveSettings from file - it always defaults to false.
    m_bSaveSettings = false;
}
//---------------------------------------------------------------------------

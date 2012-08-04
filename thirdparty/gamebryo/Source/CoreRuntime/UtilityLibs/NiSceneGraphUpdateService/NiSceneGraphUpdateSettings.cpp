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

#include "NiSceneGraphUpdateServicePCH.h"

#include "NiSceneGraphUpdateSettings.h"
#include "NiMath.h"
#include "NiMemStream.h"


//--------------------------------------------------------------------------------------------------
NiSceneGraphUpdateSettings::NiSceneGraphUpdateSettings()
{
    m_fPlaybackStartTimeInSec = 0.0f;
    m_fPlaybackStopTimeInSec = NI_INFINITY;
    m_ePlaybackMode = SGU_PLAYBACK_ONCE;
    m_bRestartPlaybackAfterEdit = false;
    m_bSimulatePastEndTime = true;
    m_bRunUpParticlesAfterEdit = true;
    m_bAddDefaultLights = false;
    m_bViewerHideUI = false;    
}
//--------------------------------------------------------------------------------------------------
NiSceneGraphUpdateSettings::~NiSceneGraphUpdateSettings()
{
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdateSettings::Load(const NiUInt32 uiBufferSize, const char* pcBuffer)
{
    NiMemStream kMemStream(pcBuffer, uiBufferSize);

    // If this platform is big endian, swap the bytes since the buffer is always saved in little
    // endian to make it more efficient in the most common case of running both the editor and
    // viewer on a PC. The viewer on the console will swap the buffer from little to big endian.
    bool bSwapBytes = !NiSystemDesc::GetSystemDesc().IsLittleEndian();
    kMemStream.SetEndianSwap(bSwapBytes);

    NiUInt32 uiPlaybackMode;
    NiStreamLoadBinary(kMemStream, uiPlaybackMode);
    m_ePlaybackMode = (PlaybackMode)uiPlaybackMode;

    NiStreamLoadBinary(kMemStream, m_fPlaybackStartTimeInSec);
    NiStreamLoadBinary(kMemStream, m_fPlaybackStopTimeInSec);

    NiStreamLoadBinary(kMemStream, m_bRestartPlaybackAfterEdit);
    NiStreamLoadBinary(kMemStream, m_bSimulatePastEndTime);
    NiStreamLoadBinary(kMemStream, m_bRunUpParticlesAfterEdit);

    NiStreamLoadBinary(kMemStream, m_bAddDefaultLights);
    NiStreamLoadBinary(kMemStream, m_bViewerHideUI);
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdateSettings::Save(NiUInt32& uiBufferSize, char*& pcBuffer)
{
    NiMemStream kMemStream;

    // If this platform is big endian, swap the bytes since the buffer is always saved in little
    // endian to make it more efficient in the most common case of running both the editor and
    // viewer on a PC. The viewer on the console will swap the buffer from little to big endian.
    bool bSwapBytes = !NiSystemDesc::GetSystemDesc().IsLittleEndian();
    kMemStream.SetEndianSwap(bSwapBytes);

    NiUInt32 uiPlaybackMode = m_ePlaybackMode;
    NiStreamSaveBinary(kMemStream, uiPlaybackMode);

    NiStreamSaveBinary(kMemStream, m_fPlaybackStartTimeInSec);
    NiStreamSaveBinary(kMemStream, m_fPlaybackStopTimeInSec);

    NiStreamSaveBinary(kMemStream, m_bRestartPlaybackAfterEdit);
    NiStreamSaveBinary(kMemStream, m_bSimulatePastEndTime);
    NiStreamSaveBinary(kMemStream, m_bRunUpParticlesAfterEdit);

    NiStreamSaveBinary(kMemStream, m_bAddDefaultLights);
    NiStreamSaveBinary(kMemStream, m_bViewerHideUI);

    uiBufferSize = kMemStream.GetSize();
    pcBuffer = (char*) kMemStream.Str();
}
//--------------------------------------------------------------------------------------------------

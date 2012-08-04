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

#ifndef NITERRAINSHADOWCLICKVALIDATOR_H
#define NITERRAINSHADOWCLICKVALIDATOR_H

#include "NiShadowClickValidator.h"

class NITERRAIN_ENTRY NiTerrainShadowClickValidator: public NiShadowClickValidator
{
    NiDeclareRTTI;

public:

    /// Returns whether or not to execute the specified render click for the
    /// specified frame.
    virtual bool ValidateClick(NiRenderClick* pkRenderClick,
        unsigned int uiFrameID);

    /// Simple factory method for creating a NiTerrainShadowClickValidator. 
    /// This method is registered as a NiShadowManager::CreateShadowRenderClickValidator
    /// call back.
    static NiShadowClickValidator* CreateTerrainShadowClickValidator();
};

#endif // NITERRAINSHADOWCLICKVALIDATOR_H
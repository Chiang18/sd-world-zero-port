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

#pragma once
#ifndef NITERRAINXMLHELPERS_H
#define NITERRAINXMLHELPERS_H

#include "NiTerrainLibType.h"
#include "NiPoint2.h"

#include <efd/TinyXML.h>

/** 
    Helper class to assist in parsing TinyXML documents for terrain
 */
class NITERRAIN_ENTRY NiTerrainXMLHelpers 
{
public:

    /// Helper to wrap loading the TinyXML document on multiple platforms
    static bool LoadXMLFile(efd::TiXmlDocument* pkXMLDocument);

    /// Helper to write the XML syntax header
    static bool WriteXMLHeader(efd::TiXmlDocument* pkDoc);

    static efd::TiXmlElement* CreateElement(const char* pcName, 
        efd::TiXmlElement* pkParentElement);

    /// Helper to load a float
    static bool ReadElement(efd::TiXmlElement* pkRootElement, const char* pcName,
        float& fData);

    /// Helper to load an int
    static bool ReadElement(efd::TiXmlElement* pkRootElement,  const char* pcName,
        int& iData);

    /// Helper to load an NiPoint2
    static bool ReadElement(efd::TiXmlElement* pkRootElement, const char* pcName,
        NiPoint2& kData);

    /// Helper to load a string
    static bool ReadElement(efd::TiXmlElement* pkRootElement,  const char* pcName,
        const char*& pcData);

    /// Helper to write a float
    static bool WriteElement(efd::TiXmlElement* pkRootElement,  const char* pcName, 
        float fData);

    /// Helper to write an integer
    static bool WriteElement(efd::TiXmlElement* pkRootElement,  const char* pcName, 
        int iData);

    /// Helper to write an NiPoint2
    static bool WriteElement(efd::TiXmlElement* pkRootElement,  const char* pcName, 
        const NiPoint2& kData);

    /// Helper to write a string
    static bool WriteElement(efd::TiXmlElement* pkRootElement,  const char* pcName, 
        const char* pcData);

    /// Helper to write a string
    static bool WritePrimitive(efd::TiXmlElement* pkElement, 
        const char* pcData);
};

#endif // NITERRAINXMLHELPERS_H

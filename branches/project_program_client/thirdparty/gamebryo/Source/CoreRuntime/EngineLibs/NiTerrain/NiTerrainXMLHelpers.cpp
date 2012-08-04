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

#include "NiTerrainPCH.h"

#include "NiTerrainXMLHelpers.h"
#include <NiSystem.h>

using namespace efd;

//---------------------------------------------------------------------------
bool NiTerrainXMLHelpers::LoadXMLFile(TiXmlDocument* pkXMLDocument)
{
    NIASSERT(pkXMLDocument);
#if defined(_WII)
    // TinyXML uses fopen to load files, which doesn't work
    // on the Wii.  Instead we need to use NiFile to open the file
    // and then pass the data to the TinyXML Parse function.
    NiFile* pkFile = NiFile::GetFile(pkXMLDocument->Value(), NiFile::READ_ONLY);
    if (!pkFile)
        return false;

    unsigned int uiSize = pkFile->GetFileSize();
    char* pBuffer = (char*)NiMalloc(uiSize + 1);
    pkFile->Read(pBuffer, uiSize);
    pBuffer[uiSize] = 0;
    bool bSuccess = (pkXMLDocument->LoadBuffer(pBuffer) != 0);
    NiFree(pBuffer);
    NiDelete pkFile;
    return bSuccess;
#else
    return pkXMLDocument->LoadFile();
#endif
}
//---------------------------------------------------------------------------
bool NiTerrainXMLHelpers::ReadElement(TiXmlElement* pkRootElement, 
    const char* pcName, NiPoint2& kData)
{
    TiXmlElement* pkCurrentSection = pkRootElement->FirstChildElement(pcName);
    if (!pkCurrentSection)
        return false;

    const char* pcData = pkCurrentSection->GetText();
#if defined(_MSC_VER) && _MSC_VER >= 1400
    int iFieldsAssigned =
        sscanf_s(pcData, "%f, %f", &kData.x, &kData.y);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    int iFieldsAssigned =
        sscanf(pcData, "%f, %f", &kData.x, &kData.y);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400
    if (iFieldsAssigned != 2)
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainXMLHelpers::ReadElement(TiXmlElement* pkRootElement, 
    const char* pcName, float& fData)
{
    TiXmlElement* pkCurrentSection = pkRootElement->FirstChildElement(pcName);
    if (!pkCurrentSection)
        return false;

    const char* pcData = pkCurrentSection->GetText();
#if defined(_MSC_VER) && _MSC_VER >= 1400
    int iFieldsAssigned = sscanf_s(pcData, "%f", &fData);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    int iFieldsAssigned = sscanf(pcData, "%f", &fData);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

    if (iFieldsAssigned != 1)
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainXMLHelpers::ReadElement(TiXmlElement* pkRootElement, 
    const char* pcName, int& iData)
{
    TiXmlElement* pkCurrentSection = pkRootElement->FirstChildElement(pcName);
    if (!pkCurrentSection)
        return false;

    const char* pcData = pkCurrentSection->GetText();
#if defined(_MSC_VER) && _MSC_VER >= 1400
    int iFieldsAssigned = sscanf_s(pcData, "%d", &iData);
#else //#if defined(_MSC_VER) && _MSC_VER >= 1400
    int iFieldsAssigned = sscanf(pcData, "%d", &iData);
#endif //#if defined(_MSC_VER) && _MSC_VER >= 1400

    if (iFieldsAssigned != 1)
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainXMLHelpers::WriteXMLHeader(TiXmlDocument* pkDoc)
{
    pkDoc->LinkEndChild(NiExternalNew TiXmlDeclaration( "1.0", "", "" ));
    return true;
}
//---------------------------------------------------------------------------
TiXmlElement* NiTerrainXMLHelpers::CreateElement(const char* pcName, 
    TiXmlElement* pkParentElement)
{
    TiXmlElement* pkElement = NiExternalNew TiXmlElement(pcName);
    if (pkParentElement)
        pkParentElement->LinkEndChild(pkElement);
    return pkElement;
}
//---------------------------------------------------------------------------
bool NiTerrainXMLHelpers::ReadElement(TiXmlElement* pkRootElement, 
    const char* pcName, const char*& pcData)
{    
    TiXmlElement* pkCurrentSection = pkRootElement->FirstChildElement(pcName);
    if (!pkCurrentSection)
        return false;

    pcData = pkCurrentSection->GetText();
    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainXMLHelpers::WriteElement(TiXmlElement* pkRootElement,
    const char* pcName, float fData)
{
    char acBuf[256];
    NiSprintf(acBuf, 256, "%.16f", fData);

    TiXmlElement* pkCurrentElement = NiExternalNew TiXmlElement(pcName);
    pkCurrentElement->LinkEndChild(NiExternalNew TiXmlText(acBuf));
    pkRootElement->LinkEndChild(pkCurrentElement);
    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainXMLHelpers::WriteElement(TiXmlElement* pkRootElement, 
    const char* pcName, int iData)
{
    char acBuf[256];
    NiSprintf(acBuf, 256, "%d", iData);
    
    TiXmlElement* pkCurrentElement = NiExternalNew TiXmlElement(pcName);
    pkCurrentElement->LinkEndChild(NiExternalNew TiXmlText(acBuf));
    pkRootElement->LinkEndChild(pkCurrentElement);
    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainXMLHelpers::WriteElement(TiXmlElement* pkRootElement, 
    const char* pcName, const NiPoint2& kData)
{
    char acBuf[256];
    NiSprintf(acBuf, 256, "%.16f, %.16f", kData.x, kData.y);

    TiXmlElement* pkCurrentElement = NiExternalNew TiXmlElement(pcName);
    pkCurrentElement->LinkEndChild(NiExternalNew TiXmlText(acBuf));
    pkRootElement->LinkEndChild(pkCurrentElement);

    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainXMLHelpers::WriteElement(TiXmlElement* pkRootElement, 
    const char* pcName, const char* pcData)
{
    if (pcData == NULL)
        pcData = "";

    TiXmlElement* pkCurrentElement = NiExternalNew TiXmlElement(pcName);
    pkCurrentElement->LinkEndChild(NiExternalNew TiXmlText(pcData));
    pkRootElement->LinkEndChild(pkCurrentElement);
    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainXMLHelpers::WritePrimitive(TiXmlElement* pkElement, 
    const char* pcData)
{
    if (pcData == NULL)
        pcData = "";
    pkElement->LinkEndChild(NiExternalNew TiXmlText(pcData));
    return true;
}
//---------------------------------------------------------------------------

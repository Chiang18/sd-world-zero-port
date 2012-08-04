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

#include "NiSceneGraphUpdateObject.h"
#include "NiSceneGraphUpdateStream.h"
#include "NiSceneGraphUpdate.h"

#include <NiCloningProcess.h>

//--------------------------------------------------------------------------------------------------
NiSceneGraphUpdateObject::NiSceneGraphUpdateObject(const NiSceneGraphUpdateObjectId& kId)
{
    m_kId = kId;
}
//--------------------------------------------------------------------------------------------------
NiSceneGraphUpdateObject::~NiSceneGraphUpdateObject()
{
    SetObject(NULL);
}
//--------------------------------------------------------------------------------------------------
void NiSceneGraphUpdateObject::SetObject(NiObject* pkObject)
{
    // If we are going to be deleting this object we need to handle it.
    if (!pkObject && m_spObject->GetRefCount() == 1)
    {
        m_spObject = pkObject;
        NiSceneGraphUpdate::GetInstance()->CleanupObjects();
    }
    else
    {
        m_spObject = pkObject;
    }
}
//--------------------------------------------------------------------------------------------------
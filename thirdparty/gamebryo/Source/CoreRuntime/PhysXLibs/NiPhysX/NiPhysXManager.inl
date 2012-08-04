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

// NiPhysXManager inline functions

//---------------------------------------------------------------------------
inline NiPhysXManager* NiPhysXManager::GetPhysXManager()
{
    return ms_pPhysXManager;
}
//---------------------------------------------------------------------------
inline void NiPhysXManager::WaitSDKLock()
{
    efdPhysX::PhysXSDKManager::GetManager()->WaitSDKLock();
}
//---------------------------------------------------------------------------
inline void NiPhysXManager::ReleaseSDKLock()
{
    efdPhysX::PhysXSDKManager::GetManager()->ReleaseSDKLock();

}
//---------------------------------------------------------------------------

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
// Emergent Game Technologies, Calabasas, CA 91302
// http://www.emergent.net

//-------------------------------------------------------------------------------------------------
efd::IAllocator* efd::CreateDefaultGlobalMemoryAllocator()
{
    efd::IAllocator* pkAllocator = NULL;

#ifdef EE_MEMORY_DEBUGGER
    pkAllocator = EE_EXTERNAL_NEW efd::MemTracker(EE_EXTERNAL_NEW efd::StandardAllocator());
#else
    pkAllocator = EE_EXTERNAL_NEW efd::StandardAllocator();
#endif

    return pkAllocator;
}
//-------------------------------------------------------------------------------------------------

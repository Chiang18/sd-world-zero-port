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


#ifndef NIPHYSXCONVEXMESH_H
#define NIPHYSXCONVEXMESH_H

#include <NiMain.h>
#include <NiSystem.h>

#include "NiPhysXLibType.h"

#include "NiPhysXMemStream.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#elif defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

/**
    NiPhysXConvexMesh is a helper class for the creation of PhysX NxConvexMesh
    objects.
    
    This class provides functionality to load a mesh from an external file or
    create a mesh from a memory stream. It also manages sharing of meshes by
    searching for an existing mesh with the same name before creating a new
    mesh.
*/
class NIPHYSX_ENTRY NiPhysXConvexMesh
{
public:
    /**
        Create a NxConvexMesh from a given filename.

        If a mesh with the given name already exists in the cache
        of the efdPhysX::PhysXSDKManager object, the cached mesh is
        returned.

        The given name must be the location of a file containing a cooked
        NxConvexMesh object, typically streamed directly out of NxCookConvexMesh.
        The platform dependent pathname stored in NiPhysXMeshDesc is used to
        resolve the file location. A NiPhysXFileStream object is then created
        from the file and passed to the PhysX SDK to create the mesh. The
        resulting mesh is stored in the efdPhysX::PhysXSDKManager cache and
        returned.
        
        This function returns NULL if anything goes wrong in this process. 
    */
    static NxConvexMesh* Create(const NiFixedString& kFilename);

    /** 
        Create a NxConvexMesh from a given memory buffer.

        If a mesh with the given name already exists in the cache
        of the efdPhysX::PhysXSDKManager object, the cached mesh is
        returned.

        The kStream object must be the result of a call to NxCookConvexMesh.
        Applications will typically use this function if they are cooking a
        PhysX mesh at runtime or in an authoring tool. This memory stream is
        passed directly to the PhysX SDK to create the mesh. The resulting
        mesh is stored in the efdPhysX::PhysXSDKManager cache and returned.
        
        This function returns NULL if anything goes wrong in this process. 
    */
    static NxConvexMesh* Create(NiPhysXMemStream& kStream,
        const NiFixedString& kName);
};

#endif // #ifdef NIPHYSXCONVEXMESH_H


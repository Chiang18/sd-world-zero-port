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

#pragma once
#ifndef _ASSETSERVER_UTILITY_H
#define _ASSETSERVER_UTILITY_H

#include <efd/utf8string.h>

namespace efd
{
    /**
     This class provides additional utility functions needed by the asset-web.
     Currently, that includes support for path value manipulations.
    */
    class EE_EFD_ENTRY AssetUtil
    {
    public:
        /**
         Determine constituent parts of a path name.

         @param pathValue The full path for a file.
         @param directory [out] The directory part.
         @param name [out] The file name part without the extension
         @param extension [out] The file name extension
        */
        static void PathParts (const efd::utf8string& pathValue,
            efd::utf8string& directory,
            efd::utf8string& name,
            efd::utf8string& extension);

    };

} // namespace
#endif


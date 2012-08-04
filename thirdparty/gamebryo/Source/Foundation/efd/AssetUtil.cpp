/****************************************************************************
*
* EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
*
* This software is supplied under the terms of a license agreement or
* nondisclosure agreement with Emergent Game Technologies and may not
* be copied or disclosed except in accordance with the terms of that
* agreement.
*
*      Copyright (c) 1996-2009 Emergent Game Technologies.
*      All Rights Reserved.
*
* Emergent Game Technologies, Calabasas, CA 91302
* http://www.emergent.net
*
******************************************************************************/
#include "efdPCH.h"

#include <efd/AssetUtil.h>
#include <efd/File.h>

using namespace efd;

void AssetUtil::PathParts (const utf8string& pathValue,
                           utf8string& directory,
                           utf8string& name,
                           utf8string& extension)
{
    vector<utf8string> parts;

    pathValue.split("/", parts);
    int sz = parts.size();

    // set directory
    directory.clear();
    for (int x=0; x<(sz-1); x++)
    {
        directory = directory + "/" + parts[x];
    }

    // set the name and extension
    name.clear();
    extension.clear();
    if (sz > 0)
    {
        utf8string full_name = parts[sz-1];
        parts.clear();
        full_name.split (".", parts);
        if (parts.size()==2)
        {
            name = parts[0];
            extension = parts[1];
        }
        else if (parts.size()==1)
        {
            name = parts[0];
        }
    }
}

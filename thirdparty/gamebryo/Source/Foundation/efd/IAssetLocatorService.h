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
#ifndef IASSETLOCATORSERVICE_H
#define IASSETLOCATORSERVICE_H

#include <efd/IDs.h>
#include <efd/ISystemService.h>
#include <efdNetwork/NetCategory.h>
#include <efd/AssetLocatorResponse.h>

namespace efd
{
    /**
      Get the Category used for broadcast messages directed to the AssetLocatorService
    */
    extern EE_EFD_ENTRY const Category& GetAssetClientServiceCategory();

    /**
      Get the private Category used by each client to receive asset fetch responses
    */
    extern EE_EFD_ENTRY Category GetAssetClientPrivateCategory(efd::UInt32 virtualProcessID);

    /**
      Generate a unique Category for use with the AssetLocate() API. All calls to AssetLocate()
      must include the Category that the response will be sent to.  The caller should use this
      API to generate the Category and Subscribe to it before they call AssetLocate().
    */
    extern EE_EFD_ENTRY Category GenerateAssetResponseCategory();

    class EE_EFD_ENTRY IAssetLocatorService : public ISystemService
    {
    public:
        EE_DECLARE_INTERFACE1(IAssetLocatorService, efd::kCLASSID_IAssetLocatorService, ISystemService);

        enum AssetClientDataState
        {
            ASSETS_NONE,                /* Init state. No local metadata opened or remote server connection */
            ASSETS_LOCAL_ONLY,          /* Local metadata successfully opened */
            ASSETS_REMOTE,              /* Remote AssetServer service has connected */
            ASSETS_LOCAL_AND_REMOTE
        };

        enum AssetLookupScope
        {
            ALS_LOCAL_OR_REMOTE,        /* Lookup asset locations on a remote server first, if one
                                        has connected. This is the default scope for AssetLocate() */

            ALS_LOCAL_ONLY,             /* Limit asset location lookups to only local metadata */

            ALS_REMOTE_ONLY             /* Limit asset location lookups by requiring remote metadata
                                        accessed via a connected AssetServer service */
        };

        enum AssetTagCategory
        {
            ASSET_STANDARD_TAGS,       /* Standard tags defined by the asset crawler */
            ASSET_LABEL_TAGS,          /* Custom labels defined by the user */
            ASSET_CLASSIFICATION_TAGS  /* Custom classifications defined by the user */
        };

        /*
        * Client API functions - connection related
        */

        /*
        * Check connection
        */
        virtual bool IsConnected() = 0;

        /*
        * Client API functions - query related
        */

        /*
        * Locate all assets matching the given URI
        *
        * @param    uri A URI value (urn: form)
        * @param    callback Category to receive the response message
        * @param    scope Specify local only or remote only lookup
        * @returns  True if success
        * @see      GetLastStatus()
        * @note     Results will include all matching AssetID / Asset URL
        */
        virtual efd::Bool AssetLocate (efd::utf8string uri,
            efd::Category callback,
            AssetLookupScope scope=ALS_LOCAL_OR_REMOTE) = 0;

    };

    typedef efd::SmartPointer<IAssetLocatorService> IAssetLocatorServicePtr;

} // end namespace efd

#endif //IASSETLOCATORSERVICE_H

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
// Emergent Game Technologies, Calabasas, California 91302
// http://www.emergent.net
#pragma once
#ifndef ASSETCONFIGSERVICE_H
#define ASSETCONFIGSERVICE_H

#include <efd/MessageService.h>
#include <efd/AssetLocatorService.h>

/**
   The AssetConfigService manages the current state of the Asset Locator and allows
   configuration changes to be applied via .ini file, command-line, or command messages.

   Additionally, the AssetConfigService provides a central repository of the current
   configuration for use by other services.
*/
namespace efd
{

class EE_EFD_ENTRY AssetConfigService : public efd::ISystemService
{
    EE_DECLARE_CLASS1(AssetConfigService, efd::kCLASSID_AssetConfigService, efd::ISystemService);
    EE_DECLARE_CONCRETE_REFCOUNT;

public:

    const static char* kConfigAssetWebSection;
    const static char* kConfigAssetLoaderSection;

    //@name Construction and Destruction
    AssetConfigService();
    ~AssetConfigService();

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual efd::AsyncResult OnInit();

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual efd::AsyncResult OnTick();

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    virtual efd::AsyncResult OnShutdown();

    // Overridden virtual functions inherit base documentation and thus are not documented here.
    inline virtual const char* GetDisplayName() const;

    /**
     Open the asset-web at the specified path and use for local lookups.

     This will only affect lookups that are handled by the local process, not lookups
     that are forwarded to a remote Asset Server.

     @param web_root The root path for the asset-web directory
    */
    virtual efd::Bool OpenAssetWebLocal (const efd::utf8string& web_root);

    /**
    Open the asset-web at the specified path and use for remote lookups.

    This will send a request to a connected, remote Asset Server to change the path it uses
    as its asset-web.  Note: An asset is raised if there is no connected, remote server for
    your process.

    @param web_root The root path for the asset-web directory
    */
    efd::Bool OpenAssetWebRemote (const efd::utf8string& web_root);

    /**
     Retrieve the list of queries to test, if any, specified in the Config.ini file.

     These are URI values that appear under [AssetWeb] with the name of "QueryTest#".
     There may be one or more specified in the Config.ini. These are used to test some
     small number of asset lookups against the current metadata at startup.

     @param query List of URIReader objects for each query
    */
    void GetQueryTests (efd::vector<utf8string>& query) const;

    /**
    Retrieve the value of a particular netry in the AssetWeb portion of the configuration.

    @param key Key for the property to be retrieved
    @param value [out] The value of the property
    */
    void GetConfigValue (const efd::utf8string& key, efd::utf8string& value);

    /// Message handler for responses to config changes (server->client)
    void HandleConfigResponse ( const efd::AssetConfigurationMessage* pMessage, efd::Category targetChannel);

    /**
     Gets the asset web root read in the configuration file.
     @return Current asset web root.
    */
    const efd::utf8string& GetAssetWebRoot() const;

protected:

    /// Get local configuration information using the ConfigManager service
    void ReadConfiguration();

    // Root path for the configured asset-web
    efd::utf8string m_asset_web_root;

    // Vector of any configured queries to test
    efd::vector<utf8string> query_tests;

    // convenient pointers to necessary services
    MessageServicePtr m_spMessageService;

private:

    /// Forward a config change response for processing by other listeners
    void SendLocalResponse(const efd::AssetConfigurationMessage* pMessage = NULL);
    efd::AssetLocatorService* m_pAssetLocator;

};

typedef efd::SmartPointer<AssetConfigService> AssetConfigServicePtr;

} // namespace efd

#include "AssetConfigService.inl"

#endif // ASSET_CONFIG_SERVICE

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

#include "efdPCH.h"

#include <efd/ConfigManager.h>
#include <efd/AssetConfigService.h>
#include <efd/IAssetServer.h>
#include <efd/efdLogIDs.h>
#include <efd/MessageHandlerBase.h>
#include <efd/ServiceManager.h>


using namespace efd;

//-------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(AssetConfigService);

EE_HANDLER_WRAP(AssetConfigService, HandleConfigResponse, AssetConfigurationMessage, 
                kMSGID_AssetConfigResponse);

//-------------------------------------------------------------------------------------------------
const /*static*/ char* AssetConfigService::kConfigAssetWebSection = "AssetWeb";
const /*static*/ char* AssetConfigService::kConfigAssetLoaderSection = "AssetLoader";

//-------------------------------------------------------------------------------------------------
AssetConfigService::AssetConfigService()
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 2140;
}

//-------------------------------------------------------------------------------------------------
AssetConfigService::~AssetConfigService()
{
}

//-------------------------------------------------------------------------------------------------
void AssetConfigService::ReadConfiguration()
{
    // Find the config manager service
    IConfigManager* pConfigManager = m_pServiceManager->GetSystemServiceAs<IConfigManager>();
    EE_ASSERT( pConfigManager );

    // Get the root config section
    const ISection* pRoot = pConfigManager->GetConfiguration();

    if (!pRoot)
    {
        EE_LOG(
            efd::kAssets,
            efd::ILogger::kERR0,
            ("AssetConfigService::OnInit could not find root section in config."));
        return;
    }

    // Get the "AssetWeb" config section
    const ISection* pWebSection = pRoot->FindSection(kConfigAssetWebSection);
    if (pWebSection)
    {
        // Store the asset-web path
        m_asset_web_root = pWebSection->FindValue( "Path" );
        if (!m_asset_web_root.empty())
        {
            EE_LOG(
                efd::kAssets,
                efd::ILogger::kLVL2,
                ("AssetConfigService::OnInit configured for Asset Web location: %s",
                m_asset_web_root.c_str()));
        }
        else
        {
            EE_LOG(
                efd::kAssets,
                efd::ILogger::kERR3,
                ("AssetConfigService::OnInit no AssetWeb.Path set. Assets may fail."));
        }

        // Load any tests specified in the configuration
        for ( UInt32 index = 1; ; ++index )
        {
            utf8string strValueName( Formatted, "QueryTest%d", index );
            utf8string strValue = pWebSection->FindValue( strValueName );
            if ( strValue.empty() )
            {
                // Empty value means non-existent, which means we're done
                break;
            }
            else
            {
                query_tests.push_back (strValue);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------------
/* virtual */
efd::AsyncResult AssetConfigService::OnInit()
{
    // we depend on the ConfigManager so we wait for it.
    EE_DEPENDS_ON_SERVICE(IConfigManager);

    // Find the local message service
    m_spMessageService = m_pServiceManager->GetSystemServiceAs<MessageService>();
    EE_ASSERT(m_spMessageService);

    // Find other local asset services
    m_pAssetLocator = m_pServiceManager->GetSystemServiceAs<AssetLocatorService>();

    // Initialize values from Config
    ReadConfiguration();

    if (m_pAssetLocator)
    {
        m_spMessageService->Subscribe(this, GetAssetClientServiceCategory(), QOS_RELIABLE);
    }

    // If there is a local asset service, pass configuration information to it now
    OpenAssetWebLocal (m_asset_web_root);
    if (!m_asset_web_root.empty())
    {
        printf("AssetConfigService: Opening Asset web: %s\n", m_asset_web_root.c_str());
    }

    // Let other local service subscribers know about the change
    AssetConfigurationMessagePtr notify = EE_NEW 
        MessageWrapper< AssetConfigurationMessage, kMSGID_AssetConfigNotify >();
    notify->SetAssetWebPath (m_asset_web_root);
    m_spMessageService->SendLocal (notify);

    return AsyncResult_Complete;
}

//-------------------------------------------------------------------------------------------------
/* virtual */
efd::AsyncResult AssetConfigService::OnTick()
{
    return efd::AsyncResult_Complete;
}

//-------------------------------------------------------------------------------------------------
/* virtual */
efd::AsyncResult AssetConfigService::OnShutdown()
{
    if ( m_spMessageService )
    {
        if (m_pAssetLocator)
        {
            m_spMessageService->Unsubscribe(this, GetAssetClientServiceCategory());
        }
        m_spMessageService = NULL;
    }

    return efd::AsyncResult_Complete;
}

//-------------------------------------------------------------------------------------------------
efd::Bool AssetConfigService::OpenAssetWebLocal (const utf8string& web_root)
{
    efd::Bool retval = false;

    m_asset_web_root = web_root;

    if (m_pAssetLocator && m_asset_web_root.length()>0)
    {
        if (!EE_VERIFY_MESSAGE (m_pAssetLocator->AssetWebOpen (m_asset_web_root),
            ("Failure: Local asset web metadata invalid. Regenerate with Asset Controller.")))
        {
            EE_LOG(
                efd::kAssets,
                efd::ILogger::kERR0,
                ("Local asset web metadata invalid. Regenerate with Asset Controller."));

            m_pServiceManager->Shutdown();
        }

        SendLocalResponse();
    }

    return retval;
}

//-------------------------------------------------------------------------------------------------
efd::Bool AssetConfigService::OpenAssetWebRemote (const utf8string& web_root)
{
    // Must be a client to do this
    EE_ASSERT(m_pAssetLocator);

    AssetConfigurationMessagePtr req = EE_NEW 
        MessageWrapper< AssetConfigurationMessage, kMSGID_AssetConfigRequest >();
    req->SetAssetWebPath (web_root);
    m_spMessageService->Send(req, GetAssetServerServiceCategory());

    return true;
}

//-------------------------------------------------------------------------------------------------
void AssetConfigService::GetQueryTests (efd::vector<utf8string>& queries) const
{
    queries = query_tests;
}

//-------------------------------------------------------------------------------------------------
void AssetConfigService::GetConfigValue(const efd::utf8string& key, efd::utf8string& value)
{
    // Find the config manager service
    IConfigManager* pConfigManager = m_pServiceManager->GetSystemServiceAs<IConfigManager>();
    EE_ASSERT( pConfigManager );

    // Get the root config section
    const ISection* pRoot = pConfigManager->GetConfiguration();

    // Get the key-value
    const ISection* pWebSection = pRoot->FindSection(kConfigAssetWebSection);
    if (pWebSection)
        value = pWebSection->FindValue (key);
}

//-------------------------------------------------------------------------------------------------
void AssetConfigService::HandleConfigResponse ( 
    const efd::AssetConfigurationMessage* pMessage,
    efd::Category targetChannel)
{
    // Only a client can send the request and receive this response
    EE_ASSERT(m_pAssetLocator);

    EE_LOG(efd::kAssets, efd::ILogger::kLVL2,
        ("%s> New Remote AssetWeb Root set: %s", 
        __FUNCTION__, 
         pMessage->GetAssetWebPath().c_str())); 
    SendLocalResponse(pMessage);

    EE_UNUSED_ARG(pMessage);
    EE_UNUSED_ARG(targetChannel);
}

//-------------------------------------------------------------------------------------------------
void AssetConfigService::SendLocalResponse(const efd::AssetConfigurationMessage* pMessage)
{
    AssetConfigurationMessage* pLocalResp = 
        EE_NEW MessageWrapper< AssetConfigurationMessage, kMSGID_AssetConfigResponse >();
    if (pMessage == NULL)
    {
        pLocalResp->SetAssetWebPath (m_asset_web_root);
        pLocalResp->SetRemoteConfigFlag(false);
    }
    else
    {
        pLocalResp->SetAssetWebPath (pMessage->GetAssetWebPath());
        pLocalResp->SetRemoteConfigFlag(true);
    }
    m_spMessageService->SendLocal(pLocalResp);
}

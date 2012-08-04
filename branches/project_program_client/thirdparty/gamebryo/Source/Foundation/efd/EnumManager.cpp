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

#include "efdPCH.h"
#include <efd/EnumManager.h>
#include <efd/DDEParser.h>
#include <efd/IConfigManager.h>
#include <efd/ServiceManager.h>
#include <efd/PathUtils.h>


//-------------------------------------------------------------------------------------------------
EE_IMPLEMENT_CONCRETE_CLASS_INFO(efd::EnumManager);

//-------------------------------------------------------------------------------------------------
efd::EnumManager::EnumManager()
: m_configLoaded( false )
#ifdef EE_PLATFORM_XBOX360
, m_loadPath( "D:\\" )
#else
, m_loadPath( ".\\" )
#endif
{
    // If this default priority is changed, also update the service quick reference documentation
    m_defaultPriority = 2600;
}

//-------------------------------------------------------------------------------------------------
efd::utf8string efd::EnumManager::GetFileForEnum( const efd::utf8string& enumName )
{
    efd::utf8string result = efd::PathUtils::PathCombine( m_loadPath, enumName );
    result = efd::PathUtils::PathAddExtension( result, "enum" );
    return result;
}

//-------------------------------------------------------------------------------------------------
efd::DataDrivenEnumBase* efd::EnumManager::LoadEnum( const efd::utf8string& enumName )
{
    EE_ASSERT( NULL == FindEnum( enumName ) );

    efd::DDEParser parser( this, m_pHeaderGen );
    parser.Parse( enumName, GetFileForEnum(enumName) );
    return FindEnum( enumName );
}

//-------------------------------------------------------------------------------------------------
efd::DataDrivenEnumBase* efd::EnumManager::LoadFile( const efd::utf8string& fileName )
{
    utf8string enumName = efd::PathUtils::PathGetFileName( fileName );
    enumName = efd::PathUtils::PathRemoveFileExtension( enumName );

    EE_ASSERT( NULL == FindEnum( enumName ) );

    efd::DDEParser parser( this, m_pHeaderGen );
    parser.Parse( enumName, fileName );
    return FindEnum( enumName );
}

//-------------------------------------------------------------------------------------------------
efd::DataDrivenEnumBase* efd::EnumManager::FindOrLoadEnum( const efd::utf8string& enumName )
{
    efd::DataDrivenEnumBase* pResult = FindEnum( enumName );
    if ( !pResult )
    {
        pResult = LoadEnum( enumName );
    }
    return pResult;
}

//-------------------------------------------------------------------------------------------------
efd::DataDrivenEnumBase* efd::EnumManager::FindEnum( const efd::utf8string& enumName )
{
    DDEnumMap::iterator iter = m_enums.find( enumName );
    if ( iter != m_enums.end() )
    {
        return iter->second;
    }

    return NULL;
}

//-------------------------------------------------------------------------------------------------
bool efd::EnumManager::AddEnum( efd::DataDrivenEnumBase* pEnum )
{
    DDEnumMap::iterator iter = m_enums.find( pEnum->GetName() );
    if ( iter == m_enums.end() )
    {
        m_enums[pEnum->GetName()] = pEnum;
        return true;
    }

    // Duplicate item
    return false;
}

//-------------------------------------------------------------------------------------------------
void efd::EnumManager::SetHeaderGenerator( efd::IDDEHeaderGenerator* pHeaderGen )
{
    m_pHeaderGen = pHeaderGen;
}

//-------------------------------------------------------------------------------------------------
void efd::EnumManager::LoadConfigOptions( efd::IConfigManager* pConfig )
{
    if ( pConfig->FindValue("Enum.Path", m_loadPath) )
    {
        m_loadPath = efd::PathUtils::PathMakeNative( m_loadPath );
        if ( !efd::PathUtils::GetPlatformSupportsRelativePaths() )
        {
            if (PathUtils::IsRelativePath(m_loadPath))
                m_loadPath = efd::PathUtils::ConvertToAbsolute( m_loadPath );
        }
    }
    m_configLoaded = true;
}

//-------------------------------------------------------------------------------------------------
efd::SyncResult efd::EnumManager::OnPreInit()
{
    if (!m_configLoaded)
    {
        efd::IConfigManager* pConfig =
            m_pServiceManager->GetSystemServiceAs<efd::IConfigManager>();
        LoadConfigOptions( pConfig );
    }
    return efd::SyncResult_Success;
}



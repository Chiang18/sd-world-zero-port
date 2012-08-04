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

#include <efd/AssetLocatorRequest.h>
#include <efd/DataStream.h>
#include <efd/ILogger.h>
#include <efd/efdLogIDs.h>

using namespace efd;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(AssetLocatorRequest);

//-------------------------------------------------------------------------------------------------
AssetLocatorRequest::AssetLocatorRequest()
{
}

//-------------------------------------------------------------------------------------------------
/* virtual */
void AssetLocatorRequest::ToStream(DataStream& stream) const
{
    EE_LOG(
        efd::kAssets,
        efd::ILogger::kLVL3,
        ("AssetLocatorRequest::ToStream"));

    IMessage::ToStream (stream);

    // stream the URI
    stream << m_uri;
    // stream the process callback category
    stream << m_process_callback;
    // stream the client callback category
    stream << m_client_callback;
}

//-------------------------------------------------------------------------------------------------
/* virtual */
bool AssetLocatorRequest::FromStream(const DataStream& stream)
{
    EE_LOG(
        efd::kAssets,
        efd::ILogger::kLVL3,
        ("AssetLocatorRequest::FromStream Size = %d, Pos = %d",
        stream.GetSize(),
        stream.GetPos()));

    IMessage::FromStream (stream);

    // save the URI
    stream >> m_uri;
    // save the process category
    stream >> m_process_callback;
    EE_ASSERT(m_process_callback != kCAT_INVALID);
    // save the client category
    stream >> m_client_callback;
    EE_ASSERT(m_client_callback != kCAT_INVALID);

    return true;
}

//-------------------------------------------------------------------------------------------------
/*virtual */
void AssetLocatorRequest::SetURI (const efd::utf8string& uri_request)
{
    m_uri = uri_request;
}

//-------------------------------------------------------------------------------------------------
/*virtual */
const efd::utf8string& AssetLocatorRequest::GetURI() const
{
    return m_uri;
}

//-------------------------------------------------------------------------------------------------
/*virtual */
void AssetLocatorRequest::CallbackCategory (const efd::Category& callback)
{
    m_process_callback = callback;
}

//-------------------------------------------------------------------------------------------------
/*virtual */
const efd::Category& AssetLocatorRequest::CallbackCategory() const
{
    return m_process_callback;
}

//-------------------------------------------------------------------------------------------------
/*virtual */
void AssetLocatorRequest::ClientCategory (const efd::Category& callback)
{
    m_client_callback = callback;
}

//-------------------------------------------------------------------------------------------------
/*virtual */
const efd::Category& AssetLocatorRequest::ClientCategory() const
{
    return m_client_callback;
}

//-------------------------------------------------------------------------------------------------
void FetchAssetDataRequest::ToStream( efd::DataStream& stream ) const
{
    AssetLocatorRequest::ToStream(stream);
    stream << m_instanceId;
    stream << m_fullFetch;
    stream << m_offset;
    stream << m_size;
}


//-------------------------------------------------------------------------------------------------
bool FetchAssetDataRequest::FromStream( const efd::DataStream& stream )
{
    AssetLocatorRequest::FromStream(stream);
    stream >> m_instanceId;
    stream >> m_fullFetch;
    stream >> m_offset;
    stream >> m_size;
    return true;
}

//-------------------------------------------------------------------------------------------------
/* virtual */
void AssetConfigurationMessage::ToStream(DataStream& stream) const
{
    IMessage::ToStream (stream);
    stream.SetVariant(m_variant);

    // stream the asset-web path
    stream << m_webRoot;
}

//-------------------------------------------------------------------------------------------------
/* virtual */
bool AssetConfigurationMessage::FromStream(const DataStream& stream)
{
    IMessage::FromStream (stream);
    m_variant = stream.GetVariant();

    // save the rest of the stream to asset-web path
    stream >> m_webRoot;

    return true;
}

//-------------------------------------------------------------------------------------------------
/* virtual */
void AssetTagsInfoRequest::ToStream(DataStream& stream) const
{
    IMessage::ToStream (stream);

    // stream the category
    UInt32 category = static_cast<UInt32> (m_category);
    stream << category;
}

//-------------------------------------------------------------------------------------------------
/* virtual */
bool AssetTagsInfoRequest::FromStream(const DataStream& stream)
{
    IMessage::FromStream (stream);

    // save the rest of the stream to category
    UInt32 category;
    stream >> category;
    m_category = static_cast<efd::IAssetLocatorService::AssetTagCategory> (category);

    return true;
}

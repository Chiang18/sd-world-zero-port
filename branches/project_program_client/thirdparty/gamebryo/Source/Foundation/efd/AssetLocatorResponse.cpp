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

#include <efd/AssetLocatorResponse.h>
#include <efd/DataStream.h>
#include <efd/ILogger.h>
#include <efd/efdLogIDs.h>

using namespace efd;

EE_IMPLEMENT_CONCRETE_CLASS_INFO(AssetLocatorResponse);


//-------------------------------------------------------------------------------------------------
AssetLocatorResponse::AssetLocatorResponse()
{
}


//-------------------------------------------------------------------------------------------------
/* virtual */
void AssetLocatorResponse::ToStream(DataStream& stream) const
{
    EE_LOG(
        efd::kAssets,
        efd::ILogger::kLVL3,
        ("AssetLocatorResponse::ToStream"));

    IMessage::ToStream (stream);

    // stream the uri that the response is for
    stream << m_uri;
    // stream the destination category
    stream << m_client_category;
    // stream the asset tuples
    // how many tuples?
    UInt32 count = (UInt32) m_AssetLocations.size();
    stream << count;

    AssetURLMap::const_iterator i;
    for (i=m_AssetLocations.begin(); i!=m_AssetLocations.end(); i++)
    {
        stream << i->first;
        stream << i->second.name;
        stream << i->second.tagset;
        stream << i->second.classes;
        stream << i->second.url;
        stream << i->second.llid;
    }
}


//-------------------------------------------------------------------------------------------------
/* virtual */
bool AssetLocatorResponse::FromStream(const DataStream& stream)
{
    EE_LOG(
        efd::kAssets,
        efd::ILogger::kLVL3,
        ("AssetLocatorResponse::FromStream Size = %d, Pos = %d",
        stream.GetSize(),
        stream.GetPos()));

    IMessage::FromStream (stream);

    // decode the uri that this response is for
    stream >> m_uri;
    // decode the destination category
    stream >> m_client_category;
    EE_ASSERT (m_client_category!=kCAT_INVALID);
    // decode the message stream, contains a list of AssetID, AssetURL tuples
    // how many tuples?
    UInt32 count;
    stream >> count;

    // load into local map
    m_AssetLocations.clear();
    for (UInt32 x=0; x<count; x++)
    {
        utf8string id, name, tagset, classes, url, llid;
        stream >> id;
        stream >> name;
        stream >> tagset;
        stream >> classes;
        stream >> url;
        stream >> llid;
        AssetLoc loc = {name, tagset, classes, url, llid};
        m_AssetLocations[id] = loc;
    }

    return true;
}


//-------------------------------------------------------------------------------------------------
void AssetSizeResponse::ToStream(DataStream &stream) const
{
    AssetLocatorResponse::ToStream(stream);
    stream << m_size;
}


//-------------------------------------------------------------------------------------------------
bool AssetSizeResponse::FromStream(const DataStream& stream)
{
    AssetLocatorResponse::FromStream(stream);
    stream >> m_size;
    return true;
}

//-------------------------------------------------------------------------------------------------
/* virtual */
void AssetTagsInfoResponse::ToStream(DataStream& stream) const
{
    IMessage::ToStream(stream);

    UInt32 count = (UInt32) m_TagInfo.size();
    stream << count;

    AssetBrowseInfo::const_iterator j;
    for (j=m_TagInfo.begin(); j!=m_TagInfo.end(); j++)
    {
        UInt32 tag_type_enum_value = j->ttype;
        stream << tag_type_enum_value;
        stream << j->tvalue;
        stream << j->count;
    }
}

//-------------------------------------------------------------------------------------------------
/* virtual */
bool AssetTagsInfoResponse::FromStream(const DataStream& stream)
{
    UInt32 count;

    IMessage::FromStream(stream);

    stream >> count;
    for (UInt32 x=0; x<count; x++)
    {
        BrowseInfoDescriptor bi;
        UInt32 tag_type_enum_value;
        stream >> tag_type_enum_value;
        bi.ttype = static_cast<efd::AWebTagType> (tag_type_enum_value);
        stream >> bi.tvalue;
        stream >> bi.count;

        m_TagInfo.push_back (bi);
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
/* virtual */
void AssetBrowseInfoResponse::ToStream(DataStream& stream) const
{
    AssetLocatorResponse::ToStream(stream);

    UInt32 count = (UInt32) m_TagBrowseInfo.size();
    stream << count;

    AssetBrowseInfo::const_iterator j;
    for (j=m_TagBrowseInfo.begin(); j!=m_TagBrowseInfo.end(); j++)
    {
        UInt32 tag_type_enum_value = j->ttype;
        stream << tag_type_enum_value;
        stream << j->tvalue;
        stream << j->count;
    }

    stream << m_root;
}

//-------------------------------------------------------------------------------------------------
/* virtual */
bool AssetBrowseInfoResponse::FromStream(const DataStream& stream)
{
    UInt32 count;

    AssetLocatorResponse::FromStream(stream);

    stream >> count;
    for (UInt32 x=0; x<count; x++)
    {
        BrowseInfoDescriptor bi;
        UInt32 tag_type_enum_value;
        stream >> tag_type_enum_value;
        bi.ttype = static_cast<efd::AWebTagType> (tag_type_enum_value);
        stream >> bi.tvalue;
        stream >> bi.count;

        m_TagBrowseInfo.push_back (bi);
    }

    stream >> m_root;

    return true;
}

//-------------------------------------------------------------------------------------------------
void FetchAssetDataResponse::ToStream(DataStream& stream) const
{
    AssetLocatorResponse::ToStream(stream);
    stream << (efd::UInt32)m_result;
    stream << m_size;
    stream.WriteRawBuffer(m_buffer, m_size);
}


//-------------------------------------------------------------------------------------------------
bool FetchAssetDataResponse::FromStream(const DataStream& stream)
{
    AssetLocatorResponse::FromStream(stream);

    efd::UInt32 tmp;
    stream >> tmp;
    m_result = (FetchAssetDataResponse::FetchResult)tmp;
    stream >> m_size;
    AllocateBuffer(m_size);
    stream.ReadRawBuffer(m_buffer, m_size);
    return true;
}

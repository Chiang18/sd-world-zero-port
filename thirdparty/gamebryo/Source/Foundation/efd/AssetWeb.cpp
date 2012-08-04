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

#include <efd/OS.h> // before other platform stuff
#if defined(EE_PLATFORM_WIN32) && !defined(EE_PLATFORM_XBOX360)
#include <Rpc.h>
#include <ShlObj.h>
#endif
#include <efd/PathUtils.h>
#include <efd/AssetUriReader.h>
#include <efd/AssetWeb.h>
#include <efd/AssetUtil.h>
#include <efd/ILogger.h>
#include <efd/efdLogIDs.h>
#include <efd/SystemUniqueIndex.h>
#include <efd/File.h>
#include <efd/ParseHelper.h>

using namespace efd;

// Set the version stamp here. Incrementing this value will cause old metadata to be
// invalidated and deleted on the next scan made by the Asset Controller.
const /*static*/ utf8string AssetWeb::ASSET_WEB_META_VERSION = "100";

//-------------------------------------------------------------------------------------------------
AssetWeb::AssetWeb()
: m_mode (AWEB_CLOSED)
, m_invalid_metadata (false)
{
}

AssetWeb::~AssetWeb()
{
    closeWeb();
}

//-------------------------------------------------------------------------------------------------
efd::Bool AssetWeb::openWeb(const utf8string& root, AWebAccessMode mode)
{
    // Clear out in case openWeb is called more than once.
    m_Graphs.clear();
    m_TagGraphs.clear();
    m_tagSoup.clear();

    // Set the read/write mode
    m_mode = mode;

    // store location path
    if (PathUtils::IsRelativePath(root))
    {
        m_location = PathUtils::ConvertToAbsolute (root);
    }
    else
    {
        m_location = root;
    }
    m_meta_location = m_location + "/asset-web-metadata";

    // we must have a metadata sub-directory
    m_meta_location = PathUtils::PathMakeNative(m_meta_location);
    MakeDir (m_meta_location.c_str());

    // ensure our metadata matches our AssetWeb code version
    m_invalid_metadata = updateVersion();

    // Create default Graphs
    // for the asset web instance id
    createGraph("awebinst", URIReader::GlobalNamespace_ids_instance, false);
    // for the asset 'name' property
    createGraph("name", URIReader::GlobalNamespace_name, true);
    // for the 'relpath' relative path property
    createGraph("relpath", URIReader::GlobalNamespace_relpath, mode==AWEB_READ_WRITE);
    // for the 'canonical' path property
    createGraph("canonical", URIReader::GlobalNamespace_canonical, mode==AWEB_READ_WRITE);
    // for the logical asset id 'llid' property
    createGraph("llid", URIReader::GlobalNamespace_llid, true);
    // for the logical path 'logpath' property
    createGraph("logpath", URIReader::GlobalNamespace_logpath, mode==AWEB_READ_WRITE);

    // load standard and user-defined tag values
    loadTagSoup();

    // If writing to the asset-web is supported?
    if (m_mode == AWEB_READ_WRITE)
    {
        // create a system unique index, since we will only allow a single write
        utf8string instance_identifier = m_location;
        instance_identifier.replace_substr("\\", "/");

        // to avoid any ambiguity with system paths, convert the whole thing to lower case.
        instance_identifier.tolower();

        m_assetWebUniqueIndex.AquireIndex(instance_identifier);

        if (m_assetWebUniqueIndex.GetIndex()>1)
        {
            // there is already an AssetController running for this path
            m_assetWebUniqueIndex.ReleaseIndex();
            return false;
        }

        // Update our AssetWebInstanceId each time we open a new asset web
        if (!EE_VERIFY_MESSAGE (updateInstanceId(),
            ("Failure: Could not create instance. File asset-web-metadata/awebinst.nt must be writable.")))
        {
            EE_LOG(
                efd::kAssets,
                efd::ILogger::kERR1,
                ("Failure: Could not create instance. File asset-web-metadata/awebinst.nt must be writable."));
        }

        // If any of the preliminary loading identified an invalid metadata or configuration file, then
        // we must purge and re-generate the metadata.
        if (m_invalid_metadata)
        {
            InvalidateMetadata();
        }

        // update time of last metadata synchronization
        updateSyncTime();
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::closeWeb()
{
    m_mode = AWEB_CLOSED;

    // Clear Graphs and tags
    m_Graphs.clear();
    m_TagGraphs.clear();
    m_tagSoup.clear();

    // Reset location path
    m_location = "";
    m_meta_location = "";

    // Unregister any taggers
    m_asset_taggers.clear();

    // Free the unique instance lock
    m_assetWebUniqueIndex.ReleaseIndex();
}

//-------------------------------------------------------------------------------------------------
efd::Bool AssetWeb::commit()
{
    efd::Bool save_result = true;

    if (m_mode != AWEB_READ_WRITE)
    {
        return false;
    }

    map<utf8string, Graph>::iterator i;
    for (i=m_Graphs.begin(); i!=m_Graphs.end(); i++)
    {
        GraphSaveResult r = i->second.save();
        if (r == GRAPH_SAVE_SUCCESS)
        {
            updateCheckpoint (i->first+".nt");
        }
        save_result &= (r != GRAPH_SAVE_FAIL);
    }

    map<utf8string, SimpleGraph>::iterator it;
    for (it=m_TagGraphs.begin(); it!=m_TagGraphs.end(); it++)
    {
        GraphSaveResult r = it->second.save();
        if (r == GRAPH_SAVE_SUCCESS)
        {
            updateCheckpoint (it->first+".nt");
        }
        save_result &= (r != GRAPH_SAVE_FAIL);
    }

    makeCheckpoint(save_result);
    return save_result;
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::RegisterTagger (const efd::utf8string& name, efd::AssetTaggerPtr tagger)
{
    // Link tagger with AssetWeb
    m_asset_taggers[name] = tagger;
    tagger->SetAssetWeb (this);
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::InvalidateMetadata()
{
    // Should not be calling this if the metadata is valid
    EE_ASSERT (m_invalid_metadata);
    // Should not be trying to fix the metadata if we are not the Asset Controller application
    EE_ASSERT (isWritable());

    // Purging all data from the Graphs will result in all assets being treated as
    // newly 'discovered' the next time the file system is scanned.
    map<utf8string, Graph>::iterator i1;
    for (i1=m_Graphs.begin(); i1!=m_Graphs.end(); i1++)
    {
        utf8string gname = i1->first;
        if (gname != "awebinst")
        {
            utf8string metafile = PathUtils::PathCombine(m_meta_location, gname+".nt");
#ifdef EE_PLATFORM_WIN32
            ::DeleteFile (metafile.c_str());
#endif
        }
    }

    vector<AssetTagDescriptor>::iterator i3=m_tagSoup.begin();
    for (; i3!=m_tagSoup.end(); ++i3)
    {
        utf8string gname = i3->tvalue;
        utf8string metafile = PathUtils::PathCombine(m_meta_location, gname+".nt");
#ifdef EE_PLATFORM_WIN32
        ::DeleteFile (metafile.c_str());
#endif
    }

    makeCheckpoint (false);
    m_invalid_metadata = false;
}

//-------------------------------------------------------------------------------------------------
bool AssetWeb::isWritable()
{
    return m_mode == AWEB_READ_WRITE;
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::loadTagGraph (const efd::utf8string& graph_name,
                             const efd::AWebTagType tag_type)
{
    // Performs a sync of tags in the graph with the tag list stored in m_tagSoup
    SimpleGraph* graph = openSimpleGraph (graph_name);
    TripleSetIterator i;

    // Copy the graph's tag definitions to the list
    for (i=graph->begin(); i!=graph->end(); ++i)
    {
        AssetTagDescriptor tag = {tag_type, (*i).object};
        if (m_tagSoup.find (tag)==m_tagSoup.end())
        {
            m_tagSoup.push_back (tag);
        }
    }
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::loadTagSoup()
{
    // Include any tags provided by plug-in asset taggers
    map<utf8string, AssetTaggerPtr>::const_iterator k=m_asset_taggers.begin();
    for (; k!=m_asset_taggers.end(); ++k)
    {
        vector<AssetTagDescriptor> tags;
        k->second->GetTagValues (tags);
        for (vector<AssetTagDescriptor>::iterator at=tags.begin(); at!=tags.end(); ++at)
        {
            addTagValue(at->tvalue, at->ttype);
            if (!m_invalid_metadata)
            {
                // Update checkpoint hashes on all the tag files
                m_invalid_metadata = updateCheckpoint(at->tvalue+".nt");
            }
        }
        // Update checkpoint hashes for the configuration file itself
        m_invalid_metadata = updateCheckpoint(k->first+".ini") ? true : m_invalid_metadata;
    }

    if (!m_invalid_metadata)
    {
        m_invalid_metadata = updateCheckpoint("soup.nt")    ||
            updateCheckpoint("nuts.nt")                     ||
            updateCheckpoint("enums.nt");
    }

    // Include any tags from the soup.nt graph
    loadTagGraph ("soup", AWEB_STANDARD_TAG_TYPE);

    // Include any tags from the nuts.nt graph
    loadTagGraph ("nuts", AWEB_LABEL_TAG_TYPE);

    // Include any tags from the enums.nt graph
    loadTagGraph ("enums", AWEB_CLASSIFICATION_TAG_TYPE);
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::addTagValue (const efd::utf8string& value, const efd::AWebTagType tag_type)
{
    if (tag_type==AWEB_LABEL_TAG_TYPE)
    {
        Triple triple;
        triple.subject = URIReader::GlobalNamespace_usertags;
        triple.subject_desc = NodeDescUri;
        triple.predicate = URIReader::GlobalNamespace_usertags_value;
        triple.predicate_desc = NodeDescUri;
        triple.object = value;
        triple.object_desc = NodeDescLiteral;

        SimpleGraph* graph = openSimpleGraph("nuts");
        graph->insert_triple (triple);
    }
    else if (tag_type==AWEB_CLASSIFICATION_TAG_TYPE)
    {
        Triple triple;
        triple.subject = URIReader::GlobalNamespace_userenums;
        triple.subject_desc = NodeDescUri;
        triple.predicate = URIReader::GlobalNamespace_userenums_value;
        triple.predicate_desc = NodeDescUri;
        triple.object = value;
        triple.object_desc = NodeDescLiteral;

        SimpleGraph* graph = openSimpleGraph("enums");
        graph->insert_triple (triple);
    }
    else if (tag_type==AWEB_STANDARD_TAG_TYPE)
    {
        Triple triple;
        triple.subject = URIReader::GlobalNamespace_stdtags;
        triple.subject_desc = NodeDescUri;
        triple.predicate = URIReader::GlobalNamespace_stdtags_value;
        triple.predicate_desc = NodeDescUri;
        triple.object = value;
        triple.object_desc = NodeDescLiteral;

        SimpleGraph* graph = openSimpleGraph("soup");
        graph->insert_triple (triple);
    }

    AssetTagDescriptor tag = {tag_type, value};
    if (m_tagSoup.find (tag)==m_tagSoup.end())
    {
        m_tagSoup.push_back (tag);
    }
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::getTagSet(efd::vector<AssetTagDescriptor>& tags, const efd::AWebTagType tag_type)
{
    vector<AssetTagDescriptor>::iterator i=m_tagSoup.begin();
    for (; i!=m_tagSoup.end(); ++i)
    {
        if (i->ttype==tag_type)
        {
            tags.push_back (*i);
        }
    }
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::getUserTags(efd::vector<AssetTagDescriptor>& tags)
{
    getTagSet(tags, AWEB_LABEL_TAG_TYPE);
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::getEnumTags(efd::vector<AssetTagDescriptor>& tags)
{
    getTagSet(tags, AWEB_CLASSIFICATION_TAG_TYPE);
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::getStdTags(efd::vector<AssetTagDescriptor>& tags)
{
    getTagSet(tags, AWEB_STANDARD_TAG_TYPE);
}

//-------------------------------------------------------------------------------------------------
efd::Bool AssetWeb::updateInstanceId()
{
    if (m_mode != AWEB_READ_WRITE)
        return false;

    // Create an Id for this instance
    utf8string guid;
    generateUUID (guid);

    // Add the instance Id to our local store of asset web identifying information
    Graph* graph = openGraph("awebinst");
    Triple t;

    t.subject = URIReader::GlobalNamespace_ids;
    t.subject_desc = NodeDescUri;
    t.predicate = URIReader::GlobalNamespace_ids_instance;
    t.predicate_desc = NodeDescUri;
    t.object = guid;
    t.object_desc = NodeDescLiteral;

    graph->merge_triple (t);
    return (graph->save()>=0);
}

//-------------------------------------------------------------------------------------------------
bool AssetWeb::makeCheckpoint(bool success)
{
    EE_ASSERT (isWritable());

    SimpleGraph* graph = openSimpleGraph("awebckpt");
    Triple t;

    // Set the checkpoint status
    utf8string str_success = (success) ? "true" : "false";
    t.subject = URIReader::GlobalNamespace_ckpt;
    t.subject_desc = NodeDescUri;
    t.predicate = URIReader::GlobalNamespace_ckpt_success;
    t.predicate_desc = NodeDescUri;
    t.object = str_success;
    t.object_desc = NodeDescLiteral;
    graph->merge_triple (t);

    // Set the Author Id
    t.subject = URIReader::GlobalNamespace_ids;
    t.subject_desc = NodeDescUri;
    t.predicate = URIReader::GlobalNamespace_ids_author;
    t.predicate_desc = NodeDescUri;
    getAuthorId (t.object);
    t.object_desc = NodeDescLiteral;
    graph->merge_triple (t);

    // Save graph and update the last sync time to now
    graph->save(true);
    updateSyncTime();
    
    return true;
}

//-------------------------------------------------------------------------------------------------
efd::Bool AssetWeb::updateCheckpoint(const efd::utf8string& metadata_file)
{
    // We don't really want to checkpoint the actual checkpoint file.
    if (metadata_file=="awebckpt.nt")
        return false;
    // And, no need to waste time computing checkpoint hashes for read-only mode
    else if (m_mode==AWEB_READ_ONLY)
        return false;

    bool retval = false;
    SimpleGraph* graph = openSimpleGraph("awebckpt");
    Triple q;
    TripleSet hash_value;

    // Retrieve last hash value for the metadata
    q.subject = URIReader::GlobalNamespace + metadata_file;
    q.subject_desc = NodeDescUri;
    q.predicate = URIReader::GlobalNamespace_ckpt_hash;
    q.predicate_desc = NodeDescUri;
    q.object_desc = NodeDescNone;
    graph->query(q, hash_value);

    // Compute new hash and compare
    utf8string hash_update;
    getDataFileHash (metadata_file, hash_update);
    bool is_changed;
    if (hash_value.size()==0)
    {
        is_changed = hash_update!="0";
    }
    else
    {
        is_changed = hash_value[0].object!=hash_update;
    }

    if (is_changed)
    {
        // Update the hash value
        q.object_desc = NodeDescLiteral;
        q.object = hash_update;
        graph->merge_triple (q);
        retval = true;
    }

    return retval;
}

//-------------------------------------------------------------------------------------------------
efd::Bool AssetWeb::updateVersion()
{
    bool retval = false;
    SimpleGraph* graph = openSimpleGraph("awebckpt");
    Triple q;
    TripleSet vers_triple;

    // Retrieve last version value for the metadata
    q.subject = URIReader::GlobalNamespace_ckpt;
    q.subject_desc = NodeDescUri;
    q.predicate = URIReader::GlobalNamespace_ckpt_version;
    q.predicate_desc = NodeDescUri;
    q.object_desc = NodeDescNone;
    graph->query(q, vers_triple);

    // Compare with the current AssetWeb code version
    bool is_changed = vers_triple.size()==0 ||
                      vers_triple[0].object!=ASSET_WEB_META_VERSION;

    if (is_changed)
    {
        // Update the version value
        q.object_desc = NodeDescLiteral;
        q.object = ASSET_WEB_META_VERSION;
        graph->merge_triple (q);
        retval = true;

        if (m_mode==AWEB_READ_WRITE)
        {
            EE_LOG(
                efd::kAssets,
                efd::ILogger::kLVL1,
                ("Metadata version is out of date and will be regenerated."));
        }
    }

    return retval;
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::updateSyncTime()
{
#ifdef EE_PLATFORM_WIN32
    // Check if there was a previous, successful metadata sync
    bool result = false;
    utf8string subj = URIReader::GlobalNamespace_ckpt;
    utf8string prop = URIReader::GlobalNamespace_ckpt_success;

    Triple Q;
    TripleSet triples;
    SimpleGraph* graph = openSimpleGraph("awebckpt");
    objectQueryTriple (subj, prop, Q);
    graph->query (Q, triples);
    if (triples.size()==1)
    {
        result = triples[0].object=="true"; // indicates successful sync in the past
    }

    if (result)
    {
        // Get the timestamp from the file
        utf8string path = PathUtils::PathCombine(m_location, "asset-web-metadata/awebckpt.nt");
        PathUtils::GetLastModifed (path, &m_last_sync);
    }
    else
    {
        m_last_sync.HiBits = 0;
        m_last_sync.LoBits = 0;
    }
#endif
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::getDataFileHash(const utf8string& file_name, utf8string& hash)
{
    utf8string dir, path;

    // Check the file name extension to know if we need to hash a metadata file or a config file
    if (file_name.find_last_of(".nt")==file_name.length()-1)
    {
        dir = m_meta_location;
    }
    else if (file_name.find_last_of(".ini")==file_name.length()-1)
    {
        dir = PathUtils::PathCombine(m_location, "/asset-web-config");
    }

    // We should not be computing hashes on other kinds of files
    EE_ASSERT (dir.length()>0);

    // Now read the file and compute the hash using FNVHash
    static const UInt64 FNVinit = 14695981039346656037ULL;
    static const UInt64 FNVprime = 1099511628211ULL;
    UInt64 hashv = FNVinit;

    path = PathUtils::PathCombine(dir, file_name);
    File* pkFile = File::GetFile(path.c_str(), File::READ_ONLY);
    char buf[512];

    if (pkFile)
    {
        unsigned int sz = pkFile->Read(buf, 512);
        while (sz)
        {
            for (unsigned int i = 0; i < sz; i++)
            {
                hashv = hashv ^ (buf[i]);
                hashv = hashv * FNVprime;
            }
            sz = pkFile->Read(buf, 512);
        }
       EE_DELETE pkFile;
    }
    else
    {
        hashv = 0;
    }

    // Convert resulting hash value to a string
    ParseHelper<UInt64>::ToString(hashv, hash);
}

//-------------------------------------------------------------------------------------------------
#ifdef EE_PLATFORM_WIN32
void AssetWeb::GetLastSyncTime(PathUtils::FileTimestamp* timestamp)
{
    *timestamp = m_last_sync;
}
#endif

//-------------------------------------------------------------------------------------------------
bool AssetWeb::getInstanceId(utf8string& guid)
{
    bool retval = false;
    utf8string subj = URIReader::GlobalNamespace_ids;
    utf8string prop = URIReader::GlobalNamespace_ids_instance;

    Graph* graph = openGraph("awebinst");
    Triple Q;
    TripleSet triples;

    objectQueryTriple (subj, prop, Q);
    graph->query (Q, triples);

    if (triples.size()==1)
    {
        guid = triples[0].object;
        retval = true;
    }
    else if (triples.size()>1)
    {
        EE_ASSERT (false);
    }

    return retval;
}

//-------------------------------------------------------------------------------------------------
bool AssetWeb::getAuthorId(efd::utf8string& guid)
{
#ifdef EE_PLATFORM_WIN32
    // On Windows, we can get the Author Id directly from the user's settings
    return getSystemAuthorId(guid);
#else
    return getWebAuthorId(guid);
#endif
}

//-------------------------------------------------------------------------------------------------
bool AssetWeb::getWebAuthorId(efd::utf8string& guid)
{
    SimpleGraph* graph = openSimpleGraph("awebckpt");
    Triple Q;
    TripleSet authid;

    Q.subject = URIReader::GlobalNamespace_ids;
    Q.subject_desc = NodeDescUri;
    Q.predicate = URIReader::GlobalNamespace_ids_author;
    Q.predicate_desc = NodeDescUri;
    Q.object_desc = NodeDescNone;
    graph->query (Q, authid);

    EE_ASSERT (authid.size()==1);
    guid = authid[0].object;
    return true;
}

//-------------------------------------------------------------------------------------------------
#ifdef EE_PLATFORM_WIN32
bool AssetWeb::getSystemAuthorId(efd::utf8string& guid)
{
    bool retval = false;

    // File is stored in:
    // \Docs & Settings\<user>\Local Settings\Application Data\Emergent Game Technologies\AssetCtrl
    utf8string path, filename;
    TCHAR szPath[MAX_PATH];

    if (SUCCEEDED(SHGetFolderPath(NULL, 
        CSIDL_LOCAL_APPDATA|CSIDL_FLAG_CREATE, 
        NULL,
        0, 
        szPath))) 
    {
#ifdef UNICODE
        // convert the resulting message to UTF8
        char *tmp = 0;
        // figure out how big a buffer we need
        int mbCount = ::WideCharToMultiByte(CP_UTF8,0,szPath,-1,tmp,0,0,0);
        tmp = EE_ALLOC(char, mbCount+1);
        tmp[mbCount] = 0;
        // now convert the message to UTF8 into the buffer.
        ::WideCharToMultiByte(CP_UTF8,0,szPath,-1,tmp,mbCount,0,0);
        filename = tmp;
        EE_FREE tmp;
#else
        path = szPath;
#endif
        path = PathUtils::PathCombine(path, "Emergent Game Technologies\\AssetCtrl");
        filename = PathUtils::PathCombine(path, "awebauth.nt");

        SimpleGraph g = SimpleGraph();
        // Determine if file already exists
        if (File::Access (filename.c_str(), File::READ_ONLY))
        {
            // Read the GUID from the file
            g.load (filename);

            utf8string msg(Formatted, "Failed to load %s. Please delete it.", filename.c_str());
            if (!EE_VERIFY_MESSAGE (g.size()>0, (msg.c_str())))
            {
                return false;
            }

            guid = g[0].object;
            retval = true;
        }
        else
        {
            // Create the file
            if (!File::DirectoryExists(path.c_str())) 
            {
                File::CreateDirectoryRecursive(path.c_str());
            }
            g.load (filename);

            // Create the user's GUID value and save
            Triple t;
            t.subject = URIReader::GlobalNamespace_ids;
            t.subject_desc = NodeDescUri;
            t.predicate = URIReader::GlobalNamespace_ids_author;
            t.predicate_desc = NodeDescUri;
            generateUUID (guid);
            t.object = guid;
            t.object_desc = NodeDescLiteral;

            g.merge_triple (t);
            retval = g.save() >=0 ;
        }
    }
    return retval;
}
#endif

//-------------------------------------------------------------------------------------------------
bool AssetWeb::assetPropertyValue( const utf8string& uuri,
                                   const utf8string& predicateTag,
                                   utf8string& value )
{
    EE_ASSERT (URIReader::isAssetId(uuri));

    Triple Q;
    TripleSet triples;
    bool retval=false;

    Graph* graph = openGraph (predicateTag);

    // query for the property of the asset
    objectQueryTriple (uuri, predicateTag, Q);
    graph->query (Q, triples);

    // This indicates the problematic circumstance that an asset has multiple
    // values for the same property. Property values are suppose to be quantity one.
    EE_ASSERT (triples.size()<=1);

    if (triples.size()==1)
    {
        value = triples[0].object;
        retval = true;
    }

    return retval;
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::getAssetTags( const utf8string& uuri, TripleSet& triples )
{
    EE_ASSERT (URIReader::isAssetId(uuri));

    Triple Q;
    triples.clear();

    // Loop over all the tag stores
    vector<AssetTagDescriptor>::iterator i = m_tagSoup.begin();
    for (; i != m_tagSoup.end(); ++i)
    {
        // Determine if the asset has the particular tag
        utf8string tag = (*i).tvalue;
        Graph* graph = openGraph (tag);
        closedQueryTriple (uuri, URIReader::GlobalNamespace_tag, tag, Q);
        graph->query (Q, triples);
    }
}

//-------------------------------------------------------------------------------------------------
bool AssetWeb::assetLabelTags( const utf8string& uuri, utf8string& value )
{
    TripleSet triples;
    getAssetTags (uuri, triples);

    // only use the tags which are label or standard tags
    vector<AssetTagDescriptor> utags;
    vector <AssetTagDescriptor>::const_iterator i;
    TripleSetIterator t=triples.begin();
    getUserTags (utags);
    getStdTags (utags);

    value = "urn";
    for (; t!=triples.end(); ++t)
    {
        utf8string chk = t->object;
        bool found = false;
        for (i=utags.begin(); i!=utags.end(); ++i)
        {
            if (i->tvalue==chk)
            {
                // this is a label tag
                found = true;
                break;
            }
        }

        if (found)
        {
            // add label tag to the result
            value += ":";
            value += chk;
        }
    }

    // append the 'name' property value
    utf8string nameVal;
    if (assetPropertyValue(uuri, URIReader::GlobalNamespace_name, nameVal))
    {
        value += ":";
        value += nameVal;
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
bool AssetWeb::assetEnumTags( const utf8string& uuri, utf8string& value, efd::UInt32* count )
{
    TripleSet triples;
    getAssetTags (uuri, triples);

    // only use the tags which are classification tags
    vector<AssetTagDescriptor> utags;
    vector <AssetTagDescriptor>::const_iterator i;
    TripleSetIterator t=triples.begin();
    getEnumTags(utags);

    value = "";
    if (count) *count=0;

    for (; t!=triples.end(); ++t)
    {
        utf8string chk = t->object;
        bool found = false;
        for (i=utags.begin(); i!=utags.end(); ++i)
        {
            if (i->tvalue==chk)
            {
                // this is a classification tag
                found = true;
                break;
            }
        }

        if (found)
        {
            // add classification tag to the result
            if (value.length()>0)
                value += ":";
            value += chk;

            if (count) (*count)++;
        }
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
bool AssetWeb::assetAllTags( const utf8string& uuri, utf8string& value )
{
    TripleSet triples;
    bool retval=false;

    getAssetTags (uuri, triples);

    // check results and build response string containing all the tags
    if (triples.size()>0)
    {
        value = "urn";
        for (size_t x=0; x<triples.size(); x++)
        {
            value += ":";
            value += triples[x].object;
        }
        retval = true;

        // append the 'name' property value
        utf8string nameVal;
        if (assetPropertyValue(uuri, URIReader::GlobalNamespace_name, nameVal))
        {
            value += ":";
            value += nameVal;
        }
    }

    return retval;
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::assetAddTag( const utf8string& uuri, const utf8string& value )
{
    Graph* graph = openGraph (value);
    graph->append (uuri, value);
}

//-------------------------------------------------------------------------------------------------
efd::Bool AssetWeb::assetHasTag( const utf8string& uuri, const utf8string& tag )
{
    // Get the graph that contains this tag
    Graph* graph = openGraph (tag);

    // Determine if there is a match for this asset id
    Triple Q;
    TripleSet triples;
    closedQueryTriple(uuri, URIReader::GlobalNamespace_tag, tag, Q);
    graph->query (Q, triples);
    return (triples.size()>0);
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::assetAddProperty(const utf8string& uuri,
                                const utf8string& prop,
                                const utf8string& value)
{
    Graph* graph = openGraph (prop);
    graph->append (uuri, value);
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::addTripleToGraph(SimpleGraph& graph,
                                const utf8string& subject,
                                const utf8string& prop,
                                const utf8string& value)
{

    Triple t;
    t.subject = subject;
    t.predicate = prop;
    t.object = value;
    graph.merge_triple(t);
}

//-------------------------------------------------------------------------------------------------
const efd::utf8string& AssetWeb::GetAssetWebRoot() const
{
    return m_location;
}
//-------------------------------------------------------------------------------------------------
void AssetWeb::assetAbsolutePath( const utf8string& relative, utf8string& o_absolute)
{
    o_absolute = PathUtils::PathCombine (m_location, relative);
    o_absolute = PathUtils::PathMakeNative(o_absolute);
}

//-------------------------------------------------------------------------------------------------
bool AssetWeb::assetPathLookup( const efd::utf8string& relativePath, efd::utf8string& assetId)
{
    TripleSet matches;
    bool found = assetWithPropertyValue(URIReader::GlobalNamespace_relpath,
                                        relativePath,
                                        matches);
    if (found)
    {
        assetId = matches[0].subject;
    }

    return found;
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::assetMetadataPath(const efd::utf8string& relative_path,
                                 efd::utf8string& metadata_path)
{
    utf8string dir, name, ext;
    AssetUtil::PathParts (relative_path, dir, name, ext);

    utf8string path = dir + "/asset-src-metadata/" + name + "." + ext + "_.nt";
    // Convert to platform compatible absolute path
    assetAbsolutePath(path, metadata_path);
    metadata_path = PathUtils::PathMakeNative(metadata_path);
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::assetLoadPersistentMetadata(const efd::utf8string& uuri)
{
    EE_ASSERT (isWritable());

#ifdef EE_PLATFORM_WIN32
    // Determine the name of the asset's metadata file
    utf8string relative_path, nt_file_name;
    assetPropertyValue(uuri, URIReader::GlobalNamespace_relpath, relative_path);
    assetMetadataPath (relative_path, nt_file_name);

    // Determine if metadata file has been updated
    PathUtils::FileTimestamp last;
    utf8string path = PathUtils::PathCombine(m_location, relative_path);
    PathUtils::GetLastModifed (path, &last);
    if ( (last.HiBits > m_last_sync.HiBits) ||
        (last.HiBits == m_last_sync.HiBits && last.LoBits > m_last_sync.LoBits) )
    {
        // File has been updated => reload the metadata.
        utf8string assetId;
        mergeMetadataFile (nt_file_name, assetId);
    }
#endif
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::mergeMetadataFile(const efd::utf8string& file_name,
                                 efd::utf8string& assetId)
{
    // Open the source metadata file
    SimpleGraph src = SimpleGraph();
    src.load (file_name);
    if (src.size()==0)
        return;

    assetId = src[0].subject;
    TripleSetIterator i = src.begin();
    for (; i!=src.end(); ++i)
    {
        // Merge any tag values into the correct tag index
        if (i->predicate == URIReader::GlobalNamespace_tag ||
            i->predicate == URIReader::GlobalNamespace_label)
        {
            addTagValue (i->object, AWEB_LABEL_TAG_TYPE);
            assetAddTag(assetId, i->object);
        }
        else if (i->predicate == URIReader::GlobalNamespace_class)
        {
            addTagValue (i->object, AWEB_CLASSIFICATION_TAG_TYPE);
            assetAddTag(assetId, i->object);
        }
        else
        {
            // Merge property values
            Graph* dest = openGraph((*i).predicate);
            dest->merge_triple (*i);
        }
    }
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::findAllAssets( const utf8string& uri, TripleSet& triples)
{
    Triple Q;
    triples.clear();

    URIReader URIRef = URIReader (uri, m_tagSoup);

    if (URIRef.isAssetId())
    {
        // Simple case is the uri is a unique asset Id.
        Graph* graph = openGraph ("name");

        // query for the name property of the asset
        utf8string asset_uri;
        URIRef.getAssetId (asset_uri);
        
        objectQueryTriple (asset_uri, URIReader::GlobalNamespace_name, Q);
        graph->query (Q, triples);
    }
    else
    {
        // More complicated case of matching multiple properties and/or tags
        utf8string scheme;
        URIRef.getScheme (scheme);
        if (scheme=="urn")
        {
            utf8string name, canonical, logical;
            vector<utf8string> labels, overrides;

            URIRef.getNameLiteral (name);
            URIRef.getCanonicalLiteral (canonical);
            URIRef.getLabels (labels);
            URIRef.getClassifications (overrides);
            URIRef.getLogicalId (logical);

            // check for a URIRef that refers to ALL assets (i.e "urn")
            if (name.length()==0
                && canonical.length()==0
                && logical.length()==0
                && labels.size()==0
                && overrides.size()==0)
            {
                assetsWithPredicate (URIReader::GlobalNamespace_name, triples);

                // return all the assets
                return;
            }

            // find assets with matching name
            if (name.length())
            {
                incrementalFindAssets (URIReader::GlobalNamespace_name, name, triples);

                if (triples.size()==0)
                {
                    // no assets with matching name attribute. don't need to look at other tags.
                    return;
                }
            }

            // find assets with matching canonical property
            if (canonical.length())
            {
                incrementalFindAssets (URIReader::GlobalNamespace_canonical, canonical, triples);

                if (triples.size()==0)
                {
                    // no assets with matching canonical attribute.
                    return;
                }
            }

            // find assets with matching logical asset id property
            if (logical.length())
            {
                incrementalFindAssets (URIReader::GlobalNamespace_llid, logical, triples);

                if (triples.size()==0)
                {
                    // no assets with matching logical id attribute.
                    return;
                }
            }

            // find assets which match all the label tags
            vector<utf8string>::iterator i;
            for (i=labels.begin(); i!=labels.end(); i++)
            {
                incrementalFindAssets (URIReader::GlobalNamespace_tag, *i, triples);
                if (triples.size()==0)
                {
                    // the last tag filtered out all of the assets
                    return;
                }
            }

            // apply any override values indicated by classification tags
            UInt32 resolved_count=0;
            for (i=overrides.begin(); i!=overrides.end(); i++)
            {
                Bool resolved;
                if ((*i)[0]=='/')
                {
                    resolved = resolveOverridePath (*i, triples);
                }
                else
                {
                    resolved = resolveOverrideTag (*i, triples);
                }

                if (!resolved)
                {
                    // Last override yielded zero assets.
                    break;
                }
                else if (triples.size()==1)
                {
                    // Last override yielded one asset. There is no point to apply other tags.
                    return;
                }
                resolved_count++;
            }

            // It is assumed that the query should
            // return at least one asset, which can be accomplished by pruning the 
            // results that include classification tags and returning the leftover.
            if (canonical.length()==0)
                pruneAssetClasses (triples, resolved_count);
        }
    }
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::incrementalFindAssets (const efd::utf8string& prop,
                                      const efd::utf8string& value,
                                      TripleSet& triples)
{
    TripleSet* incr;
    TripleSet matches;

    // Is this for incremental addition of a tag value?
    if (URIReader::GlobalNamespace_tag==prop)
    {
        // load the graph for this tag, if not already loaded
        Graph* graph = openGraph (value);

        if (triples.size() > 0)
        {
            graph->intersection (triples);
        }
        else
        {
            graph->unionize (triples);
        }
        return;
    }
    // Is this for incremental addition using path searching?
    else if (URIReader::GlobalNamespace_canonical==prop)
    {
        assetWithPathValue(value, matches);
        incr = &matches;
    }
    // Only other case is incremental addition using some property value (name, relpath, etc.)
    else
    {
        assetWithPropertyValue(prop, value, matches);
        incr = &matches;
    }

    // Combine incremental results with any previous results
    if (triples.size() > 0)
    {
        Graph::intersection (triples, *incr);
    }
    else
    {
        Graph::unionize (triples, *incr);
    }
}

//-------------------------------------------------------------------------------------------------
efd::Bool AssetWeb::resolveOverridePath(const efd::utf8string& path_value,
                                       TripleSet& triples)
{
    TripleSet overrides;

    // Apply the path to determine which assets are also in the override set
    normalAssetList(triples);
    overrides = triples;
    incrementalFindAssets(URIReader::GlobalNamespace_canonical, path_value, overrides);
    normalAssetList(overrides);

    // Filter out overridden assets
    Graph::unique_values (overrides, triples);

    // Copy the results over the original set
    if (overrides.size()>0)
    {
        triples.clear();
        triples = overrides;
        return true;
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------------
efd::Bool AssetWeb::resolveOverrideTag(const efd::utf8string& tag_value,
                                       TripleSet& triples)
{
    TripleSet overrides;

    // Apply the tag to determine which assets are also in the override set
    normalAssetList(triples);
    overrides = triples;
    incrementalFindAssets(URIReader::GlobalNamespace_tag, tag_value, overrides);
    normalAssetList(overrides);

    // Filter out overridden assets
    Graph::unique_values (overrides, triples);

    // Copy the results over the original set
    if (overrides.size()>0)
    {
        triples.clear();
        triples = overrides;
        return true;
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::pruneAssetClasses (TripleSet& triples, efd::UInt32 max_classes)
{
    TripleSet solution;
    TripleSetIterator i = triples.begin();

    for (; i!=triples.end(); ++i)
    {
        // get all the classification tags
        utf8string classes;
        UInt32 howmany;
        assetEnumTags (i->subject, classes, &howmany);

        // prune out all the assets that have classification tags
        if (howmany <= max_classes)
        {
            solution.push_back (*i);
        }
    }

    triples.clear();
    triples = solution;
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::normalAssetList(TripleSet& triples)
{
    vector<Triple>::iterator it = triples.begin();
    for (; it!=triples.end(); ++it)
    {
        if (it->predicate != URIReader::GlobalNamespace_name)
        {
            utf8string nameVal;

            it->predicate_desc = NodeDescUri;
            it->predicate = URIReader::GlobalNamespace_name;
            it->object_desc = NodeDescLiteral;
            assetPropertyValue(it->subject, URIReader::GlobalNamespace_name, nameVal);
            it->object = nameVal;
        }
    }
}

//-------------------------------------------------------------------------------------------------
bool AssetWeb::insertAsset(const efd::utf8string& relativePath, efd::utf8string& outAssetId)
{
    EE_ASSERT (isWritable());

    Bool imported_metadata = false;

    // Check for a side-by-side metadata file for the new asset
    utf8string meta_file_name;
    assetMetadataPath (relativePath, meta_file_name);
    if (File::Access (meta_file_name.c_str(), File::READ_ONLY))
    {
        // Metadata file exists. Merge any tags and properties it contains.
        mergeMetadataFile (meta_file_name, outAssetId);
        imported_metadata = true;
    }
    else
    {
        // Need to create a new asset Id
        generateUUID (outAssetId);
    }

    // Get path and name properties
    utf8string name, extension, canonical_path;
    AssetUtil::PathParts (relativePath, canonical_path, name, extension);
    if (name.empty() || extension.empty())
    {
        return false;
    }

    // Append new property values to the asset's graph
    if (!imported_metadata)
    {
        openGraph("name")->append (outAssetId, name);
    }

    openGraph("relpath")->append (outAssetId, relativePath);
    openGraph("canonical")->append (outAssetId, canonical_path);

    // Perform additional tagging provided by plug-in asset taggers, and always
    // ensure that the MimeTagger is the first tagger used.
    map<utf8string, AssetTaggerPtr>::const_iterator i;
    SimpleGraph src_meta_data;
    src_meta_data.load (meta_file_name);

    i = m_asset_taggers.find ("MimeTagger");
    if (i != m_asset_taggers.end())
    {
        i->second->DoTags(outAssetId, src_meta_data);
    }
    for (i=m_asset_taggers.begin(); i!=m_asset_taggers.end(); ++i)
    {
        if (i->first != "MimeTagger")
        {
            i->second->DoTags(outAssetId, src_meta_data);
        }
    }
    src_meta_data.save();

    return true;
}

//-------------------------------------------------------------------------------------------------
bool AssetWeb::removeAsset( const efd::utf8string& assetId)
{
    EE_ASSERT (isWritable());

    // Get the list of tags assigned to the asset
    utf8string tagsUri;
    assetAllTags (assetId, tagsUri);

    // Delete the asset from the property graphs
    openGraph("name")->deleteAllSubject (assetId);
    openGraph("relpath")->deleteAllSubject (assetId);
    openGraph("canonical")->deleteAllSubject (assetId);
    openGraph("llid")->deleteAllSubject (assetId);
    openGraph("logpath")->deleteAllSubject (assetId);

    // Delete all pointers to this asset from the tag graphs
    vector<utf8string> tag_values;
    URIReader tags (tagsUri, m_tagSoup);
    tags.getTagLiterals (tag_values);

    vector<utf8string>::iterator it = tag_values.begin();
    for (; it != tag_values.end(); ++it)
    {
        openGraph(*it)->deleteAllSubject (assetId);
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::generateUUID( efd::utf8string& assetId)
{
#if defined(EE_PLATFORM_WIN32)
    RPC_CSTR ustring;
    UUID uid;
    utf8string temp;

    UuidCreate(&uid);
    UuidToString(&uid, &ustring);
    temp = reinterpret_cast<const char*> (ustring);
    RpcStringFree(&ustring);

    assetId = "urn:uuid:" + temp;
#else
    // Only supported on WIN32
    EE_FAIL_MESSAGE(("%s only supported on Win32", __FUNCTION__ ));
#endif
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::subjectQueryTriple( const utf8string& tagPredicate,
                                   const utf8string& objectLiteral,
                                   Triple& q )
{
    q.subject_desc = NodeDescNone;
    q.subject = "";
    q.predicate = tagPredicate;
    q.predicate_desc = NodeDescUri;
    q.object = objectLiteral;
    q.object_desc = NodeDescLiteral;
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::objectQueryTriple( const utf8string& subject,
                                  const utf8string& tagPredicate,
                                  Triple& q )
{
    q.subject_desc = NodeDescUri;
    q.subject = subject;
    q.predicate = tagPredicate;
    q.predicate_desc = NodeDescUri;
    q.object = "";
    q.object_desc = NodeDescNone;
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::closedQueryTriple( const utf8string& subject,
                                  const utf8string& tagPredicate,
                                  const utf8string& tagValue,
                                  Triple& q )
{
    q.subject_desc = NodeDescUri;
    q.subject = subject;
    q.predicate = tagPredicate;
    q.predicate_desc = NodeDescUri;
    q.object = tagValue;
    q.object_desc = NodeDescLiteral;
}

//-------------------------------------------------------------------------------------------------
bool AssetWeb::assetWithPropertyValue( const efd::utf8string& predicate,
                                       const efd::utf8string& value,
                                       TripleSet& matches )
{
    Triple Q;
    subjectQueryTriple (predicate, value, Q);
    Graph* graph = openGraph(predicate);
    TripleSet m;

    graph->query (Q, matches);

    // Return 'true' if there was at least one match
    return (matches.size()>0);
}

//-------------------------------------------------------------------------------------------------
bool AssetWeb::assetsWithPredicate( const efd::utf8string& predicate,
                                    TripleSet& matches )
{
    // Get the graph for this predicate
    Graph* graph = openGraph (predicate);

    // Copy over all of the triples from the graph
    Graph::Iterator i(graph), end;
    for (i.begin(); i!=end; ++i)
    {
        matches.push_back (*i);
    }

    // Return 'true' if there was at least one match
    return (matches.size()>0);
}

//-------------------------------------------------------------------------------------------------
bool AssetWeb::compPathMatch (const Triple& a, const Triple& b)
{
    bool result = false;
    // We are just concerned with the path values
    utf8string path1 = a.object;
    utf8string path2 = b.object;

    // Check for trailing slash in the compare path
    if (path2[path2.length()-1] != '/')
    {
        // In order to match, the first path part in path1 must match path2
        // ex. '/Some/Asset/path' matches '/Some' or '/Some/Asset'
        size_t n = path1.find (path2);
        if (n == 0)
        {
            path1.erase (0, path2.length());
            result = path1.length()==0 || path1[0]=='/';
        }
    }
    else
    {
        result = path1==path2.substr(0, path2.length()-1);
    }

    return result;
}

//-------------------------------------------------------------------------------------------------
bool AssetWeb::assetWithPathValue(const efd::utf8string& value,
                                  TripleSet& matches )
{
    Triple Q;
    subjectQueryTriple (URIReader::GlobalNamespace_canonical, value, Q);
    Graph* graph = openGraph("canonical");

    graph->queryWithCompare (Q, &efd::AssetWeb::compPathMatch, matches);

    // Return 'true' if there was at least one match
    return (matches.size()>0);
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::createGraph(const efd::utf8string& name, const efd::utf8string& predicate,
                           bool isIndexed)
{
    m_Graphs[name] = Graph (predicate, isIndexed);
    if (!m_invalid_metadata && name!="awebinst")
    {
        m_invalid_metadata = updateCheckpoint(name+".nt");
    }
}

//-------------------------------------------------------------------------------------------------
Graph* AssetWeb::openGraph(const utf8string& name)
{
    // Trim namespace prefix if required
    utf8string ns = URIReader::GlobalNamespace;
    utf8string gname = name;
    if (gname.find (ns) == 0)
    {
        gname = gname.substr (ns.length(), gname.length()-ns.length());
    }

    if (m_Graphs.find (gname) == m_Graphs.end())
    {
        m_Graphs[gname] = Graph (URIReader::GlobalNamespace_tag, false, gname);
    }

    Graph* retval = &(m_Graphs[gname]);
    if (! retval->isLoaded() )
    {
        utf8string fname = m_meta_location + "/" + gname + ".nt";
        retval->load (fname);
    }

    return retval;
}

//-------------------------------------------------------------------------------------------------
SimpleGraph* AssetWeb::openSimpleGraph(const utf8string& name)
{
    if (m_TagGraphs.find (name) == m_TagGraphs.end())
    {
        m_TagGraphs[name] = SimpleGraph ();
    }

    SimpleGraph* retval = &(m_TagGraphs[name]);
    if (! retval->isLoaded() )
    {
        utf8string fname = m_meta_location + "/" + name + ".nt";
        retval->load (fname);
    }

    return retval;
}

//-------------------------------------------------------------------------------------------------
void AssetWeb::closeGraph(const utf8string& name)
{
    if (m_Graphs.find (name) != m_Graphs.end())
    {
        m_Graphs[name].purge (false);
    }
}

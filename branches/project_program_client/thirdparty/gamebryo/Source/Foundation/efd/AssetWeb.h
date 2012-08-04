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
#ifndef _ASSETSERVER_WEB_H
#define _ASSETSERVER_WEB_H

#include <efd/IAssetTagger.h>
#include <efd/PathUtils.h>
#include <efd/SystemUniqueIndex.h>

namespace efd
{
    /**
     This class provides a storage and querying mechanism for the metadata that is
     associated with an asset-web. The metadata is stored in the directory that is the
     asset-web inside a special sub-directory. This sub-directory is named 'asset-web-
     metadata', and it includes a number of text files in RDF NTriples format. Each file
     has a '.nt' extension. There are files that contain all the information associated
     with an asset, and there are index files that list all the assets tagged with a given
     value. These files may be stored in an RCS system like Perforce or Subversion, and
     changes made by multiple developers can be merged back to a base version.
    */
    class EE_EFD_ENTRY AssetWeb
    {
        public:
            /// The AssetWeb supports two access modes for the metadata - RO and RW
            enum AWebAccessMode
            {
                AWEB_CLOSED,
                AWEB_READ_ONLY,
                AWEB_READ_WRITE
            };

            /// @name Public API
            //@{
            AssetWeb();
            ~AssetWeb();

            /**
             Open an AssetWeb at a certain file system location.

             @param root The root directory path
             @param mode Specify read-only or read-write allowed
            */
            efd::Bool openWeb(const efd::utf8string& root, AWebAccessMode mode);

            /**
             Close an AssetWeb previously opened using openWeb().

             Cached metadata will be freed.
             @see openWeb
            */
            void closeWeb();

            /**
            Save all AssetWeb metadata to files.

            @see openWeb
            */
            efd::Bool commit();

            /**
            Register an asset tagger class.

            Used to provide custom tagging of assets as they are discovered by the server.
            @param name Name of the IAssetTagger derived class from its GetName() method.
            @param tagger Instance of an IAssetTagger derived class
            */
            void RegisterTagger (const efd::utf8string& name, efd::AssetTaggerPtr tagger);

            /**
             Any time the AssetWeb is opened in AWEB_READ_WRITE mode, a new unique
             ID is written into it whenever a new client connection is made.
             Clients that open the AssetWeb using AWEB_READ_ONLY can fetch the ID using
             getInstanceId().

             The purpose here is to allow a client and server to ascertain that they
             are using an identical asset-web location, and should have shared access
             to all of the contained files.

             Update and persist the instance Id unique value

             @return True if the ID value is successfully persisted
            */
            efd::Bool updateInstanceId();

            /**
             Get the current instance Id unique value

             @param guid [out] The unique ID value
             @return True if an ID is found and read from the asset-web
             */
            efd::Bool getInstanceId (efd::utf8string& guid);

            /**
            All asset web metadata is authored by a particular PC, whenever it opens an
            AssetWeb in AWEB_READ_WRITE mode. Each PC is assigned a unique GUID value.
            All asset web metadata that is authored by that PC is imprinted with the same
            unique value, so that the source of all asset webs can be ascertained.  

            @param guid [out] The unique ID value
            @return True if an ID is found and read from the system-specific location
            */
            bool getAuthorId (efd::utf8string& guid);
            bool getWebAuthorId (efd::utf8string& guid);

            /// Determine whether the AssetWeb was opened in AWEB_READ_WRITE mode
            bool isWritable();

            /**
             This is the main query method used to locate all assets that are matched
             to a particular URI.

             The URI always starts with 'urn', and can be either a colon separated
             list of tags, a UUID value, or a urn that includes a path specification
             that must start with a '/' character.

             Ex.
             urn:tag1:tag2
             urn:uuid:24270c23-77b3-434a-b31c-c301f4536e42
             urn:/Level01:gamebryo-scenegraph

             @param uri The URI value
             @param triples [out] List of Triple objects for matches. Each triple includes
             the asset id as the subject (first) value.
            */
            void findAllAssets (const efd::utf8string& uri, TripleSet& triples);

            /**
             Query method used for incremental searching. The existing search results in
             'triples' are refined by the results of querying for assets that have a given
             property or tag value.

             @param prop The property name used in the search
             @param value The value that the property must match
             @param triples [in,out] incremental search results
            */
            void incrementalFindAssets (const efd::utf8string& prop,
                const efd::utf8string& value,
                TripleSet& triples);

            /**
             Perform an in-place update on a list of triples so that each triple will contain
             the tuple (Asset Id, GlobalNamespace("name"), Asset Name).

             @param triples Set of triples where each subject is an Asset Id. The predicate will
             be replace with the "name" predicate and the object will be replaced with the
             asset's name value.
            */
            void normalAssetList (TripleSet& triples);

            /**
             Retrieve a property value for a specific asset using its unique uuid.

             @param uuri The unique Id for the asset using format urn:uuid:X
             @param predicateTag The predicate for the property of interest
             @param value [out] Value of the requested property
             @return True if the asset/property is found
            */
            bool assetPropertyValue (const efd::utf8string& uuri,
                const efd::utf8string& predicateTag,
                efd::utf8string& value);

            /**
            Determine whether an asset possesses a particular tag value.

            @param uuri The unique Id for the asset using format urn:uuid:X
            @param value The tag value to query
            */
            efd::Bool assetHasTag (const efd::utf8string& uuri, const efd::utf8string& value);

            /**
            Add a new tag for a specific asset using its unique uuid.

            @param uuri The unique Id for the asset using format urn:uuid:X
            @param value The tag value to add
            */
            void assetAddTag (const efd::utf8string& uuri, const efd::utf8string& value);

            /**
            Add a new property for a specific asset using its unique uuid.

            @param uuri The unique Id for the asset using format urn:uuid:X
            @param prop The name of the property that is added
            @param value The value for the new property
            */
            void assetAddProperty(const utf8string& uuri,
                                  const utf8string& prop, const utf8string& value);

            /**
            Add a new triple to a SimpleGraph object

            @param graph Instance of a SimpleGraph. New triple will be merged.
            @param subject The subject, normally a physical asset id value
            @param prop The name of the property
            @param value The value for the property
            */
            void addTripleToGraph(SimpleGraph& graph,
                const utf8string& subject,
                const utf8string& prop,
                const utf8string& value);

            /**
             Retrieve all of the tags for a specific asset using its unique uuid.

             @param uuri The unique Id for the asset using format urn:uuid:X
             @param value [out] URI containing all tags and the name property
             @return True if the asset is found
            */
            bool assetAllTags (const efd::utf8string& uuri, efd::utf8string& value);

            /**
            Retrieve all of the label tags for a specific asset using is unique uuid.

            @see assetAllTags
            */
            bool assetLabelTags (const efd::utf8string& uuri, efd::utf8string& value);

            /**
            Retrieve all of the classification tags for a specific asset using is unique uuid.

            @see assetAllTags
            */
            bool assetEnumTags (const efd::utf8string& uuri,
                                efd::utf8string& value,
                                efd::UInt32* count=NULL);

            /** 
             Simple conversion of relative path to absolute path for any path that falls within
             the asset-web.

             @param relative The relative path of a file in the asset-web
             @param absolute [out] Absolute path to file
            */
            void assetAbsolutePath (const efd::utf8string& relative, efd::utf8string& absolute);

            /**
             Lookup the unique Id for an asset that is stored at a certain location

             @param relativePath The relative path location where asset is stored
             @param assetId [out] Asset Id for the asset
             @return True if the asset is known by the asset-web
            */
            bool assetPathLookup (const efd::utf8string& relativePath, efd::utf8string& assetId);

            /**
            Lookup the set of assets that all have a given property value

            @param predicate The name of the property
            @param value The value of the property to search for
            @param matches [out] Set of triples that match the criteria
            */
            bool assetWithPropertyValue (const efd::utf8string& predicate,
                                         const efd::utf8string& value,
                                         TripleSet& matches);

            /**
             Load persistent metadata file for an asset.

             The persistent metadata is stored next to the asset file, in a sub-directory,
             with an extension of "_.nt" appended to the name. If such a file exists for the
             asset, then it is loaded and that metadata is applied to the asset.

             @param uuri The Asset Id for the asset
            */
            void assetLoadPersistentMetadata (const efd::utf8string& uuri);

            /**
             Insert a new asset into the asset-web's metadata.

             @param relativePath The relative location of the file in the asset-web directory
             @param outAssetId [out] The assetID that was inserted.
             @return True if the asset is successfully inserted.
             
            */
            bool insertAsset (const efd::utf8string& relativePath, efd::utf8string& outAssetId);

            /**
             Remove an asset from the asset-web's metadata.

             @param assetId Asset Id of the asset that is to be removed
             @return True if the asset is found and successfully removed
            */
            bool removeAsset (const efd::utf8string& assetId);

            /**
             Generate a unique identifier which can be used as an asset Id

             @param assetId [out] The generated ID using the format urn:uuid:X
            */
            void generateUUID (efd::utf8string& assetId);

            /// Creates, loads and parses metadata graphs on demand
            /// Internal use only.
            void createGraph(const efd::utf8string& name, const efd::utf8string& predicate,
                bool isIndexed);
            Graph* openGraph(const efd::utf8string& name);
            SimpleGraph* openSimpleGraph(const efd::utf8string& name);

            /// Unloads graph data cached in memory
            /// Internal use only.
            void closeGraph(const efd::utf8string& name);

            /// Retrieve all of the user-defined labels with their type & value
            void getUserTags(efd::vector<AssetTagDescriptor>& tags);

            /// Retrieve all of the user-defined classifications with their type & value
            void getEnumTags(efd::vector<AssetTagDescriptor>& tags);

            /// Retrieve all of the standard tags (asset/media types) with their type & value
            void getStdTags(efd::vector<AssetTagDescriptor>& tags);

            /// Retrieves the Asset-Web Root path.
            const efd::utf8string& GetAssetWebRoot() const;

#ifdef EE_PLATFORM_WIN32
            /// Retrieve the last time of synchronizing metadata
            void GetLastSyncTime(PathUtils::FileTimestamp* timestamp);
#endif
            //@}

        private:
            /// Collection of graphs of RDF data that are either loaded or can be loaded on demand
            efd::map<efd::utf8string, Graph> m_Graphs;
            /// Collection of graphs representing the known tag values (metadata about the metadata)
            efd::map<efd::utf8string, SimpleGraph> m_TagGraphs;
            /// Collection of all the known tags used for the metadata
            efd::vector<AssetTagDescriptor> m_tagSoup;
            /// Collection of asset tagger classes
            efd::map<efd::utf8string, efd::AssetTaggerPtr> m_asset_taggers;

            /// Path value for the asset-web directory
            efd::utf8string m_location;
            /// Unique instance id used to lock write access to the metadata
            efd::SystemUniqueIndex m_assetWebUniqueIndex;
            /// Path value to where the metadata files are stored
            efd::utf8string m_meta_location;
            /// Access mode specified when openWeb() is called. @see openWeb()
            AWebAccessMode m_mode;
#ifdef EE_PLATFORM_WIN32
            /// Last time asset metadata was synced
            PathUtils::FileTimestamp m_last_sync;
#endif
            /// Indicates that optimized metadata is invalidated, and needs to be re-built
            efd::Bool m_invalid_metadata;

            /// Private methods used to load all the known tag values.
            void loadTagSoup();
            void loadTagGraph (const efd::utf8string& graph_name, const efd::AWebTagType tag_type);
            void addTagValue (const efd::utf8string& value, const efd::AWebTagType tag_type);
            void getTagSet(efd::vector<AssetTagDescriptor>& tags, const efd::AWebTagType tag_type);
            void getAssetTags(const utf8string& uuri, TripleSet& triples);

            /// Used when AssetWeb mode is AWEB_READ_WRITE and it has been determined that the
            /// generated metadata files are invalid, and need to be re-generated.
            void InvalidateMetadata();

            /// For checking version stamps on metadata
            const static efd::utf8string ASSET_WEB_META_VERSION;
            efd::Bool updateVersion();

            /// Used when AssetWeb mode is AWEB_READ_WRITE to record the status of persisting
            /// the metadata files (i.e. creating a metadata "checkpoint")
            efd::Bool updateCheckpoint(const efd::utf8string& metadata_file);
            efd::Bool makeCheckpoint(efd::Bool success);
            void updateSyncTime();

            /// Used to compute a hash value for a data file. Stored with the checkpoint metadata.
            void getDataFileHash (const utf8string& file_name, utf8string& hash);

            /// Used when AssetWeb mode is AWEB_READ_WRITE to incorporate side-by-side metadata
            /// files into the cache of optimized metadata.
            void mergeMetadataFile(const efd::utf8string& file_name, efd::utf8string& assetId);
            void assetMetadataPath(const efd::utf8string& relative_path,
                                   efd::utf8string& metadata_path);

            /// Private query methods
            void subjectQueryTriple (const efd::utf8string& tagPredicate,
                                     const efd::utf8string& objectLiteral,
                                     Triple& q);
            void objectQueryTriple (const efd::utf8string& subject,
                                    const efd::utf8string& tagPredicate,
                                    Triple& q);
            void closedQueryTriple (const efd::utf8string& subject,
                                    const efd::utf8string& tagPredicate,
                                    const efd::utf8string& tagValue,
                                    Triple& q);

            bool assetsWithPredicate(
                const efd::utf8string& predicate,
                TripleSet& matches );
            bool assetWithPathValue (
                const efd::utf8string& value,
                TripleSet& matches);

            static bool compPathMatch (const Triple& a, const Triple& b);

            efd::Bool resolveOverrideTag(const efd::utf8string& tag_value, TripleSet& triples);
            efd::Bool resolveOverridePath(const efd::utf8string& tag_value, TripleSet& triples);
            void pruneAssetClasses (TripleSet& triples, efd::UInt32 max_classes);
#ifdef EE_PLATFORM_WIN32
            bool getSystemAuthorId(efd::utf8string& guid);
#endif
    };
}

#endif

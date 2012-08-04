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
#ifndef _ASSETSERVER_WEB_URIREADER_H
#define _ASSETSERVER_WEB_URIREADER_H

#include <efd/UniversalTypes.h>
#include <efd/utf8string.h>

namespace efd
{
    /// Tags have a type and a value
    typedef enum {
        AWEB_NONE_TAG_TYPE,
        AWEB_STANDARD_TAG_TYPE,
        AWEB_LABEL_TAG_TYPE,
        AWEB_CLASSIFICATION_TAG_TYPE
    } AWebTagType;
    struct AssetTagDescriptor {
        AWebTagType ttype;
        efd::utf8string tvalue;
        efd::Bool operator ==(const efd::AssetTagDescriptor& other) const
        {
            return ( (ttype==other.ttype) && (tvalue==other.tvalue) );
        };
    };

    /**
     This class provides methods for creating and dealing with URIs as they are used by
     the other Asset Service classes. The fundamental purpose is to parse a URI string into
     its constituent parts, like individual tags, the asset name, the asset Id, etc.
    */
    class EE_EFD_ENTRY URIReader
    {
        public:
            /// @name Constants
            /// We define several string constants for predicates used in the asset web metadata.
            //@{
           
            const static utf8string GlobalNamespace;
            const static utf8string GlobalNamespace_tag;
            const static utf8string GlobalNamespace_label;
            const static utf8string GlobalNamespace_class;
            const static utf8string GlobalNamespace_name;
            const static utf8string GlobalNamespace_relpath;
            const static utf8string GlobalNamespace_canonical;
            const static utf8string GlobalNamespace_llid;
            const static utf8string GlobalNamespace_logpath;
            const static utf8string GlobalNamespace_stdtags;
            const static utf8string GlobalNamespace_stdtags_value;
            const static utf8string GlobalNamespace_usertags;
            const static utf8string GlobalNamespace_usertags_value;
            const static utf8string GlobalNamespace_userenums;
            const static utf8string GlobalNamespace_userenums_value;
            const static utf8string GlobalNamespace_ids;
            const static utf8string GlobalNamespace_ids_instance;
            const static utf8string GlobalNamespace_ids_author;
            const static utf8string GlobalNamespace_ckpt;
            const static utf8string GlobalNamespace_ckpt_success;
            const static utf8string GlobalNamespace_ckpt_hash;
            const static utf8string GlobalNamespace_ckpt_version;
             //@}

            /// @name Constructors
            //@{
            /// Create a URIReader class from the given URI string value.
            URIReader (const efd::utf8string& uri);
            /// Create a URIReader class for the 'uri' string, but allow it to contain any tags in 'tagNames'.
            URIReader (const efd::utf8string& uri, const efd::vector<AssetTagDescriptor>& tagNames);
            //@}

            /// Specifies whether the URI represents a valid physical asset id (format urn:uuid:X)
            static efd::Bool isAssetId (const utf8string& uri);
            efd::Bool isAssetId();

            /// Retrieves the URI in the typical asset id format like 'urn:uuid:XXXXX'
            void getAssetId (efd::utf8string& uri);

            /// Retrieve the scheme of the URI. Should always be equal to 'urn'
            void getScheme(efd::utf8string& scheme);

            /// Retrieve the asset name element of the URI, if present
            void getNameLiteral(efd::utf8string& name);

            /// Retrieve the canonical path for the asset URI, if present
            void getCanonicalLiteral(efd::utf8string& name);

            /// Retrieve the logical asset id for the asset URI, if present
            void getLogicalId(efd::utf8string& id);

            /// Retrieve a list of all the tags in the URI
            void getTagLiterals(efd::vector<efd::utf8string>& tagNames);

            /// Retrieve a list of all the label tags in the URI
            void getLabels(efd::vector<efd::utf8string>& labelNames);

            /// Retrieve a list of all the classification tags in the URI
            void getClassifications(efd::vector<efd::utf8string>& classNames);

            /// Check and return True if 'tag' is one of the tags in the URI
            efd::Bool hasTag(const efd::utf8string& tag);

            /// Retrieve the decimal value of the upper 8 bits of the name for isAssetId()==True
            int UpperBitsValue();

        private:
            /// Private method that does the URI parsing
            void splitURI(const efd::vector<efd::AssetTagDescriptor>& tagNames);

            /// List of tags that the URI has
            efd::map<efd::utf8string, AWebTagType> m_uriTags;
            efd::vector<efd::utf8string> m_uriOverrides;

            /// Parts of the URI
            efd::utf8string m_uri, m_scheme, m_canonical, m_name, m_logical_id;
            /// Flag to indicate the URI has Asset Id format like 'urn:uuid:X'
            bool m_isUid;
    };
}

#endif


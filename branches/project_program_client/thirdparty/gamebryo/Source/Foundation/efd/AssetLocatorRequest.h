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
#ifndef EE_ASSETLOCATORREQUEST_H
#define EE_ASSETLOCATORREQUEST_H

#include <efd/IAssetLocatorService.h>
#include <efd/MessageService.h>

namespace efd
{
/**
    Message class for the AssetLocatorService request messages.
*/
    class EE_EFD_ENTRY AssetLocatorRequest: public IMessage
    {
        EE_DECLARE_CLASS1(AssetLocatorRequest, efd::kCLASSID_AssetLocatorRequest, IMessage);
        EE_DECLARE_CONCRETE_REFCOUNT;

    public:

        AssetLocatorRequest();

        /**
         Set URI value for request

         @param uri_request Asset URI value. Should be of the form for either
            and asset Id (urn:uuid:[GUID value]) or an asset URI (urn:tag1:tag2:etc.)
        */
        virtual void SetURI (const efd::utf8string& uri_request);

        /**
         Get URI value for request

         @return const utf8string Value of the message's asset URI
        */
        virtual const efd::utf8string& GetURI() const;

        /**
        Set private process callback category

        @param callback A category to use to respond to the requesting process
        */
        virtual void CallbackCategory (const efd::Category& callback);

        /**
        Get private process callback category

        @return const Category A category to use to respond to the requesting process
        */
        virtual const efd::Category& CallbackCategory() const;

        /**
        Set private client callback category

        @param callback A category to use to respond to the requesting client
        */
        virtual void ClientCategory (const efd::Category& callback);

        /**
        Get private client callback category

        @return const Category A category to use to respond to the requesting client
        */
        virtual const efd::Category& ClientCategory() const;

        // Overridden virtual functions inherit base documentation and thus are not documented here.
        virtual void ToStream( efd::DataStream& stream ) const;

        // Overridden virtual functions inherit base documentation and thus are not documented here.
        virtual bool FromStream( const efd::DataStream& stream );

        /**
         Helper function to factory a message of a specific type
        */
        static efd::SmartPointer<AssetLocatorRequest> CreateTypedMessage( efd::ClassID msgClassID, MessageService* pMessageService )
        {
            efd::IBasePtr spBase = pMessageService->CreateObject( msgClassID );
            EE_ASSERT( spBase );   //if this asserts, you may have forgotten to call the Register with this type
            return EE_DYNAMIC_CAST(AssetLocatorRequest, spBase);
        }

    protected:

        /// Asset URI value
        efd::utf8string m_uri;
        /// Categories used to route response messages
        efd::Category m_process_callback, m_client_callback;

    };

    typedef efd::SmartPointer<AssetLocatorRequest> AssetLocatorRequestPtr;

    /**
        Message for requesting asset data from an AssetServer.  This request can either ask for
        the entire asset or a range of bytes. If the 'FullFetch' flag is true the request indicates
        the caller wants the entire asset. If this is false, the caller is requesting just
        a subset of the asset, as described by the offset and size parameters.

        Fetch requests require a specific callback category to avoid broadcasting large assets.
     */
    class EE_EFD_ENTRY FetchAssetDataRequest : public AssetLocatorRequest
    {
    public:
        FetchAssetDataRequest() : m_offset(0), m_size(0), m_fullFetch(true) { }

        // Overridden virtual functions inherit base documentation and thus are not documented here.
        virtual void ToStream( efd::DataStream& stream ) const;

        // Overridden virtual functions inherit base documentation and thus are not documented here.
        virtual bool FromStream( const efd::DataStream& stream );

        /**
         Set the instance Id of the Asset Web used for the request.

         @param val utf8string The Instance Id GUID value. @see AssetWeb::getInstanceId
        */
        void AssetWebInstanceId (efd::utf8string& val)
        {
            m_instanceId = val;
        }

        /**
         Retrieve the instance Id of the Asset Web used for the request.

         @return const utf8string The Instance Id GUID value. @see AssetWeb::getInstanceId()
        */
        const efd::utf8string& AssetWebInstanceId() const
        {
            return m_instanceId;
        }

        /**
         Set whether the fetch request should retrieve the entire asset data.

         @param val True if a full fetch is desired, else False. @see StartOffset()
        */
        void FullFetch(bool val)
        {
            m_fullFetch = val;
        }

        /**
         Get whether the fetch request should retrieve the entire asset data.

         @return bool True value indicates full fetch
        */
        bool IsFullFetch() const
        {
            return m_fullFetch;
        }

        /**
         Set byte offset for reading the asset when a partial fetch is required.

         @param offset The offset in bytes
        */
        void StartOffset(efd::UInt32 offset) 
        {
            m_offset = offset;
            m_fullFetch = false;
        }

        /**
         Get byte offset for reading the asset when a partial fetch is required.

         @return UInt32 The byte offset
        */
        efd::UInt32 StartOffset() const
        {
            return m_offset;
        }

        /**
        Set byte count for reading the asset when a partial fetch is required.

        @param size The size in bytes
        */
       void BytesToRead(efd::UInt32 size)
        {
            m_size = size;
            m_fullFetch = false;
        }

       /**
        Get byte count for reading the asset when a partial fetch is required.

        @return UInt32 The size in bytes
       */
        efd::UInt32 BytesToRead() const
        {
            return m_size;
        }

    private:
        /// Instance Id for Asset Web requesting the fetch
        efd::utf8string m_instanceId;
        /// Byte offset for partial fetch
        efd::UInt32 m_offset;
        /// Byte count for partial fetch
        efd::UInt32 m_size;
        /// Indicate full or partial fetch
        efd::Bool m_fullFetch;
    };

    typedef efd::SmartPointer<FetchAssetDataRequest> FetchAssetDataRequestPtr;

    /**
     Request/confirm configuration change for AssetServer.  This request is used to ask the Asset
     Server to switch to a different asset-web path.
    */
    class EE_EFD_ENTRY AssetConfigurationMessage : public IMessage
    {
    public:
        AssetConfigurationMessage() : m_variant(ASSET_CONFIG_LOCAL) { }

        // Overridden virtual functions inherit base documentation and thus are not documented here.
        virtual void ToStream( efd::DataStream& stream ) const;

        // Overridden virtual functions inherit base documentation and thus are not documented here.
        virtual bool FromStream( const efd::DataStream& stream );

        /**
         Set the path of the asset-web that is being requested

         @param val The path to the asset-web. May be a relative path if the working directory of the
            server is known by the client. Otherwise, use an absolute path compliant with the server's
            file system.
        */
        void SetAssetWebPath (const efd::utf8string& val)
        {
            m_webRoot = val;
        }       

        /**
         Get the path for the asset-web that is being requested

         @return utf8string The path value
        */
        const efd::utf8string& GetAssetWebPath() const
        {
            return m_webRoot;
        }

        /**
         Set the message variant that indicates if the configuration is about the remote asset 
         web or local. 

         @param val true if the configuration is about the remote asset web, false if local.
        */
        void SetRemoteConfigFlag(bool val)
        {
            m_variant = val ? ASSET_CONFIG_REMOTE : ASSET_CONFIG_LOCAL;
        }       

        /**
         Get the variant that indicates if the configuration is about the remote asset web or local.

         @return true if the message is about the remote asset web configuration.
        */
        bool IsRemoteConfig() const
        {
            return m_variant == ASSET_CONFIG_REMOTE;
        }

    private:
        /// Asset-web path value
        efd::utf8string m_webRoot;

        /// The message variant used only by the local AssetConfigReponse messages, which are sent
        /// by the local asset locator service.
        enum
        {
            ASSET_CONFIG_LOCAL,
            ASSET_CONFIG_REMOTE
        };
        efd::UInt32 m_variant;
    };

    typedef efd::SmartPointer<AssetConfigurationMessage> AssetConfigurationMessagePtr;

    /**
    Request asset metadata tag information from the Asset Server.
    */
    class EE_EFD_ENTRY AssetTagsInfoRequest : public IMessage
    {
    public:
        AssetTagsInfoRequest() { }

        // Overridden virtual functions inherit base documentation and thus are not documented here.
        virtual void ToStream( efd::DataStream& stream ) const;

        // Overridden virtual functions inherit base documentation and thus are not documented here.
        virtual bool FromStream( const efd::DataStream& stream );

        /**
        Specify the category of tags to retrieve

        @param val One of the AssetTagCategory enum values
        */
        void SetCategory (const efd::IAssetLocatorService::AssetTagCategory val)
        {
            m_category = val;
        }

        /**
        Get the category of tags

        @return AssetTagCategory The category value
        */
        const efd::IAssetLocatorService::AssetTagCategory& GetCategory() const
        {
            return m_category;
        }

    private:
        efd::IAssetLocatorService::AssetTagCategory m_category;

    };

    typedef efd::SmartPointer<AssetTagsInfoRequest> AssetTagsInfoRequestPtr;

} // end namespace efd

#endif // EE_ASSETLOCATORREQUEST_H


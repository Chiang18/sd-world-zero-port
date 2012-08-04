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
#ifndef RELOADMANAGER_H
#define RELOADMANAGER_H

#include <efd/ISystemService.h>
#include <efd/utf8string.h>
#include <efd/Category.h>
#include <efd/AssetLocatorResponse.h>
#include <efd/MessageService.h>


namespace efd
{
    /**
     Interface class for an asset change handler.
    
     The asset change handler is called by the ReloadManager when an asset change is 
     detected and that asset is tagged appropriately. Classes that want these notifications
     should extend this interface and implement the HandleAssetChange callback method.
    */
    class EE_EFD_ENTRY AssetChangeHandler
    {
    public:
        virtual ~AssetChangeHandler() {}

        /**
            Pure virtual interface method to implement an Asset Change Handler instance.

            @param assetId The ID of the asset that changed.
            @param tag the matching tag, the same as that used to register the handler.
        */
        virtual void HandleAssetChange(
            const efd::utf8string& assetId,
            const efd::utf8string& tag) = 0;
    };

    /**
     Central multiplexer for forwarding asset change notify events to interested parties.

     Components register with the ReloadManager a callback that's called when an asset change 
     is detected with tag(s) that match handler criteria. The ReloadManager itself listens for
     Asset Change Notify events.
    */
    class EE_EFD_ENTRY ReloadManager : public efd::ISystemService
    {
        EE_DECLARE_CLASS1(ReloadManager, efd::kCLASSID_ReloadManager, efd::ISystemService);
        EE_DECLARE_CONCRETE_REFCOUNT;

    public:
        ReloadManager();

        virtual ~ReloadManager();

        // Overridden virtual functions inherit base documentation and thus are not documented here.
        virtual efd::AsyncResult OnTick();

        // Overridden virtual functions inherit base documentation and thus are not documented here.
        virtual efd::AsyncResult OnInit();

        // Overridden virtual functions inherit base documentation and thus are not documented here.
        virtual efd::AsyncResult OnShutdown();

        // Overridden virtual functions inherit base documentation and thus are not documented here.
        const char* GetDisplayName() const
        {
            return "ReloadManager";
        }

        /**
        Get the listener channel for the reload manager. Change notifications should be sent to this
        category.
        */
        static efd::Category GetReloadManagerCategory();

        /**
         Handle asset locator response messages - Asset Change Notify messages in particular.
        */
        void HandleAssetLocatorResponse(
            const efd::AssetLocatorResponse* pMessage,
            efd::Category targetChannel);

        /**
         Register a callback handler that gets invoked when an asset with the given
         tag is changed.

         @param tag The tag value that the handler is interested in. Ex. "gamebryo-animation", if
            the handler should be called any time an asset tagged as an animation is changed.
         @param handler Pointer to the handler method
        */
        void RegisterAssetChangeHandler(
            const efd::utf8string& tag,
            efd::AssetChangeHandler* handler);

    private:
        void InvokeHandlers(
            const efd::utf8string &uuid,
            const efd::AssetLocatorResponse::AssetLoc& assetLoc);


        // list of asset change handler pointers
        typedef efd::vector<efd::AssetChangeHandler*> AssetChangeHandlerList;

        // map of asset tag to handler list.
        typedef efd::map<efd::utf8string, AssetChangeHandlerList> AssetChangeHandlerMap;

        efd::MessageServicePtr m_spMessageService;

        // map of changed assets to their tuples
        efd::AssetLocatorResponse::AssetURLMap m_changedAssets;

        // map of asset tags to change handlers
        AssetChangeHandlerMap m_changeHandlers;
    };

    typedef efd::SmartPointer<ReloadManager> ReloadManagerPtr;

} // end namespace efd

#endif // RELOADMANAGER_H

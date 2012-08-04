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

#include <efd/ILogger.h>

namespace efd
{

//-------------------------------------------------------------------------------------------------
template<typename KeyType, typename ValueType>
inline MaskedKeyHash<KeyType, ValueType>::MaskedKeyToValueListMap::MaskedKeyToValueListMap(
    const KeyType& mask)
{
    m_mask = mask;
}
//-------------------------------------------------------------------------------------------------
template<typename KeyType, typename ValueType>
inline MaskedKeyHash<KeyType, ValueType>::MaskedKeyHash()
{
}
//-------------------------------------------------------------------------------------------------
template<typename KeyType, typename ValueType>
inline MaskedKeyHash<KeyType, ValueType>::~MaskedKeyHash()
{
    clear();
}
//-------------------------------------------------------------------------------------------------
template<typename KeyType, typename ValueType>
inline void MaskedKeyHash<KeyType, ValueType>::AddValue(KeyType key, KeyType mask,
    ValueType value)
{
    ValueListPtr  spValueList;

    EE_LOG(efd::kNetMessage, efd::ILogger::kLVL3,
        ("Subscribing internal listener with cat = %s mask = %s", key.ToString().c_str(),
        mask.ToString().c_str()));

    // First, create category match within given mask.
    KeyType masked = mask & key;

    MaskedKeyToValueListMapPtr spCM;
    if (m_maskedKeyStorage.find(mask, spCM) == true)
    {
        // If here, then already have an entry with current mask bits, so add to their category
        // map.
        if (spCM->find(masked, spValueList) == true)
        {
            // If here, we are adding another listener to an existing category.
            EE_LOG(efd::kNetMessage, efd::ILogger::kLVL3, (
                "Subscription to same cat/mask (%s) as existing one, adding listener (count %d)",
                masked.ToString().c_str(), spValueList->size()+1));
            spValueList->push_back(value);
        }
        else
        {
            // If here, it is o.k. to save a new category for this mask.
            // Note:  We are setting masked category, such that when we test later, we can look
            // directly into the hash map with masked search category to test for a match (i.e.,
            // we will not have to iterate through all possible categories and then mask each one
            // to test against search category).
            // If here, then we are adding another listener to an existing category.
            EE_LOG(efd::kNetMessage, efd::ILogger::kLVL3,
                ("Subscription to new cat/mask (%s), adding listener (count = 1)",
                masked.ToString().c_str()));
            spValueList = EE_NEW ValueList; // Create a new list.
            spValueList->push_back(value);  // Put first member into it.
            (*spCM)[ masked ] = spValueList;    // Set list into new hash table.
        }
    }
    else
    {
        // If here, then this category is the first one for the new mask.
        EE_LOG(efd::kNetMessage, efd::ILogger::kLVL3,
            ("Subscription to new mask, adding mask (%s) and listener (count = 1)",
            mask.ToString().c_str()));
        spValueList = EE_NEW ValueList; // Create a new list.
        spValueList->push_back(value);  // Put the first member into it.
        spCM = EE_NEW MaskedKeyToValueListMap(mask);    // Create a new cat map for the new mask.

        (*spCM)[masked] = spValueList;  // Set listener list into new hash table.
        m_maskedKeyStorage[mask] = spCM;    // Set cat map into mask map.
    }
}
//-------------------------------------------------------------------------------------------------
template<typename KeyType, typename ValueType>
inline void MaskedKeyHash<KeyType, ValueType>::RemoveValue(KeyType key, KeyType mask,
    ValueType value)
{
    typename KeyToMaskedKeyToValueListMapMap::iterator maskPos = m_maskedKeyStorage.find(mask);
    if (maskPos != m_maskedKeyStorage.end())
    {
        MaskedKeyToValueListMapPtr spCM = maskPos->second;

        // Create a masked category for use in the category table (all categories in this table
        // have already been masked by this mask).
        KeyType maskCat = mask & key;

        // If here, then we have an entry with current mask bits, (no error).
        typename MaskedKeyToValueListMap::iterator iterMapCap = spCM->find(maskCat);
        if (iterMapCap != spCM->end())
        {
            ValueListPtr spValueList = iterMapCap->second;
            // If here, then we found a matching category of listeners.  We need to iterate down
            // the list of listeners to find previously registered ones.
            for (typename ValueList::iterator iterValue = spValueList->begin();
                iterValue != spValueList->end(); ++iterValue)
            {
                if (value == *iterValue)
                {
                    // Found a listener to remove.
                    spValueList->erase(iterValue);  // Remove found listener.
                    if (spValueList->size() == 0)
                    {
                        // If here, then that was the only listener on the list, so remove the
                        // category entry.
                        spCM->erase(iterMapCap);

                        if (spCM->size() == 0)
                        {
                            // If here, that category of listeners was the only one for this
                            // mask, so remove the mask entry.
                            m_maskedKeyStorage.erase(maskPos);
                        }
                    }

                    // No deletes are required, since smart pointers are being used.
                    return; // Done with update of subscriber map/list.
                }
            }
        }
        else
        {
            // If here, then we failed to find an existing category to remove.
            EE_LOG(efd::kNetMessage, efd::ILogger::kERR2, (
                "Error: No matching category found for unsubscribe attempt.  Attempt ignored"));

            // Tick off that another error occurred.
            METRICS_PROBE_METRIC("NETSERVICE.SUBSCRIBE.COUNT.ERROR", 1);
        }
    }
    else
    {
        // If here, then we do not have a mask that matches given to unsubscribe.
        EE_LOG(efd::kNetMessage, efd::ILogger::kERR2, (
            "Error: No matching mask found for unsubscribe attempt.  Attempt ignored"));

        // Tick off that another error occurred.
        METRICS_PROBE_METRIC("NETSERVICE.SUBSCRIBE.COUNT.ERROR", 1);
    }
}
//-------------------------------------------------------------------------------------------------
template<typename KeyType, typename ValueType>
inline void MaskedKeyHash<KeyType, ValueType>::RemoveValue(ValueType value)
{
    typename KeyToMaskedKeyToValueListMapMap::iterator maskPos = m_maskedKeyStorage.begin();
    while (maskPos != m_maskedKeyStorage.end())
    {
        bool erasedMaskedKeyStorage = false;

        // Iterate through all masks.
        MaskedKeyToValueListMapPtr spCM = maskPos->second;

        // If here, then we found a matching category of listeners.  We need to iterate down the
        // list of listeners to find previously registered.
        typename MaskedKeyToValueListMap::iterator iterMapCap = spCM->begin();
        while (iterMapCap != spCM->end())
        {
            bool erasedIterMapCap = false;
            ValueListPtr spValueList = iterMapCap->second;
            // If here, then we found a matching category of listeners.  We need to iterate down
            // the list of listeners to find previously registered.
            typename ValueList::iterator iterValue = spValueList->begin();
            while (iterValue != spValueList->end())
            {
                if (value == *iterValue)
                {
                    typename ValueList::iterator eraseIterValue = iterValue;
                    ++iterValue;
                    // Found a listener to remove.
                    spValueList->erase(eraseIterValue); // Remove found listener.
                    if (spValueList->size() == 0)
                    {
                        typename MaskedKeyToValueListMap::iterator eraseIterMapCap = iterMapCap;
                        ++iterMapCap;

                        // If here, then that listener was the only one on the list, so remove the
                        // category entry.
                        spCM->erase(eraseIterMapCap);
                        erasedIterMapCap = true;

                        if (spCM->size() == 0)
                        {
                            typename KeyToMaskedKeyToValueListMapMap::iterator erasePos = maskPos;
                            ++maskPos;

                            // If here, then that category of listeners was the only one for this
                            // mask, so remove the mask entry.
                            m_maskedKeyStorage.erase(erasePos);
                            erasedMaskedKeyStorage = true;

                            // Make sure we do not use this pointer again.
                            spCM = NULL;
                        }

                        // Make sure that we do not use this pointer again.
                        spValueList = NULL;

                        break;  // We are done with the iterValue while loop.
                    }
                }
                else
                {
                    ++iterValue;
                }
            }

            if (!erasedIterMapCap)
                ++iterMapCap;
            else
                break;  // We are done with the iterMapCap while loop.
        }

        if (!erasedMaskedKeyStorage)
            ++maskPos;
    }
}
//-------------------------------------------------------------------------------------------------
template<typename KeyType, typename ValueType>
inline typename MaskedKeyHash<KeyType, ValueType>::ValueSetPtr
    MaskedKeyHash<KeyType, ValueType>::GetValues(KeyType key)
{
    ValueSetPtr spReturnValues = EE_NEW ValueSet();

    // Iterate through all the masks; then for each one, check if a match is present for the
    // specified category (masked with the current mask, of course).
    for (typename KeyToMaskedKeyToValueListMapMap::iterator maskPos = m_maskedKeyStorage.begin();
        maskPos != m_maskedKeyStorage.end(); ++maskPos)
    {
        // Iterate through all masks.
        MaskedKeyToValueListMapPtr spCM = maskPos->second;
        if (spCM)
        {
            // If we have a valid pointer to a category map, then first, create a category masked
            // with the current mask.
            KeyType maskedKey = key & spCM->m_mask;

            // Then see if we have a matching category for this mask.
            ValueListPtr spValueList;

            if (spCM->find(maskedKey, spValueList))
            {
                // If an entry exists that matches the current mask and category, then we have at
                // least one listener, so return the entire list.  Copy all entries to the "found"
                // list, since there may be multiple matches.
                for (typename ValueList::iterator iterValue = spValueList->begin();
                    iterValue != spValueList->end(); ++iterValue)
                {
                    // Append the list to the list of all found listeners to the given category.
                    spReturnValues->insert(*iterValue);
                }
            }
        }
    }

    if (spReturnValues->size() == 0)
    {
        // If here, then we went through all registered categories and masks with no match.
        return NULL;
    }

    return spReturnValues;
}
//-------------------------------------------------------------------------------------------------
template<typename KeyType, typename ValueType>
inline typename MaskedKeyHash<KeyType, ValueType>::MaskedKeySetPtr
    MaskedKeyHash<KeyType, ValueType>::GetMaskedKeys()
{
    MaskedKeySetPtr spReturnKeys = EE_NEW MaskedKeySet();
    for (typename KeyToMaskedKeyToValueListMapMap::iterator maskPos = m_maskedKeyStorage.begin();
        maskPos != m_maskedKeyStorage.end(); ++maskPos)
    {
        // Iterate through all masks.
        MaskedKeyToValueListMapPtr spCM = maskPos->second;
        if (spCM)
        {
            KeyType mask = spCM->m_mask;
            for (typename MaskedKeyToValueListMap::iterator maskToListPos = spCM->begin();
                 maskToListPos != spCM->end(); ++maskToListPos)
            {
                KeyType key = (*maskToListPos).first;
                spReturnKeys->insert(MaskedKey(key, mask));
            }
        }
    }
    return spReturnKeys;
}
//-------------------------------------------------------------------------------------------------
template<typename KeyType, typename ValueType>
inline void MaskedKeyHash<KeyType, ValueType>::clear()
{
    m_maskedKeyStorage.clear();
}
//-------------------------------------------------------------------------------------------------

}   // End namespace efd

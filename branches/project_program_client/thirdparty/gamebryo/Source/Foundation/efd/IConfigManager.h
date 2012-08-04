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
#ifndef EE_ICONFIGMANAGER_H
#define EE_ICONFIGMANAGER_H

#include <efd/ISystemService.h>
#include <efd/IConfigSection.h>

namespace efd
{

/**
    IConfigManager is the interface class for the Configuration Manager which provides
    applications access to a hierarchical configuration of name/value pairs drawn from multiple
    sources.

    @note See the CommandLineSource and IniSource classes for details on specifying configuration
    information.
*/
class EE_EFD_ENTRY IConfigManager : public ISystemService
{
protected:
    // Grant protected access to the ConfigSource class.
    friend class ConfigSource;

public:
    /// The default SSID of the Configuration Manager
    EE_DECLARE_INTERFACE1(IConfigManager, efd::kCLASSID_IConfigManager, ISystemService);

    /// The character '.' used to separate section names
    const static char kSectionSeperator;

    /// The character '=' used to separate the name from the value in source files and on the
    /// command-line.
    const static char kNameValueSeperator;

    /// The character '>' used to link a name to an existing name/value pair.
    const static char kNameLinkSeperator;

    /// The character '+' used to add an additional file source to the configuration with an
    /// optional section to add it under specified to the left of the separator.
    const static char kNameFileSeperator;

    /// The character '#' used to specify that the name should be converted to a unique name in a
    /// list but replacing the '#' character with unique number in sequence.
    const static char kNameListChar;

    /// Characters that start a variable name in a value or source file.
    const static char kVarStart[];

    /// Character that ends a variable name in a value or source file
    const static char kVarEnd[];

    /// Set of characters that are illegal to use in a section name
    const static char kIllegalSectionChars[10];

    /// Set of characters that are illegal to use in a value name.
    const static char kIllegalNameChars[10];

    /**
        Writes a copy of the current configuration data to the specified file.

        @param strFileName Filename for the file in which to write the data.
        @param bPrintRelative True means the INI file should be formatted with a relative
            section hierarchy.  False will produce absolute section headers.
    */
    virtual void WriteConfiguration(
        const efd::utf8string& strFileName,
        bool bPrintRelative = true) = 0;

    /**
        Adds a new configuration source to the available sources to be processed during
        initialization.

        @param pSource Pointer to the new source.
        @param iPriority Priority of the configuration source.  It does not determine the order
            that sources are processed.  It does how ever determine what happens when two sources
            attempt to set the same entry.  A higher source will overwrite a value from a lower
            source.
    */
    virtual void AddConfigSource(
        IConfigSource *pSource,
        int iPriority) = 0;

    /**
        Returns the root configuration section that contains all the configuration data and
        sections.
     
        @return Pointer to the root configuration section.
     */
    virtual const ISection *GetConfiguration() = 0;

    /**
        Searches for a given child value by name.
        
        If a dot separated section hierarchy is specified, it will start searching beginning with
        the current section as the parent.  It will then traverse the section hierarchy and find
        the value by name in the section specified.  This version of FindValue cannot distinguish
        between a missing values and a values set to an empty string.

        @note If you need to find multiple values from the same section, then it is more efficient
        to use GetConfiguration() to get the root ISection and then use FindSection() to find the
        section containing the values you need to read.  Then use FindValue() to read each value.
        This approach avoids parsing the section hierarchy multiple times.

        @param strName Name of the section to retrieve.  Dot separated section hierarchy names
            are accepted.
        @return The value, or unchanged if not found.
    */
    virtual efd::utf8string FindValue(const efd::utf8string& strName) const = 0;

    /**
        Searches for a given child value by name (alternate interface).
        
        If a dot separated section hierarchy is specified, it will start searching beginning with
        the current section as the parent.  It will then traverse the section hierarchy and find
        the value by name in the section specified.  This version of FindValue can distinguish
        between missing values and values set to empty strings.

        @note If you need to find multiple values from the same section, then it is more efficient
        to use GetConfiguration to get the root ISection and then use FindSection to find the
        section containing the values you need to read.  Then use FindValue to read each value.
        This approach avoids parsing the section hierarchy multiple times.

        @param strName Name of the section to retrieve.  Dot separated section hierarchy names
            are accepted.
        @param o_value The value, or unchanged if not found.
        @return True if the value was found.
    */
    virtual bool FindValue(
        const efd::utf8string& strName,
        efd::utf8string& o_value ) const = 0;

protected:
    /*
        Adds a new configuration source to the available sources to be processed during
        initialization.
          pSource : Pointer to the new source.
    */
    virtual void AddConfigSourceInternal(IConfigSource *pSource) = 0;
};

/// Define a smart pointer (reference counting, self deleting pointer) for the IConfigManager
/// class.
typedef efd::SmartPointer<IConfigManager> IConfigManagerPtr;

}   // End namespace efd.

#endif // EE_ICONFIGMANAGER_H

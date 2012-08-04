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
#ifndef EE_IDS_H
#define EE_IDS_H

#include <efd/ClassInfo.h>
#include <efd/efdMessageIDs.h>


namespace efd
{

	/// Minimum value for a valid SystemService class ID.
#define EE_INTERNAL_SERVICE_MIN 0x00000001

	/// Maximum value for a SystemService class ID.
#define EE_INTERNAL_SERVICE_MAX 0x07ffffff

	/// Macro used by ServiceManager::RegisterSystemService to verify the service class ID range.
#define EE_ASSERT_SERVICEID_RANGE(ID)\
	EE_ASSERT(ID >= EE_INTERNAL_SERVICE_MIN && ID <= EE_INTERNAL_SERVICE_MAX);

	/// @name External ID Declaration macros
	/// These macros assist with declaring ID's that will not conflict with other existing
	/// IDs.  The normal way to declare an ID is to add it to a data driven enumeration file;
	/// these files ensure that the IDs are assigned in the correct range and do not conflict
	/// with other IDs.  Using a data driven enumeration file also allows name-ID mapping for
	/// use in logging.  If you do not require name-id mapping and wish to instead manually
	/// ensure the uniqueness of the ID values that are used you can use the following macros
	/// instead.  These are most appropriate for small test applications or final executables
	/// and we highly recommend against using these macros in any shared libraries.
	///
	/// The range per external type is as follows:
	///     0x06000000->0x060FFFFF ?Base IDs
	///     0x06100000->0x061FFFFF ?SystemService IDs
	///     0x06200000->0x062FFFFF ?Class IDs
	///     0x06300000?0x06FFFFFF ?Reserved for future use
	//@{

	/// Macro to declare a ClassID for use by an efd::IBase derived class.
#define EE_DECLARE_EXTERNAL_CLASSID(SYMBOL, VALUE)                                      \
	EE_COMPILETIME_ASSERT(VALUE >= 0x06200000 && VALUE <= 0x062fffff);                  \
	static const efd::ClassID SYMBOL = VALUE;

	/// Macro to declare a ClassID for use by a SystemService.  The class ids used for system
	/// service's are more restricted in their range so using this macro instead of the less
	/// restricted EE_DECLARE_EXTERNAL_CLASSID ensures the generated ID is in the correct range.
#define EE_DECLARE_EXTERNAL_SYSTEMSERVICE_ID(SYMBOL, VALUE)                             \
	EE_COMPILETIME_ASSERT(VALUE >= 0x06100000 && VALUE <= 0x061fffff);                  \
	static const efd::ClassID SYMBOL = VALUE;

	/// Macro to declare a BaseId which can be used to construct a Category without conflicting
	/// with other commonly used Category base values (such as ClassIDs and the base value
	/// data driven enumeration files).
#define EE_DECLARE_EXTERNAL_BASEID(SYMBOL, VALUE)                                       \
	EE_COMPILETIME_ASSERT(VALUE >= 0x06000000 && VALUE <= 0x060fffff);                  \
	static const efd::UInt32 SYMBOL = VALUE;

	//@}

} // end namespace efd

#endif //EE_IDS_H

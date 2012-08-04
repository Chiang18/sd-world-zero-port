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
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#pragma once
#ifndef NIFILE_H
#define NIFILE_H

#include <efd/File.h>

// While not technically necessary for NiFile.h to build, NiFile was a subclass of NiBinaryStream
// in previous versions of Gamebryo. This directive preserves backwards compatibility with a
// number of NiBinaryStream functions such as NiStreamLoadBinary and NiStreamSaveBinary.
#include <NiBinaryStream.h>

#define NiFile efd::File

#if defined(_PS3)
#define NIFILE_ERROR_OPEN  FILE_ERROR_OPEN
#define NIFILE_ERROR_CLOSE FILE_ERROR_CLOSE
#define NIFILE_ERROR_SEEK  FILE_ERROR_SEEK
#define NIFILE_ERROR_READ  FILE_ERROR_READ
#define NIFILE_ERROR_WRITE FILE_ERROR_WRITE
#endif

#endif // NIFILE_H

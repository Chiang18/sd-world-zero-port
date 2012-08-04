// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef NITERRAINSECTORFILEVERSION2_H
#define NITERRAINSECTORFILEVERSION2_H

#include "NiTerrainLibType.h"
#include "NiTerrainSectorFile.h"
#include "NiIndex.h"

/** 
    The class is used to iterate over and write terrain sector files.
 */
class NITERRAIN_ENTRY NiTerrainSectorFileVersion2 : public NiTerrainSectorFile
{
    friend class NiTerrainSectorFile;

public:
    
    /**
        Destructor
    */
    virtual ~NiTerrainSectorFileVersion2();

protected:
    /**
        Constructor for this sector file iterator

        @param kSectorFile the sector file to attempt to open
        @param kAccessMode a member of NiFile::OpenMode (either read or write)
    */
    NiTerrainSectorFileVersion2(const char* kSectorFile,
        NiFile::OpenMode kAccessMode);
    
    /**
        Initialize this file reading parser. This function will attempt
        to generate the missing information from the file header 
        (NumLOD was added to the file header after this version)
    */
    virtual bool Initialize();

    /**
        Function used to load a block header from an old version of the file
        format. This basically skips over the values that were stored for
        the sum of all points as bounding information.
    */
    void LoadLegacyFileHeaderBinary(FileHeader& kFileHeader, 
        NiBinaryStream& kStream);
};

NiSmartPointer(NiTerrainSectorFileVersion2);

#endif

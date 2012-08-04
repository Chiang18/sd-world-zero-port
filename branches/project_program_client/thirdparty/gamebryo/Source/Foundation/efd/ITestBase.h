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
#ifndef ITESTBASE_H
#define ITESTBASE_H

/// @cond EMERGENT_INTERNAL

namespace efd
{
    /// All the possible results of a test.
    enum tTestResult
    {
        /// Test resulted in a failure
        kResultFailed,
        /// Test timed out
        kResultTimedOut,
        /// Test resulted in success
        kResultPassed,
        /// Test was skipped
        kResultSkipped,
        /// Test was skipped because it is not implemented yet
        kResultNeeded,
        /// Total number of results possible
        kResultMax
    };
};

/// @endcond

#endif // ITESTBASE_H

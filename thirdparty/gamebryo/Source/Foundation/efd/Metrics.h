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
#ifndef EE_METRICS_H
#define EE_METRICS_H

// Define winsock, etc properly before including metrics headers.
#include <efd/OS.h>

#ifdef EE_METRICS_ENABLED

    #include <MetricsProbeDefs.h>
    #include <FileMetricsConfig.h>
    #include <efd/Helpers.h>

    #define METRICS_STANDARD_FILE_INIT(filename, processname)                                   \
    {                                                                                           \
        eme::IMetricsConfig *pconfig = EE_EXTERNAL_NEW eme::FileMetricsConfig( filename );      \
        pconfig->SetSessionProperty(EE_METRICS_SESSION_PROP_NAME_MODULE_NAME, processname);     \
        METRICS_INIT(pconfig);                                                                  \
    }

    #define METRICS_PROBE_METRIC_FMT(nameWithArgs, data) \
        METRICS_PROBE_METRIC( (const char*)efd::PrintfHelper nameWithArgs , data )

    #define METRICS_ONLY(code) code

#else // EE_METRICS_ENABLED

    #define METRICS_PROBE_METRIC(metric, data)
    #define METRICS_INIT(config)
    #define METRICS_TICK()
    #define METRICS_DEINIT()

    #define METRICS_STANDARD_FILE_INIT(filename, processname)
    #define METRICS_PROBE_METRIC_FMT(nameWithArgs, data)
    #define METRICS_ONLY(code)

#endif // EE_METRICS_ENABLED

#endif // EE_METRICS_H

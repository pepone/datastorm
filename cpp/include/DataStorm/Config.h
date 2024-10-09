//
// Copyright (c) ZeroC, Inc. All rights reserved.
//

#ifndef DATASTORM_CONFIG_H
#define DATASTORM_CONFIG_H

#include "Ice/Config.h"

#if !defined(ICE_BUILDING_DATASTORM) && defined(DATASTORM_API_EXPORTS)
#    define ICE_BUILDING_DATASTORM
#endif

#if defined(_MSC_VER) && !defined(ICE_BUILDING_DATASTORM)
#    pragma comment(lib, ICE_LIBNAME("Glacier2")) // Automatically link with Glacier2[D].lib
#endif

#ifndef DATASTORM_API
#    if defined(DATASTORM_API_EXPORTS)
#        define DATASTORM_API ICE_DECLSPEC_EXPORT
#    else
#        define DATASTORM_API ICE_DECLSPEC_IMPORT
#    endif
#endif

#endif

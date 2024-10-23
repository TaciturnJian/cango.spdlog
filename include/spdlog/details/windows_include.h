#pragma once

#ifdef _WIN32
#ifndef NOMINMAX
    #define NOMINMAX  // prevent windows redefining min/max
#endif

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#ifndef _USING_V110_SDK71_  // fileapi.h doesn't exist in winxp
#include <fileapi.h>    // WriteFile (..)
#endif

#include <wincon.h>

#endif

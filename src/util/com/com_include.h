#pragma once

// GCC complains about the COM interfaces
// not having virtual destructors
#ifndef _MSC_VER
#  pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <unknwn.h>
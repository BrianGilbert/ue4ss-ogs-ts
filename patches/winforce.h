// winforce.h — force-include shim for the vendored UEPseudo snapshot.
// The snapshot was built against an MSVC STL that still leaked windows.h
// into standard headers; current STLs don't, so TUs using
// GetModuleHandle/CRITICAL_SECTION without including it broke. NOMINMAX
// first so windows.h's min/max macros don't collide with std::min/max.
#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <Windows.h>

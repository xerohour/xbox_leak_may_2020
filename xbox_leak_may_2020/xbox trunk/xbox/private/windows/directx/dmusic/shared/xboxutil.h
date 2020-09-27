// XBox compatability functions

#pragma once

#ifdef XBOX

// On Xbox, DeleteCriticalSection is defined as an empty macro.
// This causes problems when it is called in the DMusic sources
// as ::DeleteCriticalSection(...);
// which macro-expands to:
// ::(...);

#ifdef DeleteCriticalSection
#undef DeleteCriticalSection
inline void DeleteCriticalSection(void* /* unused */)
{
}
#endif // DeleteCriticalSection

#endif // XBOX
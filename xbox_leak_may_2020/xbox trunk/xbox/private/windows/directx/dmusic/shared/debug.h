//
// debug.h
// 
// Copyright (c) 1997-1999 Microsoft Corporation. All rights reserved.
//
// Note:
//

#ifndef DEBUG_H
#define DEBUG_H

#include "xalloc.h"

#ifdef XBOX
#include <xtl.h>
extern "C" ULONG _cdecl DbgPrint(PCH Format, ...);
#ifndef _DUMMY_MSG
#define _DUMMY_MSG
struct MSG;
typedef MSG* LPMSG;
#endif
#include <ole2.h>
#else // XBOX
#include <windows.h>
#endif // XBOX

#define DM_DEBUG_CRITICAL		1	// Used to include critical messages
#define DM_DEBUG_NON_CRITICAL	2	// Used to include level 1 plus important non-critical messages
#define DM_DEBUG_STATUS			3	// Used to include level 1 and level 2 plus status\state messages
#define DM_DEBUG_FUNC_FLOW		4	// Used to include level 1, level 2 and level 3 plus function flow messages
#define DM_DEBUG_ALL			5	// Used to include all debug messages

// Default to no debug output compiled
//
#define Trace
#define TraceI
#define assert(exp) ((void)0)

#ifdef DBG

// Checked build: include at least external debug spew
//
extern void DebugInit(void);
extern void DebugTrace(int iDebugLevel, LPSTR pstrFormat, ...);
extern void DebugAssert(LPSTR szExp, LPSTR szFile, ULONG ulLine);

# undef Trace
# define Trace DebugTrace

# undef assert
# define assert(exp) (void)( (exp) || (DebugAssert(#exp, __FILE__, __LINE__), 0) )

// If internal build flag set, include everything
//
# ifdef DMUSIC_INTERNAL
#  undef TraceI
#  define TraceI DebugTrace
# endif

#endif  // #ifdef DBG

#ifdef XBOX
#ifdef DBG
#define CHECK_ZOMBIE(exp) \
    if (exp) \
    { \
        Trace(0, "Error: Accessing object after it has been garbage collected. "); \
        return DMUS_S_GARBAGE_COLLECTED; \
    } 
#else // DBG
#define CHECK_ZOMBIE(exp) // Do nothing with XBOX retail.
#endif // !DBG
#else // XBOX
#define CHECK_ZOMBIE(exp) \
    if (exp) \
    { \
        Trace(1, "Error: Accessing object after it has been garbage collected. "); \
        return DMUS_S_GARBAGE_COLLECTED; \
    }
#endif  // !XBOX

#include "alist.h"

//Don't forget to update gpNames in DEBUG.CPP when you update this enumClass structure!!!
typedef enum enumClass
{
    DMTRACK_FILE_STREAM,
    DMTRACK_MEM_STREAM,
    DMTRACK_STREAM_STREAM,
    DMTRACK_SEGMENT,
    DMTRACK_SEGMENT_STATE,
    DMTRACK_LOADER,
    DMTRACK_PERFORMANCE,
    DMTRACK_WAVE_TRACK,
    DMTRACK_WAVE,
    DMTRACK_AUDIOPATH,
    DMTRACK_AUDIOPATH_CONFIG,
    DMTRACK_SCRIPT,
    DMTRACK_GRAPH,
    DMTRACK_BAND,
    DMTRACK_BAND_TRACK,
    DMTRACK_COLLECTION,
    DMTRACK_INSTRUMENT,
    DMTRACK_COMPOSER,
    DMTRACK_CHORDMAP,
    DMTRACK_TEMPLATE,
    DMTRACK_SIGNPOST_TRACK,
    DMTRACK_CHORDMAP_TRACK,
    DMTRACK_LYRICS_TRACK,
    DMTRACK_MARKER_TRACK,
    DMTRACK_PARAM_TRACK,
    DMTRACK_SEGTRIGGER_TRACK,
    DMTRACK_SEQUENCE_TRACK,
    DMTRACK_SONG,
    DMTRACK_SYSEX_TRACK,
    DMTRACK_TEMPO_TRACK,
    DMTRACK_TIMESIG_TRACK,
    DMTRACK_CONTAINER,
    DMTRACK_SYNTH,
    DMTRACK_MAX
} debugClass;

#ifdef DBG
class CMemTrack : public AListItem
{
public:
    CMemTrack(DWORD dwClass);
    ~CMemTrack();
    CMemTrack* GetNext() { return (CMemTrack*)AListItem::GetNext();}
private:
    DWORD   m_dwClass;
};

class CMemTrackList : public AList
{
public:
    CMemTrackList();
    ~CMemTrackList();
    LONG GetCount();
    void AddHead(CMemTrack* pMemTrack);
    CMemTrack* GetHead();
    CMemTrack* RemoveHead();
    void Remove(CMemTrack* pMemTrack);
    CRITICAL_SECTION    m_CriticalSection;
};

#else
class CMemTrack 
{
public:
    CMemTrack(DWORD dwClass) { };
};

class CMemTrackList
{
};
#endif

#if defined(DBG) || defined(PROFILE)
#define IMPLEMENT_PERFORMANCE_COUNTERS
#endif

#ifdef IMPLEMENT_PERFORMANCE_COUNTERS
#include <xbdm.h>
extern "C" void DMusicRegisterPerformanceCounter(const char* szName, unsigned long dwType, void* pvArg);
extern "C" void DMusicUnregisterPerformanceCounter(const char* szName);

extern "C" void DMusicRegisterPerformanceCounters();
extern "C" void DMusicUnregisterPerformanceCounters();

#endif





#endif  // #ifndef DEBUG_H

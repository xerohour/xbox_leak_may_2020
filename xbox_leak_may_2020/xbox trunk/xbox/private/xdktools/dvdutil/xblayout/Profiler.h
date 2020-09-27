// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
// File:     Profiler.hpp
// Contents: This file includes profiling-related defines and function declarations.
//
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++ DEFINES +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// DO_PROFILE -- define this if you want to enable profiling.  Warning - this causes things to run
//               slower, so never ship with this enabled!
//#define DO_PROFILE


// Various things that we profile:
enum {
    PROFILE_END,
    PROFILE_RESET,
    PROFILE_POPULATETOLIST,
    PROFILE_COMPACTLAYER,
    PROFILE_REFRESHRELATIONSHIPS,
    PROFILE_POPULATEDIRENTRIES,
    PROFILE_REFRESHFOLDERDIRECTORYENTRY,
    PROFILE_REFRESHDIRECTORYENTRIES,
    PROFILE_INSERT,
    PROFILE_VALIDATEPLACEHOLDERS,
    PROFILE_CHECKVALIDPLACEHOLDERLSN,
    PROFILE_REFRESHPLACEHOLDERS,
    PROFILE_PERSISTTO,
    PROFILE_CREATEFROM,
    PROFILE_PERSISTFST,
    PROFILE_PERSISTBIN,
};

#ifdef DO_PROFILE

#define END_EVENT 0x80000000
#define MAX_TAGS 2000

typedef struct tagProfileInfo
{
    DWORD dwTick;
    int   nType;
} typeProfileInfo;

#define MAX_PROFILE_EVENTS 1000000    // 1,000,000 events max
extern void StartProfile(DWORD dw);
extern void EndProfile(DWORD dw);
extern void DumpProfile();
extern void InitProfile();

#else
__inline void InitProfile() {}
__inline void StartProfile(DWORD dw) {}
__inline void EndProfile(DWORD dw) {}
__inline void DumpProfile() {}
#endif
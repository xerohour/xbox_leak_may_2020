#ifndef _DASHST_
#define _DASHST_

#ifdef __cplusplus
extern "C" {
#endif


BOOL
DashBeginUsingSoundtracks (
    VOID
    );

VOID
DashEndUsingSoundtracks (
    VOID
    );

BOOL
DashAddSoundtrack (
    PCWSTR SoundtrackName,
    PDWORD Stid
    );

BOOL
DashDeleteSoundtrack (
    DWORD Stid
    );


BOOL
DashDeleteSongFromSoundtrack (
    IN DWORD Stid,
    IN UINT Index
    );

BOOL
DashAddSongToSoundtrack (
    IN DWORD Stid,
    IN DWORD Songid,
    IN DWORD Length,
    IN PCWSTR Name
    );

BOOL
DashSwapSoundtracks (
    UINT Stid1,
    UINT Stid2
    );

BOOL
DashSwapSongs (
    DWORD Stid,
    DWORD Index1,
    DWORD Index2
    );

DashEditSoundtrackName (
    UINT Stid,
    PCWSTR NewName
    );

DashEditSongName (
    UINT Stid,
    UINT Index,
    PCWSTR NewName
    );

DWORD
DashGetSoundtrackCount (
    VOID
    );

DWORD
DashGetNewSongId (
    DWORD Stid
    );




#ifdef __cplusplus
}
#endif



#endif

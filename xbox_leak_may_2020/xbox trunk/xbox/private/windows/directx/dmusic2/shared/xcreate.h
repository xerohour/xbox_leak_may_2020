#pragma once

STDAPI DirectMusicChordTrackCreate( REFIID iid, void ** ppDMT );

STDAPI DirectMusicCommandTrackCreate( REFIID iid, void ** ppDMT );

STDAPI DirectMusicStyleTrackCreate( REFIID iid, void ** ppDMT );

STDAPI DirectMusicMelodyFormulationTrackCreate(REFIID iid, void ** ppDMT );

STDAPI DirectMusicMuteTrackCreate(REFIID iid, void ** ppDMT );

STDAPI DirectMusicChordMapTrackCreate(REFIID iid, void ** ppDMT );

STDAPI DirectMusicChordMapCreate(REFIID iid, void ** ppChordMap );

STDAPI DirectMusicSignPostTrackCreate(REFIID iid, void ** ppChordMap );

STDAPI DirectMusicTempoTrackCreate(REFIID iid, void ** ppDMT );

STDAPI DirectMusicTimeSigTrackCreate(REFIID iid, void ** ppDMT );

STDAPI DirectMusicMarkerTrackCreate(REFIID iid, void ** ppDMT );

STDAPI DirectMusicBandTrackCreate(REFIID iid, void ** ppDMT );

STDAPI DirectMusicPatternTrackCreate(REFIID iid, void ** ppDMT );

STDAPI DirectMusicMotifTrackCreate(REFIID iid, void ** ppDMT );

STDAPI DirectMusicWaveTrackCreate(REFIID iid, void ** ppDMT);

STDAPI DirectMusicSegmentCreate( REFIID iid, void ** ppDMSeg );

STDAPI DirectMusicTemplCreate(REFIID iid, void ** ppDMTempl );

STDAPI DirectMusicPerformanceCreate(REFIID iid, void ** ppDMP);

STDAPI DirectMusicBandCreate(REFIID iid, void ** ppBand);

STDAPI DirectMusicStyleCreate(REFIID iid, void ** ppStyle);

STDAPI DirectMusicSectionCreate(REFIID iid, void ** ppSection);

STDAPI DirectMusicAutAudioPathCreate(LPUNKNOWN pUnkOuter,REFIID iid, void ** ppAuto);

STDAPI DirectMusicAutAudioPathConfigCreate(LPUNKNOWN pUnkOuter,REFIID iid, void ** ppAuto);

STDAPI DirectMusicAutSongCreate(LPUNKNOWN pUnkOuter,REFIID iid, void ** ppAuto);

STDAPI DirectMusicAutPerformanceCreate(LPUNKNOWN pUnkOuter,REFIID iid, void ** ppAuto);

STDAPI DirectMusicAutSegmentCreate(LPUNKNOWN pUnkOuter,REFIID iid, void ** ppAuto);

STDAPI DirectMusicScriptCreate(REFIID iid, void ** ppScript);

STDAPI DirectMusicScriptTrackCreate(REFIID iid, void ** ppTrack);

STDAPI DirectMusicSongCreate(REFIID iid, void ** ppSong);

STDAPI DirectMusicAudioPathConfigCreate(REFIID iid, void ** ppPath);

STDAPI DirectMusicSeqTrackCreate(REFIID iid, void ** ppTrack);

STDAPI DirectMusicGraphCreate(REFIID iid, void ** ppGraph);

STDAPI DirectMusicSysexTrackCreate(REFIID iid, void ** ppTrack);

STDAPI DirectMusicTriggerTrackCreate(REFIID iid, void ** ppTrack);

STDAPI DirectMusicLyricsTrackCreate(REFIID iid, void ** ppTrack);

STDAPI DirectMusicParamsTrackCreate(REFIID iid, void ** ppTrack);

STDAPI DirectMusicSegStateCreate(REFIID iid, void ** ppSegState);

STDAPI DirectMusicComposerCreate(REFIID iid, void ** ppDMComp8 );

STDAPI DirectMusicCreate(REFIID iid, void ** ppDMC );

STDAPI DirectMusicCollectionCreate(REFIID iid, void ** ppDMC );

STDAPI DirectMusicSynthCreate(REFIID iid, void ** ppDMSynth );

STDAPI DirectSoundWaveCreate( REFIID iid, void ** ppDSW );

STDAPI DirectMusicContainerCreate( REFIID iid, void ** ppContainer );

STDAPI DirectMusicLoaderCreate( REFIID iid, void ** ppLoader );

STDAPI AutDirectMusicSegmentStateCreate(LPUNKNOWN pUnkOuter, REFIID iid, void ** ppLoader );

STDAPI DirectMusicMemStreamCreate( REFIID iid, void ** ppStream );

STDAPI DirectMusicStreamStreamCreate( REFIID iid, void ** ppStream );

STDAPI DirectMusicFileStreamCreate( REFIID iid, void ** ppStream );

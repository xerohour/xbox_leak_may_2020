#
# The contents of this file were moved from windows\directx\dmusic\makefil0.
# 

TARGETSRCS= \
    $(DMPROOT)\AudioPathDesigner\DlgAddPChannel.cpp \
    $(DMPROOT)\AudioPathDesigner\GuidDlg.cpp \
    $(DMPROOT)\AudioPathDesigner\OlePropPage.cpp \
    $(DMPROOT)\AudioPathDesigner\OlePropSheet.cpp \
    $(DMPROOT)\AudioPathDesigner\RiffStrm.cpp \
    $(DMPROOT)\BandEditor\AList.cpp \
    $(DMPROOT)\BandEditor\GuidDlg.cpp \
    $(DMPROOT)\BandEditor\Info.cpp \
    $(DMPROOT)\BandEditor\RiffStrm.cpp \
    $(DMPROOT)\BandEditor\Unknownchunk.cpp \
    $(DMPROOT)\BandStripMgr\GroupBitsPPG.cpp \
    $(DMPROOT)\BandStripMgr\RiffStrm.cpp \
    $(DMPROOT)\BandStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\ChordMapStripMgr\InvertVector.cpp \
    $(DMPROOT)\ChordMapStripMgr\Keyboard.cpp \
    $(DMPROOT)\ChordMapStripMgr\RiffStrm.cpp \
    $(DMPROOT)\ChordStripMgr\GroupBitsPPG.cpp \
    $(DMPROOT)\ChordStripMgr\InvertVector.cpp \
    $(DMPROOT)\ChordStripMgr\Keyboard.cpp \
    $(DMPROOT)\ChordStripMgr\RiffStrm.cpp \
    $(DMPROOT)\ChordStripMgr\SelectedRegion.cpp \
    $(DMPROOT)\ChordStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\CommandStripMgr\GroupBitsPPG.cpp \
    $(DMPROOT)\CommandStripMgr\RiffStrm.cpp \
    $(DMPROOT)\CommandStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\Conductor\AList.cpp \
    $(DMPROOT)\Conductor\NodeRefChunk.cpp \
    $(DMPROOT)\Conductor\RiffStrm.cpp \
    $(DMPROOT)\ContainerDesigner\DupeFileDlg.cpp \
    $(DMPROOT)\ContainerDesigner\GuidDlg.cpp \
    $(DMPROOT)\ContainerDesigner\RiffStrm.cpp \
    $(DMPROOT)\DLSDesigner\AList.cpp \
    $(DMPROOT)\DLSDesigner\DLSLoadSaveUtils.cpp \
    $(DMPROOT)\DLSDesigner\Info.cpp \
    $(DMPROOT)\DLSDesigner\myslider.cpp \
    $(DMPROOT)\DLSDesigner\RiffStrm.cpp \
    $(DMPROOT)\DLSDesigner\Unknownchunk.cpp \
    $(DMPROOT)\LyricStripMgr\BaseMgr.cpp \
    $(DMPROOT)\LyricStripMgr\BaseStrip.cpp \
    $(DMPROOT)\LyricStripMgr\GroupBitsPPG.cpp \
    $(DMPROOT)\LyricStripMgr\RiffStrm.cpp \
    $(DMPROOT)\LyricStripMgr\SelectedRegion.cpp \
    $(DMPROOT)\LyricStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\MarkerStripMgr\BaseMgr.cpp \
    $(DMPROOT)\MarkerStripMgr\BaseStrip.cpp \
    $(DMPROOT)\MarkerStripMgr\GroupBitsPPG.cpp \
    $(DMPROOT)\MarkerStripMgr\RiffStrm.cpp \
    $(DMPROOT)\MarkerStripMgr\SelectedRegion.cpp \
    $(DMPROOT)\MarkerStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\MelGenStripMgr\RiffStrm.cpp \
    $(DMPROOT)\MelGenStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\MIDIStripMgr\AList.cpp \
    $(DMPROOT)\MIDIStripMgr\DialogNewCCTrack.cpp \
    $(DMPROOT)\MIDIStripMgr\DialogVelocity.cpp \
    $(DMPROOT)\MIDIStripMgr\MidiFileIO.cpp \
    $(DMPROOT)\MIDIStripMgr\midifileIO2.cpp \
    $(DMPROOT)\MIDIStripMgr\RiffStrm.cpp \
    $(DMPROOT)\MIDIStripMgr\SharedPattern.cpp \
    $(DMPROOT)\MIDIStripMgr\SharedPianoRoll.cpp \
    $(DMPROOT)\MIDIStripMgr\SharedPropPageCurve.cpp \
    $(DMPROOT)\MIDIStripMgr\SharedPropPageNote.cpp \
    $(DMPROOT)\MIDIStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\MuteStripMgr\GroupBitsPPG.cpp \
    $(DMPROOT)\MuteStripMgr\RiffStrm.cpp \
    $(DMPROOT)\MuteStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\ParamStripMgr\BaseMgr.cpp \
    $(DMPROOT)\ParamStripMgr\BaseStrip.cpp \
    $(DMPROOT)\ParamStripMgr\DLSLoadSaveUtils.cpp \
    $(DMPROOT)\ParamStripMgr\RiffStrm.cpp \
    $(DMPROOT)\ParamStripMgr\SelectedRegion.cpp \
    $(DMPROOT)\ParamStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\PersonalityDesigner\GuidDlg.cpp \
    $(DMPROOT)\PersonalityDesigner\InvertVector.cpp \
    $(DMPROOT)\PersonalityDesigner\Keyboard.cpp \
    $(DMPROOT)\PersonalityDesigner\RiffStrm.cpp \
    $(DMPROOT)\PersRefStripMgr\GroupBitsPPG.cpp \
    $(DMPROOT)\PersRefStripMgr\RiffStrm.cpp \
    $(DMPROOT)\PersRefStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\ScriptDesigner\GuidDlg.cpp \
    $(DMPROOT)\ScriptDesigner\RiffStrm.cpp \
    $(DMPROOT)\ScriptStripMgr\BaseMgr.cpp \
    $(DMPROOT)\ScriptStripMgr\BaseStrip.cpp \
    $(DMPROOT)\ScriptStripMgr\GroupBitsPPG.cpp \
    $(DMPROOT)\ScriptStripMgr\RiffStrm.cpp \
    $(DMPROOT)\ScriptStripMgr\SelectedRegion.cpp \
    $(DMPROOT)\ScriptStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\SegmentDesigner\GroupBitsPPG.cpp \
    $(DMPROOT)\SegmentDesigner\GuidDlg.cpp \
    $(DMPROOT)\SegmentDesigner\RiffStrm.cpp \
    $(DMPROOT)\SegmentDesigner\TabBoundaryFlags.cpp \
    $(DMPROOT)\SegmentDesigner\TrackFlagsPPG.cpp \
    $(DMPROOT)\SegmentStripMgr\BaseMgr.cpp \
    $(DMPROOT)\SegmentStripMgr\BaseStrip.cpp \
    $(DMPROOT)\SegmentStripMgr\GroupBitsPPG.cpp \
    $(DMPROOT)\SegmentStripMgr\RiffStrm.cpp \
    $(DMPROOT)\SegmentStripMgr\SelectedRegion.cpp \
    $(DMPROOT)\SegmentStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\SequenceStripMgr\DialogNewCCTrack.cpp \
    $(DMPROOT)\SequenceStripMgr\DialogVelocity.cpp \
    $(DMPROOT)\SequenceStripMgr\MidiFileIO.cpp \
    $(DMPROOT)\SequenceStripMgr\midifileIO2.cpp \
    $(DMPROOT)\SequenceStripMgr\RiffStrm.cpp \
    $(DMPROOT)\SequenceStripMgr\SharedPianoRoll.cpp \
    $(DMPROOT)\SequenceStripMgr\SharedPropPageCurve.cpp \
    $(DMPROOT)\SequenceStripMgr\SharedPropPageNote.cpp \
    $(DMPROOT)\SequenceStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\SignPostStripMgr\RiffStrm.cpp \
    $(DMPROOT)\SignPostStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\StyleDesigner\AList.cpp \
    $(DMPROOT)\StyleDesigner\GuidDlg.cpp \
    $(DMPROOT)\StyleDesigner\RiffStrm.cpp \
    $(DMPROOT)\StyleDesigner\SharedPattern.cpp \
    $(DMPROOT)\StyleDesigner\TabBoundaryFlags.cpp \
    $(DMPROOT)\StyleRefStripMgr\RiffStrm.cpp \
    $(DMPROOT)\StyleRefStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\TempoStripMgr\GroupBitsPPG.cpp \
    $(DMPROOT)\TempoStripMgr\RiffStrm.cpp \
    $(DMPROOT)\TempoStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\timeline\RiffStrm.cpp \
    $(DMPROOT)\TimeSigStripMgr\RiffStrm.cpp \
    $(DMPROOT)\TimeSigStripMgr\TrackFlagsPPG.cpp \
    $(DMPROOT)\ToolGraphDesigner\DlgAddPChannel.cpp \
    $(DMPROOT)\ToolGraphDesigner\GuidDlg.cpp \
    $(DMPROOT)\ToolGraphDesigner\OlePropPage.cpp \
    $(DMPROOT)\ToolGraphDesigner\OlePropSheet.cpp \
    $(DMPROOT)\ToolGraphDesigner\RiffStrm.cpp \
    $(DMPROOT)\WaveStripMgr\BaseMgr.cpp \
    $(DMPROOT)\WaveStripMgr\BaseStrip.cpp \
    $(DMPROOT)\WaveStripMgr\DLSLoadSaveUtils.cpp \
    $(DMPROOT)\WaveStripMgr\myslider.cpp \
    $(DMPROOT)\WaveStripMgr\RiffStrm.cpp \
    $(DMPROOT)\WaveStripMgr\SelectedRegion.cpp \
    $(DMPROOT)\WaveStripMgr\TrackFlagsPPG.cpp

dmusic: $(TARGETSRCS)

clean:
     -del /F /Q $(TARGETSRCS)

# Inference rules.
.SUFFIXES:.cpp

# Copy .cpp file from here to wherever specified
{}.cpp{$(O)}.cpp:
   
    copy $** $@

{}.cpp{$(DMPROOT)\3rdParty}.cpp:
	copy $** $@ 
{}.cpp{$(DMPROOT)\ADSREnvelope}.cpp:
	copy $** $@ 
{}.cpp{$(DMPROOT)\AudioPathDesigner}.cpp:
	copy $** $@ 
{}.cpp{$(DMPROOT)\BandEditor}.cpp:
	copy $** $@ 
{}.cpp{$(DMPROOT)\BandStripMgr}.cpp:
	copy $** $@ 
{}.cpp{$(DMPROOT)\build}.cpp:
	copy $** $@ 
{}.cpp{$(DMPROOT)\ChordMapStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\ChordStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\CommandStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\Conductor}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\ContainerDesigner}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\DLSDesigner}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\DMUSProdGUID}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\DMUSProdGUIDPrivate}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\Externals}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\Framework}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\Includes}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\InstallShield}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\JazzCommon}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\JazzComplete}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\JazzDemo}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\KeysOCX}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\Libs}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\LyricStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\MarkerStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\MelGenStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\MIDIStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\MuteStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\PanVol}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\ParamStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\PersonalityDesigner}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\PersRefStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\RegionKeyboard}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\ScriptDesigner}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\ScriptStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\SegmentDesigner}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\SegmentStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\SequenceStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\Shared}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\SignPostStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\SongDesigner}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\StyleDesigner}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\StyleRefStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\TempoStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\Timeline}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\TimeSigStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\ToolGraphDesigner}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\Tutorial}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\Tutorial2}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\Uninst}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\WaveSaveDMO}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\WaveStripMgr}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\XboxAddin}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\XboxSynth}.cpp:
    copy $** $@ 
{}.cpp{$(DMPROOT)\XDmime}.cpp:
    copy $** $@ 

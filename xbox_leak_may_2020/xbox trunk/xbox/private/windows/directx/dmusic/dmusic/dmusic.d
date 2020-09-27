// Copyright (c) 1998 Microsoft Corporation
/* 

@doc EXTERNAL

@interface IDirectMusic |

The <i IDirectMusic> interface serves as a launching point for all
other DirectMusic interfaces. There should only be one instance of
this interface per application. From it, the application can
enumerate available music ports and create DirectMusicPort objects. 

Note that there is no API to create this interface. Applications use the COM
CoCreateInstance function to create a DirectMusic object.

@base public | IUnknown

@meth HRESULT | Activate | Activates or deactivates all output ports created from this interface.
@meth HRESULT | CreateMusicBuffer | Creates a buffer which holds music data for input or output.
@meth HRESULT | CreatePort | Creates a port to a hardware or software device for music input or output
@meth HRESULT | EnumMasterClock | Enumerates the possible time sources for DirectMusic.
@meth HRESULT | EnumPort | Enumerates the available ports.
@meth HRESULT | GetDefaultPort | Returns the GUID of the default port.
@meth HRESULT | GetMasterClock | Returns the GUID of and an <i IReferenceClock> interface to the current master clock.
@meth HRESULT | SetDefaultPort | Sets the default port.
@meth HRESULT | SetMasterClock | Sets the global DirectMusic master clock.
@xref <i IDirectMusicBuffer>, <i IDirectMusicPort>, <i IKsPropertySet>


@interface IDirectMusicBuffer | 

The DirectMusicBuffer object
abstracts a buffer containing timestamped message data to be
sequenced by the DirectMusicPort object. A buffer contains a
relatively small amount of data (typically less than 200
milliseconds) over which the application has control at message
granularity.  The <i IDirectMusicBuffer> interface is used to manipulate
a DirectMusicBuffer object.  Buffer objects are completely
independent of port objects until the buffer is passed to the port
through <om IDirectMusicPort::QueuePlaybackBuffer> or
<om IDirectMusicPort::Read>.

It is important to note that the lower levels of DirectMusic do not
hold on to the buffer.  Once <om IDirectMusicPort::Read> or
<om IDirectMusicPort::QueuePlaybackBuffer> returns, the application is
free to reuse the buffer.

@meth HRESULT | Flush | Empties the buffer.
@meth HRESULT | GetBufferFormat | Returns the GUID representing the buffer format.
@meth HRESULT | GetMaxBytes | Returns the maximum number of bytes the buffer can hold.
@meth HRESULT | GetNextEvent | Enumerates through the events in the buffer.
@meth HRESULT | GetRawBufferPtr | Returns a pointer to the buffer's contents.
@meth HRESULT | GetStartTime | Returns the start time of the buffer.
@meth HRESULT | GetUsedBytes | Returns the amount of music data currently in the buffer.
@meth HRESULT | PackChannelMsg | Packs a MIDI channel message into the buffer.
@meth HRESULT | PackSysEx | Packs a MIDI system exclusive message into the buffer.
@meth HRESULT | ResetReadPtr | Causes the next to GetNextEvent to return the first event in the buffer.
@meth HRESULT | SetStartTime | Sets the start time of the buffer.
@meth HRESULT | SetUsedBytes | Sets the amount of used data in the buffer.
@meth HRESULT | TotalTime | Returns the total time spanned by the data in the buffer.

@xref <i IDirectMusic>, <i IDirectMusicPort>

@interface IDirectMusicPort | 

The IDirectMusicPort interface allows
access to a DirectMusic port.  An instantiated DirectMusicPort object
gives the application control of a stream of music data through that
port. A port is any device which sends, receives, or thrus music
data. The input port of an MPU-401, the output port of an MPU-401 and
the Microsoft Software Synthesizer are examples of ports. Note that a
physical device such as an MPU-401 may provide multiple ports.
 
Ports may also support downloading DLS instruments.

To acquire a port, call <om IDirectMusic::CreatePort()>.

@meth HRESULT | Compact | Compacts downloaded DLS data to create a large chunk of contiguous sample memory.
@meth HRESULT | DeviceIoControl | Pass a device-dependent request to the underlying device driver.
@meth HRESULT | DownloadInstrument | Downloads an instrument from a DLS sample set.
@meth HRESULT | GetCaps | Gets the capabilities of the port.
@meth HRESULT | GetInterfaces | Return the underlying interfaces of a pluggable software synthesizer.
@meth HRESULT | GetLatencyClock | Gets an <i IReferenceClock> which returns the port's latency clock.
@meth HRESULT | GetNumChannelGroups | Gets the number of channel groups allocated by this application.
@meth HRESULT | GetRunningStats | Gets detailed statistics about the performance of a software synthesizer.
@meth HRESULT | IsBufferFormatSupported | Queries the port to see if a buffer format other than MIDI is supported.
@meth HRESULT | PlayBuffer | Queues a DirectMusicBuffer object for playback.
@meth HRESULT | Read | Reads captured music data into a DirectMusicBuffer.
@meth HRESULT | SetNumChannelGroups | Sets the number of channel groups requested for this port.
@meth HRESULT | SetReadNotificationHandle | Sets an event to pulse when music data has been captured.
@meth HRESULT | UnloadInstrument | Unloads a downloaded DLS instrument.

@xref <i IDirectMusic>, <i IDirectMusicBuffer>, <i IKsPropertySet>


@struct DMUS_BUFFERDESC | The <c DMUS_BUFFERDESC> struct is used to create a buffer to hold music data.

@field DWORD | dwSize |
The size of this structure, in bytes.  This member must be initialized before the structure is used.

@field DWORD | dwFlags |
No flags are yet defined.

@field GUID | guidBufferFormat |
The GUID that specifies the KS format of the buffer.  The following GUID represents KSDATAFORMAT_SUBTYPE_MIDI

@flag GUID_KSMusicFormat | The format is KSDATAFORMAT_SUBTYPE_MIDI

If this field is zeroed (GUID_NULL), KSDATAFORMAT_SUBTYPE_MIDI will be assumed.

If guidBufferFormat represents a KS format other than KSDATAFORMAT_SUBTYPE_MIDI, the application must verify that the port playing back 
the data understands the specified format or the buffer will be ignored.  To find out if the port supports a specific KS format, 
use the <om IDirectMusicPort::IsBufferFormatSupported> method.  

@field DWORD | cbBuffer |
This parameter contains the minimum amount of space in the buffer. Note that the amount of memory allocated 
may be slightly higher than the requested size as the system will pad the buffer for alignment purposes.

@struct DMUS_CLOCKINFO | <c DMUS_CLOCKINFO> is returned to the application when <om IDirectMusic::EnumMasterClock> is called.

@field DWORD | dwSize |
The size of this structure, in bytes.  This member must be initialized before the structure is used.

@field DMUS_CLOCKTYPE | ctType |
Specifies what type of clock this master clock is.  Currently there are two clock types defined:
@flag DMUS_CLOCK_SYSTEM | The clock is the system clock.
@flag DMUS_CLOCK_WAVE | The clock is on a wave playback device.

@field GUID | guidClock |

Contains the GUID that uniquely identifies this master clock. This
value may be passed to the <om IDirectMusic::SetMasterClock> method in
order to set the master clock for DirectMusic.


@field WCHAR | wszDescription[] |
Contains a description of the clock.

@struct DMUS_PORTCAPS | <c DMUS_PORTCAPS> is returned to the application when <om IDirectMusic::EnumPort> is called.

@field DWORD | dwSize |
The size of this structure, in bytes.  This member must be initialized before the structure is used.

@field DWORD | dwFlags | This field contains the bitwise or of any of the following flags:
@flag DMUS_PC_DLS | The port supports DLS leve 1 downloadable sample collections.
@flag DMUS_PC_EXTERNAL | This port connects to devices outside of the host, such as devices connected over an external MIDI port like the MPU-401.
@flag DMUS_PC_SOFTWARESYNTH | The port is a software synthesizer.
@flag DMUS_PC_MEMORYSIZEFIXED | Is set to FALSE when Memory available for DLS instruments can be adjusted otherwise it is set to TRUE.
@flag DMUS_PC_GMINHARDWARE | Is set to TRUE if synthesizer has its own GM instrument set,
so GM instruments do not need to be downloaded to it.
@flag DMUS_PC_GSINHARDWARE | This port contains the Roland GS sound set,
so any requested GS instruments do not need to be downloaded to it.

@field GUID | guidPort |
Contains the GUID that uniquely identifies this port. This value may
be passed to the CreatePort method in order to get an
<i IDirectMusicPort> interface on the port.

@field DWORD | dwClass |
Specifies the class of this port. The following classes are defined:
@flag DMUS_PC_INPUTCLASS | Specifies that this is an input port
@flag DMUS_PC_OUTPUTCLASS | Specifies that this is an output port

@field DWORD | dwMemorySize | 
The amount of memory available to store DLS instruments. 
If the memory size is only limited to the size of system
memory, then this field will contain the constant DMUS_PC_SYSTEMMEMORY.

@field DWORD | dwMaxChannelGroups |
The maximum number of channel groups supported by this port.  
A channel group is a set of 16 MIDI channels.

@field DWORD | dwMaxVoices |
The maximum number of voices that can be allocated when this port is opened. 

@field DWORD | dwMaxAudioChannels |
The maximum number of audio channels that can be rendered by this port.

@field DWORD | dwEffectFlags |
The bitwise or of zero or more of the following flags, indicating what audio effects are 
available from this port:
@flag DMUS_EFFECT_REVERB | This port supports reverb.
@flag DMUS_EFFECT_CHORUS | This port supports chorus.
@flag DMUS_EFFECT_DELAY | This port supports delay.

@field WCHAR | wszDescription[] |
Contains a description of the port. This may be a system-generated name, 
such as "MPU-401 Output Port [330]", 
or a user-specified friendly name, such as "Port w/ External SC-55".

@struct DMUS_PORTPARAMS | Contains parameters to use in the opening of 
a port

@field DWORD | dwSize | Contains the number of bytes in the structure. This field must be initialized before using the structure.
@field DWORD | dwValidParams | This DWORD is a bit field indicating which of the structure members below have been filled in. 
<om IDirectMusic::CreatePort> uses these bits to determine what paramters are important in the creation of the port. The following
flags may be included:
@flag DMUS_PORTPARAMS_VOICES | The dwVoices field is valid.
@flag DMUS_PORTPARAMS_CHANNELGROUPS | The dwChannelGroups field is valid.
@flag DMUS_PORTPARAMS_STEREO | The fStereo flag is valid.
@flag DMUS_PORTPARAMS_SAMPLERATE | The dwSampleRate field is valid.
@flag DMUS_PORTPARAMS_EFFECTS The dwEffectFlags field is valid.

@field DWORD | dwVoices | This field contains the maximum number of simaltaneous voices that the application wishes to play on this port.
@field DWORD | dwChannelGroups | This field contains the number of channel grousp requested for this port. Each channel groups contains
16 channels.
@field DWORD | dwAudioChannels | This field contains the desired number of audio output channels. 
@field DWORD | dwSampleRate | Indidicates the number of samples per second for the audio data produced by the port.

@field DWORD | dwEffectFlags |
The bitwise or of zero or more of the following flags, indicating what audio effects are 
available from this port:
@flag DMUS_EFFECT_REVERB | This port supports reverb.
@flag DMUS_EFFECT_CHORUS | This port supports chorus.
@flag DMUS_EFFECT_DELAY | This port supports delay.

*/

/* @interface IKsControl |

   The IKsControl interface is used to get, set, or query the support of properties, events, and methods. 
   This interface is part of the WDM KS (Kernel Streaming) architecture, but is also used by DirectMusic to expose
   properties of DirectMusic ports. To retrieve this interface, call <om IDirectMusicPort::QueryInterface> with 
   IID_IKsControl.
   
   A property set is a set of related property items, each of which has a value. A property set is represented
   by a GUID, and each item is represented by a zero-based index. The meanings of the indexed items for a GUID 
   never change. The KS definition allows these values to be of any type and size. All property items defined
   by DirectMusic have only one item, at index zero, and return a single DWORD of data. None of the DirectMusic
   properties require instance data. Note that these are not restrictions on the interface; the full definition of
   KS properties is supported and will be passed to the driver. It is only a rule by which the standard DirectMusic
   property sets have been defined.

   Routing of the property item request to the port varies depending on the port implementation. No properties are
   supported by ports which represent DirectMusic emulation on top of the Win32 handle-based multimedia calls (midiOut and
   midiIn API's). 

   Property item requests to a port which represents a pluggable software synthesizer are answered totally in user mode. 
   The topology of this type of port is a synthesizer (represented by an <i IDirectMusicSynth> interface) connected to a 
   sink node (an <i IDirectMusicSynthSink> interface). The property request will be given first to the synthersizer node, and
   then to the sink node if it is not recognized by the synthesizer.

   If the port represents a WDM filter implementation, then the property item will be passed directly to the WDM filter graph.
   DirectMusic reserves the right to refuse an <om IKsPropertySet::Set> call on any property which may interfere with the
   normal operation of DirectMusic methods.

   The following property set GUID's are predefined by DirectMusic:

   @flag GUID_DMUS_PROP_GM_Hardware | Item 0 is a boolean indicating whether or not this port supports GM in hardware.
   @flag GUID_DMUS_PROP_GS_Hardware | Item 0 is a boolean indicating whether or not this port supports Roland GS extensions in hardware.
   @flag GUID_DMUS_PROP_XG_Hardware | Item 0 is a boolean indicating whether or not this port supports Yamaha XG extensions in hardware.
   @flag GUID_DMUS_PROP_DLS1 | Item 0 is a boolean indicating whether or not this port supports the downloading of DLS level 1 samples.
   @flag GUID_DMUS_PROP_MemorySize | Item 0 is a boolean indicating how many bytes of sample RAM are available on this device.
   @flag GUID_DMUS_PROP_LegacyCaps | Item 0 is the MIDIINCAPS or MIDIOUTCAPS structure that describes the underlying 
   WinMM device implementing this port. A MIDIINCAPS structure will be returned if dwClass is DMUS_PC_INPUTCLASS in this
   port's capabilities structure; otherwise, a MIDIOUTCAPS structure will be returned.

   @comm

   See <om IKsControl::KsProperty> for an example of how to retrieve the value of one of these properties.

   @meth HRESULT | KsProperty | Gets, sets, or queries support for a given property item.
   @meth HRESULT | KsEvent | Enables or disables firing of the given event.
   @meth HRESULT | KsMethod | Invokes the given KS method.	

 */

/* 

	@struct KSPROPERTY | Describes a request to get, set, or query support for a given property item.

	@field GUID | Set | Indicates the property set to access. See <i IKsControl> For a list of property sets supported
	by DirectMusic. Other property sets may be added by driver and software synthesizer vendors.

	@field ULONG | Id | Specifies the item within the property set to access. Items in a property set are indexed by
	a zero-based integer. The data representing the value of each item is defined when the property set is designed and
	is never allowed to change unless the property set GUID is changed as well.

	@field ULONG | Flags | Contains exactly on of the following flags specifying the operation type:

	@flag | KSPROPERTY_TYPE_SET | This operation is requesting a change to the current value of the property item.
	@flag | KSPROPERTY_TYPE_GET | This operation will retrieve the value of the property item.
	@flag | KSPROPERTY_TYPE_BASICSUPPOR | This operation will determine basic support of the property item. 
*/
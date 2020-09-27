DirectMusic Producer for Xbox Beta 1

This release has a number of important bug fixes from the DirectX8
final and Beta 2500 releases.  It also contains additional components
for testing content on the actual Xbox hardware.
===========================================================================

GENERAL INFO
------------
DirectMusic is a DirectX technology.  Designed to provide an extensible music
solution for DirectX platforms, DirectMusic provides a large palette of tools
for creating high quality interactive and non-repetitive music and audio content.
By providing a framework for musical interactivity, DirectMusic lets you build
musical intelligence into Xbox titles. 

DirectMusic introduces technologies and file formats that have previously not
been available.  To allow composers to integrate the resources of DirectMusic 
with the consistent sound performance of DLS, an authoring tool is included in 
this XDK.  This authoring tool is DirectMusic Producer. 

Within Producer, individual file objects, such as waves, MIDI files, styles, 
chordmaps, segments, and DLS files, are supported by integrated editor modules 
that know how to communicate with each other and work synchronously with real 
time interactive editing on top of the DirectMusic engine.

DirectMusic Producer consists of the following elements:
a. Framework: the environment which hosts all authoring components;
b. The Conductor component: acts as the liaison between authoring components
   and the DirectMusic engine;
c. Style Designer component: provides for creation of interactive music files;
d. Chordmap Designer component: variable chords and scales;
e. Band Editor component: assigning instrument settings (patch changes)
   including DLS;
f. Segment Designer component: allows for open ended creation of different track
   types to be integrated into musical compositions.  MIDI, wave and style 
   files can be used together or independently.
h. DLS Designer component: for creation of Downloadable Sounds soundsets for 
   both DLS1 and DLS2 format.
e. Script Designer: allows the composer or sound designer to use a basic 
   scripting language to control the behavior of content playback.  Detailed 
   information can be found in the DirectMusic Producer Help file under the 
   topic 'Script Designer'.
f. Xbox Experimenter: allows the composer to audition content directly on an Xbox
   Development Kit running the Audio Console Application, using the full audio libraries
   of the Xbox.
g. Xbox Synthesizer: allows the Xbox to be seen as a MIDI/DLS capable device that
   can be triggered from DirectMusic Producer, or any other sequencer using DirectMusic
   Producer solely for DLS management.

WHAT'S NEW IN THIS BETA RELEASE
------------------------------------
This release of Microsoft® DirectMusic® Producer introduces many performance 
enhancements as well as new features, which allow you to:

Audition content on an Xbox. From the Add-Ins menu, choose 'Xbox Experimenter' and
drag segments into the window to copy them to the Xbox. Use the play/stop button to
the left of a segment to audition it. The DirectMusic 'engine' is running on the Xbox.

Use the Xbox as a hardware synthesizer via DirectMusic Producer. From the MIDI/Performance
Options menu, set the Default Synthesizer to the 'Microsoft Xbox Synthesizer'. MIDI content
(though not wave tracks) will now be sent to the Xbox for playback. Note that the
DirectMusic 'engine' is still running on your PC, the Xbox is acting as a synthesizer only.


Attain low-latency playback of MIDI input for easier creation of musical content in 
real time.  Click on MIDI/Performance Options in the Transport Options toolbar, check 
the "super low latency" option, then find the latency setting that works best. This 
option is only available on soundcards with WDM drivers.  

Create stereo regions in DLS Designer more easily.  Stereo regions can be created, 
rather than having to link two mono regions. 

Sort components in the project tree by name, type, and file size. Right-click on a 
project and click Sort Tree on the shortcut menu.

Import wave files directly into new segments.  Right click on the Project name, 
Import> Wave File as Segment...

Edit segment length based on clock time in the segment property page.

Insert wave files and display them much more quickly due to a number of performance 
enhancements. 

Obtain direct instrument assignment and access to Band Editor from parts in Pattern 
Editor, the Sequence Track, and the Pattern Track.  Note the Band icon and instrument
display at the top left of each part.  New instrument insertion is based on cursor 
position. 

Customize shading in track displays, and customize wallpaper in the Editing Window.

Also, Help documentation has been thoroughly revised and a new tutorial is installed 
with Producer.

See the Help documentation for more detailed information on these and other new 
features. 

INSTALLATION
------------  
The Producer setup will automatically uninstall any existing versions of
Producer found on your machine. It is recommended that you close Producer
before attempting to install. 

This version of Producer defaults to a different folder than versions prior to 
DirectX 8, so if you have a shortcut pointing to the old location, that shortcut
will no longer work. 

Producer requires the DirectX8 (or later) version of DirectMusic, as Producer
functionality is built entirely on top of DirectMusic. (The Producer installation
process will prompt to install the DirectX8 runtime bits, if they are not already present.)  

DirectMusic Producer also requires IE4.0 (or higher).  You can uninstall IE
after installing, as IE updates some system files that are required for Producer.

An additional project of sample content can be found on the SDK CD at
"\DMusProdX\DemoContent\dmpdemocontent.exe".  The project will install 
to "My Documents\DMUSProducer\Demo8\" on your hard drive.

For more detailed and up-to-date information on DirectMusic Producer, please
read dmusprod.txt after installing DirectMusic Producer.
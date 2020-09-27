DirectMusic Producer DX7 SDK

This CD contains all DirectMusic Producer DX7 SDK components.
__________________________________________________________________


Contents:

- CD Layout
- Getting Started
- Compiler Support
- Building Samples

__________________________________________________________________


CD LAYOUT

The following is a brief description of the directories found on this
CD.  

\Bin
    StripWizard.awx is a custom AppWizard that may be used with
    Visual C++ 6.0.

\Doc
    IDMUSPROD.HLP contains a complete reference to the DirectMusic
    Producer APIs. 

    \Word
       - This directory contains additional documentation to be viewed
         using Microsoft Word. 

\Include
    Contains include files for building DirectMusic Producer Components
    and Strips.
 
\Lib
    Contains library files for building DirectMusic Producer Components
    and Strips.
 
\Samples
    \LyricStripMgr
       - This directory contains a sample strip manager.

    \Shared
       - This directory contains shared source files that are used
         by the sample strip manager and strip editors created using
         StripWizard.awx.

__________________________________________________________________


GETTING STARTED:
    The "How To Write ..." documents located in the \Doc\Word directory
    contain high level overviews that explain how the various Producer 
    objects fit together.  They introduce the most common Producer
    interfaces and can help guide the reader through \Doc\IDMUSPROD.HLP.
    Reading those documents in conjuction with viewing \Doc\IDMUSPROD.HLP
    would be a great way to get up to speed quickly.

    To use the Strip Wizard, copy the file \bin\StripWizard.awx to
    \Microsoft Visual Studio\Common\MSDev98\Template.  In Visual C++'s
    New Projects dialog, "Strip AppWizard" will appear.  This wizard
    may be used to create a new strip editor that is functionally
    identical to the sample LyricStripMgr, but with a different name.
    For more information refer to the "Strip Wizard Usage Guidelines"
    section located in \Doc\Word\"How To Write DirectMusic Producer
    Dx7 Strip Editors.doc"

__________________________________________________________________


COMPILER SUPPORT

C/C++
=====

The sample strip (Samples\LyricStripMgr) was created with Visual
C++ 6.0.  We have included a Visual C++ project file (.dsp) for
Visual C++ 6.0.

When installing your compiler, we recommend installing with support 
for MFC and ATL.  The sample strip utilizes MFC and ATL.  Without
these components, the sample will not compile.

__________________________________________________________________


BUILDING SAMPLES\LYRICSTRIPMGR:

The include and library paths in the sample manager must be updated
to include the location your DirectX 7.0 SDK was installed in.  The
current settings (F:\mssdk\include and F:\mssdk\lib) may not match
the location where your DirectX 7.0 SDK is installed. 

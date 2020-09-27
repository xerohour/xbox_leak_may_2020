Xbox Development Kit for the Xbox(tm) video game console from Microsoft 
March 2001 Release Readme File

(c) Microsoft Corporation, 2000-2001. All rights reserved.
__________________________________________________________________

Contents

- Release Notes and Known Issues
- Installation
- Uninstallation
__________________________________________________________________

Release Notes and Known Issues

This file contains information you need to know before installing 
or uninstalling the Microsoft Xbox Development Kit (XDK) software,
including the Xbox SDK (the tools and documentation). For all 
other XDK release note information, including setting up the XDK 
target system and your development environment, refer to the 
release notes included in your XDK.

This release of the XDK includes an updated target system recovery
disc that contains a new version of the Xbox System Software.
Refer to the Xbox SDK online help for information on updating your
target system. 
__________________________________________________________________

Installation

Installing the latest version of the Xbox SDK is the only step
required on the development system for the March 2001 XDK release,
provided you have already installed a previous version of the SDK.

It is not necessary to uninstall a prior release of the Xbox SDK
before installing this update. If you do not have a prior release
of the Xbox SDK installed, first perform the steps in the section
"No Prior SDK Installed" below.

You must have permissions at the level of Administrator in order
to perform the installation.  Refer to the Microsoft(r) Windows(r) help system
for further information.

To install, insert the March 2001 SDK disc into your development
system CD drive, select Install Xbox SDK - March 2001 from the
installation screen, and follow the instructions. Alternately,
you can download and install the March 2001 XDK Update from the
beta support Web site (www.betaplace.com).

Note: When the installation program copies an updated file to the
SDK directory on your hard disk, it will overwrite the existing
file even if that file has been modified by you since the last SDK
install. For example, if you have made changes to the sample or
tutorial code installed with a previous SDK, but left those modified
files in the same location and with the same filename, the update
will overwrite those files with new versions and you will lose any
changes you might have made. Copy any modified files you wish to
retain to a new location prior to installing the SDK.
__________________________________________________________________

No Prior SDK Installed

If you do not have a prior release of the Xbox SDK installed,
follow these instructions before downloading and running the SDK
update:

1. If you have not already done so, upgrade your development 
   system to Microsoft Windows 2000 with Service Pack 1. For 
   installation instructions, see the Windows 2000 Service Pack 1
   documentation.  

      Note: Windows 2000 with Service Pack 1 is the only operating 
      system supported by the Xbox Development Kit. 

2. Install Microsoft Visual C++ 6.0 Professional. For 
   installation instructions, see the Visual C++ documentation. 

      Note: This step is not necessary if a licensed copy of 
      Visual C++ 6.0 is already installed on the development 
      system.

      Note: It is not necessary to install the Visual C++ Service 
      Pack 3 (included with the copy of Visual C++ 6.0 in the     
      XDK). 

3. Install Visual C++ 6.0 Service Pack 4 from the Service Pack 4 
   disc included with the XDK. The install program runs 
   automatically when this disc is inserted into the development 
   system CD drive. To install, follow the instructions on the 
   screen. Service Pack 4 does not include an uninstall utility.
   If you want to uninstall Service Pack 4, you must uninstall and
   reinstall Visual C++ 6.0.

4. Install the Microsoft Visual C++ Processor Pack from the XDK 
   March 2001 SDK disc. To install, insert the disc into your 
   development system CD drive, select Install Visual C++ 
   Processor Pack from the installation screen, and follow the 
   instructions (the Processor Pack is located in the VCPP 
   directory on the disc). 
__________________________________________________________________

Uninstallation

To uninstall the Xbox SDK:

1. Either run Setup.exe from the root folder of the XDK disc, or
   launch the Add/Remove Programs dialog from the Windows
   Control Panel, select Microsoft Xbox SDK, and then select 
   Change/Remove. 
   
2. Setup will detect the previous installation of the Xbox SDK.
   Follow the instructions on the screen. 

      Note: Uninstalling the Xbox SDK may not delete all files 
      installed with the Xbox SDK. To complete uninstallation, 
      manually delete the Xbox SDK root folder (located at 
      C:\Program Files\Microsoft Xbox SDK, by default). 

For uninstallation of Microsoft Visual C++, see the Visual C++ 
documentation. You cannot uninstall Visual C++ Service Pack 4 or 
the Processor Pack separately.
__________________________________________________________________


HVS Launcher (Hardware Verification Suite Launcher)
by James N. Helm

Copyright (c) 2001 by Microsoft Corporation

The purpose of the HVS Launcher is to allow different types of tests to be
configured and run as a test suite.  You can store these configurations on
to any Memory that is available to the Xbox (Hard Disk or Memory Unit).


AUTORUN Feature

A configuration can be launched automatically by the HVS Launcher if the
user specifies the name and location of the configuration in a file.  The
file should be placed at the root of the Applications "T:\" drive.  The
contents of the file should be the path of the memory area, a comma, and
then the name of the saved configuration surrounded by quotes.

For example:

	U:\, "Read Tests"		; Hard Disk

or

	F:\, "Hard Drive Tests"		; Memory Unit 1A


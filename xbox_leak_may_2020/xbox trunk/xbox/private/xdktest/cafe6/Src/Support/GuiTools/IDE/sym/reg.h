// reg.h : this file contains definitions needed to register packages
//         and packets with SUSHI.  This file is intended to be usable
//         by either C or C++ programs; no C++ specific language
//         features should be used.
//
//////////////////////////////////////////////////////////////////////////

// [w-amilt] 20 May 1993.  The resource compiler doesn't like #pragma's,
//                         and doesn't care about this file.
#ifndef RC_INVOKED
#ifndef __REG_H__
#define __REG_H__

#pragma pack(2)

// Package IDs
//
// Please do not modify or re-order the following #defines unless you
// know what you are doing.
//
#define PACKAGE_SUSHI		1   // Must be first
#define PACKAGE_VCPP		2
#define PACKAGE_VRES		3
#define PACKAGE_VPROJ		4
#define PACKAGE_PRJSYS		5
#define PACKAGE_MSIN		6
#define PACKAGE_LANGCPP		7
#define PACKAGE_LANGFOR		8
#define PACKAGE_VBA			9
#define PACKAGE_GALLERY     10
#define PACKAGE_LANGMST		11
#define PACKAGE_CLASSVIEW	12
#define PACKAGE_ENT			13
#define PACKAGE_LANGHTML	14
#define PACKAGE_BIN			15
#define PACAKGE_IMG			16
#define PACKAGE_NUMEGA		17
#define PACKAGE_DEBUG           25   // DEVDBG
#define PACKAGE_SIMPLE		90

#define PACKET_NIL      UINT(-1)    // For dockable windows with no part ui.

// Packet IDs for VSHELL
//
#define PACKET_DOCOBJECT 1

// Packet IDs for VRES
//
#define PACKET_COMPVIEW 1
#define PACKET_IMAGE    2
#define PACKET_DIALOG   3
#define PACKET_MENU     4
#define PACKET_STRING   5
#define PACKET_ACCEL    6
#define PACKET_BINARY   7
#define PACKET_VERSION  8

// Packet IDs for VCPP
//
#define PACKET_TEXTEDIT  1
#define PACKET_BASE      2
#define PACKET_MULTI     3

// Packet IDs for PRJSYS
//
#define PACKET_PRJSYS   1

// Packet IDs for MSIN
//
#define PACKET_TOPIC   1

// Packet IDs for ENTerprise edition
//
#define PACKET_SCHEMA			1
#define PACKET_DATASCOPE_TOOL	2
#define PACKET_CUSTOM			3

//  Streams used in a project's MSF file.  Since we use 512 byte pages, these
//  numbers must be  between 0 and 256
//
#define PROJ_MSF_PROJINTINFO    0   // Internal project info
#define PROJ_MSF_PROJEXTINFO    1   // External project info
#define PROJ_MSF_WORKSPACE      2
#define PROJ_MSF_VCPPINFO       3
#define PROJ_MSF_BRSINFO        4
#define PROJ_MSF_PROJSYSINFO    5   // new project system info
#define PROJ_MSF_PROJSCCINFO    6   // project source control info
#define PROJ_MSF_HELPINFO       7   // help system info
#define PROJ_MSF_VBA			8	// VBA state associated with the project
#define PROJ_MSF_ENT			9	// Enterprise package info
#define PROJ_MSF_DBASE          50  // no-compile browse info
                                    // this must be the last one
//
// Packet IDs for no-compile database. The first stream is for a
// a directory/list of files, and the subsequent numbers are reserved
// for file information
//
#define LANGCPP_DBASEINFO 0

#pragma pack()

#endif // __REG_H__
#endif // RC_INVOKED

// mid.h
#pragma once

// Message ID header file
//

// Reserved response codes
//
// Note: The data of a negative response must be a string describing the failure
//       Whether the string is ascii or unicode depends on the type of the 
//       original message.
//
#define MSG_OK				0x00000000
#define MSG_ACK				0xFFFFFFFF
#define MSG_NOSUBSCRIBER	0xFFFFFFFE

// Message ID fields
//
//	bits  0 to 15:	TYPE: unique to category
//	bits 16 to 23:	CATEGORY:
//							0 - Core test message TestingStarted, etc
//							1 - Non-UHC message, sent to a utility or control
//							2 - For UUT connection to host
//							4 - UHC message, but not a core message.
//
//	bits 24 to 31:	BITFIELD: ignored for routing purposes, but passed to
//					the destination.
//					Bit 31:	0 = message strings are UNICODE
//							1 = message strings are ASCII
//					Bit 30:	0 = no response from host expected
//							1 = response from host expected
//					Bit 29: 0 = ACK response expected
//							1 = no ACK response expected
//				
//
// NOTE: All STRINGs are zero terminated and must be unicode,
//       unless noted otherwise  (as in STRING (A/U))
//
// NOTE: When adding a new MID, always add to the end
//       of the list and always add 1 to the previous number

///////////////////////////////////////////////////////////////////////////



#define MID_BITFIELD_MASK			0xFF000000
#define MID_CATEGORY_MASK			0x00FF0000
#define MID_TYPE_MASK				0x0000FFFF

//
// BITFIELD
//
#define MID_STRING_BITFIELD			0x80000000

#define MID_RESPONSE_BITFIELD		0x40000000
#define MID_NO_RESPONSE_EXPECTED	0x00000000
#define MID_RESPONSE_EXPECTED		0x40000000

//
// CATEGORY
//
#define MID_CORE_CATEGORY			0x00000000
#define MID_TEST_CATEGORY			0x00010000
#define MID_CONNECT_CATEGORY		0x00020000
#define MID_SCAN_CATEGORY			0x00030000
#define MID_HOST_CATEGORY			0x00040000
#define MID_FACTORY_CATEGORY		0x00050000
#define MID_DEBUG_CATEGORY			0x00060000

#define MID_NO_DATA					""

enum enumMID {
#define X(a,b,c) a = b,
#include "mid.x"
#undef X
};

// These defines are placed after the enum definition above
// to discourage the setting of these bits in an MID

#define MID_UNICODE_STRINGS			0x00000000
#define MID_ASCII_STRINGS			0x80000000

#define MID_ACK_BITFIELD			0x20000000
#define MID_ACK_EXPECTED			0x00000000
#define MID_NO_ACK_EXPECTED			0x20000000


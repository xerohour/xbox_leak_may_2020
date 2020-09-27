
///////////////////////////////////////////////////////////////////////////
//
// mid.x : UHC Socket Message X Macro file
//
// This file is used to define the message id's
// This file is included by mid.h where the X macro is resolved.
// This file is also include by mid.cpp where the macro is
// defined so as to yeild a map of mid number to name strings.
//
// This X macro has 3 fields:
//
//	field 1: Name of the mid
//	field 2: Numeric value of the mid
//	field 3: A format string to be used for interpretting the
//				data buffer by any logging tools
//
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
//
// Message ID fields
//
//	bits  0 to 15:	TYPE: unique to category
//	bits 16 to 23:	CATEGORY:
//							0 - Core test message: TestingStarted, etc
//							1 - TEST/MODULE message, sent to a utility or control
//							2 - For UUT connection to host
//							3 - Remote scanner messages
//							4 - UHC message, but not a core message.
//							5 - Factory message, but not a core message.
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
//
///////////////////////////////////////////////////////////////////////////

// safety checking
#if !defined(X)
#error must define X to use the file
#endif


///////////////////////////////////////////////////////////////////////////
//
// Core testing messages, isolated to facilitate module level debugging
//
///////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------
// Test Executive MIDs
//
// MID_UUT_STARTING
//
// Indicates that Test Executive on the UUT has started running
//
// Data: none
//
X (MID_UUT_STARTING, (MID_CORE_CATEGORY | 0x00000001), MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_UUT_ENDING
//
// Indicates that Test Executive on the UUT has stopped running
//
// Data: none
// 	
X (MID_UUT_ENDING, (MID_CORE_CATEGORY | 0x00000002), MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_UUT_ACTIVE
//
// Indicates that Test Executive on the UUT has emmitted a 
// heart beat message.  It must be assumed that this message
// is evidence of tests actually running.
//
// Data: none
// 	
X (MID_UUT_ACTIVE, (MID_CORE_CATEGORY | 0x00000003), MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_UUT_TESTSTART
//
// Indicates that a test has started on the UUT
//
// Field 1:	STRING (ascii/unicode), Name of starting test
//		
// 	
X (MID_UUT_TESTSTART, (MID_CORE_CATEGORY | 0x00000004), "%s")

//-----------------------------------------------------------------------------
// MID_UUT_TESTEND
//
// Indicates that a test has ended on the UUT
//
// Field 1:	STRING (ascii/unicode), Name of starting test
// Field 2: DWORD, Indicating milliseconds elapsed during testing
// Field 3: DWORD, Result: 1 means all tests passes, 0 means at least one test failed 
//		
// 	
X (MID_UUT_TESTEND, (MID_CORE_CATEGORY | 0x00000005), "%s%d%d")

//-----------------------------------------------------------------------------
// MID_UUT_STAT
//
// Indicates that a test has a statistic message
//
// Field 1:	STRING (ascii/unicode), Name of test
// Field 2:	STRING (ascii/unicode), Keyword
// Field 3:	STRING (ascii/unicode), Value
//		
// 	
X (MID_UUT_STAT, (MID_CORE_CATEGORY | 0x00000006), "%s%s%s")

//-----------------------------------------------------------------------------
// MID_UUT_WARNING
//
// Indicates that a test has a warning message
//
// Field 1:	STRING (ascii/unicode), Name of test
// Field 2:	STRING (ascii/unicode), Warning message
//		
// 	
X (MID_UUT_WARNING, (MID_CORE_CATEGORY | 0x00000007), "%s%s")

//-----------------------------------------------------------------------------
// MID_UUT_DEBUG
//
// Indicates that a test has a debug message
//
// Field 1:	STRING (ascii/unicode), Name of test
// Field 2:	STRING (ascii/unicode), Debug message
//		
// 	
X (MID_UUT_DEBUG, (MID_CORE_CATEGORY | 0x00000008), "%s%s")

//-----------------------------------------------------------------------------
// MID_UUT_ERROR
//
// Indicates that a test has an error message
//
// Field 1:	STRING (ascii/unicode), Name of test
// Field 2:	DWORD, Standard Error Code
// Field 3:	STRING (ascii/unicode), Description of error
//		
// 	
X (MID_UUT_ERROR, (MID_CORE_CATEGORY | 0x00000009), "%s%d%s")

///////////////////////////////////////////////////////////////////////////
//
// UUT Debug Messages
//
///////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// MID_DEBUG_IGNORE_ERROR
//
// Indicates that an error is expected and the logger should not
// save the log file on account of this next error.
// If the field 1 is 0 then the logger simply increments the
// count of expected errors.  ...
// Not implemented yet: Otherwise, the specific error number is
// ignored.  MID_UUT_STARTING will reset the expected error count
// for unspecified errors, but not for specified errors.
//
// Field 1: DWORD, The error code to ignore or zero for any
// 	
X (MID_DEBUG_IGNORE_ERROR, (MID_DEBUG_CATEGORY | 0x00000001), "%d")

//-----------------------------------------------------------------------------

// The difference between error and warning is that error will cause
// a log to be saved, while warning will not.

X (MID_DEBUG_ERROR, (MID_DEBUG_CATEGORY | 0x00000002), "%s")
X (MID_DEBUG_WARNING, (MID_DEBUG_CATEGORY | 0x00000003), "%s")
X (MID_DEBUG_2RESPONSES, (MID_DEBUG_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000004), "")
X (MID_DEBUG_ECHO_BACK, (MID_DEBUG_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000005), "")

///////////////////////////////////////////////////////////////////////////
//
// Host Request Messages
//
///////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------
// MID_LIST_TO_SUBSCRIBE
//
// Special reserved MID used to get a module to produce a list
// of support MIDs
//
// Data: None
// 	
X (MID_LIST_TO_SUBSCRIBE, (0x00000001 | MID_HOST_CATEGORY), MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_UUT_MSG_TIMEOUT
//
// Indicates the timeout value between UUT messages the host uses for 
// determining that the UUT is hung. Value is in seconds.
//
// Field 1:	DWORD, Timeout in seconds
// 	
X (MID_UUT_MSG_TIMEOUT, (0x00000002 | MID_HOST_CATEGORY), "%d")

//-----------------------------------------------------------------------------
// MID_UUT_RESTARTING
//
// Indicates the Uut is about to reboot and UHC should expect 
// another MID_UUT_STARTING message.
//
// Data: None
// 	
X (MID_UUT_RESTARTING, (0x00000003 | MID_HOST_CATEGORY), MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_UUT_GET_ELAPSED_TIME
//
// Indicates the elapsed time value since the TIU was started.
//
// Message:
// Data: None
//		
// Response:
// Field 1:	DWORD, Elapsed test time, in seconds.
// 	
X (MID_UUT_GET_ELAPSED_TIME, (0x00000004 | MID_HOST_CATEGORY | MID_RESPONSE_EXPECTED), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_UUT_GET_BOOT_COUNT
//
// Indicates the current boot count.  MID_UUT_RESTARTING will cause the boot count to
// be incremented on the host.  Boot count is initialized to 0 when the TIU is started.
//
// Message:
// Data: None
//		
// Response:
// Field 1:	DWORD, Current boot count, starts at 0.
// 	
X (MID_UUT_GET_BOOT_COUNT, (0x00000005 | MID_HOST_CATEGORY | MID_RESPONSE_EXPECTED), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_ECHO_BACK
//
// This message will cause the host to send the contents of the message data 
// buffer back in the response. This is used just for testing the message 
// passing capabilities of UHC.
//
// Message:
// Field 1:	Data Buffer, contents of the buffer to be returned in the response
//		
// Response:
// Field 1:	Data Buffer, the exact same contents that were sent.
// 	
X (MID_ECHO_BACK, (0x00000006 | MID_HOST_CATEGORY | MID_RESPONSE_EXPECTED), "")

//-----------------------------------------------------------------------------
// MID_GET_FILE
//
// This message will cause the host to send the contents of the specified file
// in the response.
//
// Message:
// Field 1:	string - name of file to fetch
//		
// Response:
// Field 1:	binary buffer that contains the entire file that was read
// 	
// Utilit(y)(ies) where implemented:
// Get_File.EXE
//
X (MID_GET_FILE, (0x0000007 | MID_HOST_CATEGORY | MID_RESPONSE_EXPECTED), "%s")


//-----------------------------------------------------------------------------
// MID_GET_CURRENT_TIME
//
// This message will cause the host to send the current time of the host
// in the response. The time is formatted as hh:mm:ss where hh is two digits 
// representing the hour in 24-hour notation, mm is two digits representing 
// the minutes past the hour, and ss is two digits representing seconds. 
// For example, the string 18:23:44 represents 23 minutes and 44 seconds 
// past 6 P.M. 
//
// Message:
// Field 1:	Empty.
//		
// Response:
// Field 1:	STRING, the time string
// 	
// UHC will respond to this message.
// 	
X (MID_GET_CURRENT_TIME, (0x00000008 | MID_HOST_CATEGORY | MID_RESPONSE_EXPECTED), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_GET_CURRENT_DATE
//
// This message will cause the host to send the current date of the host
// in the response. The date is formatted mm/dd/yyyy, where mm is two digits 
// representing the month, dd is two digits representing the day, and yyyy is 
// the year. For example, the string 12/05/2001 represents December 5, 2001. 
//
// Message:
// Field 1:	Empty.
//		
// Response:
// Field 1:	STRING, the date string
// 	
// UHC will respond to this message.
// 	
X (MID_GET_CURRENT_DATE, (0x00000009 | MID_HOST_CATEGORY | MID_RESPONSE_EXPECTED), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_GET_TESTMODE
//
// This message will cause the host to send the current test mode of the host
// in the response. 
// Examples: "STBL" - Standard Board Level, "OQM" - Outgoing Quality Monitor  
//
// Message:
// Field 1:	Empty.
//		
// Response:
// Field 1:	STRING, the test mode string
// 	
// UHC will respond to this message.
// 	
X (MID_GET_TESTMODE, (0x0000000a | MID_HOST_CATEGORY | MID_RESPONSE_EXPECTED), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_GET_FILE_EX
//
// This message will cause the host to send a portion of the contents of the specified file
// in the response.
//
// Message:
// Field 1:	DWORD, Byte offset of the file at which to start reading
// Field 2:	DWORD, Maximum number of bytes to fetch from the file
// Field 3:	string - name of file to fetch
//		
// Response:
// Field 1:	binary buffer that contains the entire file that was read
// 	
// Utilit(y)(ies) where implemented:
// CMidClient
//
X (MID_GET_FILE_EX, (0x000000b | MID_HOST_CATEGORY | MID_RESPONSE_EXPECTED), "%d%d%s")

//-----------------------------------------------------------------------------
// MID_GET_FILE_SIZE
//
// This message will cause the host to send the size of the specified file in bytes
//
// Message:
// Field 1:	string - name of file
//		
// Response:
// Field 1:	DWORD, Size of the file in bytes
// 	
// Utilit(y)(ies) where implemented:
// CMidclient
//
X (MID_GET_FILE_SIZE, (0x000000c | MID_HOST_CATEGORY | MID_RESPONSE_EXPECTED), "%d")

//-----------------------------------------------------------------------------
// MID_GET_NAMED_STATE
//
// This message will cause the host to send a saved state value indexed by a unique
// string
//
// Message:
// Field 1:	string - name of state
//		
// Response:
// Field 1:	DWORD, saved state
// 	
// Utilit(y)(ies) where implemented:
// CMidClient
//
X (MID_GET_NAMED_STATE, (0x000000d | MID_HOST_CATEGORY | MID_RESPONSE_EXPECTED), "%s")

//-----------------------------------------------------------------------------
// MID_SET_NAMED_STATE
//
// This message will cause the host to save a named state value indexed by a unique
// string for later retreival by the uut. This is a good way for tests to save/retrieve
// state information that will survive a boot cycle on the UUT.
//
// Message:
// Field 1:	DWORD, state value to save
// Field 2:	string - name of state
//		
// Response:
// none
// 	
// Utilit(y)(ies) where implemented:
// CMidClient
//
X (MID_SET_NAMED_STATE, (0x000000e | MID_HOST_CATEGORY), "%d%s")

///////////////////////////////////////////////////////////////////////////
//
// Factory Request Messages
//
///////////////////////////////////////////////////////////////////////////


//-----------------------------------------------------------------------------
// MID_GET_SERIAL_NUMBER
//
// This message will cause the host to send the Serial Number of the current UUT
// in the response.  
//
// Message:
// Field 1:	Empty.
//		
// Response:
// Field 1:	STRING, the serial number string
// 	
// UHC's Factory interface will respond to this message.
// 	
X (MID_GET_SERIAL_NUMBER, \
	(MID_FACTORY_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000001), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_GET_PRODUCT_CODE
//
// This message will cause the host to send the Product Code of the current UUT
// in the response.  
//
// Message:
// Field 1:	Empty.
//		
// Response:
// Field 1:	STRING, the Product Code string
// 	
// UHC's Factory interface will respond to this message.
// 	
X (MID_GET_PRODUCT_CODE, \
	(MID_FACTORY_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000002), \
	MID_NO_DATA)

///-----------------------------------------------------------------------------
// MID_GET_MAC_ADDRESSES
//
// This message will cause the host to send a list of the MAC Addresses of the
// current UUT in the response.  
//
// Message:
// Field 1:	Empty.
//		
// Response:
// Field 1:	STRING, a comma seperated string of MAC Addresses associated with this UUT
// 	
// UHC's Factory interface will respond to this message.
// 	
X (MID_GET_MAC_ADDRESSES, \
	(MID_FACTORY_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000003), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_GET_PRODUCT_FAMILY
//
// This message will cause the host to send the current family name of the UUT
// which it gets based off of the Product Code.
//
// Message:
// Field 1:	Empty.
//		
// Response:
// Field 1:	STRING, the family name string
// 	
// UHC's TIU for this UUT will respond to this message.
// 	
X (MID_GET_PRODUCT_FAMILY, \
	(MID_FACTORY_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000004), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_FACTORY_GET_VALUE
//
// This message will cause the host factory interface to send the "value" associated
// with the "key" for the current UUT in the response.
//
// Message:
// Field 1:	STRING, the key.
//		
// Response:
// Field 1:	STRING, the value.
// 	
// UHC's Factory interface will respond to this message.
// 	
X (MID_FACTORY_GET_VALUE, \
	(MID_FACTORY_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000005), "%s")

//-----------------------------------------------------------------------------
// MID_FACTORY_SET_VALUE
//
// This message will cause the host factory interface to store the "value" associated
// with the "key" for the current UUT.
//
// Message:
// Field 1:	STRING, the key.
// Field 2:	STRING, the value.
// 	
// UHC's Factory interface will handle this message.
// 	
X (MID_FACTORY_SET_VALUE, \
	(MID_FACTORY_CATEGORY | 0x00000006), "%s%s")

///////////////////////////////////////////////////////////////////////////
//
// Connection Request Messages
//
///////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// MID_CONNECT_BY_MAC_ADDRESS
//
// Indicates that the UUT is requesting a connection based on its
// MAC Address.
//
// Data: none
//
// Response: If connection is rejected, the host will close the socket.
//				The UUT will receive 0 (zero) bytes as an indicator.
//				If not rejected then a response of 0 is returned.
// 	
X (MID_CONNECT_BY_MAC_ADDRESS, \
	(MID_CONNECT_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000001), \
	MID_NO_DATA)


//-----------------------------------------------------------------------------
// MID_CONNECT_BY_TIU_NUMBER
//
// Indicates that the UUT is requesting a connection based on its
// TIU Number.
//
// Field 1:	DWORD, TIU Number
// 	
// Response: If connection is rejected, the host will close the socket.
//				The UUT will receive 0 (zero) bytes as an indicator.
//				If not rejected then a response of 0 is returned.
// 	
X (MID_CONNECT_BY_CHANNEL, \
	(MID_CONNECT_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000002), \
	"%d")


//-----------------------------------------------------------------------------
// MID_CONNECT_BY_UNIQUE_ID
//
// Indicates that the UUT is requesting a connection based on its
// unique ID string (i.e. product serial number or SMBIOS UUID.)
//
// Field 1:	STRING - ascii/unicode, Unique ID String
// 	
// Response: If connection is rejected, the host will close the socket.
//				The UUT will receive 0 (zero) bytes as an indicator.
//				If not rejected then a response of 0 is returned.
// 	
X (MID_CONNECT_BY_UNIQUE_ID, \
	(MID_CONNECT_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000003), \
	"%s")



///////////////////////////////////////////////////////////////////////////
//
// Remote Scanner Messages
//
///////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// MID_SCAN_TESTSTART
//
// Sent by the remote scanner to indicate that a unit is being placed in a tester bay
//
// Field 1:	STRING, Serial Number
// Field 1:	STRING, Location Name
// 	
X (MID_SCAN_TESTSTART, \
	(MID_SCAN_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000001), \
	"%s%s")


//-----------------------------------------------------------------------------
// MID_SCAN_TESTQUERY
//
// Sent by the remote scanner to indicate that a unit is being placed in a tester bay
//
// Field 1:	STRING, Location Name
// 	
X (MID_SCAN_TESTQUERY, \
	(MID_SCAN_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000002), \
	"%s")


//-----------------------------------------------------------------------------
// MID_SCAN_RACKSTART
//
// Indicates that units have been placed in a rack and are ready for testing
//
// Field 1:	DWORD, RACK NUMBER
// Field 2:	DWORD, CART NUMBER
//
// followed by any number of pairs:
//
// Field 3: DWORD, BAY NUMBER
// Field 4: STRING, SERIAL LNUMBER
//
// Response
//
//	Status = 0 - Okay
//	Status = 1 - Rack still busy, tests running
//	Status = 2 - Invalid rack, cart or bay number
//
//	Field 1: STRING, explanation of rejection
// 	
X (MID_SCAN_RACKSTART, \
	(MID_SCAN_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000003), \
	"%d%d(%d%s)")

//-----------------------------------------------------------------------------
// MID_SCAN_RACKCLEAR
//
// Indicates that units have been removed from a rack
//
// Field 1:	DWORD, RACK LOCATION
// Field 2:	DWORD, RACK NUMBER
//
// followed by any number of:
//
// Field 3: DWORD, SLOT NUMBER
// 	
X (MID_SCAN_RACKCLEAR, \
	(MID_SCAN_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000004), \
	"%s")

//-----------------------------------------------------------------------------
// MID_SCAN_RACKQUERY
//
// Queries the status of a rack/slot
//
// Field 1:	DWORD, RACK LOCATION
// Field 2:	DWORD, RACK NUMBER
// Field 3: DWORD, SLOT NUMBER
// 	
X (MID_SCAN_RACKQUERY, \
	(MID_SCAN_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000005), \
	"%d%d%d")

//-----------------------------------------------------------------------------
// MID_SCAN_SNQUERY
//
// Queries the status of a UUT
//
// Field 1:	STRING, SERIAL NUMBER
// 	
X (MID_SCAN_SNQUERY, \
	(MID_SCAN_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000006), \
	"%s")

///////////////////////////////////////////////////////////////////////////
//
// Test Specific MIDs
//
///////////////////////////////////////////////////////////////////////////

//-----------------------------------------------------------------------------
// MID_REPORT_SUPPORTED_MESSAGES
//
// This is a special message used by the utility displatcher on the host to determine
// what MID messages are supported by each utility.  Each utility is required to
// implement this message and send back a list of supported MIDs when this message
// is issued.
//
// Message:
// Field 1:	Empty.
//		
// Response:
// Field 1:	MID array, a DWORD array of the MIDs that the utility supports, not
// including the MID_REPORT_SUPPORTED_MESSAGES message.
// 	
// Utilit(y)(ies) where implemented:
// All utilities are required to support this message
//
X (MID_REPORT_SUPPORTED_MESSAGES, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000000), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_READ_SIGNAL
//
// This message will cause the host to read the specified signal and 
// send the value back in the response.  The signal can be of any type:
// IO bits, relays, A/D readings etc, it is up to the host to determine 
// the signal type and read it accordingly.
// Default: is 0 is off & 1 is on for bits and relays.
//
// Message:
// Field 1:	STRING, Signal Name.
//		
// Response:
// Field 1:	DOUBLE, Signal Value.
// 	
// UHC's Base or Test Control will respond to this message.
// 	
X (MID_READ_SIGNAL, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x0000000A), \
	"%s")

//-----------------------------------------------------------------------------
// MID_WRITE_SIGNAL
//
// This message will cause the host to write the specified value to the signal.
// The signal can be of any type: IO bits, relays, A/D readings etc, it is up 
// to the host to determine the signal type and write it accordingly.
// Default: is 0 is off & >1 is on for bits and relays.
//
// Message:
// Field 1: DOUBLE, Signal Value.
// Field 2:	STRING, Signal Name.
//		
// UHC's Base or Test Control will respond to this message.
// 	
X (MID_WRITE_SIGNAL, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x0000000B), \
	"%f%s")

//-----------------------------------------------------------------------------
// MID_XBOX_RANDOM_STRING
//
// This message will cause the host to send an array of random byte values back
// in the response.
//
// Message:
// Field 1:	DWORD number of random bytes to return
//		
// Response:
// Field 1:	BYTE Array of the random bytes being returned (the length of the array is the
//          same as the length specified in Field 1 of the message.
// 	
// Utilit(y)(ies) where implemented:
// XBOX_RANDOM_STRING.EXE
//
X (MID_XBOX_RANDOM_STRING, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x0000000C), \
	"%!")

//-----------------------------------------------------------------------------
// MID_AUDIO_RECORD_ANALOG
//
// This message will cause the host to record analog audio and send the results back
// in the response.
//
// Message:
// Field 1:	revision - DWORD revision of the data structure that follows.  Rev 1 is listed here
// Field 2: audio_mux_com_port - DWORD indicating the COM port that the audio mux is attached to
// Field 3: mux_control_low_dword - DWORD indicating the low 32 bits of the audio mux setting
// Field 4: mux_control_high_dword - DWORD indicating the high 32 bits of the audio mux setting
// Field 5: mixer_name - string.  The name of the mixer to adjust for recording.  "none" indicates no mixer is to be used
// Field 6: number_of_mixer_controls - DWORD indicating how many mixer control strings are in the following list
// Field 7: mixer_controls - list of strings indicating the names and settings of all mixer controls to be adjusted
// Field 8: wave_input_device_name - string.  The name "none" implies that the default analog record device should be used
// Field 9: number_of_channels - DWORD indicating how many channels to record
// Field 10: sample_rate - DWORD indicating the record rate in KHz
// Field 11: bits_per_sample - DWORD indicating the resolution of the recording (8 and 16 are common)
// Field 12: wave_input_number_of_samples - DWORD indicating the number of samples to capture to the returned buffer
// Field 13: delay_between_playback_start_and_record_start - DWORD inidcating the delay between playback starting and record starting
//		
// Response:
// Field 1:	The raw buffer of recorded audio
// 	
// Utilit(y)(ies) where implemented:
// Audio_Record.EXE
//
X (MID_AUDIO_RECORD_ANALOG,	\
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x0000000D), \
	"%d%d%d%d%s%d(%s)%s%d%d%d%d%d")

//-----------------------------------------------------------------------------
// MID_AUDIO_RECORD_DIGITAL
//
// This message will cause the host to record digital audio and send the results back
// in the response.
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// Audio_Record.EXE
//
X (MID_AUDIO_RECORD_DIGITAL, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x0000000E), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_HDVMU_GET_PIXELCOUNT
//
// This message will cause the host to request a pixel count measurement from the HDVMU
// and send the measured values back in the response.  The UUT application is responsible
// for determining if the measured values are in spec - this is merely a "middle-man" app.
//
// Message:
// Field  1: hSyncPolarity - INT the value of the horizontal sync signal polarity (positive or negative).
// Field  2: vSyncPolarity - INT the value of the vertical sync signal polarity (positive or negative)
// Field  3: Interlaced - INT value to determine if the video signal is interlaced (true or false)
// Field  4: wPixelThreshold - WORD value of the pixel threshold in millivolts.
// Field  5: wHSyncThreshold - WORD value of the horizontal sync threshold in millivolts.
// Field  6: wVSyncThreshold - WORD value of the vertical sync threshold in millivolts.
// Field  7: wStartLine - WORD value of the line in which to start collecting measurements.
// Field  8: wNumLines - WORD value of the number of lines to collect for a measurement.
// Field  9: wNumSamples - WORD value of the number of times a measurement is sampled before the value is returned.
// Field 10: hDelay - BYTE value of number of 24MHz clock cycles to delay before performing a measurement.
// Field 11: Mode - BYTE value that determines the HDTV mode and standard.
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// remote_hdvmu.exe
//
X (MID_HDVMU_GET_PIXELCOUNT, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x0000000F), \
	"%d%d%d%w%w%w%w%w%w%b%b")


//-----------------------------------------------------------------------------
// MID_HDVMU_GET_TIMINGS
//
// This message will cause the host to request a video signal timings measurement from the
// HDVMU and send the measured values back in the response.  The UUT application is
// responsible for determining if the measured values are in spec - this is merely
// a "middle-man" app.
//
// Message:
// Field  1: hSyncPolarity - INT the value of the horizontal sync signal polarity (positive or negative).
// Field  2: vSyncPolarity - INT the value of the vertical sync signal polarity (positive or negative)
// Field  3: Interlaced - INT value to determine if the video signal is interlaced (true or false)
// Field  4: wPixelThreshold - WORD value of the pixel threshold in millivolts.
// Field  5: wHSyncThreshold - WORD value of the horizontal sync threshold in millivolts.
// Field  6: wVSyncThreshold - WORD value of the vertical sync threshold in millivolts.
// Field  7: wStartLine - WORD value of the line in which to start collecting measurements.
// Field  8: wNumLines - WORD value of the number of lines to collect for a measurement.
// Field  9: wNumSamples - WORD value of the number of times a measurement is sampled before the value is returned.
// Field 10: hDelay - BYTE value of number of 24MHz clock cycles to delay before performing a measurement.
// Field 11: Mode - BYTE value that determines the HDTV mode and standard.
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// remote_hdvmu.exe
//
X (MID_HDVMU_GET_TIMINGS, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000010), \
	"%d%d%d%w%w%w%w%w%w%b%b")


//-----------------------------------------------------------------------------
// MID_HDVMU_GET_ANALOG_COLORS
//
// This message will cause the host to request a DAC measurement for analog color linearity
// from the HDVMU and send the measured values back in the response.  The UUT application is
// responsible for determining if the measured values are in spec - this is merely
// a "middle-man" app.
//
// Message:
// Field  1: hSyncPolarity - INT the value of the horizontal sync signal polarity (positive or negative).
// Field  2: vSyncPolarity - INT the value of the vertical sync signal polarity (positive or negative)
// Field  3: Interlaced - INT value to determine if the video signal is interlaced (true or false)
// Field  4: wPixelThreshold - WORD value of the pixel threshold in millivolts.
// Field  5: wHSyncThreshold - WORD value of the horizontal sync threshold in millivolts.
// Field  6: wVSyncThreshold - WORD value of the vertical sync threshold in millivolts.
// Field  7: wStartLine - WORD value of the line in which to start collecting measurements.
// Field  8: wNumLines - WORD value of the number of lines to collect for a measurement.
// Field  9: wNumSamples - WORD value of the number of times a measurement is sampled before the value is returned.
// Field 10: hDelay - BYTE value of number of 24MHz clock cycles to delay before performing a measurement.
// Field 11: Mode - BYTE value that determines the HDTV mode and standard.
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// remote_hdvmu.exe
//
X (MID_HDVMU_GET_ANALOG_COLORS, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000011), \
	"%d%d%d%w%w%w%w%w%w%b%b")


//-----------------------------------------------------------------------------
// MID_HDVMU_GET_TRILEVEL_SYNC_1080I
//
// This message will cause the host to request a measurement for the analog level of the 
// tri-level sync pulse in 1080i mode from the HDVMU and send the measured values back in
// the response.  The UUT application is responsible for determining if the measured values
// are in spec - this is merely a "middle-man" app.
//
// Message:
// Field  1: hSyncPolarity - INT the value of the horizontal sync signal polarity (positive or negative).
// Field  2: vSyncPolarity - INT the value of the vertical sync signal polarity (positive or negative)
// Field  3: Interlaced - INT value to determine if the video signal is interlaced (true or false)
// Field  4: wPixelThreshold - WORD value of the pixel threshold in millivolts.
// Field  5: wHSyncThreshold - WORD value of the horizontal sync threshold in millivolts.
// Field  6: wVSyncThreshold - WORD value of the vertical sync threshold in millivolts.
// Field  7: wStartLine - WORD value of the line in which to start collecting measurements.
// Field  8: wNumLines - WORD value of the number of lines to collect for a measurement.
// Field  9: wNumSamples - WORD value of the number of times a measurement is sampled before the value is returned.
// Field 10: hDelay - BYTE value of number of 24MHz clock cycles to delay before performing a measurement.
// Field 11: Mode - BYTE value that determines the HDTV mode and standard.
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// remote_hdvmu.exe
//
X (MID_HDVMU_GET_TRILEVEL_SYNC_1080I, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000012), \
	"%d%d%d%w%w%w%w%w%w%b%b")


//-----------------------------------------------------------------------------
// MID_HDVMU_GET_TRILEVEL_SYNC_720P
//
// This message will cause the host to request a measurement for the analog level of the 
// tri-level sync pulse in 720p mode from the HDVMU and send the measured values back in
// the response.  The UUT application is responsible for determining if the measured values
// are in spec - this is merely a "middle-man" app.
//
// Message:
// Field  1: hSyncPolarity - INT the value of the horizontal sync signal polarity (positive or negative).
// Field  2: vSyncPolarity - INT the value of the vertical sync signal polarity (positive or negative)
// Field  3: Interlaced - INT value to determine if the video signal is interlaced (true or false)
// Field  4: wPixelThreshold - WORD value of the pixel threshold in millivolts.
// Field  5: wHSyncThreshold - WORD value of the horizontal sync threshold in millivolts.
// Field  6: wVSyncThreshold - WORD value of the vertical sync threshold in millivolts.
// Field  7: wStartLine - WORD value of the line in which to start collecting measurements.
// Field  8: wNumLines - WORD value of the number of lines to collect for a measurement.
// Field  9: wNumSamples - WORD value of the number of times a measurement is sampled before the value is returned.
// Field 10: hDelay - BYTE value of number of 24MHz clock cycles to delay before performing a measurement.
// Field 11: Mode - BYTE value that determines the HDTV mode and standard.
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// remote_hdvmu.exe
//
X (MID_HDVMU_GET_TRILEVEL_SYNC_720P, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000013), \
	"%d%d%d%w%w%w%w%w%w%b%b")


//-----------------------------------------------------------------------------
// MID_HDVMU_GET_TRILEVEL_SYNC_480P
//
// This message will cause the host to request a measurement for the analog level of the 
// tri-level sync pulse in 480p mode from the HDVMU and send the measured values back in
// the response.  The UUT application is responsible for determining if the measured values
// are in spec - this is merely a "middle-man" app.
//
// Message:
// Field  1: hSyncPolarity - INT the value of the horizontal sync signal polarity (positive or negative).
// Field  2: vSyncPolarity - INT the value of the vertical sync signal polarity (positive or negative)
// Field  3: Interlaced - INT value to determine if the video signal is interlaced (true or false)
// Field  4: wPixelThreshold - WORD value of the pixel threshold in millivolts.
// Field  5: wHSyncThreshold - WORD value of the horizontal sync threshold in millivolts.
// Field  6: wVSyncThreshold - WORD value of the vertical sync threshold in millivolts.
// Field  7: wStartLine - WORD value of the line in which to start collecting measurements.
// Field  8: wNumLines - WORD value of the number of lines to collect for a measurement.
// Field  9: wNumSamples - WORD value of the number of times a measurement is sampled before the value is returned.
// Field 10: hDelay - BYTE value of number of 24MHz clock cycles to delay before performing a measurement.
// Field 11: Mode - BYTE value that determines the HDTV mode and standard.
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// remote_hdvmu.exe
//
X (MID_HDVMU_GET_TRILEVEL_SYNC_480P, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000014), \
	"%d%d%d%w%w%w%w%w%w%b%b")

//-----------------------------------------------------------------------------
// MID_ASK_QUESTION
//
// This message will cause the host to pop up a dialog to ask the operarot
// a question. The dialog has two buttons that are programmed by the test
//
// Message:
// Field 1:	string - name of bitmap file to display in the dialog
//			This must be a path to a BMP file or a NULL (zero length) string
//			If this is a null string, UHC will use a default picture
// Field 2:	string - name of file to display for extended help
//			This can be a path to a BMP, JPG, MPG or AVI file or a NULL (zero length) string
//			If this is a null string, UHC will the BMP file in Field 1
// Field 3: string - the string to prompt the operator with
//			Required
// Field 4: string - the caption for the left button
//			If this is a null string, UHC will use "Okay"
// Field 5: string - the caption for the right button
//			If this is a null string, UHC will use "Cancel"
//		
// Response:
// Field 1:	the operator response: 
//			ID_OK if the user clicked on the left button
//			ID_CANCEL if the user clicked on the right button
// 	
// Utilit(y)(ies) where implemented:
// CMidClient
//
X (MID_ASK_QUESTION, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000017), \
	"%s%s%s%s%s")

//-----------------------------------------------------------------------------
// MID_GET_OPERATION_ID
//
// This message will cause the host to send the current opid of the host
// in the response. 
// Examples: ATEL, PCBA  
//
// Message:
// Field 1:	Empty.
//		
// Response:
// Field 1:	STRING, the opid string
// 	
// UHC will respond to this message.
// 	
X (MID_GET_OPERATION_ID, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000018), \
	MID_NO_DATA)


//-----------------------------------------------------------------------------
// MID_XBOX_SET_VIDEO_MODE_LINES
//
// This message will send a number indicating the how the video mode lines are to be set
// A delay of 500mS should be allowed for the relays to settle after
// making this call.
//
// Message:
// Field 1:	BYTE indicating the video mode to be selected
//			#define AVMODEDDISABLE		0
//			#define AVMODENTSCRFU		4
//			#define AVMODEPALRFU		2
//			#define AVMODEHDTV			6
//			#define AVMODESDTVNTSC		1
//			#define AVMODEVGA			5
//			#define AVMODENTSDTVPAL		3
//			#define AVMODESCART			7
//
// 	
// Utilit(y)(ies) where implemented:
// Base Control DLL XBASEPCBA
//
X (MID_XBOX_SET_VIDEO_MODE_LINES, \
	(MID_TEST_CATEGORY | 0x00000019), \
	"%d")
//-----------------------------------------------------------------------------
// MID_XBOX_MEASURE_CSYNCS
//
// This test instructs the the base control DLL to perform CSYNCE line testing
// The require the video output to be in a stable mode.  The return DWORDS are 
// the number of counts in 1 second.  This call takes 4 seconds.
// Host computer.
//
// Message:
// Field 1:	None
//
// Responce:
//		DWORD CSync1 count
//		DWORD CSync2 count
//		DWORD CSync3 count
//		DWORD CSync4 count
// 	
// Utilit(y)(ies) where implemented:
// Base Control DLL XBASEPCBA
//
X (MID_XBOX_MEASURE_CSYNCS, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x0000001A), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_XBOX_SENSOR_LINES
//
// This message will send a number indicating the state of the sensors on the XBOX
//
// Message:
// Field 1:None		
//
// Response:
// Field 1:	FLOAT Fan Voltage - 8 Bytes
//			BYTE  1 if Air Sensor off, 0 if fan air sensor on
//			BYTE  Green LED line - 0 off 1 on 
//			BYTE  Red LED line - 0 off 1 on 
//			BYTE[17] for additions
//			
//       
// 	
// Utilit(y)(ies) where implemented:
// Base Control DLL XBASEPCBA
//
X (MID_XBOX_SENSOR_LINES, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x0000001B), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_XBOX_FAN_CONTROL
//
// This message will send a number indicating the state of the sensors on the XBOX
//
// Message:
// Field 1:	BYTE indicating the sensors set
//			BIT 0 = 0 turn fan off, 1 turn fan on	
// 	
// Utilit(y)(ies) where implemented:
// Base Control DLL XBASEPCBA
//
X (MID_XBOX_FAN_CONTROL, \
	(MID_TEST_CATEGORY | 0x0000001C), \
	"%d")

//-----------------------------------------------------------------------------
// MID_XBOX_SET_VIDEO_MUX
//
// Programs the routing of the video mux on the TIU interface board 
// A delay of 500mS should be allowed for the relays to settle after
// making this call.
//
// Message:
// Field 1:	BYTE indicating the video mode to be selected
//            #define AVSELECTDISABLE	0
//            #define AVSELECTVGA		1
//            #define AVSELECTHDTV		2
//            #define AVSELECTNTSCCOMP	3
//            #define AVSELECTNTSCSVID	4
//            #define AVSELECTSECAM		5
//            #define AVSELECTPALSCART	6
//
// 	
// Utilit(y)(ies) where implemented:
// Base Control DLL XBASEPCBA
//
X (MID_XBOX_SET_VIDEO_MUX, \
	(MID_TEST_CATEGORY | 0x0000001D), \
	"%d")



//-----------------------------------------------------------------------------
// MID_NTSC_M_COMP_CB
//
// This message will cause the host to setup TV standard to NTSC-M format with Composite input
// source and captures output of NTSC-M/Composite color bars from Xbox to generate a N_MCompCB.bmp file
// for the quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_NTSC_M_COMP_CB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x0000001E), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_NTSC_M_SVIDEO_CB
//
// This message will cause the host to setup TV standard to NTSC-M format with S-Video input
// source and captures output of NTSC-M/S-Video color bars from Xbox to generate a N_MSVCB.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_NTSC_M_SVIDEO_CB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x0000001F), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_PAL_BDGHI_COMP_CB
//
// This message will cause the host to setup TV standard to PAL-BDGHI format with Composite input
// source and captures output of PAL-BDGHI/Composite color bars from Xbox to generate a P_BCompCb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_PAL_BDGHI_COMP_CB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000020), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_PAL_BDGHI_SVIDEO_CB
//
// This message will cause the host to setup TV standard to PAL-BDGHI format with S-Video input
// source and captures output of PAL-BDGHI/S-Video color bars from Xbox to generate a P_BSvCb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_PAL_BDGHI_SVIDEO_CB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000021), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_NTSC_JAPAN_COMP_CB
//
// This message will cause the host to setup TV standard to NTSC-Japan format with Composite input
// source and captures output of NTSC-Japan/Composite color bars from Xbox to generate a N_JCompCb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_NTSC_JAPAN_COMP_CB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000022), \
	MID_NO_DATA)
//-----------------------------------------------------------------------------
// MID_NTSC_JAPAN_SVIDEO_CB
//
// This message will cause the host to setup TV standard to NTSC-Japan format with S-Video input
// source and captures output of NTSC-Japan/S-Video color bars from Xbox to generate a N_JSvCb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_NTSC_JAPAN_SVIDEO_CB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000023), \
	MID_NO_DATA)
//-----------------------------------------------------------------------------
// MID_PAL_M_COMP_CB
//
// This message will cause the host to setup TV standard to PAL-M format with Composite input
// source and captures output of PAL-M/Composite color bars from Xbox to generate a P_MCompCb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_PAL_M_COMP_CB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000024), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_PAL_M_SVIDEO_CB
//
// This message will cause the host to setup TV standard to PAL-M format with S-Video input
// source and captures output of PAL-M/S-Video color bars from Xbox to generate a P_MSvCb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_PAL_M_SVIDEO_CB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000025), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_PAL_N_COMP_CB
//
// This message will cause the host to setup TV standard to PAL-N format with Composite input
// source and captures output of PAL-N/Composite color bars from Xbox to generate a P_NCompCb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_PAL_N_COMP_CB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000026), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_PAL_N_SVIDEO_CB
//
// This message will cause the host to setup TV standard to PAL-N format with S-Video input
// source and captures output of PAL-N/S-Video color bars from Xbox to generate a P_NSvCb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_PAL_N_SVIDEO_CB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000027), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_SECAM_COMP_CB
//
// This message will cause the host to setup TV standard to SECAM format with Composite input
// source and captures output of SECAM/Composite color bars from Xbox to generate a S_CompCb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_SECAM_COMP_CB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000028), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_SECAM_SVIDEO_CB
//
// This message will cause the host to setup TV standard to SECAM format with S-Video input
// source and captures output of SECAM/S-Video color bars from Xbox to generate a S_SvCb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_SECAM_SVIDEO_CB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000029), \
	MID_NO_DATA)


//-----------------------------------------------------------------------------
//  MID_NTSC_M_COMP_MB
//
// This message will cause the host to setup TV standard to NTSC-M format with Composite input
// source and captures output of NTSC-M/Composite Multiburst from Xbox to generate a N_MCompMb.bmp file
// for the quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X  (MID_NTSC_M_COMP_MB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x0000002A), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_NTSC_M_SVIDEO_MB
//
// This message will cause the host to setup TV standard to NTSC-M format with S-Video input
// source and captures output of NTSC-M/S-Video Multiburst from Xbox to generate a N_MSvMb.bmp file
// for the quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X  (MID_NTSC_M_SVIDEO_MB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x0000002B), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_PAL_BDGHI_COMP_MB
//
// This message will cause the host to setup TV standard to PAL-BDGHI format with Composite input
// source and captures output of PAL-BDGHI/Composite Multiburst from Xbox to generate a P_BCompMb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_PAL_BDGHI_COMP_MB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x0000002C), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_PAL_BDGHI_SVIDEO_MB
//
// This message will cause the host to setup TV standard to PAL-BDGHI format with S-Video input
// source and captures output of PAL-BDGHI/S-Video Multiburst from Xbox to generate a P_BSvMb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_PAL_BDGHI_SVIDEO_MB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x0000002D), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_NTSC_JAPAN_COMP_MB
//
// This message will cause the host to setup TV standard to NTSC-Japan format with Composite input
// source and captures output of NTSC-Japan/Composite Multiburst from Xbox to generate a N_JCompMb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_NTSC_JAPAN_COMP_MB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x0000002E), \
	MID_NO_DATA)
//-----------------------------------------------------------------------------
// MID_NTSC_JAPAN_SVIDEO_MB
//
// This message will cause the host to setup TV standard to NTSC-Japan format with S-Video input
// source and captures output of NTSC-Japan/S-Video Multiburst from Xbox to generate a N_JSvMb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_NTSC_JAPAN_SVIDEO_MB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x0000002F), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_PAL_M_COMP_MB
//
// This message will cause the host to setup TV standard to PAL-M format with Composite input
// source and captures output of PAL-M/Composite Multiburst from Xbox to generate a P_MCompMb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_PAL_M_COMP_MB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000030), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_PAL_M_SVIDEO_MB
//
// This message will cause the host to setup TV standard to PAL-M format with S-Video input
// source and captures output of PAL-M/S-Video Multiburst from Xbox to generate a P_MSvMb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_PAL_M_SVIDEO_MB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000031), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_PAL_N_COMP_MB
//
// This message will cause the host to setup TV standard to PAL-N format with Composite input
// source and captures output of PAL-N/Composite Multiburst from Xbox to generate a P_NCompMb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_PAL_N_COMP_MB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000032), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_PAL_N_SVIDEO_MB
//
// This message will cause the host to setup TV standard to PAL-N format with S-Video input
// source and captures output of PAL-N/S-Video Multiburst from Xbox to generate a P_NSvMb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_PAL_N_SVIDEO_MB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000033), \
	MID_NO_DATA)


//-----------------------------------------------------------------------------
// MID_SECAM_COMP_MB
//
// This message will cause the host to setup TV standard to SECAM format with Composite input
// source and captures output of SECAM/Composite Multiburst from Xbox to generate a S_CompMb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_SECAM_COMP_MB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000034), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_SECAM_SVIDEO_MB
//
// This message will cause the host to setup TV standard to SECAM format with S-Video input
// source and captures output of SECAM/S-Video Multiburst from Xbox to generate a S_SvMb.bmp file
// for the further quality analysis.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_SECAM_SVIDEO_MB, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000035), \
	MID_NO_DATA)

//-----------------------------------------------------------------------------
// MID_GENPERBOXDATA
//
// This message will cause the host to generate a data blob that can be stored in the XBox eeprom
//
// Message:
// Field 1:	GameRegion - DWORD Microsoft game region code value
// Field 2: TVRegion - DWORD Microsoft TV region code value
// Field 3: MACAddress - BYTE[8], a big-endian array of 8 bytes representing the XBox MAC address.  The first two bytes are always 0
// Field 4: XboxSerialNumber - string.  A 12 character ASCII string plus 0 terminator representing the XBox serial number
//		
// Response:
// Field 1:	HardDriveKey - string.  A 24 byte ASCII string plus 0 terminator that is the base64 encoded 16 byte hard drive key for the XBox
// Field 2:	RecoveryKey - string.  An 8 byte ASCII string plus 0 terminator that is the recovery key for the XBox
// Field 3:	OnlineKeyVersion - DWORD.  The version of the online key for the XBox
// Field 4:	OnlineKey - string.  A 172 byte ASCII string plus 0 terminator that is the base64 encoded 128 byte online key for the XBox
// Field 5:	OutputBufferSize - DWORD.  The size of the following OutputBuffer array in bytes
// Field 6:	OutputBuffer - BYTE array.  An array of length OutputBufferSize that contains data to be written to the XBox eeprom
// 	
// Utilit(y)(ies) where implemented:
// GenPerBoxData.EXE
//
X (MID_GENPERBOXDATA,	\
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000036), \
	"%d%d%d%d%s")


//-----------------------------------------------------------------------------
// MID_POWER_DOWN_METHOD
//
// This message tells the Test Control the method to use Powering Down the UUT.
// Method Enum:	0 -	Terminate all power emmediately
//				1 - Hard Power Switch
//				2 - Soft Power Switch (ACPI Soft Off)
//
// Message:
// Field 1: ULONG, Method Enum.
// Field 2:	ULONG, Time out in seconds.
//		
// UHC's Base or Test Control will respond to this message.
// 	
X (MID_POWER_DOWN_METHOD, \
	(MID_TEST_CATEGORY | 0x00000037), \
	"%d%d")

//-----------------------------------------------------------------------------
// MID_UDP_RESPONDER
//
// This message will cause the host to run the UPD responder applet.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_UDP_RESPONDER, \
	(MID_TEST_CATEGORY  | 0x00000038), \
	MID_NO_DATA)


//-----------------------------------------------------------------------------
// MID_TV_LINEARITY
//
// This message will cause the host to run the TV Linearity test.
// This test requires the use of the xtvcapture.exe utility
// to analyze a gradient shade of red, green, and blue bands for
// lineal defects.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// xtvcapture.exe
//
X (MID_TV_LINEARITY, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000039), \
	"%d%d%d%d%d%d%d%d%s%s%b")

//-----------------------------------------------------------------------------
// MID_CSYNC
//
// This message will cause the host to measure the CSYNC frequency for 100ms
// The number return in the number of counts for 100ms period.  For example:
// For NTSC this should be something near 1575.  The count is not sync to the 
// signal to depending where we are in the vertical scans the value will vary.
// 
//
// Message:
// Field 1:	TBD
//		
// Response:
// Field 1:	TBD
// 	
// Utilit(y)(ies) where implemented:
// XPCBABASE.DLL
//
X (MID_CSYNC, \
	(MID_TEST_CATEGORY | MID_RESPONSE_EXPECTED | 0x00000040), \
	"%d%d%d%d")


// Add before this
#undef X
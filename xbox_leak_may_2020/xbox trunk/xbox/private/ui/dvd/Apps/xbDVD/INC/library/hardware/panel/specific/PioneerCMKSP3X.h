// FILE:			library\hardware\panel\specific\pioneer.h
// AUTHOR:		M Spinnenweber
// COPYRIGHT:	(c) 2000 RAVISENT TECHNOLOGIES.  All Rights Reserved.
// CREATED:		FEB.22.2000
//
// PURPOSE:		Pioneer Front Panel CMKS-P3X
//
// HISTORY:
//
//


#ifndef PIONEERCMKSP3X_H
#define PIONEERCMKSP3X_H




#include "..\generic\panels.h"
#include "library\common\VDDEBUG.h"
#include "library\common\krnlsync.h"
#include "library\lowlevel\memmpdio.h"
#include "embedded\library\generic\cemsg.h"




// This section defines in general a generic panel object. These are the symbols and routines which are
// common to all front panel objects. This code could be merged into panel.h and panel.cpp if desired
// by Viona.


#define PANEL_SEGMENT_ALL                 0xFFFF
#define PANEL_SEGMENT_NONE                0x0000

#define PANEL_SEGMENT_5_1CH               1
#define PANEL_SEGMENT_L                   2
#define PANEL_SEGMENT_R                   3
#define PANEL_SEGEMNT_SLASH               4
#define PANEL_SEGMENT_LOCKED              5
#define PANEL_SEGMENT_ZOOM                6
#define PANEL_SEGMENT_LAST                7
#define PANEL_SEGMENT_PBC                 8
#define PANEL_SEGMENT_REPEAT              9
#define PANEL_SEGMENT_A_TO                10
#define PANEL_SEGMENT_B                   11
#define PANEL_SEGMENT_MINUTE_SEPERATOR    12
#define PANEL_SEGMENT_HOUR_SEPERATOR      13
#define PANEL_SEGMENT_REMAIN              14
#define PANEL_SEGMENT_SINGLE              15
#define PANEL_SEGMENT_TRACK               16
#define PANEL_SEGMENT_CHAPTER             17
#define PANEL_SEGMENT_CHANNEL             18
#define PANEL_SEGMENT_TITLE               19
#define PANEL_SEGMENT_ANGLE               20
#define PANEL_SEGMENT_DVD                 21
#define PANEL_SEGMENT_VCD                 22
#define PANEL_SEGMENT_CD                  23
#define PANEL_SEGMENT_SUPER               24
#define PANEL_SEGMENT_DOLBY_DIGITAL       25
#define PANEL_SEGMENT_DTS                 26
#define PANEL_SEGMENT_PLAY                27
#define PANEL_SEGMENT_PAUSE               28
#define PANEL_SEGMENT_SLASH               29
#define PANEL_SEGMENT_CONDITION				30
#define PANEL_SEGMENT_LAST_MEMO				31
#define PANEL_SEGMENT_TOTAL					32
#define PANEL_SEGMENT_96KHZ					33
#define PANEL_SEGMENT_DOT_LEFT				34
#define PANEL_SEGMENT_DOT_RIGHT			   35
#define PANEL_SEGMENT_COLON					36
#define PANEL_SEGMENT_GUI						37
#define PANEL_SEGMENT_LED                 38


#define PANEL_FIELD_JUSTIFICATION_LEFT    0
#define PANEL_FIELD_JUSTIFICATION_RIGHT   1
#define PANEL_FIELD_JUSTIFICATION_CENTER  2


#define PANEL_FIELD_NONE                  0x00
#define PANEL_FIELD_CHAPTER               0x01
#define PANEL_FIELD_TITLE                 0x02
#define PANEL_FIELD_TRACK                 0x04
#define PANEL_FIELD_TIME                  0x08
#define PANEL_FIELD_ANGLE                 0x10


class GenericPanel 
   { 

   protected:

      BYTE           displaySize;
      BOOL           power;
      DWORD          fieldsLocked;
      DWORD          fieldsBeingEdited;
      DWORD          fieldsEnabled;
      WORD           editValue;

		PanelNumType   testNumType;



   protected:

                   GenericPanel           ( BYTE displaySize );
                  ~GenericPanel           ( void );

     virtual void  ClearDisplay           ( void )=0;
     virtual void  EnableSegment          ( WORD segmentId )=0;
     virtual void  DisableSegment         ( WORD segmentId )=0;
     virtual void  Write                  ( BYTE position, BYTE data )=0;



             void  WriteDigit             ( BYTE position, BYTE data );
             void  WriteNumberField       ( DWORD number, BYTE fieldPosition, BYTE fieldSize, BYTE fieldJustification, BOOL removeLeadingZeros );
             void  WriteStringField       ( char* data, BYTE fieldPosition, BYTE fieldSize, BYTE fieldJustification );
             void  SetField               ( BYTE fieldPposition, BYTE fieldSize, BYTE data );
             void  WriteTestNumber        ( int number );
             void  WriteTestResult        ( BOOL result );
             void  SetPower               ( void );


    virtual  void  LockField              ( DWORD field );
             void  UnlockField            ( DWORD field );
             BOOL  FieldLocked            ( DWORD field );

             void  EnableField            ( DWORD field );
             void  DisableField           ( DWORD field );
             BOOL  FieldEnabled           ( DWORD field );


    virtual  void  BeginEditField         ( DWORD field );
             void  EndEditField           ( DWORD field );
             BOOL  FieldBeingEdited       ( DWORD field );
         
     
   };




// Pioneer CMKSP3X specific code


// This will include diagnostic code for testing the front panel. This code will run during the execution 
// of the constructor. The front panel will execute a reset, a display all segments, and wait for 1 second.
// Next it will start the couting test. This test will count up from 0 each 1/10 of a second to verify no 
// loss of communications to the front panel. Pressing any key will advance to the panel/remote button 
// test. Each remote button / panel button press will be displayed as it's name on the front panel. Press
// power to exit this test. The next test is the segment light test. Each panel button/ remote button press
// will cause a different LED or playback segment to light, one at a time. Press Power to exit this test
// and resume the player.
// 
//#define  PIONEER_FRONT_PANEL_DIAGNOSTICS  1
#undef   PIONEER_FRONT_PANEL_DIAGNOSTICS


#define GNR_PIONEER_FRONT_PANEL_LT1_ERROR                  MKERR(ERROR, FRONTPANEL, HARDWARE, 0x01)
#define GNR_PIONEER_FRONT_PANEL_XRDY_ERROR                 MKERR(ERROR, FRONTPANEL, HARDWARE, 0x02)
#define GNR_PIONEER_FRONT_PANEL_CHECKSUM_ERROR             MKERR(ERROR, FRONTPANEL, HARDWARE, 0x03)
#define GNR_PIONEER_FRONT_PANEL_UNKNOWN_PACKET_TYPE_ERROR  MKERR(ERROR, FRONTPANEL, HARDWARE, 0x04)

#define PIONEER_FRONT_PANEL_XRDY_MAXTIME                                         1250    // 1250 = 80ms  =(1250 * 64EXP-6)seconds

#define PIONEER_FRONT_PANEL_SEND_PACKET_MAX_RETRIES                              5


#define PIONEER_FRONT_PANEL_BUFFER_SIZE			                                 64		  // size of the PanelOperations (POP) array
#define PIONEER_FRONT_PANEL_POLLING_INTERVAL                                     20000   // 20000us = 20ms


#define PIONEER_FRONT_PANEL_PACKET_SIZE								                  16
#define PIONEER_FRONT_PANEL_PACKET_REGISTER_BASE_ADDRESS			                  0x80

#define PIONEER_FRONT_PANEL_MYCHIP_EMI_BASE_ADDRESS                              0x60000000
#define PIONEER_FRONT_PANEL_MYCHIP_EMI_SIZE                                      0x1000


#define PIONEER_FRONT_PANEL_INTERRUPT_MASK_REGISTER							         0x03      
#define PIONEER_FRONT_PANEL_PIN_FUNCTION_REGISTER								         0x70
#define PIONEER_FRONT_PANEL_PORTA_DIRECTION_REGISTER							         0x71
#define PIONEER_FRONT_PANEL_PORTB_DIRECTION_REGISTER							         0x72
#define PIONEER_FRONT_PANEL_PORTC_DIRECTION_REGISTER						            0x73
#define PIONEER_FRONT_PANEL_PORTA_DATA_REGISTER 								         0x74
#define PIONEER_FRONT_PANEL_PORTB_DATA_REGISTER									         0x75
#define PIONEER_FRONT_PANEL_PORTC_DATA_REGISTER							               0x76
#define PIONEER_FRONT_PANEL_CH0_AUTO_TRANSMISSION_MODE_CONTROL_REGISTER1         0xa0
#define PIONEER_FRONT_PANEL_CH0_AUTO_TRANSMISSION_MODE_CONTROL_REGISTER2         0xa1
#define PIONEER_FRONT_PANEL_CH0_AUTO_TRANSMISSION_LENGTH_REGISTER 		         0xa3
#define PIONEER_FRONT_PANEL_CH0_AUTO_TRANSMISSION_CLOCK_REGISTER	               0xa4
#define PIONEER_FRONT_PANEL_AUTO_TRANSMISSION_INTERVAL_REGISTER                  0xa5
#define PIONEER_FRONT_PANEL_CLOCK_PRESCALER_REGISTER										0xa8 
#define PIONEER_FRONT_PANEL_CH0_INTERRUPT_CONTROL_REGISTER							   0xa9
#define PIONEER_FRONT_PANEL_CH0_COMMAND_REGISTER					                  0xaa
#define PIONEER_FRONT_PANEL_CH2_COMMAND_REGISTER                                 0xac

#define PIONEER_FRONT_PANEL_LT1_BIT_MASK	   						                  0x02
#define PIONEER_FRONT_PANEL_XRDY_BIT_MASK                                        0x01

#define PIONEER_FRONT_PANEL_VERSION5                                             0x05
#define PIONEER_FRONT_PANEL_VERSION14                                            0x14

#define PIONEER_FRONT_PANEL_PACKET_TYPE_COMMAND                                  0x02
#define PIONEER_FRONT_PANEL_PACKET_TYPE_SEGMENT                                  0x03

#define PIONEER_FRONT_PANEL_PACKET_TYPE_FRONT_PANEL_BUTTON                       0x01
#define PIONEER_FRONT_PANEL_PACKET_TYPE_DVD_REMOTE                               0xa3
#define PIONEER_FRONT_PANEL_PACKET_TYPE_NO_KEY_INPUT				                  0xff
#define PIONEER_FRONT_PANEL_PACKET_TYPE_LASERDISC_REMOTE			                  0xa8


#define PIONEER_FRONT_PANEL_PACKET_VERSION_OFFSET                                0x0e
#define PIONEER_FRONT_PANEL_PACKET_KEY_DATA0_OFFSET                              0x00
#define PIONEER_FRONT_PANEL_PACKET_KEY_DATA1_OFFSET                              0x01
#define PIONEER_FRONT_PANEL_PACKET_KEY_DATA2_OFFSET                              0x02
#define PIONEER_FRONT_PANEL_PACKET_KEY_DATA3_OFFSET                              0x03


#define PIONEER_FRONT_PANEL_KEYCODE_VERSION5_OPEN_CLOSE                          0x01
#define PIONEER_FRONT_PANEL_KEYCODE_VERSION5_REVERSE				                  0x11
#define PIONEER_FRONT_PANEL_KEYCODE_VERSION5_FORWARD                             0x10
#define PIONEER_FRONT_PANEL_KEYCODE_VERSION5_STOP                                0x12
#define PIONEER_FRONT_PANEL_KEYCODE_VERSION5_PAUSE                               0x13
#define PIONEER_FRONT_PANEL_KEYCODE_VERSION5_PLAY                                0x00
#define PIONEER_FRONT_PANEL_KEYCODE_VERSION5_POWER                               0x06
#define PIONEER_FRONT_PANEL_KEYCODE_VERSION5_VNR                                 0x20

#define PIONEER_FRONT_PANEL_KEYCODE_VERSION14_OPEN_CLOSE                         0x01
#define PIONEER_FRONT_PANEL_KEYCODE_VERSION14_REVERSE				                  0x13
#define PIONEER_FRONT_PANEL_KEYCODE_VERSION14_FORWARD                            0x12
#define PIONEER_FRONT_PANEL_KEYCODE_VERSION14_STOP                               0x11
#define PIONEER_FRONT_PANEL_KEYCODE_VERSION14_PAUSE                              0x10
#define PIONEER_FRONT_PANEL_KEYCODE_VERSION14_PLAY                               0x00
#define PIONEER_FRONT_PANEL_KEYCODE_VERSION14_POWER                              0x06
#define PIONEER_FRONT_PANEL_KEYCODE_VERSION14_VNR                                0x20

#define PIONEER_FRONT_PANEL_REMOTE_POWER								                  0xbc
#define PIONEER_FRONT_PANEL_REMOTE_OPEN_CLOSE						                  0xb6

#define PIONEER_FRONT_PANEL_REMOTE_SELECT_UP                                     0xf2
#define PIONEER_FRONT_PANEL_REMOTE_SELECT_LEFT                                   0x63
#define PIONEER_FRONT_PANEL_REMOTE_SELECT_RIGHT                                  0x64
#define PIONEER_FRONT_PANEL_REMOTE_SELECT_DOWN                                   0xf3
#define PIONEER_FRONT_PANEL_REMOTE_SELECT                                        0xef

#define PIONEER_FRONT_PANEL_REMOTE_DISPLAY                                       0xe3
#define PIONEER_FRONT_PANEL_REMOTE_SETUP                                         0xb0
#define PIONEER_FRONT_PANEL_REMOTE_MENU                                          0xb9

#define PIONEER_FRONT_PANEL_REMOTE_SKIP_REVERSE                                  0x9d
#define PIONEER_FRONT_PANEL_REMOTE_SKIP_FORWARD                                  0x9c
#define PIONEER_FRONT_PANEL_REMOTE_SCAN_REVERSE                                  0x9b
#define PIONEER_FRONT_PANEL_REMOTE_SCAN_FORWARD                                  0x9a
#define PIONEER_FRONT_PANEL_REMOTE_STILLFRAME_REVERSE                            0xb8
#define PIONEER_FRONT_PANEL_REMOTE_STILLFRAME_FORWARD                            0xb7

#define PIONEER_FRONT_PANEL_REMOTE_AUDIO                                         0xbe
#define PIONEER_FRONT_PANEL_REMOTE_SUBTITLE                                      0x36
#define PIONEER_FRONT_PANEL_REMOTE_ANGLE                                         0xb5
#define PIONEER_FRONT_PANEL_REMOTE_RETURN                                        0xf4
#define PIONEER_FRONT_PANEL_REMOTE_PLAY                                          0x9e
#define PIONEER_FRONT_PANEL_REMOTE_STOP                                          0x98
#define PIONEER_FRONT_PANEL_REMOTE_PAUSE                                         0x9f
#define PIONEER_FRONT_PANEL_REMOTE_SEARCH_MODE                                   0xb3
#define PIONEER_FRONT_PANEL_REMOTE_CLEAR                                         0xe5
#define PIONEER_FRONT_PANEL_REMOTE_PROGRAM                                       0xec
#define PIONEER_FRONT_PANEL_REMOTE_RANDOM                                        0xfe
#define PIONEER_FRONT_PANEL_REMOTE_REPEAT                                        0xe4
#define PIONEER_FRONT_PANEL_REMOTE_AB                                            0xe8

#define PIONEER_FRONT_PANEL_REMOTE_1                                             0xa1
#define PIONEER_FRONT_PANEL_REMOTE_2									                  0xa2
#define PIONEER_FRONT_PANEL_REMOTE_3                                             0xa3
#define PIONEER_FRONT_PANEL_REMOTE_4                                             0xa4
#define PIONEER_FRONT_PANEL_REMOTE_5                                             0xa5
#define PIONEER_FRONT_PANEL_REMOTE_6                                             0xa6
#define PIONEER_FRONT_PANEL_REMOTE_7                                             0xa7
#define PIONEER_FRONT_PANEL_REMOTE_8                                             0xa8
#define PIONEER_FRONT_PANEL_REMOTE_9                                             0xa9
#define PIONEER_FRONT_PANEL_REMOTE_0                                             0xa0
#define PIONEER_FRONT_PANEL_REMOTE_PLUS10                                        0xbf

#define PIONEER_FRONT_PANEL_REMOTE_CONDITION_MEMORY                              0xb1
#define PIONEER_FRONT_PANEL_REMOTE_LASTMEMORY                                    0xf6
#define PIONEER_FRONT_PANEL_REMOTE_VNR                                           0x30
#define PIONEER_FRONT_PANEL_REMOTE_TOP_MENU							                  0xb4

#define PIONEER_FRONT_PANEL_COMMAND_DISPLAY                                      0x20
#define PIONEER_FRONT_PANEL_COMMAND_ENABLE_DISPLAY_ALL                           0x01
#define PIONEER_FRONT_PANEL_COMMAND_ENABLE_RESET_ON_ERROR                        0x03
#define PIONEER_FRONT_PANEL_COMMAND_DISABLE_DISPLAY_ALL                          0x06
#define PIONEER_FRONT_PANEL_COMMAND_DISABLE_TESTMODE                             0x0f
#define PIONEER_FRONT_PANEL_COMMAND_POWER_OFF                                    0x10
#define PIONEER_FRONT_PANEL_COMMAND_RESET_REQUEST                                0x12
#define PIONEER_FRONT_PANEL_COMMAND_ENABLE_TESTMODE                              0x14
#define PIONEER_FRONT_PANEL_COMMAND_DISABLE_RESET_ON_ERROR                       0x15

#define PIONEERFRONTPANEL_COMPLETIONCODE_BUTTONPRESSED                           0x01
#define PIONEERFRONTPANEL_COMPLETIONCODE_DVDREMOTEPRESSED                        0xa3
#define PIONEERFRONTPANEL_COMPLETIONCODE_LASERDISCREMOTEPRESSED                  0xa8
#define PIONEERFRONTPANEL_COMPLETIONCODE_NOCOMMAND                               0xff

#define PIONEER_FRONT_PANEL_SEGMENT_CONDITION						                  0x0001
#define PIONEER_FRONT_PANEL_SEGMENT_LAST_MEMO						                  0x0002
#define PIONEER_FRONT_PANEL_SEGMENT_ANGLE								                  0x0004
#define PIONEER_FRONT_PANEL_SEGMENT_TITLE								                  0x0008
#define PIONEER_FRONT_PANEL_SEGMENT_TOTAL								                  0x0010
#define PIONEER_FRONT_PANEL_SEGMENT_REMAIN							                  0x0020
#define PIONEER_FRONT_PANEL_SEGMENT_DOLBY_DIGITAL					                  0x0040
#define PIONEER_FRONT_PANEL_SEGMENT_96KHZ								                  0x0080
#define PIONEER_FRONT_PANEL_SEGMENT_CHPTRK   						                  0x0100
#define PIONEER_FRONT_PANEL_SEGMENT_DOT_LEFT							                  0x0200
#define PIONEER_FRONT_PANEL_SEGMENT_DOT_RIGHT						                  0x0400
#define PIONEER_FRONT_PANEL_SEGMENT_COLON								                  0x0800
#define PIONEER_FRONT_PANEL_SEGMENT_GUI							                     0x1000
#define PIONEER_FRONT_PANEL_SEGMENT_LED                                          0x2000

#define PIONEER_FRONT_PANEL_DISPLAY_SIZE			                                 10      // ten character display

#define PIONEER_FRONT_PANEL_TITLE_FIELD_POSITION                                 1
#define PIONEER_FRONT_PANEL_TITLE_FIELD_WIDTH                                    2

#define PIONEER_FRONT_PANEL_TIME_FIELD_POSITION                                  6
#define PIONEER_FRONT_PANEL_TIME_FIELD_WIDTH                                     5

#define PIONEER_FRONT_PANEL_CHAPTER_FIELD_POSITION                               4
#define PIONEER_FRONT_PANEL_CHAPTER_FIELD_WIDTH                                  2

#define PIONEER_FRONT_PANEL_TRACK_FIELD_POSITION                                 4
#define PIONEER_FRONT_PANEL_TRACK_FIELD_WIDTH                                    2

#define PIONEER_FRONT_PANEL_ANGLE_FIELD_POSITION                                 6
#define PIONEER_FRONT_PANEL_ANGLE_FIELD_WIDTH                                    5


#define PIONEER_FRONT_PANEL_TRAY_STATUS_FIELD_POSITION                           1
#define PIONEER_FRONT_PANEL_TRAY_STATUS_FIELD_WIDTH                              10

#define PIONEER_FRONT_PANEL_TIME_FIELD_POSITION                                  6
#define PIONEER_FRONT_PANEL_TIME_FIELD_WIDTH                                     5

#define PIONEER_FRONT_PANEL_MINUTE_FIELD_POSITION                                6
#define PIONEER_FRONT_PANEL_MINUTE_FIELD_WIDTH                                   3

#define PIONEER_FRONT_PANEL_SECOND_FIELD_POSITION                                9
#define PIONEER_FRONT_PANEL_SECOND_FIELD_WIDTH                                   2

#define PIONEER_FRONT_PANEL_POLLING_STATE_UPDATE_SEGMENTS                        0
#define PIONEER_FRONT_PANEL_POLLING_STATE_UPDATE_CHARACTERS                      1


#define PIONEER_FRONT_PANEL_ANGLE_FIELD_DISPLAY_TIME                             5000    // milsec = 5sec


class PioneerFrontPanel : public GenericPanel, public PollingPanel
	{
	private:
      MemoryMappedIO       mychip;

      BYTE                 packet[PIONEER_FRONT_PANEL_PACKET_SIZE];

      WORD                 segmentBuffer;
      BYTE                 characterBuffer[PIONEER_FRONT_PANEL_DISPLAY_SIZE];
 
      BYTE                 pollingState;
      BOOL                 initialized;

      BOOL                 segmentUpdateRequired;
      BOOL                 displayUpdateRequired;
      BOOL                 testMode;

      DWORD                fieldLockFlags;

      PanelDiskType        diskType;
      PanelAudioType       audioType;


      PanelState           prePanelState;
      PanelState           panelState;
      PanelTrayStatus      trayStatus;

      DWORD                angleTimer;

      BYTE                 editPos;
      PanelTimeMode        timeMode;
      int                  time;
      int                  title;
      int                  numOfTitles;
      int                  chapter;
      int                  numOfChapters;
      int                  angle;
      int                  numOfAngles;



	protected:

           BOOL     PollCEMessage         ( DWORD &ceMsg );
           void     ExecuteOperation      ( POP pop );

           void     InitializeMychip      ( void );

           void     ExecuteCommand        ( BYTE command );
           void     Reset                 ( void );
           void     EnableDisplayAll      ( void );				
			  void     DisableDisplayAll     ( void );
           void     EnableResetOnError    ( void );
           void     DisableResetOnError   ( void );
           void     EnableTestMode        ( void );
           void     DisableTestMode       ( void );
			  BYTE     CalculateChecksum     ( void );
           void     MakePacket            ( BYTE type, BYTE command, BYTE data1, BYTE data2, BYTE data3, BYTE data4, BYTE data5, BYTE data6, BYTE data7, BYTE data8, BYTE data9, BYTE data10, BYTE data11, BYTE data12, BYTE data13 );
           DWORD    TranslateRemote       ( BYTE code );
           DWORD    TranslateButton       ( BYTE version, BYTE code );
           DWORD    ProcessReturnPacket   ( DWORD* message, BOOL* messageReceived );
           DWORD    SendPacket            ( DWORD* message, BOOL* messageReceived );
           DWORD    UpdateSegments        ( DWORD* message, BOOL* messageReceived );
           DWORD    UpdateCharacters      ( DWORD* message, BOOL* messageReceived );
  
           void     EnableSegment         ( WORD segmentId );
           void     DisableSegment        ( WORD segmentId );
           void     ClearDisplay          ( void );

           void     SetTitle              ( void );
           void     ClearTitleField       ( void );

           void     SetChapter            ( void );
           void     ClearChapterField     ( void );

           void     SetTrack              ( void );
           void     ClearTrackField       ( void );

           void     SetTime               ( void );
           void     ClearTimeField        ( void );

           void     SetAngle              ( void );
           void     ClearAngleField       ( void );

           void     BeginEditField        ( DWORD field );
           void     EditWriteDigit        ( BYTE position, BYTE number );


           void     PowerOff              ( void );

           void     LockField             ( DWORD field );

           void     SetTrayStatus         ( PanelTrayStatus trayStatus );

           void     Write                 ( BYTE position, BYTE data );

           void     SetDiskType           (void);
           void     SetAudioType          (void);
           void     SetPanelState         (void);



#ifdef PIONEER_FRONT_PANEL_DIAGNOSTICS
           void     ShiftCharactersLeft   ( void );
           void     WriteScrollingMessage ( char* string, DWORD shiftDelay );
           void     Diagnostics           ( void );
#endif


	public:
                    PioneerFrontPanel ( int taskPriority );
                   ~PioneerFrontPanel ( void );
      POPList       GetInitPOPList    ( int __far & size );
      VirtualUnit*  CreateVirtual     ( void );
      void          SetSystemPower    ( BOOL on );

         

	};


class VirtualPioneerFrontPanel : public VirtualPanel
	{
	public:
		VirtualPioneerFrontPanel(PioneerFrontPanel* panel) : VirtualPanel(panel) {;}
	};


#endif


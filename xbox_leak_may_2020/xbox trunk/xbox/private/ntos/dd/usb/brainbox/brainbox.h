//Declare the type
extern   XPP_DEVICE_TYPE       XDEVICE_TYPE_BRAINBOX_TABLE;
#define  XDEVICE_TYPE_BRAINBOX (&XDEVICE_TYPE_BRAINBOX_TABLE)

#include <PSHPACK1.H>
typedef struct _XINPUT_BRAINBOX_DATA
{
    WORD  bmButtons[3];
    WORD  wRightLeverX;
    WORD  wRightLeverY;
    WORD  wLeftLeverX;
    WORD  wUpperLeftLeverX;
    WORD  wUpperLeftLeverY;
    WORD  wFootPedal1;
    WORD  wFootPedal2;
    WORD  wFootPedal3;
    BYTE  bTuner;
    BYTE  bShiftLever;
} XINPUT_BRAINBOX_DATA;

typedef struct _XINPUT_BRAINBOX
{
  DWORD dwPacketNumber;
  XINPUT_BRAINBOX_DATA data;
} XINPUT_BRAINBOX, *PXINPUT_BRAINBOX;

// You can write your own named macros for getting at the digital buttons.
typedef struct _XINPUT_BRAINBOX_LEDS
{
    XINPUT_FEEDBACK_HEADER Header;
    WORD bmLEDs[3];
} XINPUT_BRAINBOX_LEDS, *PXINPUT_BRAINBOX_LEDS;

//UNTIL POST DECEMBER RELEASE DON'T USE GET CAPABILITIES 

#include <POPPACK.H>



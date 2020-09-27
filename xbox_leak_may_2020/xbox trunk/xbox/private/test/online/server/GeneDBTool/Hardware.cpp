#include <stdio.h>
#include <xapip.h>
#include <winbasep.h>
#include <rc4.h>
#include <sha.h>
#include <xdbg.h>
#include <xboxp.h>
#include "Hardware.h"
#include "xboxvideo.h"
#include "usbmanager.h"
#include "constants.h"

USBManager Controllers;
extern Key m_keyPad[DISPLAY_NUM_COLS][DISPLAY_NUM_ROWS];
extern int m_xKeypos;
extern int m_yKeypos;
extern WCHAR g_szSerialNumber[120];
extern INT   g_iSerialNumberIndex;
extern bool g_bLoop;


bool m_bDropKeyPress;  
DWORD          m_keyPressDelayTimer;                // Used to temper the speed of holding down the d-pad
DWORD          m_keyPressDelayInterval;             // The current interval to delay before processing a keypress
int            m_nJoystickDeadZone;                 // The dead zone for the joysticks




void GetIntelCPUID( IntelCPUID* pCPUID )
{
    DWORD cpuid_low = 0;
    DWORD cpuid_mid = 0;
    DWORD cpuid_high = 0;

    _asm {
        mov eax, 1
        _asm cpuid
        //_asm _emit 0x0f   //CPUID instruction
        //_asm _emit 0xa2

        mov  cpuid_high, eax
        
        mov eax, 3
        _asm cpuid
        //_asm _emit 0x0f   //CPUID instruction
        //_asm _emit 0xa2

        mov  cpuid_mid, edx
        mov  cpuid_low, ecx
    }

    pCPUID->low = cpuid_low;
    pCPUID->mid = cpuid_mid;
    pCPUID->high = cpuid_high;
}



 // Handle input from the user
HRESULT ProcessInput( void )
{

   bool bButtonPressed;
   bool bFirstPress;
   bool bFirstXPress;
   bool bFirstYPress;
   int  nThumbY = 0;
   int  nThumbX = 0;

   // Update the state of all controllers
   Controllers.ProcessInput();

   // A BUTTON
   if ( bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_A, BUTTON_THRESHOLD, &bFirstPress ) ) {
      HandleInput( BUTTON_A, bFirstPress );
   }

   // B BUTTON
   if ( bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_B, BUTTON_THRESHOLD, &bFirstPress ) ) {
      HandleInput( BUTTON_B, bFirstPress );
   }

   // X BUTTON
   if ( bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_X, BUTTON_THRESHOLD, &bFirstPress ) ) {
      HandleInput( BUTTON_X, bFirstPress );
   }

   // Y BUTTON
   if ( bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_Y, BUTTON_THRESHOLD, &bFirstPress ) ) {
      HandleInput( BUTTON_Y, bFirstPress );
   }

   // BLACK BUTTON
   if ( bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_BLACK, BUTTON_THRESHOLD, &bFirstPress ) ) {
      HandleInput( BUTTON_BLACK, bFirstPress );
   }

   // WHITE BUTTON
   if ( bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_WHITE, BUTTON_THRESHOLD, &bFirstPress ) ) {
      HandleInput( BUTTON_WHITE, bFirstPress );
   }

   // LEFT TRIGGER BUTTON
   if ( bButtonPressed = Controllers.IsButtonPressed( PORT_ANY, BUTTON_LEFT_TRIGGER, BUTTON_THRESHOLD, &bFirstPress ) ) {
      HandleInput( BUTTON_LEFT_TRIGGER, bFirstPress );
   }

   // UP DPAD
   if ( bButtonPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_UP, &bFirstPress ) ) {
      HandleInput( CONTROL_DPAD_UP, bFirstPress );
   }

   // DOWN DPAD
   if ( bButtonPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_DOWN, &bFirstPress ) ) {
      HandleInput( CONTROL_DPAD_DOWN, bFirstPress );
   }

   // LEFT DPAD
   if ( bButtonPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_LEFT, &bFirstPress ) ) {
      HandleInput( CONTROL_DPAD_LEFT, bFirstPress );
   }

   // RIGHT DPAD
   if ( bButtonPressed = Controllers.IsControlPressed( PORT_ANY, CONTROL_DPAD_RIGHT, &bFirstPress ) ) {
      HandleInput( CONTROL_DPAD_RIGHT, bFirstPress );
   }

   // LEFT THUMB STICK
   nThumbY = Controllers.GetJoystickY( PORT_ANY, JOYSTICK_LEFT, JOYSTICK_DEAD_ZONE, &bFirstYPress );
   nThumbX = Controllers.GetJoystickX( PORT_ANY, JOYSTICK_LEFT, JOYSTICK_DEAD_ZONE, &bFirstXPress );
   if ( ( nThumbY != 0 ) || ( nThumbX != 0 ) ) {
      HandleInput( JOYSTICK_LEFT, nThumbY, nThumbX, bFirstYPress, bFirstXPress );
   }

   // RIGHT THUMB STICK
   nThumbY = Controllers.GetJoystickY( PORT_ANY, JOYSTICK_RIGHT, JOYSTICK_DEAD_ZONE, &bFirstYPress );
   nThumbX = Controllers.GetJoystickX( PORT_ANY, JOYSTICK_RIGHT, JOYSTICK_DEAD_ZONE, &bFirstXPress );
   if ( ( nThumbY != 0 ) || ( nThumbX != 0 ) ) {
      HandleInput( JOYSTICK_RIGHT, nThumbY, nThumbX, bFirstYPress, bFirstXPress );
   }
   
   XDBGWRN( APP_TITLE_NAME_A, "Process Input Finished.");

   return S_OK;
}


// Handle the Joystick Input
void 
HandleInput( enum JOYSTICK joystick, 
             int  nThumbY, 
             int  nThumbX, 
             bool bFirstYPress, 
             bool bFirstXPress )
{
   switch ( joystick ) {
   // This is the LEFT Thumb Joystick on the controller
   case JOYSTICK_LEFT:
      {
         // Y
         if ( ( nThumbY < 0 ) && ( abs( nThumbY ) > m_nJoystickDeadZone ) ) { // Move the left joystick down
            // Hand off this to the "DPad" handler
            HandleInput( CONTROL_DPAD_DOWN, bFirstYPress );
         } else if ( ( nThumbY > 0 ) && ( abs( nThumbY ) > m_nJoystickDeadZone ) ) {// Move left joystick up
            // Hand off this to the "DPad" handler
            HandleInput( CONTROL_DPAD_UP, bFirstYPress );
         }

         // X
         if ( ( nThumbX < 0 ) && ( abs( nThumbX ) > m_nJoystickDeadZone ) ) { // Move the left joystick left
            // Hand off this to the "DPad" handler
            HandleInput( CONTROL_DPAD_LEFT, bFirstXPress );
         } else if ( ( nThumbX > 0 ) && ( abs( nThumbX ) > m_nJoystickDeadZone ) ) {// Move the left joystick right
            HandleInput( CONTROL_DPAD_RIGHT, bFirstXPress );
         }
         break;
      }
      // This is the RIGHT Thumb Joystick on the controller
   case JOYSTICK_RIGHT:
      {
         // Y
         if ( nThumbX < 0 ) { // Move the right joystick to the left
            // Hand off this to the "DPad" handler
            HandleInput( CONTROL_DPAD_DOWN, bFirstYPress );

         } else if ( nThumbX > 0 ) { // Move right joystick to the right
            // Hand off this to the "DPad" handler
            HandleInput( CONTROL_DPAD_UP, bFirstYPress );
         }
         // X
         if ( nThumbX < 0 ) { // Move the right joystick left
            // Hand off this to the "DPad" handler
            HandleInput( CONTROL_DPAD_LEFT, bFirstXPress );
         } else if ( nThumbX > 0 ) { // Move the right joystick right
            HandleInput( CONTROL_DPAD_RIGHT, bFirstXPress );
         }
         break;
      }
   }
}


// Handles input (of the CONTROLS) for the current menu
void 
HandleInput( enum CONTROLS controlPressed, 
             bool bFirstPress )
{
   if ( bFirstPress ) {
      
      m_keyPressDelayTimer = GetTickCount();
      m_keyPressDelayInterval = KEY_PRESS_INITIAL_DELAY;
   
   } else { // Check to see if the repeat press is within our timer, otherwise bail
      
      // If the interval is too small, bail
      if ( ( GetTickCount() - m_keyPressDelayTimer ) < m_keyPressDelayInterval ) {
         return;
      }
      
      m_keyPressDelayTimer = GetTickCount();
      m_keyPressDelayInterval = KEY_PRESS_REPEAT_DELAY;
   }


    // Handle Buttons being pressed
    switch( controlPressed )
    {
	case CONTROL_START:
		{
			break;
		}
	case CONTROL_BACK:
		{
			break;
		}
	case CONTROL_LEFT_THUMB:
		{
			break;
		}
	case CONTROL_RIGHT_THUMB:
		{
			break;
		}
    case CONTROL_DPAD_UP:
        {
			if( ( m_yKeypos != 0 ) && ( m_keyPad[m_xKeypos][m_yKeypos - 1].getRender() ) )
			{
				m_yKeypos--;
			}
			else if( ( m_yKeypos != 0 ) && ( !m_keyPad[m_xKeypos][m_yKeypos - 1].getRender() ) )
			{
				m_yKeypos = m_yKeypos - 2;
			}
            break;
        }
    case CONTROL_DPAD_DOWN:
        {
         if ( ( m_yKeypos != 3 ) && ( m_keyPad[m_xKeypos][m_yKeypos + 1].getRender() ) ) {
            m_yKeypos++;
         } else if ( ( m_yKeypos != 3 ) && ( !m_keyPad[m_xKeypos][m_yKeypos + 1].getRender() ) ) {
            m_yKeypos = m_yKeypos + 2;
         }
            break;
        }
    case CONTROL_DPAD_LEFT:
        {
			if( m_xKeypos != 0 )
			{
				m_xKeypos--;
			}
            break;
        }
    case CONTROL_DPAD_RIGHT:
        {
			if( ( m_xKeypos != 9 ) && ( m_keyPad[m_xKeypos + 1][m_yKeypos].getRender())) 
			{
				m_xKeypos++;
			}
			else if((m_xKeypos != 9) && (!m_keyPad[m_xKeypos + 1][m_yKeypos].getRender()))
			{
				m_xKeypos++;
				m_yKeypos--;
			}
            break;
        }
	}
}



// Handles input (of the BUTTONS) for the current menu
void 
HandleInput( enum BUTTONS buttonPressed, 
             bool bFirstPress )
{

   if ( bFirstPress ) {
      m_bDropKeyPress = false;

      m_keyPressDelayTimer = GetTickCount();
      m_keyPressDelayInterval = KEY_PRESS_INITIAL_DELAY;
   } else { // Check to see if the repeat press is within our timer, otherwise bail
      // If the interval is too small, bail
      if ( ( GetTickCount() - m_keyPressDelayTimer ) < m_keyPressDelayInterval ) {
         return;
      }
      m_keyPressDelayTimer = GetTickCount();
      m_keyPressDelayInterval = KEY_PRESS_REPEAT_DELAY;
   }

   if ( m_bDropKeyPress ) {
      return;
   }

   // Handle Buttons being pressed
   g_iSerialNumberIndex = wcslen(g_szSerialNumber) - 1;
   
   switch ( buttonPressed ) {
   case BUTTON_A:
      {
         // Deal with keyboard input
         if (0 == wcscmp(m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"«")) {            //backspace
            
            if (4 == g_iSerialNumberIndex ||
                8 == g_iSerialNumberIndex ||
                16 == g_iSerialNumberIndex ) {
               g_szSerialNumber[g_iSerialNumberIndex] = L'\0';
            }
            g_szSerialNumber[g_iSerialNumberIndex] = L'\0';
            XDBGTRC( APP_TITLE_NAME_A, "%s", g_szSerialNumber );
         
         } else if (0 == wcscmp(m_keyPad[m_xKeypos][m_yKeypos].resultChar, L"ok")) {      //ok
            
            if (22 > g_iSerialNumberIndex) {
               break;
            }
            g_bLoop = FALSE;
            m_xKeypos = m_yKeypos = 0;
            XDBGTRC( APP_TITLE_NAME_A, "CKeyboardMenu::HandleInput():Ok Selected" );
         }else {
            if (4 == g_iSerialNumberIndex ||
                8 == g_iSerialNumberIndex ||
                16 == g_iSerialNumberIndex ) {
               wcscat(g_szSerialNumber, L"-");
            }
            if (21 >= g_iSerialNumberIndex )
               wcscat(g_szSerialNumber, m_keyPad[m_xKeypos][m_yKeypos].resultChar);
         }


         break;
      }
   case BUTTON_B:
      {
         //m_xKeypos = m_yKeypos = 0;
         // erase a character; backup
         if (5 == g_iSerialNumberIndex ||
             9 == g_iSerialNumberIndex ||
             17 == g_iSerialNumberIndex ) {
            g_szSerialNumber[g_iSerialNumberIndex] = L'\0';
         }
         g_szSerialNumber[g_iSerialNumberIndex] = L'\0';
         break;
      }
   case BUTTON_X:
      {
         break;
      }
   case BUTTON_Y:
      {
         break;
      }
   case BUTTON_BLACK:
      {
         break;
      }
   case BUTTON_WHITE:
      {
         break;
      }
   case BUTTON_LEFT_TRIGGER:
      {
         break;
      }
   case BUTTON_RIGHT_TRIGGER:
      {
         break;
      }
   }
}

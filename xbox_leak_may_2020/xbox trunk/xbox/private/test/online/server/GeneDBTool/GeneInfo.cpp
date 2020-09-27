//#include <xtl.h>
#include <stdio.h>
#include <xapip.h>
#include <winbasep.h>
#include <rc4.h>
#include <sha.h>
#include <xdbg.h>
#include <xboxp.h>
#include <XbDm.h>
#include "hardware.h"
#include <xapip.h>
#include "xboxvideo.h"
#include "constants.h"

#define BREAK_INTO_DEBUGGER     _asm { int 3 }

int   m_xKeypos;
int   m_yKeypos;
Key   m_keyPad[DISPLAY_NUM_COLS][DISPLAY_NUM_ROWS];           // Keypad
CONST PCHAR SZDATAFILE = "t:\\Genealogy.log";
bool g_bLoop = TRUE;
CXBoxVideo g_x;
WCHAR g_szSerialNumber[120]= L"\0";
INT   g_iSerialNumberIndex = 1;


void GenerateKeypad(void);
HRESULT DrawScreen( void );
HRESULT WriteGenelogyLog( void );



void __cdecl main()
{
   //BREAK_INTO_DEBUGGER;
   g_x.Initialize( SCREEN_WIDTH, SCREEN_HEIGHT );
   
   
   do {
      DrawScreen();
      ProcessInput();
   } while ( g_bLoop == TRUE );
   g_x.DeInitialize();
   
   WriteGenelogyLog();

   // Reboot this baby!
   DmReboot(DMBOOT_WARM);
	return;
}



void GenerateKeypad(void) 
{
   // First create numberpad
   int xgrid  = 3;
   int ygrid  = 4;
   int xstart = 210;
   int ystart = KEYPAD_TOPROW_Y1; // 234;
   int width  = 32;
   int height = 32;
   int spacer = 2;

   int currentx = xstart;
   int currenty = ystart;

   for (int y = 0; y < ygrid; y++) {
      for (int x = 0; x < xgrid; x++) {
         m_keyPad[x][y].define( currentx, currenty, width, height, COLOR_YELLOW );
         currentx += width + spacer;
      }
      currentx = xstart;
      currenty += height + spacer;
   }


   // Second generate side larger buttons for OK CANCEL etc.
   int xbase  = xgrid;
   xgrid  = 1;
   ygrid  = 4;
   xstart = 323;
   ystart = KEYPAD_TOPROW_Y1; // 234;
   width  = 103;
   height = 32;
   spacer = 2;

   currentx = xstart;
   currenty = ystart;
   for (y = 0; y < ygrid; y++) {
      for (int x = xbase; x < (xbase + xgrid); x++) {
         m_keyPad[x][y].define( currentx, currenty, width, height, COLOR_LIME_GREEN );
         currentx += width + spacer;

         // clear out placeholder
         if (y == 1) m_keyPad[x][y].setRender(false);
         if (y == 2) m_keyPad[x][y].setRender(false);
         if (y == 3) m_keyPad[x][y].setRender(false);
      }
      currentx = xstart;
      currenty += height + spacer;
   }


   // Set the text of the keys
   m_keyPad[0][0].defineText(L"1");  m_keyPad[1][0].defineText(L"2");  m_keyPad[2][0].defineText(L"3");
   m_keyPad[0][1].defineText(L"4");  m_keyPad[1][1].defineText(L"5");  m_keyPad[2][1].defineText(L"6");
   m_keyPad[0][2].defineText(L"7");  m_keyPad[1][2].defineText(L"8");  m_keyPad[2][2].defineText(L"9");
   m_keyPad[0][3].defineText(L".");  m_keyPad[1][3].defineText(L"0");  m_keyPad[2][3].defineText(L"«");
   m_keyPad[3][0].defineText(L"ok");
   m_keyPad[3][1].defineText(L"placeholder");
   m_keyPad[3][2].defineText(L"placeholder");
   m_keyPad[3][3].defineText(L"placeholder");
}


// Draw our screen

HRESULT DrawScreen( void )
{

   
   g_x.ClearScreen( SCREEN_DEFAULT_BACKGROUND_COLOR );
   
   // draw the number we are building on screen
   g_x.DrawText( 150, 
                 70, 
                 SCREEN_DEFAULT_TEXT_FG_COLOR, 
                 COLOR_LIME_GREEN, 
                 L"Enter the serial number please");
   g_x.DrawText( 170, 
                 100, 
                 SCREEN_DEFAULT_TEXT_FG_COLOR, 
                 COLOR_WHITE, 
                 L"%s", g_szSerialNumber);


   // Render outlines of keys	
   float x1, x2;
   float y1, y2;
   GenerateKeypad();
   for (int x = 0; x < KEYPAD_NUM_ROWS; x++) {
      for (int y = 0; y < KEYPAD_NUM_COLS; y++) {
         if (m_keyPad[x][y].getRender()){
            
            x1 = (float)m_keyPad[x][y].xorigin;
            x2 = (float)(m_keyPad[x][y].xorigin + m_keyPad[x][y].width);

            y1 = (float)m_keyPad[x][y].yorigin;
            y2 = (float)(m_keyPad[x][y].yorigin + m_keyPad[x][y].height);

            g_x.DrawOutline( x1, y1, x2, y2, KEYPAD_LINE_WIDTH, m_keyPad[x][y].selectColor );


            // hilight key if currently selected
            DWORD dwCharColor;
            if ((m_xKeypos == x) && (m_yKeypos == y)) {
               // Draw the selector box around the currently highlighted number
               g_x.DrawBox( x1, y1, x2, y2, ITEM_SELECTOR_COLOR );

               dwCharColor = ITEM_SELECTED_TEXT_COLOR;
            } else {
               dwCharColor = SCREEN_DEFAULT_TEXT_FG_COLOR;
            }

            unsigned int decent;
            unsigned int fontHeight;
            g_x.GetFontSize( &fontHeight, &decent );

            float textX = (float)((m_keyPad[x][y].width / 2) + m_keyPad[x][y].xorigin) - g_x.GetStringPixelWidth( m_keyPad[x][y].resultChar ) / 2;
            float textY = (float)((m_keyPad[x][y].height / 2) + m_keyPad[x][y].yorigin) - (fontHeight / 2);
            g_x.DrawText( textX, textY, dwCharColor, COLOR_WHITE, L"%s", m_keyPad[x][y].resultChar);

         }
      }
   }
   g_x.ShowScreen();
   return S_OK;
}

	

HRESULT WriteGenelogyLog( void )
{


	CHAR szBuffer[ 1024 ];
	CHAR szDiskModelNumber[ 60 ];
	CHAR szDiskSerialNumber[ 60 ];
   HANDLE h;
	DWORD dwNumberOfBytesWritten;
	DWORD dwNumberOfBytesToWrite;
   
   ZeroMemory(szDiskModelNumber,60);
   ZeroMemory(szDiskSerialNumber,60);
   
   h = CreateFile ( SZDATAFILE, 
					 GENERIC_READ | GENERIC_WRITE,
					 FILE_SHARE_WRITE,
					 NULL,
					 CREATE_ALWAYS,
					 FILE_ATTRIBUTE_NORMAL,
					 NULL);


	IntelCPUID CPUID;
   GetIntelCPUID( &CPUID);
   _snprintf( szDiskModelNumber, HalDiskModelNumber->Length, 
              "%s",
              HalDiskModelNumber->Buffer);

   _snprintf( szDiskSerialNumber, HalDiskSerialNumber->Length, 
              "%s",
              HalDiskSerialNumber->Buffer); 
  
   sprintf(szBuffer, // + strlen(szBuffer),
             "Model  %s\r\nSerial %s\r\nCpuId: %lu %lu %lu \r\nS/N: %S\r\n\0",
             szDiskModelNumber,
             szDiskSerialNumber,
             CPUID.high, CPUID.mid ,CPUID.low,
             g_szSerialNumber); 

   dwNumberOfBytesToWrite = strlen(szBuffer);
   WriteFile( h, 
              szBuffer,
              dwNumberOfBytesToWrite,
              &dwNumberOfBytesWritten, 
              NULL );
   CloseHandle(h);

   return S_OK;
}

								  





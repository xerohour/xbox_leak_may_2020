#include <windows.h>
#pragma hdrstop
#include "..\..\inc\BLib.h"
#include "resource.h"

int WINAPI WinMain
(
  HINSTANCE hInstance,
  HINSTANCE hPrevInstance,
  LPSTR     lpCmdLine,
  int       nCmdShow
)
{
    //
    // make simple screen
    //
    if (1) 
    {
        BScreen screen;

        screen.create (hInstance);

        //screen.setFont ("Arial",20);
        //screen.setFont ("FixedSys");
        screen.showCaret();
        screen.gotoXY(10,10);
        screen.print ("Hello\nthere");

        BScreen::MESSAGE msg;
        for (BOOL bQuit = FALSE; !bQuit;)
        {
            if (screen.getMessage(&msg))
            {
                switch (msg.uMessage)
                {
                    case WM_CHAR: if (msg.wParam == 27) bQuit = TRUE;
                                  break;
                    case WM_CLOSE: bQuit = TRUE;
                                   break;
                }
            }
            else
            {
                // idle
                Sleep (10); // NOTE: waiting a bit on idle makes the screen update must more responsive
                //screen.setCell (5,5,screen.getCell(5,5) + 1);
            }
        }
        screen.destroy ();
    }

    //
    // make screen with user defined font
    //
    if (0) 
    {
        BScreen screen;

        screen.create (hInstance,80,25);

        // screen takes ownership and will call DeleteObject for you
        HBITMAP hBitmap = LoadBitmap(hInstance,MAKEINTRESOURCE(IDB_FONT));
        screen.setFont (hBitmap,
                        16,16,  // there are 16 x 16 cells (to logically grow, make this 16x17, 16x18, ...)
                        16,16); // each cell has 16 x 16 pixels

        screen.fillCells (0,0,80,25,0);
        screen.setCell   (1,1,0x0001);
        screen.setCell   (2,1,0x0101);
        screen.setCell   (2,2,0x0002);
        screen.setCell   (3,3,0x0003);

        BScreen::MESSAGE msg;
        for (BOOL bQuit = FALSE; !bQuit;)
        {
            if (screen.getMessage(&msg))
            {
                switch (msg.uMessage)
                {
                    case WM_CHAR: if (msg.wParam == 27) bQuit = TRUE;
                                  break;
                    case WM_CLOSE: bQuit = TRUE;
                                   break;
                }
            }
            else
            {
                // idle
                Sleep (10); // NOTE: waiting a bit on idle makes the screen update must more responsive
                //screen.setCell (5,5,screen.getCell(5,5) + 1);
            }
        }
        screen.destroy ();
    }

    //
    // make simple console
    //
    if (0)
    {
        BConsole console;

        console.create (hInstance,80,120);
        console.enableCommandHistory();
        console.enableNameCompletion();

        console.ncAdd ("aaabbb1");
        console.ncAdd ("aaabbb2");
        console.ncAdd ("abcdde");
        console.ncAdd ("abcd2");
        console.ncAdd ("dcb");
        console.ncAdd ("dce");

        for (int i = 0; i < 1000; i++)
        {
            console.print ("%d\r\n",i);
        }

        console.print (">");
        for (BOOL bQuit = FALSE; !bQuit;)
        {
            BString sz;
            switch (console.read(&sz))
            {
                case BConsole::closed: // user has issued a WM_CLOSE
                {
                    bQuit = 1; 
                    break;
                }
                case BConsole::success:
                {
                    console.print ("text \"%s\"\r\n",(const char*)sz);
                    console.print (">");
                    break;
                }
            }
        }

        console.destroy();
    }

    return 1;
}

 
 /*=======================================================================*/
 /*                                                                       */
 /*           (c) InstallShield Software Corporation (1996-1997)          */
 /*            (c)  InstallShield Corporation  (1990-1996)                */
 /*                       Schaumburg, Illinois 60173                      */
 /*                          All Rights Reserved                          */
 /*                           InstallShield (R)                           */
 /*                                                                       */
 /*                                                                       */
 /*      FILE:   WINSUB.H/*                                               */
 /*                                                                       */
 /*      PURPOSE:/*                                                       */
 /*         This header file defines the constants, global variables, and */
 /*         the prototype of the functions in the releated files.         */
 /*                                                                       */
 /*      RELATED FILES: WINSUB.RUL   /*                                   */
 /*                                                                       */
 /*=======================================================================*/   

#ifndef _WINSUBRUL_H
#define _WINSUBRUL_H  1

declare
     //
     // Function definitions
     //

     prototype _WinSubPostMessage     ( HWND, SHORT, SHORT, LONG );
     prototype _WinSubMoveWindow      ( HWND, INT, INT, INT, INT, INT );
     prototype _WinSubGetClientRect   ( HWND, BYREF INT, BYREF INT,
                                              BYREF INT, BYREF INT );
     prototype _WinSubGetModuleHandle ( STRING );
     prototype _WinSubShowCursor      ( INT );
     prototype _WinSubShowWindow      ( HWND, INT );
     prototype _WinSubFocusControl    ( HWND, INT );
     prototype _WinSubFocusWindow     ( HWND );
     prototype _WinSubEnableControl   ( HWND, INT, INT );
     prototype _WinSubGetChildWindow  ( HWND, INT );
     prototype _WinSubEnableWindow    ( HWND, INT );
     prototype _WinSubSetWindowTitle  ( HWND, STRING );
     prototype _WinSubCenterWindow    ( HWND );
     prototype _WinSubIsWindow        ( HWND );
     prototype _WinSubGetWindowRect   ( HWND, BYREF INT, BYREF INT,
                                              BYREF INT, BYREF INT );
     prototype _WinSubSetWindowPos    ( HWND, INT, INT, INT, INT, INT );


#ifndef _WIN_PROTOTYPES
#define _WIN_PROTOTYPES  1

     // Referenced DLL functions

     prototype  HWND GDI.SelectObject( HWND, HWND );
     prototype  BOOL GDI.GetTextExtentPoint( HWND, STRING, INT, POINTER );
     prototype  BOOL USER.EnableWindow( HWND, SHORT );
     prototype   INT USER.GetClassName( HWND, POINTER, INT );
     prototype  HWND USER.GetDC( HWND );
     prototype  HWND USER.GetDlgItem( HWND, INT );
     prototype  HWND USER.GetFocus();
     prototype  LONG USER.GetWindowLong( HWND, INT );
     prototype       USER.GetWindowRect( HWND, POINTER );
     prototype SHORT USER.GetWindowWord( HWND, INT );
     prototype  BOOL USER.IsIconic( HWND );
     prototype  BOOL USER.IsWindow( HWND );
     prototype  BOOL USER.IsWindowEnabled( HWND );
     prototype   INT USER.MoveWindow( HWND, INT, INT, INT, INT, INT );
     prototype   INT USER.ReleaseDC( HWND, HWND );
     prototype  HWND USER.SetFocus( HWND );
     prototype       USER.SetWindowText( HWND, STRING );
     prototype   INT USER.ShowWindow( HWND, SHORT );

     prototype  INT  USER.LoadString( INT, INT, STRING, INT );
     prototype  HWND KERNEL.GetModuleHandle( STRING );
     prototype       USER.GetClientRect( HWND, POINTER );
     prototype   INT USER.SetWindowPos( HWND, INT, INT, INT, INT, INT, INT );
     prototype  BOOL USER.PostMessage( HWND, SHORT, SHORT, LONG );
     prototype   INT USER.ShowCursor( SHORT );
     prototype  BOOL USER.SystemParametersInfo( HWND, INT, POINTER , INT );

#endif
     // Private entries
     prototype _WinSubSetWindowText ( HWND, STRING );
     prototype _WinSubErrDLL        ( STRING );
     prototype _WinSubErrDLLFunc    ( STRING );

      /****************  Constants definition  ****************/

// Constants retrieved from windows.h
#ifndef SWP_NOSIZE
      #define SWP_NOSIZE              0x0001
      #define SWP_NOMOVE              0x0002
      #define SWP_NOZORDER            0x0004
      #define SWP_NOREDRAW            0x0008
      #define SWP_NOACTIVATE          0x0010
      #define SWP_FRAMECHANGED        0x0020
      #define SWP_SHOWWINDOW          0x0040
      #define SWP_HIDEWINDOW          0x0080
      #define SWP_NOCOPYBITS          0x0100
      #define SWP_NOOWNERZORDER       0x0200
      #define SWP_DRAWFRAME           SWP_FRAMECHANGED
      #define SWP_NOREPOSITION        SWP_NOOWNERZORDER
      #define SWP_NOSENDCHANGING      0x0400
      #define SWP_DEFERERASE          0x2000
#endif

#ifndef SPI_GETBEEP
     #define SPI_GETBEEP                 1
     #define SPI_SETBEEP                 2
     #define SPI_GETMOUSE                3
     #define SPI_SETMOUSE                4
     #define SPI_GETBORDER               5
     #define SPI_SETBORDER               6
     #define SPI_GETKEYBOARDSPEED        10
     #define SPI_SETKEYBOARDSPEED        11
     #define SPI_LANGDRIVER              12
     #define SPI_ICONHORIZONTALSPACING   13
     #define SPI_GETSCREENSAVETIMEOUT    14
     #define SPI_SETSCREENSAVETIMEOUT    15
     #define SPI_GETSCREENSAVEACTIVE     16
     #define SPI_SETSCREENSAVEACTIVE     17
     #define SPI_GETGRIDGRANULARITY      18
     #define SPI_SETGRIDGRANULARITY      19
     #define SPI_SETDESKWALLPAPER        20
     #define SPI_SETDESKPATTERN          21
     #define SPI_GETKEYBOARDDELAY        22
     #define SPI_SETKEYBOARDDELAY        23
     #define SPI_ICONVERTICALSPACING     24
     #define SPI_GETICONTITLEWRAP        25
     #define SPI_SETICONTITLEWRAP        26
     #define SPI_GETMENUDROPALIGNMENT    27
     #define SPI_SETMENUDROPALIGNMENT    28
     #define SPI_SETDOUBLECLKWIDTH       29
     #define SPI_SETDOUBLECLKHEIGHT      30
     #define SPI_GETICONTITLELOGFONT     31
     #define SPI_SETDOUBLECLICKTIME      32
     #define SPI_SETMOUSEBUTTONSWAP      33
     #define SPI_SETICONTITLELOGFONT     34
     #define SPI_GETFASTTASKSWITCH       35
     #define SPI_SETFASTTASKSWITCH       36
#endif

// SystemParametersInfo flags
#ifndef SPIF_UPDATEINIFILE
     #define SPIF_UPDATEINIFILE          0x0001
     #define SPIF_SENDWININICHANGE       0x0002
#endif

// Private constants
#define MAX_WINSUB_TMPSTRING_LENGTH     80
#define MAX_WINSUB_MSGSTRING_LENGTH     255

      /****************  Structure definition  ****************/
typedef WINRECTSTRUCT
  begin
        INT     origX;
        INT     origY;
        INT     relX;
        INT     relY;
  end;


#endif

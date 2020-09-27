#include "sndtrpch.h"
#include "sndtrstr.h"
#include "xboxverp.h"
#include "xboxp.h"
#include "xboxvideo.h"
#include "imagefile.h"
#include "dashst.h"



typedef struct {

    PCWSTR Name;
    PCWSTR Songs[MAX_SONGS];
    DWORD  Ids[MAX_SONGS];
    DWORD  Lengths[MAX_SONGS];


} CREATE_STRUCT, *PCREATE_STRUCT;


CREATE_STRUCT g_Soundtracks[] = {

    {L"Racing Tunes", {L"Ham", L"Take Care of the Lonely", L"Lead Head", L"Beauty Queen"}, {0x00000007, 0x00000009, 0x00000008, 0x00000006}, {203000,177000,216000,242000}},
    {L"Fighting Mix", {L"Breath of Giants", L"Journey", L"Wilderness", NULL}, {0x00010003, 0x00010004, 0x00010005},{81000,331000,364000}},
    {NULL},
};





VOID
pCreateDatabase (
    VOID
    )
{

    UINT i;
    UINT u;
    UINT song;
    DWORD val;

    DashBeginUsingSoundtracks ();
    DashGetNewSongId (0); // not creating a 0 id file.
    DashGetNewSongId (0); // Becky was here.
    DashGetNewSongId (0); // and here.

    XDBGWRN ("Soundtracks", "Creating soundtracks");

    for (i = 0; g_Soundtracks[i].Name; i++) {

        DashAddSoundtrack ((PWSTR) g_Soundtracks[i].Name, &val);




        XDBGWRN ("Soundtracks", "Creating songs for soundtrack.");
        for (u = 0; g_Soundtracks[i].Songs[u]; u++) {


            DashGetNewSongId (0);
            DashAddSongToSoundtrack (val, g_Soundtracks[i].Ids[u],  g_Soundtracks[i].Lengths[u], (PWSTR) g_Soundtracks[i].Songs[u]);

        }
    }

    DashEndUsingSoundtracks ();






    return;
}






VOID
DrawMessage(LPCWSTR pcszTextLine1, LPCWSTR pcszTextLine2, int iProgPercent);

CXBoxVideo* g_pVideo;
BitmapFile* g_pBackgroundBitmap;


VOID
DrawMessage(LPCWSTR pcszTextLine1, LPCWSTR pcszTextLine2, int iProgPercent)
{
    ASSERT(g_pVideo && g_pBackgroundBitmap && pcszTextLine1);

    g_pVideo->ClearScreen(COLOR_BLACK);
    g_pBackgroundBitmap->render(g_pVideo->GetD3DDevicePtr());

    g_pVideo->DrawText(SCREEN_X_CENTER,
                       MENUBOX_TEXTAREA_Y1,
                       SCREEN_DEFAULT_TEXT_FG_COLOR,
                       SCREEN_DEFAULT_TEXT_BG_COLOR,
                       pcszTextLine1);

    if (NULL != pcszTextLine2)
    {
        g_pVideo->DrawText(SCREEN_X_CENTER,
                           MENUBOX_TEXTAREA_Y1 + FONT_DEFAULT_HEIGHT + ITEM_VERT_SPACING,
                           SCREEN_DEFAULT_TEXT_FG_COLOR,
                           SCREEN_DEFAULT_TEXT_BG_COLOR,
                           pcszTextLine2);
    }

    if (-1 != iProgPercent)
    {
        //
        // Draw progress bar background
        //

        g_pVideo->DrawBox(PROGRESS_X1 - PROGRESS_BORDER_OUTER_OFFSET,
                          PROGRESS_Y1 - PROGRESS_BORDER_OUTER_OFFSET,
                          PROGRESS_X2 + PROGRESS_BORDER_OUTER_OFFSET,
                          PROGRESS_Y2 + PROGRESS_BORDER_OUTER_OFFSET,
                          COLOR_BRIGHT_GREEN);

        //
        // Draw progress bar bkgnd
        //

        g_pVideo->DrawBox(PROGRESS_X1 - PROGRESS_BORDER_GAP,
                          PROGRESS_Y1 - PROGRESS_BORDER_GAP,
                          PROGRESS_X2 + PROGRESS_BORDER_GAP,
                          PROGRESS_Y2 + PROGRESS_BORDER_GAP,
                          COLOR_BLACK);

        //
        // Draw progress bar
        //

        g_pVideo->DrawBox(PROGRESS_X1,
                          PROGRESS_Y1,
                          PROGRESS_X1 + ((PROGRESS_WIDTH / 100) * min(iProgPercent, 100)),
                          PROGRESS_Y2,
                          SCREEN_DEFAULT_TEXT_FG_COLOR);
    }

    WCHAR szBuildString[64];
    wsprintfW(szBuildString, RStrings[RS_BUILD_NUMBER_FORMAT], VER_PRODUCTVERSION_STR);
    g_pVideo->DrawText(SCREEN_X_CENTER,
                       MENUBOX_BUILDNUM_Y1,
                       SCREEN_DEFAULT_TEXT_FG_COLOR,
                       SCREEN_DEFAULT_TEXT_BG_COLOR,
                       szBuildString);

    g_pVideo->ShowScreen();

}

// Threshold for analog buttons
#define XINPUT_ANALOGBUTTONTHRESHOLD 20

VOID
WaitForAnyButton()
{
    BOOL fButtonPressed = FALSE;
    static DWORD dwPads = 0;
    DWORD dwInsertions, dwRemovals;
    int i;

    while (!fButtonPressed)
    {
        if (XGetDeviceChanges(XDEVICE_TYPE_GAMEPAD, &dwInsertions, &dwRemovals))
        {
            dwPads |= dwInsertions;
            dwPads &= ~dwRemovals;
        }

        for (i = 0; i < XGetPortCount(); i++)
        {
            if ((1 << i) & dwPads)
            {
                HANDLE hPad = XInputOpen(XDEVICE_TYPE_GAMEPAD, XDEVICE_PORT0 + i, XDEVICE_NO_SLOT, NULL);

                if (NULL != hPad)
                {
                    XINPUT_STATE State;

                    //
                    // BUGBUG: remove this sleep once XInput works properly!
                    //

                    Sleep(20);

                    if (ERROR_SUCCESS == XInputGetState(hPad, &State))
                    {
                        if ((0 != State.Gamepad.wButtons) ||
                            (State.Gamepad.bAnalogButtons[0] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                            (State.Gamepad.bAnalogButtons[1] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                            (State.Gamepad.bAnalogButtons[2] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                            (State.Gamepad.bAnalogButtons[3] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                            (State.Gamepad.bAnalogButtons[4] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                            (State.Gamepad.bAnalogButtons[5] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                            (State.Gamepad.bAnalogButtons[6] > XINPUT_ANALOGBUTTONTHRESHOLD) ||
                            (State.Gamepad.bAnalogButtons[7] > XINPUT_ANALOGBUTTONTHRESHOLD))
                        {
                            fButtonPressed = TRUE;
                        }
                    }

                    XInputClose(hPad);
                }
            }

            if (fButtonPressed)
            {
                break;
            }
        }
    }
}





void __cdecl main()
{
    unsigned int i;
    WCHAR szText[128];
    DWORD dwVideoFlags = 0;
    DWORD length;
    HANDLE h;
    XSOUNDTRACK_DATA data;
    WCHAR buf[MAX_PATH];
    DWORD val;
    HANDLE h2;

    DWORD st1, st2;
    static XDEVICE_PREALLOC_TYPE deviceTypes[] = {
                                    {XDEVICE_TYPE_GAMEPAD, 4},
                                    {XDEVICE_TYPE_MEMORY_UNIT, 2}
                                    };


    //
    // Initialize core peripheral port support
    //
    XInitDevices(2, deviceTypes);



    g_pVideo = new CXBoxVideo;
    g_pBackgroundBitmap = new BitmapFile;

    if ((NULL == g_pVideo) || (NULL == g_pBackgroundBitmap))
    {
        XDBGWRN("RECOVERY", "Couldn't allocate video objects");
        Sleep(INFINITE);
    }

    g_pBackgroundBitmap->read(0, 0, FILE_DATA_IMAGE_DIRECTORY_A "\\backgrnd.bmp");

    g_pVideo->Initialize( SCREEN_WIDTH, SCREEN_HEIGHT );

    g_pVideo->ClearScreen(COLOR_BLACK);
    g_pBackgroundBitmap->render(g_pVideo->GetD3DDevicePtr());
    g_pVideo->ShowScreen();


    DrawMessage(RStrings[RS_GAMEPAD_BEGIN_LINE1], RStrings[RS_GAMEPAD_BEGIN_LINE2], -1);


    DrawMessage (L"Creating DB.", L"", -1);
    DeleteFile ("T:\\Music\\st.db");
    WaitForAnyButton ();
    pCreateDatabase ();
#if 0



    DrawMessage (L"Testing Soundtracks.",L"", -1);
    DashBeginUsingSoundtracks ();
    if (!DashAddSoundtrack (L"Soundtrack 1", &st1)) {
        XDBGWRN ("TEST", "Failed to add st1.");
    }
    if (!DashAddSoundtrack (L"Soundtrack 2", &st2)) {
        XDBGWRN ("TEST", "Failed to add st2.");
    }
    val = DashGetNewSongId ();
    if (!DashAddSongToSoundtrack (st1, val, L"Song 1")) {
        XDBGWRN ("TEST", "Failed to add Song1.");
    }
    val = DashGetNewSongId ();
    if (!DashAddSongToSoundtrack (st1, val, L"Song 2")) {
        XDBGWRN ("TEST", "Faield to add Song2.");
    }
    val = DashGetNewSongId ();
    if (!DashAddSongToSoundtrack (st2, val, L"Song 3")) {
        XDBGWRN ("TEST", "Failed to add Song3.");
    }

    val = DashGetNewSongId ();
    if (!DashAddSongToSoundtrack (st2, val, L"Song 4")) {
        XDBGWRN ("TEST", "Failed to add Song4.");
    }

    val = DashGetNewSongId ();
    if (!DashAddSongToSoundtrack (st2, val, L"Song 5")) {
        XDBGWRN ("TEST", "Failed to add Song5.");
    }

    val = DashGetNewSongId ();
    if (!DashAddSongToSoundtrack (st2, val, L"Song 6")) {
        XDBGWRN ("TEST", "Failed to add Song6.");
    }

    val = DashGetNewSongId ();
    if (!DashAddSongToSoundtrack (st2, val, L"Song 7")) {
        XDBGWRN ("TEST", "Failed to add Song7.");
    }

    val = DashGetNewSongId ();
    if (!DashAddSongToSoundtrack (st2, val, L"Song 8")) {
        XDBGWRN ("TEST", "Failed to add Song8.");
    }

    val = DashGetNewSongId ();
    if (!DashAddSongToSoundtrack (st2, val, L"Song 9")) {
        XDBGWRN ("TEST", "Failed to add Song9.");
    }


    val = DashGetNewSongId ();
    if (!DashAddSongToSoundtrack (st2, val, L"Song 10")) {
        XDBGWRN ("TEST", "Failed to add Song10.");
    }

    val = DashGetNewSongId ();
    if (!DashAddSongToSoundtrack (st2, val, L"Song 11")) {
        XDBGWRN ("TEST", "Failed to add Song11.");
    }

    val = DashGetNewSongId ();
    if (!DashAddSongToSoundtrack (st2, val, L"Song 12")) {
        XDBGWRN ("TEST", "Failed to add Song12.");
    }

    if (!DashSwapSoundtracks (st1, st2)) {
        XDBGWRN ("TEST", "FAiled to swap soundtracks");
    }

    if (!DashEditSoundtrackName (st1, L"Alpha")) {
        XDBGWRN ("TEST", "Failed to edit st1.");
    }

    if (!DashEditSoundtrackName (st2, L"Beta")) {
        XDBGWRN ("TEST", "Failed to edit st2.");
    }

    if (!DashEditSongName (st1, 0, L"Song the first.")) {
        XDBGWRN ("TEST", "Faield to edit song.");
    }

    if (!DashEditSongName (st1, 1, L"Song the second.")) {
        XDBGWRN ("TEST", "Failed to edit song.");
    }

    if (!DashSwapSongs (st1, 0, 1)) {
        XDBGWRN ("TEST", "Failed to swap songs.");
    }

    if (!DashSwapSongs (st2, 0, 8)) {
        XDBGWRN ("TEST", "Failed to swap songs.");
    }

    if (!DashDeleteSongFromSoundtrack (st2, 0)) {
        XDBGWRN ("TEST", "Failed to delete song.");
    }

    if (!DashDeleteSoundtrack (st1)) {
        XDBGWRN ("TEST", "Unable to delete soundtrack.");
    }

#endif




    XDBGWRN ("TEST", "There are %u soundtracks in the datbase.", DashGetSoundtrackCount ());

//    DashEndUsingSoundtracks ();

    WaitForAnyButton ();
    DrawMessage (L"Testing Soundtracks.", L"", -1);
    h = XFindFirstSoundtrack (&data);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            XDBGWRN ("Soundtrack Test", "Found soundtrack %ws with %u songs. Length, %u:%u", data.szName, data.uSongCount, data.uSoundtrackLength / 60000, data.uSoundtrackLength % 60000);

            for (i = 0; i < data.uSongCount; i++) {
                if (!XGetSoundtrackSongInfo (data.uSoundtrackId, i, &val, &length, buf, MAX_PATH)) {
                    XDBGWRN ("Soundtrack Test", "Could not get data for index %i.", i);
                }
                else {
                    XDBGWRN ("Soundtrack Test", "Found song with name %ws val %u, length: %u:%u", buf, val, length / 60000, length % 60000);
                    h2 = XOpenSoundtrackSong (val, FALSE);
                    if (h2 == INVALID_HANDLE_VALUE) {
                        XDBGWRN ("Soundtrack Test", "Unable to open song %u.", val);
                    }
                    else {
                        CloseHandle (h2);
                        XDBGWRN ("Soundtrack Test", "Song %ws successfuly opened.", buf);
                    }
                }
            }


        } while (XFindNextSoundtrack (h, &data));


        XFindClose (h);



    }

    DrawMessage (L"Done.", L"", -1);
    WaitForAnyButton ();
    WaitForAnyButton ();




}


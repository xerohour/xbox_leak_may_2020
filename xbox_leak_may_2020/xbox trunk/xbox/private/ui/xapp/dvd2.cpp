/*
    Base DVD Functionality
    ----------------------
    0-9
    Return
    Repeat
    Repeat A-B
    Scan Forward/Backward
    Skip Forward/Backward
    Stop
    Play/Pause
    Left, Right, Up, Down
    Enter
    Menu
    Top Menu
    Step
    Audio
    Subtitle
    Angle

    Extra DVD Functionality
    -----------------------
    Play Backwards
    Play Fast/Slow
    Zoom/Pan
    Bookmarks

    Resume Function

      When you press stop, the player should remember where you were and
      resume from that position when you press play again. Pressing Stop
      twice clears the memory.

    Auto Stop

      When the player is left in Pause mode for five minutes, it will Stop.

    Screen Save

      Stop mode enables the screen saver; all other modes disable it.

    Step will auto pause.

    Fast Scanning

      The Scan buttons cycle between 2x, 4x, 8x, 16x, and 32x normal speed.

    Slow Motion Playback

      In Pause mode, the Forward and Reverse Scan buttons may be used to
      cycle between 1/2, 1/4, and 1/8 normal speed.

    Skipping

      The Back button moves to the start of a chapter. One more press moves
      to the start of the previous chapter.
*/

#include "std.h"

#include "xapp.h"
#include "Node.h"
#include "Date.h"
#include "Runner.h"
#include "Locale.h"

#define XDASH_COLORKEY  D3DCOLOR_XRGB(0, 0, 1)
#define MKLANG(c, d)    (((WORD)c << 8) | (WORD)d)

//#include "../../dvd/Apps/xbDVD/INC/dvdcpl32.h"
// dvdcpl32.h has a lot of extra stuff including a bunch of warnings and a
// few errors, so we use a modified local version here...
#include "Ravisent.h"
#include "dvdlib.h"

//#include "../../dvd/Apps/xbDVD/INC/dvdpld32.h"
// dvdpld32.h pulls in a lot of stuff and causes lots of warnings and errors,
// all we use is the definition of Error, so here it is...
typedef unsigned long Error;

#ifdef _DEBUG
extern void DumpHex(const BYTE* pbData, int cbData, int cbMax=0);
#endif

extern const TCHAR* LanguageNameFromCode(WORD code);

#ifdef _DEBUG
static void LogDvdError(const TCHAR* szFile, int nLine, Error error);
#define VERIFYDVD(f) do { Error errverify = (f); if (IS_GNERROR(errverify)) LogDvdError(_T(__FILE__), __LINE__, errverify); } while (0)
#else
#define VERIFYDVD(f) ((void)(f))
#endif

#ifdef _DEBUG
static const TCHAR* rgszDisplayPresentationMode [] =
{
    _T("DPM_4BY3"),
    _T("DPM_16BY9"),
    _T("DPM_LETTERBOXED"),
    _T("DPM_PANSCAN"),
    _T("DPM_NONE")
};
#endif

////////////////////////////////////////////////////////////////////////////

#define EVENTS_STORED 10

struct DVDEVENT
{
    DWORD event;
    DWORD info;
};


#define AB_DISABLED 0
#define AB_SETTING_B 1
#define AB_REPEATING 2

#define AUTOSTOP_TIMEOUT (5.0f * 60.0f)

//
// Minimum interval in milliseconds of a AB repeat video
//

#define MIN_REPEAT_AB_INTERVAL (1 * 1000)

//
// CDVDBookmark
// Useful wrapper to store the DVD frozen state used on DDP_Freeze, DDP_Defrost
//

class CDVDPlayer;

class CDVDBookmark
{
public:

    //
    // CDVDBookmark
    // constructor
    //
    CDVDBookmark()
    {
        //
        // Initially, when we will probe DDP_Freeze for the buffer size,
        // we can't pass NULL as the buffer (or so says the sample). Therefore
        // our initial buffer will be one byte long instead of NULL
        //
        m_pStateBuffer = m_rgOneByteBuffer;
        m_cbStateBuffer = sizeof(m_rgOneByteBuffer);
        m_bSet = false;
    }

    //
    // ~CDVDBookmark
    // destructor
    //
    ~CDVDBookmark()
    {
        if(m_pStateBuffer != m_rgOneByteBuffer)
        {
            delete [] m_pStateBuffer;
        }
    }

    //
    // IsSet
    // Returns if the bookmark is currently set or not
    //
    bool IsSet()
    {
        return m_bSet;
    }

    //
    // Reset
    // Mark bookmark as not set (or not valid)
    //
    void Reset()
    {
        m_bSet = false;
    }

    //
    // Alloc
    // Allocate/Expand buffer
    //
    bool Alloc(
        DWORD cbStateBufferSize
    )
    {
        bool bRet = true;

        m_bSet = false;

        //
        // Current buffer big enough?
        //

        if(m_cbStateBuffer < cbStateBufferSize)
        {
            //
            // Apparently not. Allocate new buffer
            //

            BYTE* pNewBuffer = new BYTE[cbStateBufferSize];

            if(pNewBuffer)
            {
                //
                // Success. Make it the new official buffer.
                //

                if(m_pStateBuffer != m_rgOneByteBuffer)
                {
                    delete [] m_pStateBuffer;
                }

                m_pStateBuffer = pNewBuffer;
                pNewBuffer = NULL;

                m_cbStateBuffer = cbStateBufferSize;
            }
            else
            {
                //
                // Out of memory
                //

                bRet = false;
            }
        }

        return bRet;
    }

    //
    // GetSize
    // Return current allocated size
    //
    DWORD GetSize()
    {
        return m_cbStateBuffer;
    }

    //
    // GetBuffer
    // Return pointer to allocated buffer
    //
    BYTE* GetBuffer()
    {
        return m_pStateBuffer;
    }

    //
    // Set
    // Mark the bookmark as set (or valid)
    //
    void Set()
    {
        m_bSet = true;
    }

    BYTE* m_pStateBuffer;
    BYTE m_rgOneByteBuffer[1];
    DWORD m_cbStateBuffer;
    bool m_bSet;
};


class CDVDPlayer : public CNode
{
    DECLARE_NODE(CDVDPlayer, CNode)
public:
    CDVDPlayer();
    ~CDVDPlayer();

    bool OnSetProperty(const PRD* pprd, const void* pvValue);

    void init();
    void cleanup();

    bool m_bStillOn;
    bool m_bMenuOn;
    bool m_autoStop;

    float m_speed;
    bool m_closedCaption;
    int m_subTitle;
    int m_title;
    int m_chapter;

    int m_hours;
    int m_minutes;
    int m_seconds;
    int m_frames;

    int m_videoModePreferrence; // 0=title default, 1=16x9, 2=4x3/panscan, 3=4x3/letterbox
    int m_parentalLevel; // 1=G, 3=PG, 4=PG13, 6=R, 7=NC17
    int m_audioStream;
    int m_angle;
    int m_angleCount;

    int m_videoWidth;
    int m_videoHeight;

    int m_left;
    int m_top;
    int m_width;
    int m_height;

    // Player State (updated by events)
    DVDPlayerMode m_playbackMode;
    DVDDomain m_domain;

    DVDAudioCodingMode m_audioFormat;
    int m_audioChannels;
    WORD m_audioLanguageCode;
    TCHAR* m_audioLanguage;
    TCHAR* m_subTitleLanguage;
    WORD m_subTitleLanguageCode;

    int m_scanSpeed;
    bool m_scanSlow;

    void goUp();
    void play();
    void stop();
    void pause();
    void playOrPause();

    void selectUp();
    void selectDown();
    void selectLeft();
    void selectRight();
    void activate();

    void nextAudioStream();
    void nextAngle();

    void startChapter();
    void nextChapter();
    void prevChapter();

    void frameAdvance();
    void frameReverse();

    void menu();
    void titleMenu();

    void nextSubtitle();

    void forwardScan();
    void backwardScan();
    void stopScan();

    void eject();

    void setScale(float nScale);
    void setZoomPos(float x, float y);

    void digit(int nDigit);

    int isPlaybackDomain();
    int isUOPValid(int nUOP);

    void abRepeat();
    int m_abRepeatState;
    int m_number;

    void refreshAudioSettings();

    void enableWideScreen();
    void disableWideScreen();

    void Advance(float nSeconds);
    void Render();

private:
    Error CheckUserOp(Error e);
    void UpdateZoom();
    void OnSubPictureStreamChange();
    Error SetBookmark(CDVDBookmark* pBookmark);
    Error GotoBookmark(CDVDBookmark* pBookmark);
//  void OnAudioStreamChange();

    bool m_bResetPosSize;
    XTIME m_timeToStop;

    XTIME m_numberTime;
    XTIME m_prevChapterTime;

    DVDLocation m_stopLocation;

    DVDLocation m_locationA;
    DVDLocation m_locationB;

    CDVDBookmark m_StopBookmark;
    CDVDBookmark m_LocationABookmark;

    // Player Library
    VDRHandle m_hvdr;
    UnitSet m_hUnitSet;
    DVDDiskPlayer m_hPlayer;

    // Event Queue
    static void __stdcall EventHandler(DWORD event, void* userData, DWORD info);
    void InstallEventHandlers();
    void RemoveEventHandlers();
    void ProcessEvents();
    bool GetNextEvent(DWORD& event, DWORD& info);
    DVDEVENT m_events [EVENTS_STORED];
    int m_eventRead;
    int m_eventWrite;
    CRITICAL_SECTION m_eventLock;

    // Zooming and Panning
    XTIME m_nZoomTime;
    float m_nScale;
    float m_nOffsetX;
    float m_nOffsetY;
    float m_nScaleStart;
    float m_nOffsetXStart;
    float m_nOffsetYStart;
    float m_nScaleSet;
    float m_nOffsetXSet;
    float m_nOffsetYSet;

    // Scanning
    bool m_bScanBackward;

    // Flag indicating that parental control is too low
    bool m_parentalTooLow;
    bool m_parentalPopup;

    LPDIRECT3DVERTEXBUFFER8 m_pVB;

    DECLARE_NODE_FUNCTIONS()

    DECLARE_NODE_PROPS()
};

CDVDPlayer* g_pDVDPlayer = NULL;

IMPLEMENT_NODE("DVDPlayer", CDVDPlayer, CNode)

START_NODE_PROPS(CDVDPlayer, CNode)
    NODE_PROP(pt_number, CDVDPlayer, speed)
    NODE_PROP(pt_integer, CDVDPlayer, title)
    NODE_PROP(pt_integer, CDVDPlayer, chapter)
    NODE_PROP(pt_integer, CDVDPlayer, hours)
    NODE_PROP(pt_integer, CDVDPlayer, minutes)
    NODE_PROP(pt_integer, CDVDPlayer, seconds)
    NODE_PROP(pt_integer, CDVDPlayer, frames)
    NODE_PROP(pt_integer, CDVDPlayer, videoModePreferrence)
    NODE_PROP(pt_integer, CDVDPlayer, parentalLevel)
    NODE_PROP(pt_integer, CDVDPlayer, audioStream)
    NODE_PROP(pt_integer, CDVDPlayer, left)
    NODE_PROP(pt_integer, CDVDPlayer, top)
    NODE_PROP(pt_integer, CDVDPlayer, width)
    NODE_PROP(pt_integer, CDVDPlayer, height)
    NODE_PROP(pt_boolean, CDVDPlayer, closedCaption)
    NODE_PROP(pt_integer, CDVDPlayer, subTitle)
    NODE_PROP(pt_integer, CDVDPlayer, domain)
    NODE_PROP(pt_integer, CDVDPlayer, angle)
    NODE_PROP(pt_integer, CDVDPlayer, angleCount)
    NODE_PROP(pt_integer, CDVDPlayer, playbackMode)
    NODE_PROP(pt_integer, CDVDPlayer, audioFormat)
    NODE_PROP(pt_integer, CDVDPlayer, audioChannels)
    NODE_PROP(pt_string, CDVDPlayer, audioLanguage)
    NODE_PROP(pt_string, CDVDPlayer, subTitleLanguage)
    NODE_PROP(pt_integer, CDVDPlayer, abRepeatState)
    NODE_PROP(pt_integer, CDVDPlayer, number)
    NODE_PROP(pt_integer, CDVDPlayer, scanSpeed)
    NODE_PROP(pt_boolean, CDVDPlayer, scanSlow)
    NODE_PROP(pt_boolean, CDVDPlayer, autoStop)
    NODE_PROP(pt_boolean, CDVDPlayer, bScanBackward)
END_NODE_PROPS()

START_NODE_FUN(CDVDPlayer, CNode)
    NODE_FUN_VV(init)
    NODE_FUN_VV(play)
    NODE_FUN_VV(stop)
    NODE_FUN_VV(goUp)
    NODE_FUN_VV(pause)
    NODE_FUN_VV(playOrPause)
    NODE_FUN_VV(selectUp)
    NODE_FUN_VV(selectDown)
    NODE_FUN_VV(selectRight)
    NODE_FUN_VV(selectLeft)
    NODE_FUN_VV(activate)
    NODE_FUN_VV(nextAudioStream)
    NODE_FUN_VV(nextAngle)
    NODE_FUN_VV(startChapter)
    NODE_FUN_VV(nextChapter)
    NODE_FUN_VV(prevChapter)
    NODE_FUN_VV(menu)
    NODE_FUN_VV(titleMenu)
    NODE_FUN_VV(nextSubtitle)
    NODE_FUN_VV(forwardScan)
    NODE_FUN_VV(backwardScan)
    NODE_FUN_VV(stopScan)
    NODE_FUN_VV(eject)
    NODE_FUN_VN(setScale)
    NODE_FUN_VNN(setZoomPos)
    NODE_FUN_VV(frameAdvance)
    NODE_FUN_VV(frameReverse)
    NODE_FUN_II(isUOPValid)
    NODE_FUN_VV(abRepeat)
    NODE_FUN_VI(digit)
    NODE_FUN_VV(refreshAudioSettings)
    NODE_FUN_IV(isPlaybackDomain)
    NODE_FUN_VV(enableWideScreen)
    NODE_FUN_VV(disableWideScreen)
END_NODE_FUN()

CDVDPlayer::CDVDPlayer() :
    m_autoStop(true),
    m_speed(1.0f),
    m_closedCaption(false),
    m_parentalTooLow(false),
    m_parentalPopup(false),
    m_subTitle(0),
    m_title(0),
    m_chapter(0),
    m_hours(0),
    m_minutes(0),
    m_seconds(0),
    m_frames(0),
    m_videoModePreferrence(0),
    m_parentalLevel(8),
    m_audioStream(0),
    m_angle(1),
    m_angleCount(1),
    m_left(0),
    m_top(0),
    m_width(0),
    m_height(0),
    m_videoWidth(0),
    m_videoHeight(0),
    m_audioFormat(DAM_AC3),
    m_audioChannels(0),
    m_audioLanguage(NULL),
    m_audioLanguageCode(0),
    m_subTitleLanguage(NULL),
    m_subTitleLanguageCode(0),
    m_abRepeatState(AB_DISABLED)
{
    m_hvdr = NULL;
    m_hUnitSet = NULL;
    m_hPlayer = NULL;
    m_playbackMode = DPM_STOPPED;
    m_domain = STOP_DOM;
    m_bResetPosSize = true;

    m_nScale = 1.0f;
    m_nOffsetX = 0.0f;
    m_nOffsetY = 0.0f;
    m_timeToStop = 0.0f;
    m_bScanBackward = false;
    m_scanSlow = false;
    m_scanSpeed = 1;

    m_pVB = NULL;
    m_nZoomTime = 0.0f;
    m_nScaleSet = 1.0f;
    m_nOffsetXSet = 0.0f;
    m_nOffsetYSet = 0.0f;

    m_number = 0;
    m_numberTime = 0.0f;
    m_prevChapterTime = 0.0f;

    ZeroMemory(&m_locationA, sizeof (m_locationA));
    ZeroMemory(&m_locationB, sizeof (m_locationB));

    ZeroMemory(&m_stopLocation, sizeof (m_stopLocation));
}

CDVDPlayer::~CDVDPlayer()
{
    delete [] m_audioLanguage;
    delete [] m_subTitleLanguage;

    cleanup();

    if (m_pVB != NULL)
        m_pVB->Release();
}

bool CDVDPlayer::OnSetProperty(const PRD* pprd, const void* pvValue)
{
    if (m_hPlayer != NULL)
    {
        if ((int)pprd->pbOffset == offsetof(m_videoModePreferrence))
        {
            int n = *(int*)pvValue;
            DisplayPresentationMode dpm = DPM_4BY3;
            switch (n)
            {
            case 0:
                dpm = DPM_PANSCAN;
                break;

            case 1:
                dpm = DPM_LETTERBOXED;
                break;

            case 2:
                dpm = DPM_16BY9;
                break;
            }

            DDP_SetDisplayMode(m_hPlayer, dpm);
        }
        else if ((int)pprd->pbOffset == offsetof(m_closedCaption))
        {
            BOOL b = *(bool*)pvValue ? TRUE : FALSE;
            VDR_ConfigureUnitsTags(m_hUnitSet, SET_MPEG2_CC_ENABLE(b), TAGDONE);
        }
        else if ((int)pprd->pbOffset == offsetof(m_angle))
        {
            int n = *(int*)pvValue;
            if (IS_ERROR(DDP_AngleChange(m_hPlayer, (WORD)n)))
                return false;
        }
        else if ((int)pprd->pbOffset == offsetof(m_left) ||
            (int)pprd->pbOffset == offsetof(m_top) ||
            (int)pprd->pbOffset == offsetof(m_width) ||
            (int)pprd->pbOffset == offsetof(m_height))
        {
            m_bResetPosSize = true;
        }
        else if ((int)pprd->pbOffset == offsetof(m_speed))
        {
            float speed = *(float*)pvValue; // NOTE: Should be <= 2
            DDP_SetPlaybackSpeed(m_hPlayer, (int)(speed * 1000.0f));
        }
        else if ((int)pprd->pbOffset == offsetof(m_autoStop))
        {
            m_autoStop = *(bool*)pvValue ? true : false;

            // Refresh the auto stop timer
            if (m_autoStop && m_playbackMode == DPM_PAUSED)
                m_timeToStop = XAppGetNow() + AUTOSTOP_TIMEOUT;
        }
        else if ((int)pprd->pbOffset == offsetof(m_parentalLevel))
        {
            m_parentalLevel = *(int*)pvValue + 1; // offset by one since we start from zero
            return false;
        }
        else if ((int)pprd->pbOffset == offsetof(m_visible))
        {
            D3DDevice_SetFlickerFilter(*(bool*)pvValue ? 0 : 5);
        }

        // Update playback mode, changing some properties might have changed the playback mode
        m_playbackMode = DDP_GetPlayerMode(m_hPlayer);
    }

    return CNode::OnSetProperty(pprd, pvValue);
}

struct DVDVERTEX
{
    float x, y, z;
};

extern D3DXMATRIX g_matView;
extern D3DXMATRIX g_matProjection;
extern D3DXMATRIX g_matIdentity;

int DVDTime::Compare(const DVDTime u) const
{
    return stamp - u.stamp;
}


void CDVDPlayer::Render()
{
    if (m_videoWidth > 0 && m_videoHeight > 0)
    {
        if (m_pVB == NULL)
            VERIFYHR(XAppGetD3DDev()->CreateVertexBuffer(4 * sizeof(DVDVERTEX), D3DUSAGE_WRITEONLY, D3DFVF_XYZ, D3DPOOL_MANAGED, &m_pVB));

        int width = m_width == 0 ? m_videoWidth : m_width;
        int height = m_height == 0 ? m_videoHeight : (m_height - 1);

        DVDVERTEX* v;
        VERIFYHR(m_pVB->Lock(0, 4 * sizeof (DVDVERTEX), (BYTE**)&v, 0));
        v[0].x = m_left + width - theApp.m_nViewWidth / 2.0f;
        v[0].y = -(m_top + height - theApp.m_nViewHeight / 2.0f);
        v[0].z = 0.0f;
        v[1].x = m_left - theApp.m_nViewWidth / 2.0f;
        v[1].y = -(m_top + height - theApp.m_nViewHeight / 2.0f);
        v[1].z = 0.0f;
        v[2].x = m_left + width - theApp.m_nViewWidth / 2.0f;
        v[2].y = -(m_top - theApp.m_nViewHeight / 2.0f);
        v[2].z = 0.0f;
        v[3].x = m_left - theApp.m_nViewWidth / 2.0f;
        v[3].y = -(m_top - theApp.m_nViewHeight / 2.0f);
        v[3].z = 0.0f;
        VERIFYHR(m_pVB->Unlock());

        XAppSetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
        XAppSetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
        XAppSetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
        XAppSetRenderState(D3DRS_TEXTUREFACTOR, XDASH_COLORKEY);
        XAppSetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

        VERIFYHR(XAppGetD3DDev()->SetStreamSource(0, m_pVB, sizeof (DVDVERTEX)));
        VERIFYHR(XAppGetD3DDev()->SetVertexShader(D3DFVF_XYZ));
        VERIFYHR(XAppGetD3DDev()->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2));

        XAppSetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    }
}

void CDVDPlayer::Advance(float nSeconds)
{

    if (m_hPlayer != NULL)
    {
        ASSERT(m_hUnitSet != NULL);

        if (m_width <= 0 || m_height <= 0)
        {
            m_width = (int)theApp.m_nViewWidth;
            m_height = (int)theApp.m_nViewHeight;
        }

    //if (m_number != 0/* || m_numberTime > 0.0f*/)
    //  TRACE(_T("  %f: %d\n"), (XAppGetNow() - m_numberTime), m_number);

        if (m_numberTime > 0.0f && (XAppGetNow() - m_numberTime) >= 5.0f)
        {
            TRACE(_T("Auto activate number...\n"));
            activate();
        }

        if (m_timeToStop != 0.0f && XAppGetNow() >= m_timeToStop && m_autoStop)
        {
            if (m_playbackMode == DPM_PAUSED)
                stop();
            m_timeToStop = 0.0f;
        }

        if (m_playbackMode != DPM_STOPPED && m_playbackMode != DPM_INACTIVE && m_autoStop)
            ResetScreenSaver();

        // BLOCK: Check playback mode...
        {
            DVDPlayerMode playbackMode = DDP_GetPlayerMode(m_hPlayer);
            if (m_playbackMode != playbackMode)
            {
                m_playbackMode = playbackMode;

                if (m_playbackMode == DPM_PAUSED)
                    m_timeToStop = XAppGetNow() + AUTOSTOP_TIMEOUT;
                else
                    m_timeToStop = 0.0f;

                CallFunction(this, _T("OnPlaybackModeChange"));
            }

            VDR_EnablePIP(m_hUnitSet, m_playbackMode != DPM_STOPPED);
        }

        // BLOCK: Update position and check for A-B repeat
        {
            DVDLocation loc;
            VERIFYDVD(DDP_GetCurrentLocation(m_hPlayer, loc));

            if (m_abRepeatState == AB_REPEATING && (m_playbackMode == DPM_PLAYING || m_playbackMode == DPM_SCANNING))
            {
                if (loc.title >= m_locationB.title && loc.titleTime >= m_locationB.titleTime)
                {
                    TRACE(_T("Hit B; going to A...\n"));
                    GotoBookmark(&m_LocationABookmark);

                    //
                    // GotoBookmark failing or not, the position should be different.
                    // Read it again.
                    //

                    VERIFYDVD(DDP_GetCurrentLocation(m_hPlayer, loc));
                }
            }

            if (m_domain != loc.domain)
            {
                m_domain = loc.domain;

                //
                // Reset our internal state variables. We need to do this to prevent
                // bogus state when you go to the menu during a fast forward, slow scan, etc...
                //

                m_bScanBackward = false;
                m_scanSpeed = 1;
                m_scanSlow = false;

                CallFunction(this, _T("OnDomainChange"));
            }

            if (m_title != loc.title)
            {
                m_title = loc.title;
                CallFunction(this, _T("OnTitleChange"));
            }

            if (m_chapter != loc.partOfTitle)
            {
                m_chapter = loc.partOfTitle;
                CallFunction(this, _T("OnChapterChange"));
            }

            int hours = loc.titleTime.Hours();
            int minutes = loc.titleTime.Minutes();
            int seconds = loc.titleTime.Seconds();
            m_frames = loc.titleTime.Frames();

            if (hours != m_hours || minutes != m_minutes || seconds != m_seconds)
            {
                m_hours = hours;
                m_minutes = minutes;
                m_seconds = seconds;
                CallFunction(this, _T("OnTimeChange"));
            }
        }

        // BLOCK: Angle info...
        {
            int angleCount = DDP_GetNumberOfAngles(m_hPlayer, (WORD)m_title);
            int angle = DDP_GetCurrentAngle(m_hPlayer);

            if (angleCount == 0)
            {
                angleCount = 1;
            }

            if(angle > angleCount)
            {
                // there is this strange behavior when you switch domains, the
                // angle count returned is always 1 and the current angle remains
                // the same resulting into something like angle 3 of 1. This is
                // a hack to prevent that.

                angle = 1;
            }

            if (m_angleCount != angleCount || m_angle != angle)
            {
                m_angleCount = angleCount;
                m_angle = angle;
                CallFunction(this, _T("OnAngleChange"));
            }
        }

        // BLOCK: Update audio info...
        {
            bool bChange = false;

            int audioStream = DDP_GetCurrentAudioStream(m_hPlayer);
            if (m_audioStream != audioStream)
            {
                m_audioStream = audioStream;
                bChange = true;
            }

            DVDAudioStreamFormat format;
            VERIFYDVD(DDP_GetAudioStreamAttributes(m_hPlayer, (WORD)m_audioStream, format));

            if (m_audioFormat != format.codingMode || m_audioChannels != format.channels)
            {
                m_audioFormat = format.codingMode;
                m_audioChannels = format.channels;
                bChange = true;
            }

            if (m_audioLanguageCode != format.languageCode)
            {
                m_audioLanguageCode = format.languageCode;
                bChange = true;

                const TCHAR* sz = LanguageNameFromCode(m_audioLanguageCode);

                delete [] m_audioLanguage;
                m_audioLanguage = NULL;

                if (sz != NULL)
                {
                    TCHAR buffer[MAX_TRANSLATE_LEN];
                    lstrcpy(buffer, _T("Lang "));
                    lstrcat(buffer, sz);
                    m_audioLanguage = new TCHAR [MAX_TRANSLATE_LEN];
                    Translate(buffer, m_audioLanguage);
                }
            }

            if (bChange)
            {
                TRACE(_T("Audio Changed:\n"));
                TRACE(_T("\tstream: %d\n"), m_audioStream);
                TRACE(_T("\tformat: %d\n"), m_audioFormat);
                TRACE(_T("\tchannels: %d\n"), m_audioChannels);
                TRACE(_T("\tlanguage: 0x%04x %s\n"), m_audioLanguageCode, m_audioLanguage == NULL ? _T("UNKNOWN") : m_audioLanguage);
//              TRACE(_T("\tapplication mode: %d\n"), format.applicationMode);
                TRACE(_T("\tbits per sample: %d\n"), format.bitsPerSample);
                TRACE(_T("\tsamples per second: %d\n"), format.samplesPerSecond);

                CallFunction(this, _T("OnAudioChange"));
            }
        }

        // BLOCK: Update sub-title info...
        {
            int subTitle = 0;
            if (DDP_IsCurrentSubPictureEnabled(m_hPlayer) && m_domain == TT_DOM)
                subTitle = DDP_GetCurrentSubPictureStream(m_hPlayer) + 1;

            if (m_subTitle != subTitle)
            {
                m_subTitle = subTitle;
                OnSubPictureStreamChange();
                CallFunction(this, _T("OnSubTitleChange"));
            }
        }

        // BLOCK: Update video size...
        {
            unsigned short videoWidth, videoHeight;
            VDR_ConfigureUnitsTags(m_hUnitSet,
                GET_MPEG_DISPLAY_WIDTH(videoWidth),
                GET_MPEG_DISPLAY_HEIGHT(videoHeight),
                TAGDONE);

            if (videoWidth != m_videoWidth || videoHeight != m_videoHeight)
            {
                m_videoWidth = videoWidth;
                m_videoHeight = videoHeight;
                TRACE(_T("DVD: video size: %dx%d\n"), m_videoWidth, m_videoHeight);
                CallFunction(this, _T("OnVideoSizeChange"));
            }
        }

        // BLOCK: Update scale and position
        {
            if (m_nZoomTime != 0.0f)
            {
                float t = (float) (XAppGetNow() - m_nZoomTime) / 0.5f;
                if (t >= 1.0f)
                {
                    m_nZoomTime = 0.0f;
                    m_nScaleStart = m_nScaleSet = m_nScale;
                    m_nOffsetXStart = m_nOffsetXSet = m_nOffsetX;
                    m_nOffsetYStart = m_nOffsetYSet = m_nOffsetY;
                }
                else
                {
                    m_nScaleSet = m_nScaleStart + (m_nScale - m_nScaleStart) * t;
                    m_nOffsetXSet = m_nOffsetXStart + (m_nOffsetX - m_nOffsetXStart) * t;
                    m_nOffsetYSet = m_nOffsetYStart + (m_nOffsetY - m_nOffsetYStart) * t;
                }
            }

            UpdateZoom();
        }

/*      if (m_bResetPosSize)
        {
            int width = m_width == 0 ? m_videoWidth : m_width;
            int height = m_height == 0 ? m_videoHeight : m_height;

            // REVIEW: These don't do what it seems they should; need docs from Ravisent...
            if (m_width == 0 || m_height == 0)
            {
                VDR_ConfigureUnitsTags(m_hUnitSet,
                    SET_PIP_ADAPT_SOURCE_SIZE(TRUE),
                    SET_PIP_DEST_LEFT(m_left),
                    SET_PIP_DEST_TOP(m_top),
                    TAGDONE);
            }
            else
            {
                VDR_ConfigureUnitsTags(m_hUnitSet,
                    SET_PIP_ADAPT_SOURCE_SIZE(FALSE),
                    SET_PIP_DEST_LEFT(m_left),
                    SET_PIP_DEST_TOP(m_top),
                    SET_PIP_DEST_WIDTH(m_width),
                    SET_PIP_DEST_HEIGHT(m_height),
                    TAGDONE);
            }
            m_bResetPosSize = false;
        }
*/
    }

    if (m_parentalTooLow && !m_parentalPopup)
    {
        m_parentalPopup = true;
        VERIFYDVD(VDR_EnablePIP(m_hUnitSet, FALSE));
        CallFunction(this, _T("OnParentalLevelViolation"));
    }

    ProcessEvents();

    CNode::Advance(nSeconds);
}

void CDVDPlayer::init()
{
    Error err;
    BYTE DvdRegion;

    if (m_hPlayer != NULL)
        return;

    HalEnableSecureTrayEject();

    if (m_hvdr == NULL)
    {
        if (!DvdKeyInitialize(&DvdRegion, &err))
        {
            TRACE(_T("DvdKeyInitilize failed (0x%x) from DVDMediaPlayer!\n"), err);
            D3DDevice_SetFlickerFilter(5);
            CallFunction(this, _T("OnNoDongle"));
            return;
        }

        if (IS_ERROR(VDR_OpenDriver("CINEMAST", 0, m_hvdr)))
        {
            TRACE(_T("\001VDR_OpenDriver failed!\n"));
            cleanup();
            return;
        }

        VERIFY(VDR_OpenUnits(m_hvdr, MPEG_DECODER_UNIT | VIDEO_ENCODER_UNIT | PIP_UNIT, m_hUnitSet) == GNR_OK);

        DWORD dwVideoStandard = XGetVideoStandard();
        bool bPal = (dwVideoStandard == XC_VIDEO_STANDARD_PAL_I || dwVideoStandard == XC_VIDEO_STANDARD_PAL_M);

        VDR_ConfigureUnitsTags(m_hUnitSet,
            SET_MPEG_AUDIO_MUTE(FALSE),
            SET_MPEG2_PRESENTATION_MODE(MPM_FULLSIZE),
            SET_VID_MODE(VMOD_PLAYBACK),
            SET_PIP_VIDEOSTANDARD(bPal ? VSTD_PAL : VSTD_NTSC),
            SET_VID_VIDEOSTANDARD(bPal ? VSTD_PAL : VSTD_NTSC),
            SET_PIP_WINDOW(NULL),
            SET_PIP_ADAPT_SOURCE_SIZE(FALSE),
            SET_PIP_ADAPT_CLIENT_SIZE(FALSE),
            SET_PIP_COLORKEY_COLOR(XDASH_COLORKEY),
            TAGDONE);

        VDR_ConfigureUnitsTags(m_hUnitSet,
            SET_PIP_DISPLAY_LEFT(0),
            SET_PIP_DISPLAY_TOP(0),
            SET_PIP_DISPLAY_WIDTH((USHORT)theApp.m_pp.BackBufferWidth),
            SET_PIP_DISPLAY_HEIGHT((USHORT)theApp.m_pp.BackBufferHeight),
            TAGDONE);

        VDR_ConfigureUnitsTags(m_hUnitSet,
            SET_PIP_ADAPT_SOURCE_SIZE(FALSE),
            SET_PIP_DEST_LEFT(0),
            SET_PIP_DEST_TOP(0),
            SET_PIP_DEST_WIDTH((USHORT)theApp.m_pp.BackBufferWidth),
            SET_PIP_DEST_HEIGHT((USHORT)theApp.m_pp.BackBufferHeight),
            TAGDONE);

        refreshAudioSettings();
    }

    DWORD dwType, dwDvdRegion;
    DWORD dwErr = XQueryValue(XC_DVD_REGION, &dwType, &dwDvdRegion, sizeof(dwDvdRegion), NULL);

    if (dwErr != ERROR_SUCCESS || dwDvdRegion == 0 || dwDvdRegion > 8)
    {
        dwDvdRegion = DvdRegion;
        XSetValue(XC_DVD_REGION, REG_DWORD, &dwDvdRegion, sizeof(dwDvdRegion));
        TRACE(_T("XDASH: DVD region of Xbox is now stamped to %d\n"), DvdRegion);
    }
#ifndef COOL_XDASH
    else if (dwDvdRegion != DvdRegion)
    {
        TRACE(_T("XDASH: DVD region of Xbox doesn't match dongle's\n"));
        D3DDevice_SetFlickerFilter(5);
        CallFunction(this, _T("OnXboxDVDRegionViolation"));
        return;
    }
#endif

#ifdef COOL_XDASH
    // For testing purpose, auto-detect the DVD region
    for (DvdRegion=1; DvdRegion<=6; DvdRegion++)
    {
        DDP_SetSystemRegion(m_hUnitSet, 'D', (BYTE) ~(1 << (DvdRegion - 1)));
        if (!IS_ERROR(DDP_OpenPlayer(m_hUnitSet, 'D', m_hPlayer)))
        {
            break;
        }
    }
#else
    DDP_SetSystemRegion(m_hUnitSet, 'D', (BYTE) ~(1 << (DvdRegion - 1)));

    err = DDP_OpenPlayer(m_hUnitSet, 'D', m_hPlayer);
    if (IS_ERROR(err))
    {
        if (err == GNR_REGIONS_DONT_MATCH)
        {
            D3DDevice_SetFlickerFilter(5);
            CallFunction(this, _T("OnDiscDVDRegionViolation"));
        }

        TRACE(_T("\001DDP_OpenPlayer failed (%d 0x%x)!\n"), err, err);
        return;
    }
#endif

    InstallEventHandlers();

    DisplayPresentationMode dpm = DPM_4BY3;

    DWORD dwVideoFlags;

    if (ERROR_SUCCESS == XQueryValue(XC_VIDEO_FLAGS, &dwType, &dwVideoFlags, sizeof(DWORD), NULL))
    {
        ASSERT(REG_DWORD == dwType);

        if (dwVideoFlags & AV_FLAGS_WIDESCREEN)
        {
            dpm = DPM_16BY9;
        }
        else if (dwVideoFlags & AV_FLAGS_LETTERBOX)
        {
            dpm = DPM_LETTERBOXED;
        }
        else
        {
            dpm = DPM_4BY3;
        }
    }

    dwErr = XQueryValue(XC_PARENTAL_CONTROL_MOVIES, &dwType, &m_parentalLevel, sizeof(m_parentalLevel), NULL);

    if (dwErr != ERROR_SUCCESS)
         m_parentalLevel = 8;
    else
        m_parentalLevel = 8 - m_parentalLevel; // revert it since we store the level backward in EEPROM

    DDP_SetDisplayMode(m_hPlayer, dpm);

    static const TCHAR rgchLangCodes [] = { _T("enjadefresit") };
    ASSERT(g_nCurLanguage >= 0 && g_nCurLanguage < countof (rgchLangCodes) / 2);
    WORD lang = MKLANG(rgchLangCodes[g_nCurLanguage * 2], rgchLangCodes[g_nCurLanguage * 2 + 1]);

    VERIFYDVD(DDP_InitialLanguageSelect(m_hPlayer, lang, 0, lang, 0));
    VERIFYDVD(DDP_ParentalCountrySelect(m_hPlayer, 0x5553));

    // Parental control level 8 means to disble parental control
    if (m_parentalLevel != 8)
    {
        VERIFYDVD(DDP_ParentalLevelSelect(m_hPlayer, (WORD)m_parentalLevel));
    }

    err = DDP_StartPresentation(m_hPlayer);
    VERIFYDVD(VDR_EnablePIP(m_hUnitSet, TRUE));

    if (err == GNR_INSUFFICIENT_RIGHTS || err == GNR_PARENTAL_LEVEL_TOO_LOW)
    {
        m_parentalTooLow = true;
    }
}

void CDVDPlayer::cleanup()
{
    if (m_hPlayer != NULL)
    {
        DDP_Stop(m_hPlayer);
        RemoveEventHandlers();
        DDP_ClosePlayer(m_hPlayer);
        m_hPlayer = NULL;
    }

    if (m_hUnitSet != NULL)
    {
        VDR_CloseUnits(m_hUnitSet);
        m_hUnitSet = NULL;
    }

#ifndef _XBOX
    if (m_hvdr != NULL)
    {
        VDR_CloseDriver(m_hvdr);
        m_hvdr = NULL;
    }
#endif

    m_playbackMode = DPM_STOPPED;
}


Error CDVDPlayer::CheckUserOp(Error e)
{
    if (e == GNR_UOP_PROHIBITED || e == GNR_OPERATION_PROHIBITED)
    {
        CallFunction(this, _T("OnProhibited"));
        return NO_ERROR;
    }

    CallFunction(this, _T("OnUserOpAllowed"));
    return e;
}

void CDVDPlayer::goUp()
{
    if (m_hPlayer == NULL)
        return;

    VERIFYDVD(CheckUserOp(DDP_GoUp(m_hPlayer)));
}

void CDVDPlayer::play()
{
    if (m_hPlayer == NULL)
    {
        init();
        return;
    }

    DisplayPresentationMode dpm = DDP_GetCurrentDisplayMode(m_hPlayer);

    if (dpm == DPM_16BY9)
    {
        enableWideScreen();
    }
    else
    {
        disableWideScreen();
    }

    // Get a fresh playback mode. We need to do this because this method
    // may be called before m_playbackMode gets updated (BUG#6628).

    m_playbackMode = DDP_GetPlayerMode(m_hPlayer);

    switch (m_playbackMode)
    {
    case DPM_STOPPED:

        if (m_StopBookmark.IsSet())
        {
            //
            // Resume from last position using bookmarks
            //

            Error e = GotoBookmark(&m_StopBookmark);

            if(IS_GNERROR(e))
            {
                //
                // if that failed, start at the beginning (better than nothing)
                //

                VERIFYDVD(DDP_StartPresentation(m_hPlayer));
            }

            m_StopBookmark.Reset();
        }
        else
        {
            //
            // No bookmark? Start at the beginning.
            //

            VERIFYDVD(DDP_StartPresentation(m_hPlayer));
        }
        break;

    case DPM_PAUSED:
        {
            VERIFYDVD(DDP_Resume(m_hPlayer));

            if (DDP_GetPlayerMode(m_hPlayer) != DPM_PLAYING)
            {
                VERIFYDVD(DDP_PauseOff(m_hPlayer));
                VERIFYDVD(DDP_StopScan(m_hPlayer));
            }
        }
        break;

    case DPM_STILL:
        VERIFYDVD(CheckUserOp(DDP_StillOff(m_hPlayer)));
        break;

    case DPM_SCANNING:
    case DPM_REVERSEPLAY:
    case DPM_TRICKPLAY:
    case DPM_REVERSESCAN:
        VERIFYDVD(DDP_StopScan(m_hPlayer));
        break;

    case DPM_PLAYING:
        {
            //
            // Check if we're in a menu. If so, press the current
            // selected button
            //

            ULONG uops = DDP_GetForbiddenUserOperations(m_hPlayer);

            if(! (uops & UOP_BUTTON))
            {
                VERIFYDVD(CheckUserOp(DDP_ButtonActivate(m_hPlayer)));
            }
        }
        break;
    }

    DDP_SetPlaybackSpeed(m_hPlayer, 1000);
    m_bScanBackward = false;
    m_scanSpeed = 1;
    m_scanSlow = false;

    //
    // Depending on the new mode or domain, the script
    // will show the status message. Calling Advance
    // will refresh these variables
    //

    Advance(0.0f);

}

void CDVDPlayer::playOrPause()
{
    if (m_hPlayer == NULL)
    {
        init();
        return;
    }

    DDP_SetPlaybackSpeed(m_hPlayer, 1000);
    m_bScanBackward = false;
    m_scanSpeed = 1;
    m_scanSlow = false;

    // Get a fresh playback mode. We need to do this because this method
    // may be called before m_playbackMode gets updated (BUG#6628).

    m_playbackMode = DDP_GetPlayerMode(m_hPlayer);

    switch (m_playbackMode)
    {
    case DPM_STOPPED:
        if (m_stopLocation.title != 0)
        {
            // Resume from last stopped position
            TRACE(_T("Resuming from saved stop position...\n"));
            VERIFYDVD(DDP_TimePlay(m_hPlayer, m_stopLocation.title, m_stopLocation.titleTime));
            ZeroMemory(&m_stopLocation, sizeof (m_stopLocation));
        }
        else
        {
            TRACE(_T("Starting presentation...\n"));
            VERIFYDVD(DDP_StartPresentation(m_hPlayer));
        }
        break;

    case DPM_SCANNING:
    case DPM_REVERSEPLAY:
    case DPM_TRICKPLAY:
    case DPM_REVERSESCAN:
        VERIFYDVD(DDP_StopScan(m_hPlayer));
        break;

    case DPM_STILL:
        VERIFYDVD(CheckUserOp(DDP_StillOff(m_hPlayer)));
        break;

    case DPM_PAUSED:
        VERIFYDVD(CheckUserOp(DDP_PauseOff(m_hPlayer)));
        break;

    default:
        VERIFYDVD(CheckUserOp(DDP_PauseOn(m_hPlayer)));
        break;
    }
}

void CDVDPlayer::eject()
{
    stop();
    DDP_EjectDiskInDrive('D');
}

void CDVDPlayer::stop()
{
    if (m_hPlayer == NULL)
        return;

    // Get a fresh playback mode. We need to do this because this method
    // may be called before m_playbackMode gets updated (BUG#6628).

    m_playbackMode = DDP_GetPlayerMode(m_hPlayer);

    switch (m_playbackMode)
    {
    case DPM_STOPPED:
        TRACE(_T("Clearing saved stop position...\n"));
        m_StopBookmark.Reset();
        break;

    default:
        TRACE(_T("Saving stop position...\n"));
        SetBookmark(&m_StopBookmark);
        VERIFYDVD(DDP_Stop(m_hPlayer));
        break;
    }

    m_abRepeatState = AB_DISABLED;
}

void CDVDPlayer::pause()
{
    if (m_hPlayer != NULL)
    {
        // Get a fresh playback mode. We need to do this because this method
        // may be called before m_playbackMode gets updated (BUG#6628).

        m_playbackMode = DDP_GetPlayerMode(m_hPlayer);

        switch (m_playbackMode)
        {
        case DPM_PAUSED:
            if (m_bScanBackward)
            {
                TRACE(_T("Frame back\n"));
                VERIFYDVD(DDP_TrickPlay(m_hPlayer));
                VERIFYDVD(DDP_AdvanceFrameBy(m_hPlayer, -1));
            }
            else
            {
                VERIFYDVD(CheckUserOp(DDP_AdvanceFrame(m_hPlayer)));
            }

//          CheckUserOp(DDP_PauseOff(m_hPlayer));
            break;

        case DPM_SCANNING:
        case DPM_REVERSEPLAY:
//      case DPM_TRICKPLAY:
        case DPM_REVERSESCAN:
            VERIFYDVD(DDP_StopScan(m_hPlayer));
            // fall through
        case DPM_PLAYING:
            VERIFYDVD(CheckUserOp(DDP_PauseOn(m_hPlayer)));
            m_scanSpeed = 1;
            m_scanSlow = false;
            m_bScanBackward = false;
            break;
        }
    }
}

void CDVDPlayer::selectUp()
{
    if (m_hPlayer != NULL)
        VERIFYDVD(CheckUserOp(DDP_UpperButtonSelect(m_hPlayer)));
}

void CDVDPlayer::selectDown()
{
    if (m_hPlayer != NULL)
        VERIFYDVD(CheckUserOp(DDP_LowerButtonSelect(m_hPlayer)));
}

void CDVDPlayer::selectRight()
{
    if (m_hPlayer != NULL)
        VERIFYDVD(CheckUserOp(DDP_RightButtonSelect(m_hPlayer)));
}

void CDVDPlayer::selectLeft()
{
    if (m_hPlayer != NULL)
        VERIFYDVD(CheckUserOp(DDP_LeftButtonSelect(m_hPlayer)));
}

void CDVDPlayer::digit(int nDigit)
{
    if (nDigit < 0)
    {
        TRACE(_T("Resetting number\n"));
        m_numberTime = 0.0f;
        m_number = 0;
        return;
    }

#if 0
    if (m_number == 0 && nDigit == 0)
    {
        DisplayPresentationMode dpm = DDP_GetCurrentDisplayMode(m_hPlayer);
        TRACE(_T("Starting display mode: %s\n"), rgszDisplayPresentationMode[dpm]);

        for (int i = 0; i < 5; i += 1)
        {
            switch (dpm)
            {
            case DPM_4BY3:
                dpm = DPM_16BY9;
                break;
            case DPM_16BY9:
                dpm = DPM_LETTERBOXED;
                break;
            case DPM_LETTERBOXED:
                dpm = DPM_PANSCAN;
                break;
            case DPM_PANSCAN:
                dpm = DPM_NONE;
                break;
            case DPM_NONE:
                dpm = DPM_4BY3;
                break;
            }

            VERIFYDVD(DDP_SetDisplayMode(m_hPlayer, dpm));

            if (DDP_GetCurrentDisplayMode(m_hPlayer) == dpm)
                break;

            TRACE(_T("Failed to set display mode %s\n"), rgszDisplayPresentationMode[dpm]);
        }

        TRACE(_T("New display mode: %s\n"), rgszDisplayPresentationMode[dpm]);
        return;
    }
#endif

    m_playbackMode = DDP_GetPlayerMode(m_hPlayer);

    // check valid playback modes before computing the number

    if ( ((TT_DOM == m_domain) &&
          ((DPM_PLAYING == m_playbackMode) ||
           (DPM_PAUSED == m_playbackMode) ||
           (DPM_TRICKPLAY == m_playbackMode)) && (!m_scanSlow)) ||
         (STOP_DOM == m_domain && DPM_STOPPED == m_playbackMode) )
    {
        // valid mode

        m_numberTime = XAppGetNow();
        if (m_number < 1000)
            m_number = m_number * 10 + nDigit;
    }
    else
    {
        // invalid mode

        CallFunction(this, _T("OnProhibited"));
        m_numberTime = 0.0f;
        m_number = 0;
    }
}

void CDVDPlayer::activate()
{
    TRACE(_T("activate\n"));

    if (m_hPlayer == NULL)
        return;

    if (m_numberTime > 0.0f)
    {
        int nChapterCount = DDP_GetNumberOfPartOfTitle(m_hPlayer, (WORD)m_title);
        int nTitleCount = DDP_GetNumberOfTitles(m_hPlayer);
        m_playbackMode = DDP_GetPlayerMode(m_hPlayer);

        if (m_playbackMode == DPM_STOPPED)
        {
            if (m_number > 0 && m_number < nTitleCount)
            {
                VERIFYDVD(CheckUserOp(DDP_TitlePlay(m_hPlayer, (WORD)m_number)));
            }
            else
            {
                VERIFYDVD(CheckUserOp(GNR_OPERATION_PROHIBITED));
            }
        }
        else
        {
            if (m_number > 0 && m_number <= nChapterCount)
            {
                // Before switching chapters, check if the player was paused
                // so we can pause again after switching

                bool bWasPaused = (m_playbackMode == DPM_PAUSED);

                VERIFYDVD(CheckUserOp(DDP_PTTSearch(m_hPlayer, (WORD)m_number)));

                if(bWasPaused)
                {
                    // Restore paused state

                    VERIFYDVD(DDP_PauseOn(m_hPlayer));
                }
            }
            else
            {
                VERIFYDVD(CheckUserOp(GNR_OPERATION_PROHIBITED));
            }
        }

        m_numberTime = 0.0f;
        m_number = 0;
        return;
    }

    VERIFYDVD(CheckUserOp(DDP_ButtonActivate(m_hPlayer)));
}

void CDVDPlayer::nextAudioStream()
{
    if (m_hPlayer == NULL)
        return;

    // Check prerequisites before executing.
    // Some modes specifically don't allow angle changes.

    m_playbackMode = DDP_GetPlayerMode(m_hPlayer);

    if(TT_DOM == m_domain &&
        DPM_PLAYING == m_playbackMode &&
        !m_scanSlow &&
        1 == m_scanSpeed &&
        isUOPValid(UOP_AUDIO_STREAM_CHANGE))
    {
        BYTE bAudio = 0;
        DWORD dwSubPicture = 0;
        VERIFYDVD(DDP_GetAvailStreams(m_hPlayer, bAudio, dwSubPicture));

        if (m_audioStream != 0 && (bAudio & (1 << m_audioStream)) == 0)
            m_audioStream = 0;

        m_audioStream += 1;
        while (m_audioStream < 8 && (bAudio & (1 << m_audioStream)) == 0)
            m_audioStream += 1;
        if (m_audioStream == 8)
            m_audioStream = 0;

        VERIFYDVD(CheckUserOp(DDP_AudioStreamChange(m_hPlayer, (WORD)m_audioStream)));

        // Reset A-B repeat. We've seen problems with it after switching modes

        m_abRepeatState = AB_DISABLED;

    }
    else if (TT_DOM != m_domain &&
        isUOPValid(UOP_MENU_CALL_AUDIO) &&
        DDP_CheckMenuAvail(m_hPlayer, VMT_AUDIO_MENU))
    {
        CallFunction(this, _T("OnMenuCall"));
        VERIFYDVD(CheckUserOp(DDP_MenuCall(m_hPlayer, VMT_AUDIO_MENU)));
    }
    else
    {
        // Wrong mode, alert user

        CallFunction(this, _T("OnProhibited"));
    }

//  OnAudioStreamChange();
}

void CDVDPlayer::nextAngle()
{
    if (m_hPlayer == NULL)
        return;

/*
    NOTE: This would use the DVD's own angle menu where there is one,
    but Victor doesn't want to do that...

    if (DDP_CheckMenuAvail(m_hPlayer, VMT_ANGLE_MENU))
    {
        DDP_MenuCall(m_hPlayer, VMT_ANGLE_MENU);
        return;
    }
*/

    // Check prerequisites before executing.
    // Some modes specifically don't allow angle changes.

    m_playbackMode = DDP_GetPlayerMode(m_hPlayer);

    if(TT_DOM == m_domain &&
        DPM_PLAYING == m_playbackMode &&
        !m_scanSlow &&
        1 == m_scanSpeed &&
        isUOPValid(UOP_ANGLE_CHANGE))
    {
        // Before switching angles, check if the player was paused
        // so we can pause again after switching

        bool bWasPaused = (m_playbackMode == DPM_PAUSED);

        m_angleCount = DDP_GetNumberOfAngles(m_hPlayer, (WORD)m_title);
        if (m_angleCount == 0)
            m_angleCount = 1;
        m_angle = DDP_GetCurrentAngle(m_hPlayer);

        m_angle += 1;
        if (m_angle > m_angleCount)
            m_angle = 1;

        VERIFYDVD(CheckUserOp(DDP_AngleChange(m_hPlayer, (WORD)m_angle)));
        m_angle = DDP_GetCurrentAngle(m_hPlayer);

        // Reset A-B repeat. We've seen problems with it after switching modes

        m_abRepeatState = AB_DISABLED;

        if(bWasPaused)
        {
            // Restore paused state

            VERIFYDVD(DDP_PauseOn(m_hPlayer));
        }
    }
    else
    {
        // Wrong mode, alert user

        CallFunction(this, _T("OnProhibited"));
    }
}

void CDVDPlayer::startChapter()
{
    if (m_hPlayer == NULL)
        return;

    VERIFYDVD(CheckUserOp(DDP_TopPGSearch(m_hPlayer)));
    m_abRepeatState = AB_DISABLED;
}

void CDVDPlayer::nextChapter()
{
    if (m_hPlayer != NULL)
    {
        // Other modes, skip to next chapter

        VERIFYDVD(CheckUserOp(DDP_NextPGSearch(m_hPlayer)));

        m_abRepeatState = AB_DISABLED;
        Advance(0.0f);
    }

    return;
}

void CDVDPlayer::prevChapter()
{
    if (m_hPlayer != NULL)
    {
        // Other modes. Depending on how far we're in this chapter go either
        // to the beginning or to the previous chapter.

        VERIFYDVD(CheckUserOp(DDP_StopScan(m_hPlayer)));

        if ((XAppGetNow() - m_prevChapterTime) > 4.0f)
        {
            // Go to beginning of chapter

            VERIFYDVD(CheckUserOp(DDP_TopPGSearch(m_hPlayer)));
        }
        else
        {
            // Skip to previous chapter

            VERIFYDVD(CheckUserOp(DDP_PrevPGSearch(m_hPlayer)));
        }

        m_abRepeatState = AB_DISABLED;
        m_prevChapterTime = XAppGetNow();
        Advance(0.0f);
    }

    return;
}

void CDVDPlayer::menu()
{
    if (m_hPlayer == NULL)
        return;

    VERIFYDVD(CheckUserOp(DDP_MenuCall(m_hPlayer, VMT_ROOT_MENU)));
    m_abRepeatState = AB_DISABLED;
}

void CDVDPlayer::titleMenu()
{
    if (m_hPlayer == NULL)
        return;

    VERIFYDVD(CheckUserOp(DDP_MenuCall(m_hPlayer, VMT_TITLE_MENU)));
    m_abRepeatState = AB_DISABLED;
}

void CDVDPlayer::nextSubtitle()
{
    if (m_hPlayer == NULL)
        return;

    if (TT_DOM == m_domain && isUOPValid(UOP_SUB_PICTURE_STREAM_CHANGE))
    {
        BYTE bAudio = 0;
        DWORD dwSubPicture = 0;
        VERIFYDVD(DDP_GetAvailStreams(m_hPlayer, bAudio, dwSubPicture));

        TRACE(_T("Available sub pictures: 0x%08x\n"), dwSubPicture);

        do
        {
            m_subTitle += 1;
        }
        while (m_subTitle < 33 && (dwSubPicture & (1 << (m_subTitle - 1))) == 0);

        if (m_subTitle > 32)
            m_subTitle = 0;

        if (m_subTitle == 0)
            VERIFYDVD(CheckUserOp(DDP_SubPictureStreamChange(m_hPlayer, 0, FALSE)));
        else
            VERIFYDVD(CheckUserOp(DDP_SubPictureStreamChange(m_hPlayer, (WORD)(m_subTitle - 1), TRUE)));

        // Reset A-B repeat. We've seen problems with it after switching modes

        m_abRepeatState = AB_DISABLED;

        OnSubPictureStreamChange();
    }
    else if (TT_DOM != m_domain &&
        isUOPValid(UOP_MENU_CALL_SUB_PICTURE) &&
        DDP_CheckMenuAvail(m_hPlayer, VMT_SUBPICTURE_MENU))
    {
        CallFunction(this, _T("OnMenuCall"));
        VERIFYDVD(CheckUserOp(DDP_MenuCall(m_hPlayer, VMT_SUBPICTURE_MENU)));
    }
    else
    {
        // Wrong mode, alert user

        CallFunction(this, _T("OnProhibited"));
    }
}

void CDVDPlayer::frameAdvance()
{
    if (m_hPlayer == NULL)
        return;

    // Get a fresh playback mode. We need to do this because this method
    // may be called before m_playbackMode gets updated (BUG#6628).

    m_playbackMode = DDP_GetPlayerMode(m_hPlayer);

    // Paused or trick-play mode required by DDP_AdvanceFrame.

    if( m_playbackMode != DPM_PAUSED &&
        m_playbackMode != DPM_TRICKPLAY )
    {
        // other mode currently, lets go with trick-play mode because it's required
        // by DDP_AdvanceFrameBy used in frameReverse.

        VERIFYDVD(CheckUserOp(DDP_TrickPlay(m_hPlayer)));

        m_scanSlow = false;
        m_bScanBackward = false;
        m_scanSpeed = 1;
    }

    VERIFYDVD(CheckUserOp(DDP_AdvanceFrame(m_hPlayer)));
}

void CDVDPlayer::frameReverse()
{
    if (m_hPlayer == NULL)
        return;

    // Get a fresh playback mode. We need to do this because this method
    // may be called before m_playbackMode gets updated (BUG#6628).

    m_playbackMode = DDP_GetPlayerMode(m_hPlayer);

    // trick-play mode required by DDP_AdvanceFrameBy

    if( m_playbackMode != DPM_TRICKPLAY )
    {
        VERIFYDVD(CheckUserOp(DDP_TrickPlay(m_hPlayer)));

        m_scanSlow = false;
        m_bScanBackward = false;
        m_scanSpeed = 1;
    }

    VERIFYDVD(CheckUserOp(DDP_AdvanceFrameBy(m_hPlayer, -1)));
}

void CDVDPlayer::forwardScan()
{
    if (m_hPlayer == NULL)
        return;

    // scanning is only available in the TT_DOMain

    if (isPlaybackDomain())
    {
        // Get a fresh playback mode. We need to do this because this method
        // may be called before m_playbackMode gets updated (BUG#6628).

        m_playbackMode = DDP_GetPlayerMode(m_hPlayer);

        if( (m_playbackMode == DPM_PAUSED) ||
            (m_playbackMode == DPM_TRICKPLAY))  // starting slow forward
        {
            // if player is currently paused, start slow scan, forward, at speed 2
            m_scanSlow = true;
            m_bScanBackward = false;
            m_scanSpeed = 2;

            VERIFYDVD(CheckUserOp(DDP_SetPlaybackSpeed(m_hPlayer, 1000 / m_scanSpeed)));
            VERIFYDVD(CheckUserOp(DDP_PauseOff(m_hPlayer)));
            VERIFYDVD(CheckUserOp(DDP_StopScan(m_hPlayer)));
        }
        else if ((m_scanSpeed > 1) && (!m_scanSlow)) // fast scan update
        {
            if (!m_bScanBackward)
            {
                // Already scanning forward. Double speed. Limit is 32

                if (m_scanSpeed < 32)
                    m_scanSpeed *= 2;

                VERIFYDVD(CheckUserOp(DDP_ForwardScan(m_hPlayer, (WORD)m_scanSpeed)));
            }
            else
            {
                // Scanning backwards. Cut speed in half. If we reach 1,
                // we'll start the forward scan at 2x

                m_scanSpeed /= 2;

                if(m_scanSpeed <= 1)
                {
                    // Start forward scan at 2x

                    m_scanSpeed = 2;
                    m_bScanBackward = false;

                    VERIFYDVD(DDP_StopScan(m_hPlayer));
                    VERIFYDVD(CheckUserOp(DDP_SetPlaybackSpeed(m_hPlayer, 1000 * m_scanSpeed)));
                }
                else
                {
                    // Update backward scan speed

                    if(m_scanSpeed > 2)
                    {
                        VERIFYDVD(CheckUserOp(DDP_BackwardScan(m_hPlayer, (WORD)m_scanSpeed)));
                    }
                    else
                    {
                        VERIFYDVD(DDP_StopScan(m_hPlayer));
                        VERIFYDVD(CheckUserOp(DDP_SetPlaybackSpeed(m_hPlayer, 1000 * m_scanSpeed)));
                    }
                }
            }
        }
        else
        {
            if (!m_scanSlow)
            {
                // Player is playing at regular speed. Start forward scan. Initial speed is 2x

                m_scanSpeed = 2;

                VERIFYDVD(CheckUserOp(DDP_StopScan(m_hPlayer)));
                VERIFYDVD(CheckUserOp(DDP_SetPlaybackSpeed(m_hPlayer, 1000 *  m_scanSpeed)));
            }
            else
            {
                // Player is in slow scan mode

                if (!m_bScanBackward)
                {
                    // Already going forwards. Double speed divisor. Limit is 8.

                    if (m_scanSpeed < 8)
                    {
                        m_scanSpeed *= 2;
                        VERIFYDVD(CheckUserOp(DDP_SetPlaybackSpeed(m_hPlayer, 1000 / m_scanSpeed)));
                    }
                }
                else
                {
                    // Going backwards. Cut speed divisor in half. If we reach 1,
                    // go to forward scan.

                    m_scanSpeed /= 2;

                    if(m_scanSpeed <= 1)
                    {
                        // Start slow forward scan at 1/2

                        m_scanSpeed = 2;
                        m_bScanBackward = false;

                        VERIFYDVD(CheckUserOp(DDP_StopScan(m_hPlayer)));
                        VERIFYDVD(CheckUserOp(DDP_SetPlaybackSpeed(m_hPlayer, 1000 / m_scanSpeed)));
                    }
                    else
                    {
                        // Update speed

                        if(m_scanSpeed > 2)
                        {
                            VERIFYDVD(CheckUserOp(DDP_SetPlaybackSpeed(m_hPlayer, 1000 / m_scanSpeed)));
                        }
                        else
                        {
                            VERIFYDVD(DDP_StopScan(m_hPlayer));
                            VERIFYDVD(CheckUserOp(DDP_SetPlaybackSpeed(m_hPlayer, 1000 / m_scanSpeed)));
                            VERIFYDVD(CheckUserOp(DDP_ReversePlayback(m_hPlayer)));
                        }
                    }
                }
            }
        }
    }
    else
    {
        // Other domains = failure.
        // The script will detect it by checking scanSpeed

        m_scanSpeed = 1;
    }
}

void CDVDPlayer::backwardScan()
{
    if (m_hPlayer == NULL)
        return;

    // scanning is only available in the TT_DOMain

    if (isPlaybackDomain())
    {
        // Get a fresh playback mode. We need to do this because this method
        // may be called before m_playbackMode gets updated (BUG#6628).

        m_playbackMode = DDP_GetPlayerMode(m_hPlayer);

        if( (m_playbackMode == DPM_PAUSED) ||
            (m_playbackMode == DPM_TRICKPLAY))
        {
            // if player is currently paused, start slow scan, backwards, at speed 2

            m_scanSlow = true;
            m_bScanBackward = true;
            m_scanSpeed = 2;

            VERIFYDVD(CheckUserOp(DDP_StopScan(m_hPlayer)));
            VERIFYDVD(CheckUserOp(DDP_SetPlaybackSpeed(m_hPlayer, 1000 / m_scanSpeed)));
            VERIFYDVD(CheckUserOp(DDP_ReversePlayback(m_hPlayer)));
            VERIFYDVD(CheckUserOp(DDP_PauseOff(m_hPlayer)));
        }
        else if ((m_scanSpeed > 1) && (!m_scanSlow))
        {
            if (m_bScanBackward)
            {
                // Already scanning backwards. Double speed. Limit is 32

                if (m_scanSpeed < 32)
                    m_scanSpeed *= 2;

                VERIFYDVD(CheckUserOp(DDP_BackwardScan(m_hPlayer, (WORD)m_scanSpeed)));
            }
            else
            {
                // Scanning forward. Cut speed in half. If we reach 1,
                // we'll start the backward scan at 2x

                m_scanSpeed /= 2;

                if(m_scanSpeed <= 1)
                {
                    // Start backward scan at 2x

                    m_scanSpeed = 2;
                    m_bScanBackward = true;

                    VERIFYDVD(DDP_StopScan(m_hPlayer));
                    VERIFYDVD(CheckUserOp(DDP_ReversePlayback(m_hPlayer)));
                    VERIFYDVD(CheckUserOp(DDP_SetPlaybackSpeed(m_hPlayer, 1000 *  m_scanSpeed)));
                }
                else
                {
                    // Update forward scan speed

                    if(m_scanSpeed > 2)
                    {
                        VERIFYDVD(CheckUserOp(DDP_ForwardScan(m_hPlayer, (WORD)m_scanSpeed)));
                    }
                    else
                    {
                        VERIFYDVD(DDP_StopScan(m_hPlayer));
                        VERIFYDVD(CheckUserOp(DDP_SetPlaybackSpeed(m_hPlayer, 1000 *  m_scanSpeed)));
                    }
                }
            }
        }
        else
        {
            if (!m_scanSlow)
            {
                // Player is playing at regular speed. Start backward scan. Initial speed is 2x

                m_scanSpeed = 2;
                m_bScanBackward = true;

                VERIFYDVD(DDP_StopScan(m_hPlayer));
                VERIFYDVD(CheckUserOp(DDP_ReversePlayback(m_hPlayer)));
                VERIFYDVD(CheckUserOp(DDP_SetPlaybackSpeed(m_hPlayer, 1000 *  m_scanSpeed)));
            }
            else
            {
                // Player is in slow scan mode

                if (m_bScanBackward)
                {
                    // Already going backwards. Double speed divisor. Limit is 8.

                    if (m_scanSpeed < 8)
                    {
                        m_scanSpeed *= 2;
                        VERIFYDVD(CheckUserOp(DDP_SetPlaybackSpeed(m_hPlayer, 1000 / m_scanSpeed)));
                    }
                }
                else
                {
                    // Going forward. Cut speed divisor in half. If we reach 1,
                    // start slow backward scan.

                    m_scanSpeed /= 2;

                    if(m_scanSpeed <= 1)
                    {
                        m_scanSpeed = 2;
                        m_bScanBackward = true;

                        VERIFYDVD(CheckUserOp(DDP_StopScan(m_hPlayer)));
                        VERIFYDVD(CheckUserOp(DDP_SetPlaybackSpeed(m_hPlayer, 1000 / m_scanSpeed)));
                        VERIFYDVD(CheckUserOp(DDP_ReversePlayback(m_hPlayer)));
                    }
                    else
                    {
                        // Update speed

                        VERIFYDVD(CheckUserOp(DDP_SetPlaybackSpeed(m_hPlayer, 1000 / m_scanSpeed)));
                    }
                }
            }
        }
    }
    else
    {
        // Other domains = failure.
        // The script will detect it by checking scanSpeed

        m_scanSpeed = 1;
    }
}

void CDVDPlayer::stopScan()
{
    if (m_hPlayer == NULL)
        return;

    VERIFYDVD(CheckUserOp(DDP_StopScan(m_hPlayer)));
    m_bScanBackward = false;
    m_scanSpeed = 1;
}

int CDVDPlayer::isUOPValid(int nUOP)
{
    if (m_hPlayer == NULL)
        return 0;

    return (nUOP & DDP_GetForbiddenUserOperations(m_hPlayer)) == 0;
}

int CDVDPlayer::isPlaybackDomain()
{
    if (m_domain == VMGM_DOM || m_domain == VTSM_DOM || m_domain == TT_DOM)
        return 1;
    else
        return 0;
}

void CDVDPlayer::refreshAudioSettings()
{
    ASSERT(m_hUnitSet != NULL);

    DWORD AudioFlags = XGetAudioFlags();
    AC3SpeakerConfig SpeakerConfig;
    AudioTypeConfig DigitalConfig;

    switch (DSSPEAKER_BASIC(AudioFlags))
    {
    case DSSPEAKER_STEREO:
        SpeakerConfig = AC3SC_20_NORMAL;
        break;

    case DSSPEAKER_MONO:
        SpeakerConfig = AC3SC_10;
        break;

    case DSSPEAKER_SURROUND:
    default:
        SpeakerConfig = AC3SC_20_SURROUND_COMPATIBLE;
    }

    VERIFYDVD(VDR_ConfigureUnitsTags(m_hUnitSet, SET_MPEG2_AC3_SPEAKER_CONFIG(SpeakerConfig), TAGDONE));

    DigitalConfig.config.spdifOutMode = DSSPEAKER_ENCODED(AudioFlags) & DSSPEAKER_ENABLE_DTS ? SPDIFOM_DEFAULT : SPDIFOM_MUTE_NULL;
    VERIFYDVD(VDR_ConfigureUnitsTags(m_hUnitSet, SET_MPEG2_DTS_AUDIO_TYPE_CONFIG(DigitalConfig), TAGDONE));

    DigitalConfig.config.spdifOutMode = DSSPEAKER_ENCODED(AudioFlags) & DSSPEAKER_ENABLE_AC3 ? SPDIFOM_DEFAULT : SPDIFOM_MUTE_NULL;
    VERIFYDVD(VDR_ConfigureUnitsTags(m_hUnitSet, SET_MPEG2_AC3_AUDIO_TYPE_CONFIG(DigitalConfig), TAGDONE));

    if (XGetAVPack() == XC_AV_PACK_RFU)
    {
        // Set AC3 decoder to "TV Style" dynamic range compression
        VDR_ConfigureUnitsTags(m_hUnitSet, SET_MPEG2_AC3_DYNAMIC_RANGE(AC3DYNRG_TV), TAGDONE);

        // Set AC3 decoder to mono
        VDR_ConfigureUnitsTags(m_hUnitSet, SET_MPEG2_AC3_SPEAKER_CONFIG(AC3SC_10), TAGDONE);
    }
}

void CDVDPlayer::UpdateZoom()
{
    ASSERT(m_hUnitSet != NULL);

    WORD w, h;
    VDR_ConfigureUnitsTags(m_hUnitSet,
        GET_MPEG_DISPLAY_WIDTH(w),
        GET_MPEG_DISPLAY_HEIGHT(h),
        TAGDONE);

    float cx = (float)w / m_nScaleSet;
    float cy = (float)h / m_nScaleSet;

    int width = m_width == 0 ? m_videoWidth : m_width;
    int height = m_height == 0 ? m_videoHeight : m_height;

    float ws = width * m_nScaleSet;
    float hs = height * m_nScaleSet;

    if (m_nScaleSet > 1.0f)
    {
        float xo = (m_nOffsetXSet + 1.0f) / 2.0f;
        float yo = (m_nOffsetYSet + 1.0f) / 2.0f;

        WORD x = (WORD)(xo * ((float)w - cx));
        WORD y = (WORD)(yo * ((float)h - cy));

        VDR_ConfigureUnitsTags(m_hUnitSet,
            SET_PIP_ADAPT_SOURCE_SIZE(FALSE),
            SET_PIP_SOURCE_LEFT(x),
            SET_PIP_SOURCE_TOP(y),
            SET_PIP_SOURCE_WIDTH((WORD)cx),
            SET_PIP_SOURCE_HEIGHT((WORD)cy),
            TAGDONE);

        VDR_ConfigureUnitsTags(m_hUnitSet,
            SET_PIP_DEST_LEFT(m_left),
            SET_PIP_DEST_TOP(m_top),
            SET_PIP_DEST_WIDTH(m_width),
            SET_PIP_DEST_HEIGHT(m_height),
            TAGDONE);
    }
    else
    {
//      if (m_videoHeight > m_height)
//      {
            VDR_ConfigureUnitsTags(m_hUnitSet,
                SET_PIP_ADAPT_SOURCE_SIZE(FALSE),
                SET_PIP_SOURCE_LEFT((WORD)0),
                SET_PIP_SOURCE_TOP((WORD)0),
                SET_PIP_SOURCE_WIDTH((WORD)m_videoWidth),
                SET_PIP_SOURCE_HEIGHT((WORD)m_videoHeight),
                TAGDONE);

            VDR_ConfigureUnitsTags(m_hUnitSet,
                SET_PIP_DEST_LEFT(m_left),
                SET_PIP_DEST_TOP(m_top),
                SET_PIP_DEST_WIDTH(m_width),
                SET_PIP_DEST_HEIGHT(m_height),
                TAGDONE);
//      }
//      else if (m_videoWidth > m_width)
//      {
//      }
//      else
//      {
//      }
/*
        int nSrcWidth = m_videoWidth;
        int nSrcHeight = m_videoHeight;

        float rWidth = (float)m_videoWidth / (float)m_width;
        float rHeight = (float)m_height / (float)m_videoHeight;

        if (rWidth < rHeight)
        {
            nSrcHeight = (int)(rWidth * m_height);
            nSrcWidth = m_videoWidth;
        }
        else
        {
            nSrcWidth = (int)(rHeight * m_videoWidth);
            nSrcHeight = m_videoHeight;
        }

        int x = (m_videoWidth - nSrcWidth) / 2;
        if (x < 0)
            x = 0;

        int y = (m_videoHeight - nSrcHeight) / 2;
        if (y < 0)
            y = 0;

        VDR_ConfigureUnitsTags(m_hUnitSet,
            SET_PIP_ADAPT_SOURCE_SIZE(FALSE),
            SET_PIP_SOURCE_LEFT((WORD)x),
            SET_PIP_SOURCE_TOP((WORD)y),
            SET_PIP_SOURCE_WIDTH((WORD)nSrcWidth),
            SET_PIP_SOURCE_HEIGHT((WORD)nSrcHeight),
            TAGDONE);

        int left = m_left;
        int top = m_top;
        int width = m_width;
        int height = m_height;

        if (m_width > nSrcWidth)
        {
            int cx = m_width - nSrcWidth;
            left += cx / 2;
            width -= cx;
        }

        if (m_height > nSrcHeight)
        {
            int cy = m_height - nSrcHeight;
            top += cy / 2;
            height -= cy;
        }

        VDR_ConfigureUnitsTags(m_hUnitSet,
            SET_PIP_DEST_LEFT(left),
            SET_PIP_DEST_TOP(top),
            SET_PIP_DEST_WIDTH(width),
            SET_PIP_DEST_HEIGHT(height),
            TAGDONE);

#ifdef _DEBUG
        {
            static int _x, _y, _cx, _cy;
            if (x != _x || y != _y || nSrcWidth != _cx || nSrcHeight != _cy)
            {
                _x = x;
                _y = y;
                _cx = nSrcWidth;
                _cy = nSrcHeight;

                TRACE(_T("PIP SOURCE: %d,%d %dx%d\n"), _x, _y, _cx, _cy);
            }
        }
#endif
*/
    }
}

void CDVDPlayer::setScale(float nScale)
{
    if (m_hUnitSet != NULL)
    {
        m_nScale = nScale;
        if (m_nScale == 1.0f)
        {
            m_nScaleSet = m_nScaleStart = 1.0f;
            m_nOffsetX = m_nOffsetXSet = m_nOffsetXStart = 0.0f;
            m_nOffsetY = m_nOffsetYSet = m_nOffsetYStart = 0.0f;
            m_nZoomTime = 0.0f;
            return;
        }

        m_nScaleStart = m_nScaleSet;
        m_nOffsetXStart = m_nOffsetXSet;
        m_nOffsetYStart = m_nOffsetYSet;
        m_nZoomTime = XAppGetNow();
    }
}

void CDVDPlayer::setZoomPos(float x, float y)
{
    // the offsets should be from -1 to 1 so joystick inputs will suffice

    m_nOffsetX = x;
    m_nOffsetY = y;

    m_nScaleStart = m_nScaleSet;
    m_nOffsetXStart = m_nOffsetXSet;
    m_nOffsetYStart = m_nOffsetYSet;
    m_nZoomTime = XAppGetNow();
}

void CDVDPlayer::OnSubPictureStreamChange()
{
    const TCHAR* sz = NULL;

    if (m_subTitle != 0)
    {
        DVDSubPictureStreamFormat format;
        VERIFYDVD(DDP_GetSubPictureStreamAttributes(m_hPlayer, (WORD)(m_subTitle - 1), format));
        sz = LanguageNameFromCode(format.languageCode);
    }

    delete [] m_subTitleLanguage;
    m_subTitleLanguage = NULL;

    if (sz != NULL)
    {
        TCHAR buffer[MAX_TRANSLATE_LEN];
        lstrcpy(buffer, _T("Lang "));
        lstrcat(buffer, sz);
        m_subTitleLanguage = new TCHAR [MAX_TRANSLATE_LEN];
        Translate(buffer, m_subTitleLanguage);
    }
}

/*
void CDVDPlayer::OnAudioStreamChange()
{
    DVDAudioStreamFormat format;
    DDP_GetAudioStreamAttributes(m_hPlayer, (WORD)m_audioStream, format);

    m_audioFormat = format.codingMode;
    m_audioChannels = format.channels;

    const TCHAR* sz = LanguageNameFromCode(format.languageCode);

    delete [] m_audioLanguage;
    m_audioLanguage = NULL;

    if (sz != NULL)
    {
        m_audioLanguage = new TCHAR [_tcslen(sz) + 1];
        _tcscpy(m_audioLanguage, sz);
    }
}
*/

void CDVDPlayer::abRepeat()
{
    Error err = GNR_OK;

    //
    // Check prerequisites before executing
    //

    VERIFYDVD(DDP_GetCurrentLocation(m_hPlayer, m_locationB));

    if( (TT_DOM == m_locationB.domain) &&
        (DPM_PLAYING == m_playbackMode ||
        DPM_PAUSED == m_playbackMode ||
        DPM_TRICKPLAY == m_playbackMode) &&
        (! m_scanSlow) &&
        1 == m_scanSpeed)
    {
        switch (m_abRepeatState)
        {
        case AB_DISABLED:
            TRACE(_T("Set A...\n"));
            VERIFYDVD(DDP_GetCurrentLocation(m_hPlayer, m_locationA));
            if (m_locationA.domain != TT_DOM)
            {
                TRACE(_T("A-B repeat is invalid at this time\n"));
                CheckUserOp(GNR_UOP_PROHIBITED);
                return;
            }
            VERIFYDVD(err = CheckUserOp(SetBookmark(&m_LocationABookmark)));
            if(IS_GNERROR(err))
            {
                return;
            }

            m_abRepeatState = AB_SETTING_B;
            break;

        case AB_SETTING_B:
            {
                TRACE(_T("Set B...\n"));

                //
                // Only set point B if we're in the same title
                // as point A and the time difference is greater
                // than MIN_REPEAT_AB_INTERVAL
                //

                if(m_locationB.title == m_locationA.title &&
                    m_locationB.titleTime.Millisecs() > m_locationA.titleTime.Millisecs() &&
                    (m_locationB.titleTime.Millisecs() - m_locationA.titleTime.Millisecs()) > MIN_REPEAT_AB_INTERVAL)
                {
                    m_abRepeatState = AB_REPEATING;

                    Error e = GotoBookmark(&m_LocationABookmark);
                    VERIFYDVD(CheckUserOp(e));
                    if (e == GNR_UOP_PROHIBITED)
                    {
                        TRACE(_T("A-B repeat is invalid at this time\n"));
                        m_abRepeatState = AB_DISABLED;
                    }
                }
                else
                {
                    //
                    // Didn't pass prerequisites. Disable AB repeat
                    //

                    m_abRepeatState = AB_DISABLED;
                    CheckUserOp(GNR_UOP_PROHIBITED);
                }
            }
            break;

        case AB_REPEATING:
            TRACE(_T("Disabling A-B Repeat...\n"));
            m_abRepeatState = AB_DISABLED;
            break;
        }
    }
    else
    {
        TRACE(_T("A-B repeat is invalid at this time\n"));
        m_abRepeatState = AB_DISABLED;
        CheckUserOp(GNR_UOP_PROHIBITED);
        return;
    }
}


////////////////////////////////////////////////////////////////////////////
// The EventHandler callback is invoked from a DVD-Player specific thread.
// The CDVDPlayer maintains a queue of messages yet to be processed and will
// deal with the entire queue when Advance() is called...
//
void __stdcall CDVDPlayer::EventHandler(DWORD event, void* pvUserData, DWORD info)
{
    CDVDPlayer* pDVDPlayer = (CDVDPlayer*)pvUserData;
    EnterCriticalSection(&pDVDPlayer->m_eventLock);
    pDVDPlayer->m_events[pDVDPlayer->m_eventWrite].event = event;
    pDVDPlayer->m_events[pDVDPlayer->m_eventWrite].info = info;
    if (pDVDPlayer->m_eventRead != ((pDVDPlayer->m_eventWrite + 1) % EVENTS_STORED))
        pDVDPlayer->m_eventWrite = (pDVDPlayer->m_eventWrite + 1) % EVENTS_STORED;
    LeaveCriticalSection(&pDVDPlayer->m_eventLock);
}

bool CDVDPlayer::GetNextEvent(DWORD& event, DWORD& info)
{
    EnterCriticalSection(&m_eventLock);
    if (m_eventRead == m_eventWrite)
    {
        LeaveCriticalSection(&m_eventLock);
        return false;
    }

    event = m_events[m_eventRead].event;
    info = m_events[m_eventRead].info;
    m_eventRead = (m_eventRead + 1) % EVENTS_STORED;
    LeaveCriticalSection(&m_eventLock);

    return true;
}

void CDVDPlayer::InstallEventHandlers()
{
    ASSERT(m_hPlayer != NULL);

    InitializeCriticalSection(&m_eventLock);

    m_eventRead = 0;
    m_eventWrite = 0;

    // TODO: Don't install handlers for events we don't care about...

    DDP_InstallEventHandler(m_hPlayer, DNE_TITLE_CHANGE, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_PART_OF_TITLE_CHANGE, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_VALID_UOP_CHANGE, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_ANGLE_CHANGE, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_AUDIO_STREAM_CHANGE, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_SUBPICTURE_STREAM_CHANGE, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_DOMAIN_CHANGE, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_PARENTAL_LEVEL_CHANGE, EventHandler, this);
//  DDP_InstallEventHandler(m_hPlayer, DNE_BITRATE_CHANGE, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_STILL_ON, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_STILL_OFF, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_PLAYBACK_MODE_CHANGE, EventHandler, this);
//  DDP_InstallEventHandler(m_hPlayer, DNE_CDA_PLAYING_GAP, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_READ_ERROR, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_DISPLAY_MODE_CHANGE, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_STREAMS_CHANGE, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_SCAN_SPEED_CHANGE, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_PLAYBACK_SPEED_CHANGE, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_VIDEO_STANDARD_CHANGE, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_BREAKPOINT_REACHED, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_DRIVE_DETACHED, EventHandler, this);
    DDP_InstallEventHandler(m_hPlayer, DNE_ERROR, EventHandler, this);
}

void CDVDPlayer::RemoveEventHandlers()
{
    if (m_hPlayer == NULL)
        return;

    DDP_RemoveEventHandler(m_hPlayer, DNE_TITLE_CHANGE);
    DDP_RemoveEventHandler(m_hPlayer, DNE_PART_OF_TITLE_CHANGE);
    DDP_RemoveEventHandler(m_hPlayer, DNE_VALID_UOP_CHANGE);
    DDP_RemoveEventHandler(m_hPlayer, DNE_ANGLE_CHANGE);
    DDP_RemoveEventHandler(m_hPlayer, DNE_AUDIO_STREAM_CHANGE);
    DDP_RemoveEventHandler(m_hPlayer, DNE_SUBPICTURE_STREAM_CHANGE);
    DDP_RemoveEventHandler(m_hPlayer, DNE_DOMAIN_CHANGE);
    DDP_RemoveEventHandler(m_hPlayer, DNE_PARENTAL_LEVEL_CHANGE);
//  DDP_RemoveEventHandler(m_hPlayer, DNE_BITRATE_CHANGE);
    DDP_RemoveEventHandler(m_hPlayer, DNE_STILL_ON);
    DDP_RemoveEventHandler(m_hPlayer, DNE_STILL_OFF);
    DDP_RemoveEventHandler(m_hPlayer, DNE_PLAYBACK_MODE_CHANGE);
//  DDP_RemoveEventHandler(m_hPlayer, DNE_CDA_PLAYING_GAP);
    DDP_RemoveEventHandler(m_hPlayer, DNE_READ_ERROR);
    DDP_RemoveEventHandler(m_hPlayer, DNE_DISPLAY_MODE_CHANGE);
    DDP_RemoveEventHandler(m_hPlayer, DNE_STREAMS_CHANGE);
    DDP_RemoveEventHandler(m_hPlayer, DNE_SCAN_SPEED_CHANGE);
    DDP_RemoveEventHandler(m_hPlayer, DNE_PLAYBACK_SPEED_CHANGE);
    DDP_RemoveEventHandler(m_hPlayer, DNE_VIDEO_STANDARD_CHANGE);
    DDP_RemoveEventHandler(m_hPlayer, DNE_BREAKPOINT_REACHED);
    DDP_RemoveEventHandler(m_hPlayer, DNE_DRIVE_DETACHED);
    DDP_RemoveEventHandler(m_hPlayer, DNE_ERROR);

    DeleteCriticalSection(&m_eventLock);
}


#ifdef _DEBUG
static const TCHAR* rgszPlayerMode [] =
{
    _T("DPM_STOPPED"),
    _T("DPM_PAUSED"),
    _T("DPM_STILL"),
    _T("DPM_PLAYING"),
    _T("DPM_BUSY"),
    _T("DPM_SCANNING"),
    _T("DPM_INACTIVE"),
    _T("DPM_REVERSEPLAY"),
    _T("DPM_TRICKPLAY"),
    _T("DPM_REVERSESCAN")
};

static const TCHAR* rgszDomain [] =
{
    _T("FP_DOM"),
    _T("VMGM_DOM"),
    _T("VTSM_DOM"),
    _T("TT_DOM"),
    _T("STOP_DOM")
};


static const TCHAR* rgszAudioCodingMode [] =
{
    _T("DAM_AC3"),
    _T("DAM_MPEG1"),
    _T("DAM_MPEG2"),
    _T("DAM_LPCM"),
    _T("DAM_DTS"),
    _T("DAM_SDDS")
};

static const TCHAR* rgszVideoStandard [] =
{
    _T("VSTD_NTSC"),
    _T("VSTD_PAL"),
    _T("VSTD_SECAM"),
    _T("VSTD_HDTV"),
    _T("VSTD_UNKNOWN"),
};

static const TCHAR* rgszUOP [] =
{
    _T("TIME PLAY SEARCH"),
    _T("PTT PLAY SEARCH"),
    _T("TITLE PLAY"),
    _T("STOP"),
    _T("GO UP"),
    _T("TIME PTT SEARCH"),
    _T("PREV TOP PG SEARCH"),
    _T("NEXT PG SEARCH"),
    _T("FORWARD SCAN"),
    _T("BACKWARD SCAN"),
    _T("MENU CALL TITLE"),
    _T("MENU CALL ROOT"),
    _T("MENU CALL SUB PICTURE"),
    _T("MENU CALL AUDIO"),
    _T("MENU CALL ANGLE"),
    _T("MENU CALL PTT"),
    _T("RESUME"),
    _T("BUTTON"),
    _T("STILL OFF"),
    _T("PAUSE ON"),
    _T("AUDIO STREAM CHANGE"),
    _T("SUB PICTURE STREAM CHANGE"),
    _T("ANGLE CHANGE"),
    _T("KARAOKE MODE CHANGE"),
    _T("VIDEO MODE CHANGE"),
    _T("SCAN OFF"),
    _T("PAUSE OFF"),
    _T("unknown 1"),
    _T("unknown 2"),
    _T("unknown 3"),
    _T("unknown 4"),
    _T("unknown 5"),
    _T("unknown 6")
};

static void DumpUOP(DWORD uop)
{
    TRACE(_T("User Operations:\n"));
    for (int i = 0; i < 32; i += 1)
    {
        if (uop & (1 << i))
            TRACE(_T("\t%s\n"), rgszUOP[i]);
    }
}


struct DVDERROR
{
    Error m_error;
    TCHAR* m_sz;
};

static const DVDERROR rgerror [] =
{
    { GNR_FILE_NOT_FOUND, _T("general file not found error") },
    { GNR_FILE_IN_USE, _T("the file is currently in use") },
    { GNR_FILE_WRONG_FORMAT, _T("the file is in the wrong file format") },
    { GNR_END_OF_FILE, _T("attempt to read behind the end of a file") },
    { GNR_NOT_ENOUGH_MEMORY, _T("general not enough memory left") },
    { GNR_MEM_NOT_ALLOCATED, _T("the memory that was requested to be freed was not allocated before.") },
    { GNR_MEM_ALLOCATED_BEFORE, _T("the memory was allocated before") },
    { GNR_UNIMPLEMENTED, _T("the function is not yet implemented, and will never be") },
    { GNR_OBJECT_NOT_FOUND, _T("the requested object could not be found") },
    { GNR_OBJECT_EXISTS, _T("the object requested to be created does already exist") },
    { GNR_OBJECT_IN_USE, _T("an operation that needs exclusive access to an object, found the object already in use") },
    { GNR_OBJECT_FOUND, _T("an object that was to be inserted in a data structure with unique objects was already in there") },
    { GNR_RANGE_VIOLATION, _T("a given parameter was out of bounds") },
    { GNR_INVALID_CONFIGURE_STATE, _T("the unit was in an invalid state for configuration") },
    { GNR_OBJECT_FULL, _T("GNR_OBJECT_FULL") },
    { GNR_OBJECT_EMPTY, _T("GNR_OBJECT_EMPTY") },
    { GNR_OBJECT_NOT_ALLOCATED, _T("GNR_OBJECT_NOT_ALLOCATED") },
    { GNR_OBJECT_READ_ONLY, _T("GNR_OBJECT_READ_ONLY") },
    { GNR_OBJECT_WRITE_ONLY, _T("GNR_OBJECT_WRITE_ONLY") },
    { GNR_OPERATION_PROHIBITED, _T("GNR_OPERATION_PROHIBITED") },
    { GNR_OBJECT_INVALID, _T("GNR_OBJECT_INVALID") },
    { GNR_INSUFFICIENT_RIGHTS, _T("GNR_INSUFFICIENT_RIGHTS") },
    { GNR_TIMEOUT, _T("GNR_TIMEOUT") },
    { GNR_FILE_READ_ERROR, _T("GNR_FILE_READ_ERROR") },
    { GNR_FILE_WRITE_ERROR, _T("GNR_FILE_WRITE_ERROR") },
    { GNR_INVALID_PARAMETERS, _T("GNR_INVALID_PARAMETERS") },
    { GNR_CONNECTION_LOST, _T("GNR_CONNECTION_LOST") },
    { GNR_OPERATION_ABORTED, _T("GNR_OPERATION_ABORTED") },
    { GNR_DMA_PIP_NOT_POSSIBLE, _T("Current screen mode does not allow DMA PIP") },
    { GNR_VIDEO_STD_NOT_SUPPORTED, _T("not supported.") },
    { GNR_PIP_NOT_RUNNING, _T("GNR_PIP_NOT_RUNNING") },
    { GNR_NO_OPTIMAL_MINPIX, _T("GNR_NO_OPTIMAL_MINPIX") },
    { GNR_DEST_RECT_SIZE, _T("Destination rectangle too small or too big.") },
    { GNR_WRONG_GRAB_SIZE, _T("Dimensions of frame to grab are too big.") },
    { GNR_NO_VBLANK_IRQ, _T("An expected VBlank IRQ did not occur.") },
    { GNR_GRAB_TIMEOUT, _T("A timeout occurred during grabbing a frame.") },
    { GNR_WRONG_GRAB_FORMAT, _T("An unsupported capture format was requested") },
    { GNR_SAMPLEMODE_NOT_SUPPORTED, _T("Desired video sample mode not supported") },
    { GNR_VP_ERROR, _T("General error with Video Port") },
    { GNR_VP_NO_PORT_MATCH, _T("Video source does not match Video Port") },
    { GNR_VP_HARDWARE_IN_USE, _T("Video Port Hardware in use") },
    { GNR_VPM_ERROR, _T("General error with VPM services") },
    { GNR_VPE_ERROR, _T("General error with DirectX5/VPE services") },
    { GNR_NOT_ENOUGH_VIDEO_MEMORY, _T("GNR_NOT_ENOUGH_VIDEO_MEMORY") },
    { GNR_NO_OVERLAY_HARDWARE, _T("GNR_NO_OVERLAY_HARDWARE") },
    { GNR_OVERLAY_BUSY, _T("GNR_OVERLAY_BUSY") },
    { GNR_PRIMARY_NOT_AVAILABLE, _T("GNR_PRIMARY_NOT_AVAILABLE") },
    { GNR_VENC_PARAMS, _T("illegal parameters") },
    { GNR_MACROVISION_NOT_SUPPORTED, _T("GNR_MACROVISION_NOT_SUPPORTED") },
    { GNR_CC_NOT_SUPPORTED, _T("GNR_CC_NOT_SUPPORTED") },
    { GNR_DISPLAY_MODE_NOT_SUPPORTED, _T("The display mode could not be set") },
    { GNR_AUDIO_TYPE_NOT_SUPPORTED, _T("The audio format is not supported") },
    { GNR_NO_SPDIF_HARDWARE, _T("We do not SPDIF hardware") },
    { GNR_FRAME_ADVANCE_BOUNDARY_REACHED, _T("GNR_FRAME_ADVANCE_BOUNDARY_REACHED") },
    { GNR_CSS_NOT_SUPPORTED, _T("GNR_CSS_NOT_SUPPORTED") },
    { GNR_AUTHENTICATION_FAILED, _T("GNR_AUTHENTICATION_FAILED") },
    { GNR_DRIVE_FAILURE, _T("General drive failure") },
    { GNR_NO_VALID_DISK, _T("No valid/known disk was found") },
    { GNR_NO_DRIVE, _T("Drive does not exist/could not be found") },
    { GNR_DRIVE_DETACHED, _T("The drive has been detached -> is not available for playback") },
    { GNR_NO_DVD_DRIVE, _T("The drive is not a DVD drive (used on PCs)") },
    { GNR_INVALID_DRIVE_LETTER, _T("The drive letter specified was not correct/out of range") },
    { GNR_DISK_READ_ONLY, _T("The disk is read-only") },
    { GNR_DRIVE_LOCK_FAILED, _T("Unable to lock/unlock the drive") },
    { GNR_DRIVE_ALREADY_LOCKED, _T("The drive was already locked") },
    { GNR_DRIVE_LOAD_FAILED, _T("Could not load/unload the drive") },
    { GNR_DRIVE_NOT_LOADABLE, _T("Drive does not support loading/unloading the disk") },
    { GNR_COPY_PROTECTION_VIOLATION, _T("GNR_COPY_PROTECTION_VIOLATION") },
    { GNR_COPY_PROTECTION_FAILED, _T("GNR_COPY_PROTECTION_FAILED") },
//  { GNR_PATH_NOT_FOUND, _T("The path specified was not found") },
//  { GNR_INVALID_PATH, _T("The path specified was invalid (e.g. contained invalid letters)") },
//  { GNR_FILE_READ_ONLY, _T("GNR_FILE_READ_ONLY") },
    { GNR_UNEXPECTED_NAVIGATION_ERROR, _T("Some internal error happened") },
    { GNR_INVALID_PLAYER, _T("The player handle is invalid") },
    { GNR_INVALID_NAV_INFO, _T("The navigation information is invalid (possible causes: wrong authoring, read error from drive, internal error)") },
    { GNR_LOST_DECODER, _T("The decoder has been preempted") },
    { GNR_OPERATION_NOT_SUPPORTED, _T("This operation is (currently) not supported by the player") },
    { GNR_TITLE_NOT_FOUND, _T("The title specified does not exist") },
    { GNR_CHAPTER_NOT_FOUND, _T("The chapter specified does not exist") },
    { GNR_TIME_OUT_OF_RANGE, _T("The time specified (e.g. for a search operation) is out of range") },
    { GNR_PROGRAM_LINK_NOT_FOUND, _T("The current program does not have a next/prev/upper/etc. program") },
    { GNR_MENU_DOES_NOT_EXIST, _T("The requested menu does not exist") },
    { GNR_STREAM_NOT_FOUND, _T("The stream (audio/subpicture) does not exist") },
    { GNR_FLAGS_NOT_SUPPORTED, _T("The flags specified for this operation are not supported") },
    { GNR_BUTTON_NOT_FOUND, _T("The specified button does not exist") },
    { GNR_UNKNOWN_EVENT_TYPE, _T("The event type specified does not exist") },
    { GNR_BREAKPOINT_NOT_FOUND, _T("The breakpoint to be deleted does not exist") },
    { GNR_INVALID_UNIQUE_KEY, _T("The unique key could not be created/was invalid") },
    { GNR_UOP_PROHIBITED, _T("The current UOPs do not allow this action") },
    { GNR_REGIONS_DONT_MATCH, _T("The system and disk regions don't match") },
    { GNR_REGION_CODE_INVALID, _T("The region code specified was invalid (e.g. multiregion when trying to set region)") },
    { GNR_NO_MORE_REGION_SETS, _T("The number of region changes is exhausted") },
    { GNR_REGION_WRITE_ERROR, _T("Unable to write region") },
    { GNR_REGION_READ_ERROR, _T("Unable to read region") },
    { GNR_PARENTAL_LEVEL_TOO_LOW, _T("The parental level currently set is too low to play the disc") },
    { GNR_ILLEGAL_NAVIGATION_COMMAND, _T("The DVD contains an illegal navigation command") },
    { GNR_ILLEGAL_DOMAIN, _T("The domain is illegal for this operation") },
    { GNR_NO_RESUME_INFORMATION, _T("GNR_NO_RESUME_INFORMATION") },
};

static const TCHAR* DVDErrorText(Error error)
{
    for (int i = 0; i < countof (rgerror); i += 1)
    {
        if (rgerror[i].m_error == error)
        {
            return rgerror[i].m_sz;
        }
    }

    return NULL;
}

static void LogDvdError(const TCHAR* szFile, int nLine, Error error)
{
    const TCHAR* szError = DVDErrorText(error);
    if (szError == NULL)
        Trace(_T("\001%s(%d): DVD Error: unknown error code 0x%08x\n"), szFile, nLine, error);
    else
        Trace(_T("\001%s(%d): DVD Error: %s\n"), szFile, nLine, szError);
}

#endif

void CDVDPlayer::ProcessEvents()
{
    if (m_hPlayer == NULL)
        return;

    DWORD event, info;

    while (GetNextEvent(event, info))
    {
        switch (event)
        {
        default:
            TRACE(_T("DVD event: Unknown Event 0x%x\n"), info);
            break;

        case DNE_NONE:
            TRACE(_T("DVD event: none\n"));
            break;

        case DNE_TITLE_CHANGE:
            TRACE(_T("DVD event: Title Change %d\n"), info);
/*
            m_title = (int)info;
            CallFunction(this, _T("OnTitleChange"));
*/
            break;

        case DNE_PART_OF_TITLE_CHANGE:
            TRACE(_T("DVD event: Part Of Title Change: %d\n"), info);
/*
            m_chapter = (int)info;
            CallFunction(this, _T("OnChapterChange"));
*/
            break;

        case DNE_VALID_UOP_CHANGE:
#ifdef _DEBUG
            TRACE(_T("DVD event: Valid UOP Change 0x%08x\n"), info);
            DumpUOP(info);
#endif
            CallFunction(this, _T("OnValidUOPChange"));
            break;

        case DNE_ANGLE_CHANGE:
            TRACE(_T("DVD event: Angle Change: %d\n"), info);
/*
            m_angle = (int)info;
            CallFunction(this, _T("OnAngleChange"));
*/
            break;

        case DNE_AUDIO_STREAM_CHANGE:
            TRACE(_T("DVD event: Audio Stream Change: %d\n"), info);
/*
            {
                m_audioStream = info;
                OnAudioStreamChange();
            }
*/
            break;

        case DNE_SUBPICTURE_STREAM_CHANGE:
            TRACE(_T("DVD event: Subpicture Stream Change: %d\n"), info);
/*
            {
                int subTitle = 0;
                if (DDP_IsCurrentSubPictureEnabled(m_hPlayer))
                    subTitle = DDP_GetCurrentSubPictureStream(m_hPlayer) + 1;

                if (m_subTitle != subTitle)
                {
                    m_subTitle = subTitle;
                    OnSubPictureStreamChange();
                }
            }
*/
            break;

        case DNE_PARENTAL_LEVEL_CHANGE:
            TRACE(_T("DVD event: Parental Level Change: %d\n"), info);
            if ((int)info > m_parentalLevel)
            {
                m_parentalTooLow = true;
            }
            break;

        case DNE_BITRATE_CHANGE:
            TRACE(_T("DVD event: Bitrate Change: %d\n"), info);
            break;

        case DNE_DOMAIN_CHANGE:

#ifdef _DEBUG
            TRACE(_T("DVD event: Domain Change: %s\n"), rgszDomain[info]);
#endif
/*
            m_domain = (DVDDomain)info;
            CallFunction(this, _T("OnDomainChange"));
*/
            break;

        case DNE_STILL_ON:
            TRACE(_T("DVD event: Still On: %d\n"), info);
            break;

        case DNE_STILL_OFF:
            TRACE(_T("DVD event: Still Off: %d\n"), info);
            break;

        case DNE_PLAYBACK_MODE_CHANGE:
#ifdef _DEBUG
            TRACE(_T("DVD event: Playback Mode Change: %s\n"), rgszPlayerMode[info]);
#endif
/*
            m_playbackMode = (DVDPlayerMode)info;

            // Five minutes of pausing results in a stop...
            if (m_playbackMode == DPM_PAUSED)
                m_timeToStop = XAppGetNow() + 5.0f * 60.0f;
            else
                m_timeToStop = 0.0f;

            CallFunction(this, _T("OnPlaybackModeChange"));
*/
            break;

//      case DNE_CDA_PLAYING_GAP:
//          TRACE(_T("DVD event: Playing Gap 0x%x\n"), info);
//          break;

        case DNE_READ_ERROR:
            TRACE(_T("DVD event: Read Error 0x%x\n"), info);
/*
            // REVIEW: Should we retry here?
            cleanup();
            CallFunction(this, _T("OnErrorStop"));
*/
            break;

        case DNE_DISPLAY_MODE_CHANGE:
            TRACE(_T("DVD event: Display Mode Change: 0x%x\n"), info);
            AvSendTVEncoderOption(0, AV_OPTION_WIDESCREEN, info == DPM_16BY9 ? TRUE : FALSE, NULL);
            break;

        case DNE_STREAMS_CHANGE:
            TRACE(_T("DVD event: Streams Change 0x%x\n"), info);
            break;

        case DNE_SCAN_SPEED_CHANGE:
            TRACE(_T("DVD event: Scan Speed Change: %d\n"), info);
            break;

        case DNE_PLAYBACK_SPEED_CHANGE:
            TRACE(_T("DVD event: Playback Speed Change: %d\n"), info);
            break;

        case DNE_VIDEO_STANDARD_CHANGE:
#ifdef _DEBUG
            TRACE(_T("DVD event: Video Change: %s\n"), rgszVideoStandard[info]);
#endif
            break;

        case DNE_BREAKPOINT_REACHED:
            TRACE(_T("DVD event: Breakpoint Reached 0x%x\n"), info);
            break;

        case DNE_DRIVE_DETACHED:
            TRACE(_T("DVD event: Drive Detached 0x%x\n"), info);
            break;

        case DNE_ERROR:
            TRACE(_T("DVD event: Error 0x%x\n"), info);
            if (info == GNR_PARENTAL_LEVEL_TOO_LOW)
            {
                m_parentalTooLow = true;
            }
/*
            // REVIEW: Should we retry here?
            cleanup();
            CallFunction(this, _T("OnErrorStop"));
*/
            break;
        }
    }
}


Error CDVDPlayer::SetBookmark(CDVDBookmark* pBookmark)
{
    Error errRet = GNR_OK;

    //
    // Player initialized?
    //

    if (m_hPlayer != NULL)
    {
        //
        // Reset bookmark and make first attempt to set the bookmark. If we don't have
        // enough buffer, the call should fail returning the required buffer size.
        //

        pBookmark->Reset();

        DWORD cbSize = pBookmark->GetSize();

        errRet = DDP_Freeze(m_hPlayer, pBookmark->GetBuffer(), cbSize);

        if(IS_GNERROR(errRet))
        {
            //
            // Adjust buffer size to value returned by DDP_Freeze
            // and try again
            //

            if(pBookmark->Alloc(cbSize))
            {
                VERIFYDVD(errRet = DDP_Freeze(m_hPlayer, pBookmark->GetBuffer(), cbSize));
            }
            else
            {
                errRet = GNR_NOT_ENOUGH_MEMORY;
            }
        }

        //
        // If one of our two attempts succeeded, mark the bookmark as
        // successfully set.
        //

        if(! IS_GNERROR(errRet))
        {
            pBookmark->Set();
        }
    }
    else
    {
        //
        // Error, player not initialized
        //

        errRet = GNR_ERROR;
    }

    return errRet;
}

Error CDVDPlayer::GotoBookmark(CDVDBookmark* pBookmark)
{
    Error errRet = GNR_OK;

    //
    // Player initialized?
    //

    if (m_hPlayer != NULL)
    {
        if(pBookmark->IsSet())
        {

            //
            // Activate bookmark
            //

            DWORD dwSize = pBookmark->GetSize();
            VERIFYDVD(errRet = DDP_Defrost(m_hPlayer, pBookmark->GetBuffer(), dwSize));

            if(!IS_GNERROR(errRet))
                {
                m_bScanBackward = false;
                m_scanSpeed = 1;
                m_scanSlow = false;
                }
        }
        else
        {
            errRet = GNR_OBJECT_INVALID;
        }
    }
    else
    {
        //
        // Error, player not initialized
        //

        errRet = GNR_ERROR;
    }

    return errRet;
}

void CDVDPlayer::enableWideScreen()
{
    AvSendTVEncoderOption(0, AV_OPTION_WIDESCREEN, TRUE, NULL);
}

void CDVDPlayer::disableWideScreen()
{
    AvSendTVEncoderOption(0, AV_OPTION_WIDESCREEN, FALSE, NULL);
}


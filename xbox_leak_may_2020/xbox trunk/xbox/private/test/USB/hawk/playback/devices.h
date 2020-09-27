/*++
    The CHawkUnit class manages a single hawk unit.
    It records from the microphone, and then plays
    back the data with a 2 second delay through the headphone
    speaker.

    Both the headphone and microphone XMO's need to have
    a max attach buffer of at least two so that the always
    have an additional buffer queued.  The simplest scheme
    would have 4 XMEDIAPACKET contexts so that both the headphone
    and the microphone have two buffers queued at all times.
    This is a little wasteful.  Staggering can get away
    with only three buffers.

    So we could have three XMEDIAPACKET contexts to keep both XMO's
    from starving.  Say we have context permanently mapped to a
    fixed size buffer.  We would then attach the first buffer
    completed by the microphone to the headphone, 1 1/2
    buffer times after the microphone completes.  The playback
    latency would then be 1 1/2 buffer times.  So for a 2 
    second latency each buffer is 1 1/3 seconds for a total
    4 seconds of buffers.  This seems somewhat wasteful, the total
    buffer needed in theory should be very close to two seconds.

    A better way is to allocate a single large buffer, and have
    the XMEDIAPACKET contexts map a smaller region of it.  The stagger
    delay is then some multiple of the smaller region time.  If we
    want to use only three contexts, we need to delay an additional half
    of the small region time.  This is a pain to do cleanly assuming
    that your app is doing something besides streaming this one device
    (infact this app can play this trick with four hawks at a time.)
    However, since the context of a buffer (at least the part this
    class keeps track of) is only two DWORD, it makes very little sense
    to skimp, and we just go with four contexts.

    How big should the small region be?  The hawk XMO's run most
    efficiently if the small regions are a multiple of 8 USB frames
    (though this is not a requirement).  This will result in the fewest
    number of interrupts per bytes transferred.  Everytime one of these
    small buffers complete an event is signalled.  Presumably, we want
    to minimize this as well, so it is a good idea to make them as large
    as possible.  However, if the latency is N*the number of small regions,
    the full buffer must be (N+2)*smaller region.  So if you break the large
    region into 12 regions, you are using about 20% more buffer than the
    theoretical minimum.  22 regions wastes only 10%, etc.

    So our choice is to have 2080 ms of buffers, with each small region
    being only 40 ms, giving us a total of 52 regions.  The buffer is
    only 4% larger than it needs to be for a two second delay.

--*/

#define XHAWK_PLAYBACK_BYTES_PER_MS (((ULONG)(TEST_HAWK_SAMPLING_RATE/1000.0))*2)
#define XHAWK_PLAYBACK_BUFFER_REGION_TIME 80
#define XHAWK_PLAYBACK_BUFFER_REGION_SIZE (XHAWK_PLAYBACK_BYTES_PER_MS*XHAWK_PLAYBACK_BUFFER_REGION_TIME)
#define XHAWK_PLAYBACK_PREBUFFER_REGIONS 20
#define XHAWK_PLAYBACK_TOTAL_REGIONS (XHAWK_PLAYBACK_PREBUFFER_REGIONS+2)
#define XHAWK_PLAYBACK_BUFFER_SIZE (XHAWK_PLAYBACK_BUFFER_REGION_SIZE*XHAWK_PLAYBACK_TOTAL_REGIONS)

struct PLAYBACK_MEDIABUFFER_CONTEXT
{
    DWORD        dwCompletedSize;
    DWORD        dwStatus;
};

struct PLAYBACK_REGION
{
    BYTE Data[XHAWK_PLAYBACK_BUFFER_REGION_SIZE];
};

class CHawkUnit
{
  public:

      CHawkUnit() :
        m_pMicrophone(NULL),
        m_pHeadphone(NULL),
        m_dwPortNumber(0),
        m_iPosX(0),
        m_iPosY(0),
        m_hEvent(NULL),
        m_dwMicrophoneRegionIndex(0),
        m_dwHeadphoneRegionIndex(0),
        m_dwMicrophoneContextIndex(0),
        m_dwHeadphoneContextIndex(0),
        m_fConnected(FALSE),
        m_fPreBuffering(FALSE),
        m_dwPreBufferRegions(0),
        m_dwBuffersReceived(0),
        m_dwBytesReceived(0),
        m_dwBuffersSent(0),
        m_dwBytesSent(0),
        m_dwErrorCount(0)
        {}

    ~CHawkUnit();
    void Init(DWORD dwPort, INT iPosX, INT iPosY, HANDLE hEvent);
    void Inserted();
    void Removed();
    void Process();
    void AttachMicrophoneContext(DWORD dwIndex);
    void AttachHeadphoneContext(DWORD dwIndex);
    void Draw(CDraw& draw);
    void Log(WCHAR *pwstrText);
  
  private:

    //
    // XMO objects created on
    // call to Inserted.
    //
    
    XMediaObject *m_pMicrophone;
    XMediaObject *m_pHeadphone;

    //
    // Stuff given to us at init
    //
    DWORD m_dwPortNumber;
    INT m_iPosX;
    INT m_iPosY;
    HANDLE m_hEvent;
        
    //
    //  The buffer is just an array of regions,
    //  which are simply fixed sized arrays
    //  of bytes.
    //

    PLAYBACK_REGION m_Buffer[XHAWK_PLAYBACK_TOTAL_REGIONS];

    //
    //  m_dwMicrophoneRegionIndex (m_dwHeadphoneRegionIndex)
    //  is the next next region that needs to be attached to
    //  the microphone (headphone) XMO.
    //

    DWORD m_dwMicrophoneRegionIndex;
    DWORD m_dwHeadphoneRegionIndex;

    //
    //  These contexts are used to track the status
    //  of an attached media buffer.
    //
    PLAYBACK_MEDIABUFFER_CONTEXT m_MicrophoneContexts[2];
    PLAYBACK_MEDIABUFFER_CONTEXT m_HeadphoneContexts[2];
    
    //
    //  m_dwMicrophoneContextIndex (m_dwHeadphoneContexIndex)
    //  is the next next region that the microphone (headphone) XMO,
    //  is expected to complete.
    //
    DWORD m_dwMicrophoneContextIndex;
    DWORD m_dwHeadphoneContextIndex;

    //
    //  Status.
    //
    BOOL   m_fConnected;
    BOOL   m_fPreBuffering;
    DWORD  m_dwPreBufferRegions;
    DWORD  m_dwBuffersReceived;
    DWORD  m_dwBytesReceived;
    DWORD  m_dwBuffersSent;
    DWORD  m_dwBytesSent;
    DWORD  m_dwErrorCount;
    
};

class CHawkUnits
{
    public:
        CHawkUnits();
        ~CHawkUnits();
        void Run(CDraw& draw);
    private:
        CHawkUnit m_pHawkUnits[4];
        HANDLE m_pProcessEvents[4];
};


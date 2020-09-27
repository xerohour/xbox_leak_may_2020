

//
//  The buffer size should not be divisible by 8 USB frames.
//  Depending on the hardware, it is either 32 bytes or 48
//  bytes.  So it is either (29*3) ms or (29*2) ms of data.
//
#define XHAWK_TEST_BUFFER_SIZE (16*2*3*29)
#define XHAWK_TEST_BUFFER_COUNT 3

struct HAWKMEDIABUFFER
{
    XMEDIAPACKET xMediaPacket;
    DWORD        dwCompletedSize;
    DWORD        dwStatus;
};

class CHawkUnit
{
  public:

    CHawkUnit();
    ~CHawkUnit();
    void Init(DWORD dwPort, INT iPosX, INT iPosY, HANDLE hEvent);
    void Inserted();
    void Removed();
    void Process();
    void Draw(CDraw& draw);
    void Log(WCHAR *pwstrText);
  
  private:

    XMediaObject *m_pMicrophone;
    XMediaObject *m_pHeadphone;
    DWORD m_dwPortNumber;
    INT m_iPosX;
    INT m_iPosY;
    BOOL m_fConnected;
    BOOL m_fStarted;
    
    //
    //  Buffers for test.
    //

    HAWKMEDIABUFFER HeadphoneBuffers[XHAWK_TEST_BUFFER_COUNT];
    HAWKMEDIABUFFER MicrophoneBuffers[XHAWK_TEST_BUFFER_COUNT];

    //
    //  Progress Stats.
    //

    DWORD  m_dwPatternPosition;
    DWORD  m_dwBuffersSent;
    DWORD  m_dwBuffersReceived;
    DWORD  m_dwLoopbackErrors;

    //
    //  Keeps track of the next buffer that is 
    //  expected to complete.
    //

    ULONG m_NextMicrophoneBufferIndex;
    ULONG m_NextHeadphoneBufferIndex;

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


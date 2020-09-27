#define PINK_NOISE_ROWS 16
#define PINK_MAX 0x5555		// 1/3 of max for IEC 60065

class PinkNumber {
	
	private:
		WORD m_Rows[PINK_NOISE_ROWS];
		WORD m_RunningSum;
		WORD m_Index;
		WORD m_IndexMask;

	public: 
		PinkNumber();
		~PinkNumber();
		WORD GetNextValue();
};

//
//  The buffer size should not be divisible by 8 USB frames.
//  Depending on the hardware, it is either 32 bytes or 48
//  bytes.  So it is either (29*3) ms or (29*2) ms of data.
//
#define XHAWK_VOICE_SAMPLING_RATE 8000
#define XHAWK_TEST_BUFFER_SIZE (16*2*3*29)		// 16 bytes/ms * time
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
    void Init(DWORD dwPort, HANDLE hEvent);
    void Inserted(HANDLE LogHandle);
    void Removed();
    void Process();
    // void Draw();
    void Log(WCHAR *pwstrText);
	BOOL IsConnected();
	void StartStreaming();
  
  private:
	HANDLE m_LogHandle;
    XMediaObject *m_pMicrophone;
    XMediaObject *m_pHeadphone;
    DWORD m_dwPortNumber;
    BOOL m_fConnected;
    BOOL m_fStarted;
    
    //
    //  Buffers for test.
    //
    HAWKMEDIABUFFER HeadphoneBuffers[XHAWK_TEST_BUFFER_COUNT];
    HAWKMEDIABUFFER MicrophoneBuffers[XHAWK_TEST_BUFFER_COUNT];

	//
	// Pink Noise
	//
	PinkNumber m_PinkNoise;

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
        void Run(HANDLE LogHandle);
    private:
        CHawkUnit m_pHawkUnits[4];
        HANDLE m_pProcessEvents[4];
};

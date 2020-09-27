
typedef struct _GFXTEST_RESULTS {
    HRESULT hr;
    UINT    uFrame;
    DWORD   dwCRCResult[3];
    DWORD   dwCRCExpected[3];
    bool    bHardwareChecksum;
} GFXTEST_RESULTS, *PGFXTEST_RESULTS;

extern "C" 
{
	HRESULT Grafx_StartupAnimation_TestMain(PGFXTEST_RESULTS pgtr);
}

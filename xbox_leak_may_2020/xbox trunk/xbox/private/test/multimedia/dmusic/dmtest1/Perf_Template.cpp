/********************************************************************************
Function template used by performance tests.

PARAMETERS:
lTime: 
    PERF_ONEITERATION:  



********************************************************************************/
#define PERF_ONEITERATION (-696969)


HRESULT PerformanceTemplate(LONG lTime,
                            LONG *lTimeSpent,
                            DWORD *dwIterations
                            )
{
//LOCAL VARIABLES
HRESULT hr = S_OK;
LARGE_INTEGER liStartTime  = 0;
LARGE_INTEGER liFinishTime = 0;
DWORD dwIterations = ;
DWORD dwCount;


//INITIALIZATION CODE
//-------------------





//TIMING CODE
//-----------
dwCount = 0;
QueryPerformanceCounter(&liStartTime);
while (





//RELEASE Code


};
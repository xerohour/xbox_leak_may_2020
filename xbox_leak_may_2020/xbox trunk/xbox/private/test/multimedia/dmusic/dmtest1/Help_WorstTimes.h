#include "globals.h"

//Basically a high score table of the longest tests to run.

HRESULT InitializeWorstTimes(void);
HRESULT UnInitializeWorstTimes(void);
HRESULT ProcessTime(DWORD dwTime, LPCSTR szTestName);
HRESULT PrintTimes(void);

// XTV.h: interface for the CXModuleTV class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_XTV_H__68FC7951_E13F_4E1F_925A_2F29AE3E15A0__INCLUDED_)
#define AFX_XTV_H__68FC7951_E13F_4E1F_925A_2F29AE3E15A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <xgraphics.h>
#include "..\testobj.h"

#define XTV_TIMEOUT	180
#define NUMBER_OF_COLOR_BARS 8
#define NUMBER_OF_FREQ	6
#define MAX_ELEM	16
#define NumFreq 8
#define AMP             255.0/(714.5-53.6)
#define NTSC_COL_MODE1    640
#define NTSC_ROW_MODE1    480
#define NTSC_COL_MODE3    720
#define NTSC_ROW_MODE3    480
#define PAL_COL     640   
#define PAL_ROW	    480
#define PAL_COL_MODE3	  720
#define PAL_ROW_MODE3     480
#define PAL_COL_MODE4	640
#define PAL_ROW_MODE4	576
#define PAL_COL_MODE6	720
#define PAL_ROW_MODE6	576
#define MAXATTENU		-26
#define NTSC_Pixel_Clock     12.2727
#define PAL_Pixel_Clock      12.29   
#define BUFSIZE 2500
#define DEFAULTPARAM 5
#define PHASEDEFAULTPARAM 3
#define ZEROPARAM 0
#define RED   0
#define GREEN 1
#define BLUE  2
#define NTSC_M 1
#define H_PATTERN 0
#define V_PATTERN 1


typedef enum{
	xbWhite=0,
	xbYellow,
	xbCyan,
	xbGreen,
	xbMagenta,
	xbRed,
	xbBlue,
	xbBlack,
	NUMBER_OF_CBARS
}COLORBARS;




typedef struct {
	double dbLumaWhiteIRE;  
	double dbLumaYellowIRE;  
	double dbLumaCyanIRE; 
	double dbLumaGreenIRE;  
	double dbLumaMagentaIRE;  
	double dbLumaRedIRE; 
	double dbLumaBlueIRE;  
	double dbLumaBlackIRE;  
} STD_NTSC_CB_LUMA_IRE;

typedef struct {
	double dbChromaWhiteIRE;  
	double dbChromaYellowIRE;  
	double dbChromaCyanIRE; 
	double dbChromaGreenIRE;  
	double dbChromaMagentaIRE;  
	double dbChromaRedIRE; 
	double dbChromaBlueIRE;  
	double dbChromaBlackIRE;  
} STD_NTSC_CB_CHROMA_IRE;

typedef struct {
//	double dbChromaWhitePhase;  
	double dbChromaYellowPhase;  
	double dbChromaCyanPhase; 
	double dbChromaGreenPhase;  
	double dbChromaMagentaPhase;  
	double dbChromaRedPhase; 
	double dbChromaBluePhase;  
//	double dbChromaBlackPhase;  
} STD_NTSC_CB_CHROMA_PHASE;



typedef struct{
	DWORD	dwNumElements;
	double	dblLumaIRE;
	double  dblChromaIRE;
	double  dblChromaPhase;
}NTSCM_COMP_CB_DATA;

typedef struct{
	DWORD	dwNumElements;
	double	dblLumaIRE;
	double  dblChromaIRE;
	double  dblChromaPhase;
}NTSCM_SV_CB_DATA;


typedef struct{
	DWORD	dwNumElements;
	double	dbFreqResponse;
}MULTIBURST_CONFIG_DATA;


typedef struct {

	LPCTSTR pwszCXTVOutDevName;
	DWORD	dwNumberOfCbs;
	DWORD	dwUpperLimitLumaIRE;
	DWORD	dwUpperLimitChromaIRE;
	DWORD	dwUpperLimitChromaPhase;
	DWORD	dwLowerLimitLumaIRE;
	DWORD	dwLowerLimitChromaIRE;
	DWORD	dwLowerLimitChromaPhase;



// ntsc std+upper and std-lower
	DWORD	dwLumaIREUpperLimit;
	DWORD	dwLumaIRELowerLimit;
	DWORD   dwJapanLumaUpper;
	DWORD   dwJapanLumaLower;
	DWORD	dwChromaIREUpperLimit;
	DWORD	dwChromaIRELowerLimit;
	DWORD	dwChromaPhaseUpperLimit;
	DWORD	dwChromaPhaseLowerLimit;

//Pal std+upper and std-lower

	DWORD	dwPalCbLumaIREUpperLimit;
	DWORD	dwPalCbLumaIRELowerLimit;
	DWORD	dwPalCbChromaIREUpperLimit;
	DWORD	dwPalCbChromaIRELowerLimit;
	DWORD	dwPalCbChromaPhaseUpperLimit;
	DWORD	dwPalCbChromaPhaseLowerLimit;

	

} XTV_CB_COFIG_ATTR;


typedef struct {

//std ntsccb parameters
	DWORD   dwStdNtscCbParamLumaIRE;
	DWORD	dwStdNtscCbParamChromaIRE;
	DWORD	dwStdNtscCbParamChromaPhase;
	DWORD	dwStdNtscJapanCbParamLumaIRE;
	
}XTV_STD_NTSC_PARAMS;

/*
typedef struct {

//std ntsccb parameters
	DWORD   dwStdNtscjCbParamLumaIRE;
	DWORD	dwStdNtscjCbParamChromaIRE;
	DWORD	dwStdNtscjCbParamChromaPhase;
}XTV_STD_NTSCJ_PARAMS;*/


typedef struct {

	DWORD   dwStdPalCbParamLumaIRE;
	DWORD	dwStdPalCbParamChromaIRE;
	DWORD	dwStdPalCbParamChromaPhase;
}XTV_STD_PAL_PARAMS;

typedef struct {
int NtscCompLowerLimit;
int  NtscCompUpperLimit;
//int NtscJapanCompUpperLimit;
//int NtscJapanCompLowerLimit;
int PalCompLowerLimit;
int PalCompUpperLimit;
int NtscSvLowerLimit;
int  NtscSvUpperLimit;
int PalSvLowerLimit;
int PalSvUpperLimit;


} XTV_MB_CONFIG_ATTR;


struct FreqElement {
    double freq;          // Hz
    double start_time;    // micro second
    double end_time;
    double amp;           // mv
    double offset;
};

//amp=(500-71.43)/2=214.285=214.3mv
//tektronix

static struct FreqElement NTSC_mulit[]={
   {      0.0,   9.5, 12.7,    0.0,  500.0},
   {      0.0,  12.7, 15.7,    0.0,   71.4},
  // {	  0.0,  15.7, 17.2,    0.0,   285.7},
   { 500000.0,  17.2, 23.2,  214.3,  285.7},
   {1000000.0,  24.6, 30.6,  214.3,  285.7},
   {2000000.0,  32.0, 38.0,  214.3,  285.7},
   {3000000.0,  39.4, 45.4,  214.3,  285.7},
   {3580000.0,  46.8, 52.8,  214.3,  285.7},
  //{0.0,        52.8, 53.98,   0.0,  285.7},
   {4200000.0,  54.2, 60.2,  214.3,  285.7},
   {0.0,        60.2, 61.98,   0.0,  285.7},
};

static struct FreqElement NTSCMode3_mulit[]={
   {      0.0,   10.7, 14.3,    0.0,  500.0},
   {      0.0,  14.3, 16.9,    0.0,   71.4},
   { 500000.0,  19.4, 26.1,  214.3,  285.7},
   {1000000.0,  27.7, 34.4,  214.3,  285.7},
   {2000000.0,  36.0, 42.8,  214.3,  285.7},
   {3000000.0,  44.3, 51.1,  214.3,  285.7},
   {3580000.0,  52.7, 59.4,  214.3,  285.7},
   {4200000.0,  60.9, 67.7,  214.3,  285.7},
   {0.0,        67.7, 69.8,   0.0,  285.7},
};

/*
//bitmap ntsc

static struct FreqElement NTSC_mulit[]={
   {      0.0,   9.4, 13.5,    0.0,  500.0},
   {      0.0,  13.5, 17.5,    0.0,   71.4},
   { 500000.0,  19.3, 25.5,  214.3,  285.7},
   {1000000.0,  27.0, 32.5,  214.3,  285.7},
   {2000000.0,  34.0, 39.5,  214.3,  285.7},
   {3000000.0,  41.0, 46.5,  214.3,  285.7},
   {3580000.0,  48.2, 54.2,  214.3,  285.7},
   {4200000.0,  56.2, 60.2,  214.3,  285.7},
   {	0.0,    60.2, 62.0,    0.0, 285.7},
};
*/



/* tektronix
static struct FreqElement PAL_mulit[]={
   {      0.0,  12.0, 16.0,    0.0,  560.0},
   {      0.0,  16.0, 20.0,    0.0,  140.0},
   {	  0.0,  20.0, 23.6,    0.0,  350.0},
   { 500000.0,  23.6, 29.1,  280.0,  350.0},
   {1000000.0,  29.6, 35.1,  280.0,  350.0},
   {2000000.0,  35.6, 41.1,  280.0,  350.0},
   {3000000.0,  41.6, 47.1,  280.0,  350.0},
   {4000000.0,  47.6, 53.1,  280.0,  350.0},
   {5000000.0,  53.6, 59.1,  280.0,  350.0},
};
*/

// bitmap: 640x480

static struct FreqElement PAL_mulit[]={
   {      0.0,  9.5,  13.2,    0.0,  560.0},
   {      0.0,  13.2, 17.5,    0.0,  140.0},
   { 500000.0,  19.5, 25.5,  280.0,  350.0},
   {1000000.0,  27.0, 32.5,  280.0,  350.0},
   {2000000.0,  34.0, 39.6,  280.0,  350.0},
   {4000000.0,  41.2, 46.6,  280.0,  350.0},
   {4800000.0,  48.5, 54.1,  280.0,  350.0},
   {5800000.0,  55.8, 61.1,  280.0,  350.0},
   {0.0,        61.1, 62.0,     0.0, 350.0},
};

//bitmap: Mode3:720x480

static struct FreqElement PALMode3_mulit[]={
   {      0.0,  10.7, 14.9,    0.0,  560.0},
   {      0.0,  14.9, 19.6,    0.0,  140.0},
   { 500000.0,  21.9, 28.7,  280.0,  350.0},
   {1000000.0,  30.4, 36.5,  280.0,  350.0},
   {2000000.0,  38.3, 44.6,  280.0,  350.0},
   {4000000.0,  46.4, 52.4,  280.0,  350.0},
   {4800000.0,  54.6, 60.7,  280.0,  350.0},
   {5800000.0,  62.8, 68.7,  280.0,  350.0},
   {0.0,        68.7, 69.8,     0.0, 350.0},
};
//Mode6: 720x576

static struct FreqElement PALMode6_mulit[]={
   {      0.0,  10.7, 14.9,    0.0,  560.0},
   {      0.0,  14.9, 19.6,    0.0,  140.0},
   { 500000.0,  21.9, 28.7,  280.0,  350.0},
   {1000000.0,  30.4, 36.5,  280.0,  350.0},
   {2000000.0,  38.3, 44.6,  280.0,  350.0},
   {4000000.0,  46.4, 52.4,  280.0,  350.0},
   {4800000.0,  54.6, 60.7,  280.0,  350.0},
   {5800000.0,  62.8, 68.7,  280.0,  350.0},
   {0.0,        68.7, 69.8,     0.0, 350.0},
};


// For deleting and releasing objects
#define SAFE_DELETE(p)       { delete (p);     (p)=NULL; }
#define SAFE_DELETE_ARRAY(p) { delete[] (p);   (p)=NULL; }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }


class CXModuleTV : public CTestObj  
{
public:
	
	DWORD gdwIgnoreError;
	DECLARE_XMTAMODULE (CXModuleTV, "TV", MODNUM_TV);
	XTV_CB_COFIG_ATTR	*gpXboxCbConfigAttr;
	XTV_CB_COFIG_ATTR	gdwColorBarLimitLumaIRE[NUMBER_OF_COLOR_BARS];
	XTV_CB_COFIG_ATTR	gdwColorBarLimitChromaIRE[NUMBER_OF_COLOR_BARS];
	XTV_CB_COFIG_ATTR	gdwColorBarLimitChromaPhase[NUMBER_OF_COLOR_BARS];
	XTV_CB_COFIG_ATTR   gdwJapanCBLumaIRE[NUMBER_OF_COLOR_BARS];
	XTV_MB_CONFIG_ATTR	dbFreqLimit[NUMBER_OF_FREQ];
	
	XTV_STD_NTSC_PARAMS dbStandardNtscParams[NUMBER_OF_COLOR_BARS];
//	XTV_STD_NTSCJ_PARAMS dbStandardNtscjParams[NUMBER_OF_COLOR_BARS];
	XTV_STD_PAL_PARAMS dbStandardPalParams[NUMBER_OF_COLOR_BARS];

protected:
	BOOL DrawMode6PALMB();
	BOOL DrawMode4PALMB();
	BOOL DrawMode3PALMB();
	BOOL DrawMode3NTSCMB();
	BOOL DrawMode6PALCB();
	BOOL DrawMode4PALCB();
	BOOL DrawMode3PALCB();
	BOOL DrawMode3NTSCCB();
	virtual bool InitializeParameters();
	BOOL DrawPalMB();
	VOID NTSCMBRender();
    VOID PALMBRender();
	BOOL DrawNTSCMB();
	VOID PALCBRender();
	VOID NTSCCBRender();
	BOOL DrawPALCB();
	BOOL DrawNTSCCB();
	BOOL TVTestDrawHorizontalLinearityPattern();
	BOOL TVTestDrawVerticalLinearityPattern();
	VOID TVTestRender(int pattern);
	BOOL FillSurfaceWithHorizontalPattern();
	void FillBuffer(unsigned long *start_address, int count, unsigned long data);
	void FillBuffer(unsigned char *start_address, int count, unsigned long pattern);
	VOID Cleanup();
	int m_testScenes;
	int m_vidMode;
	int m_bpp;
	int m_sectionsToDraw;
	int m_testRetries;
	int m_bUseMemoryFill;
	DWORD m_dwDisplayWidth;
	DWORD m_dwDisplayHeight;
	VOID ReportD3DError(DWORD error);
	LPDIRECT3DVERTEXBUFFER8		m_pVB;
	LPDIRECT3DVERTEXBUFFER8		m_pTVPatternVB;	
	//LPDIRECT3D8					m_pD3D;	
	//D3DPRESENT_PARAMETERS		m_d3dpp;



	double m_LumaIREWhite;
	double m_LumaIREYellow;
	double m_LumaIRECyan;
	double m_LumaIREGreen;
	double m_LumaIREMagenta;
	double m_LumaIRERed;
	double m_LumaIREBlue;
	double m_LumaIREBlack;

	double m_ChromaIREWhite;
	double m_ChromaIREYellow;
	double m_ChromaIRECyan;
	double m_ChromaIREGreen;
	double m_ChromaIREMagenta;
	double m_ChromaIRERed;
	double m_ChromaIREBlue;
	double m_ChromaIREBlack;

	double m_ChromaPhaseWhite;
	double m_ChromaPhaseYellow;
	double m_ChromaPhaseCyan;
	double m_ChromaPhaseGreen;	
	double m_ChromaPhaseMagenta;	
	double m_ChromaPhaseRed;	
	double m_ChromaPhaseBlue;
	double m_ChromaPhaseBlack;

	double m_NtscmFreq1;
	double m_NtscmFreq2;
	double m_NtscmFreq3;
	double m_NtscmFreq4;
	double m_NtscmFreq5;
	double m_NtscmFreq6;
	double m_NtscmFreq7;


	double m_NtscjFreq1;
	double m_NtscjFreq2;
	double m_NtscjFreq3;
	double m_NtscjFreq4;
	double m_NtscjFreq5;
	double m_NtscjFreq6;
	double m_NtscjFreq7;

	double m_PalbFreq1;
	double m_PalbFreq2;
	double m_PalbFreq3;
	double m_PalbFreq4;
	double m_PalbFreq5;
	double m_PalbFreq6;
	double m_PalbFreq7;

	double m_PalmFreq1;
	double m_PalmFreq2;
	double m_PalmFreq3;
	double m_PalmFreq4;
	double m_PalmFreq5;
	double m_PalmFreq6;
	double m_PalmFreq7;


	double m_SecamFreq1;
	double m_SecamFreq2;
	double m_SecamFreq3;
	double m_SecamFreq4;
	double m_SecamFreq5;
	double m_SecamFreq6;
	double m_SecamFreq7;


	bool err_BADPARAMETER (LPCTSTR s1)
	{
		ReportError (0x010, L"Failed to find configuration parameter \"%s\"\nThis configuration parameter doesn't exist or is invalid", s1); 
		return (CheckAbort (HERE));
//return false;
}


	//Default message for general DirectX API error.
	bool err_DIRECTX_API (HRESULT hr, LPCTSTR s1) //Default DirectX API error message
	{
		ReportError (0x011, L"A DirectX API returned error 0x%08lx (%s).", hr, s1 ); 
		return (CheckAbort (HERE));
//		return false;
	}

	/*bool err_BADVIDEOMODE (int mode)
	{
		ReportError (0x013, L"The videomode selected (mode=%d) does not exist.\n Check the 'videomode' parameter in the ini file.", mode);
		return (CheckAbort (HERE));
//		return false;
	}
	bool err_NODIRECT3D ()
	{
		ReportError (0x014, L"Failed to create the Direct3D Device - check for proper DirectX runtime installation."); 
		return (CheckAbort (HERE));
//		return false;
	}*/

	bool err_EchoBufferResponseIsNull ()
	{
		ReportError (0x012, L"The response buffer to a host echo command is empty"); 
		return (CheckAbort (HERE)); 
	}


	bool err_HostResponseError (DWORD dwErrorCodeFromHost, LPCTSTR s1) { if (gdwIgnoreError != TRUE)
	{
		ReportError ((unsigned short)dwErrorCodeFromHost, L"The host responded with the following error message:\n%s", s1);return (CheckAbort (HERE)); } return false;
	}

		bool err_HostCommunicationError (int i1) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x013, L"iSendHost communication routine returned an error code of 0x%x", i1); return (CheckAbort (HERE)); } return false;
	}

	bool err_RecordBufferIsEmpty () { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x014, L"The measure result data buffer from the host is empty"); return (CheckAbort (HERE)); } return false;
	}
	bool err_RecordBufferIsNotCorrectLength (DWORD dw1, DWORD dw2) { if (gdwIgnoreError != TRUE)
	{
		ReportError (0x015, L"The length of the measurement of result data buffer from the host is not correct.\nExpected %ld bytes, received %ld bytes", dw1, dw2); return (CheckAbort (HERE)); } return false;
	}

/*	bool err_BadVideoParam (LPCTSTR s1, DWORD x)
	{
		ReportError (0x019, L"Failed to find configuration parameter \"%s[%lu]\"\nThis configuration parameter doesn't exist or is invalid", s1, x); 
		return (CheckAbort (HERE));
//		return false;
	}*/


	bool err_LumaResultOutOfRange (DWORD i1,double fp,DWORD i2, DWORD i3, DWORD i4)
	{
		ReportError (0x16, L"\nThe measured color bar number(%lu) for Luma IRE  is %.1f out of limit range\nThe Tektronix standard parameter is %lu\nThe upper limits is %lu\nThe lower limit is %lu", i1, fp, i2, i3, i4); 
		return (CheckAbort (HERE));
	//	return false;
	}
//	
	

	bool err_ChromaIREResultOutOfRange  (DWORD i1,double fp,DWORD i2,DWORD i3, DWORD i4)
	{
		ReportError (0x017, L"\nThe measured color bar number(%lu) for  Chroma IRE is %.1f out of limit range\nThe Tektronix standard parameter is %lu\nThe upper limits is %lu\nThe lower limit is %lu", i1, fp, i2, i3, i4); 
		return (CheckAbort (HERE));
//		return false;
	}

	bool err_ChromaPhaseResultOutOfRange (DWORD i1 ,double fp, DWORD i2,DWORD i3, DWORD i4)
	{
		ReportError (0x018, L"\nThe measured color bar number(%lu) for Chroma Phase is %.1f out of limit range\nThe Tektronix standard parameter is %lu\nThe upper limits is %lu\nThe lower limit is %lu", i1,fp, i2, i3, i4); 
		return (CheckAbort (HERE));
//		return false;
	}

bool err_MBurstResultOutOfRange (DWORD i1, double fp,int i2,int i3)
	{
		ReportError (0x019, L"\nThe measured attenuatio of amplitude at frequency packet number(%lu) is %.1f(dB) out of limit range\nThe lower limits is %d(dB)\nThe upper limit is %d(dB)",i1 ,fp, i2, i3); 
		return (CheckAbort (HERE));
//		return false;
	}

bool err_NoConnection()
	{
		ReportError(0x01A, L"\nNo video signal detected or switch video modes may not working properly");
		return (CheckAbort (HERE));
//		return false;
	}

bool err_TVTestLinearity(LPCTSTR tvmode, LPCTSTR pattern, LPCTSTR color, int line, int column, int meas, int exp, int diff, int tol)
	{
		ReportError (0x01B, L"TV Linearity Check Failed: TVMode=%s, Pattern=%s, SampleColor=%s, Line=%d, Column=%d, Meas=%d, Exp=%d, Diff=%d, Tolerance=%d\n", tvmode, pattern, color, line, column, meas, exp, diff, tol); 
		return (CheckAbort (HERE));
//		return false;
	}



};

#endif // !defined(AFX_XTV_H__68FC7951_E13F_4E1F_925A_2F29AE3E15A0__INCLUDED_)

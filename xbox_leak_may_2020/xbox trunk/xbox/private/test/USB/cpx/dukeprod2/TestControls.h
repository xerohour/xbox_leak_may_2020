#ifndef __testcontrols_h_
#define __testcontrols_h_
/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    testcontrols.h

Abstract:

    function declarations, type definitions, includes, etc, for testing peripheral controls

Author:

    Dennis Krueger <a-denkru>   May 2000

Revision History:

--*/

//#include "controls.h"
#include <xtl.h>
#include "base.h"
#include <draw.h>

// controller types
#define STANDARD_TYPE		1
#define ACHIBONO_TYPE		2
#define WHEEL_TYPE			0x10
#define ARCADE_STICK_TYPE	0x20
#define FLIGHT_STICK_TYPE	0x30
#define BOARD_TYPE			0x40




enum  {
	esA = 0,
	esB,
	esX,
	esY,
	esBlack,
	esWhite,
	esLTrigger,
	esRTrigger,
	esLThumb,
	esDPad,
	esBackButton,
	esStartButton,
	esRThumb,
	esMax
};

// test state flags
enum {
	eNotyet,
	eTesting,
	eGood,
	eBad,
	eNever
};

// Controller

class CTestController;


// Base Control class
class CTestControl
{
public:
	CTestControl(CTestController * pController) { m_pTestController = pController;};
	virtual ~CTestControl() { ; } ;
	void SetStart(CPoint Start) { m_Location = Start; };
	virtual void Draw() = 0;

protected:
	CPoint	m_Location;
	int		m_iSize;
	CTestController * m_pTestController;
};


// Analog Buttons

#define TESTREGIONS 8
#define TESTDEVISOR	    255/TESTREGIONS

class CTestAnalogButton:public CTestControl
{
public:
	CTestAnalogButton(CTestController * pController, int iType);
	virtual ~CTestAnalogButton() {;};
	void	Draw();
	BYTE	m_bTestResults[TESTREGIONS + 2 + 1]; // Test regions + 0 + 255 + button
	BOOL	Test();
	BYTE	TGetState();

	BYTE	m_Resolution;
	int		m_iType;
	BOOL	m_fUnderTest;


};

class CTestDigitalButton: public CTestControl
{
public:
	CTestDigitalButton(CTestController * pController, WORD wType);
	virtual	~CTestDigitalButton();
	void	Draw();
	BOOL	TGetState();
	BYTE	m_bTestResult;
	BOOL	Test();

	WORD m_wType;


};


class CTestDPad: public CTestControl
{
public:
	CTestDPad(CTestController * pController);
	virtual ~CTestDPad();
	void Draw();
	void SetStart(CPoint Start); // override base
	CTestDigitalButton * m_pButton[4]; // UP,DOWN,LEFT,RIGHT
	BOOL TGetUp();
	BOOL TGetDown();
	BOOL TGetLeft();
	BOOL TGetRight();
	BOOL Test();

private:
	CRect  m_TestRect[9]; // 
	BYTE	m_bTestResults[9];
	BOOL	m_fUnderTest;

};

class CAxis
{
public:
	CAxis() {;};
	~CAxis() {;};
	void SetResolution(SHORT Resolution) { m_Resolution = Resolution;};
	SHORT GetResolution() { return m_Resolution;};
private:
	SHORT m_Resolution;
};


class CTestThumbstick: public CTestControl
{
public:
	CTestThumbstick(CTestController * pController, WORD wThumbType);
	virtual ~CTestThumbstick();
	void	Draw();
	void	SetStart(CPoint Start);

	BOOL	ExistsX() { return m_X.GetResolution() ? TRUE : FALSE; };
	BOOL	ExistsY() { return m_Y.GetResolution() ? TRUE: FALSE ; };
	BOOL	ExistsButton() { return m_pTestButton ? TRUE : FALSE; };

	SHORT	TGetX();
	SHORT	TGetY();
	BOOL	TGetButtonState();
	BOOL	fUnderTest;
	BOOL	Test();

private:
	CTestDigitalButton * m_pTestButton;
	CRect	m_TestRect[10];
	BYTE	m_bTestResults[10];  // we test 10 components on a thumbstick
	CAxis	m_X;
	CAxis	m_Y;
	WORD	m_wThumbType;


};


class CTestController
{
public:
	virtual	~CTestController();
	virtual DWORD InitTest(DWORD dwPort);
	DWORD	Refresh();
	void	Draw();
	void	SetCountDown(int);
	int		GetCountDown();
	void	DecCountDown();
	BOOL	Test();


	CTestThumbstick		* m_pTThumbStick[2]; // Left and Right Thumbsticks
	CTestDPad			* m_pTDPad;
	CTestDigitalButton	* m_pTStartButton;
	CTestDigitalButton	* m_pTBackButton;
	CTestAnalogButton	* m_pTAnalogButton[8]; // indexes are the same as the xinput_gamepad defines

	XINPUT_STATE		* m_pInputState;
	XINPUT_CAPABILITIES * m_pInputCaps;

	BOOL				m_fFlash;
	BOOL				m_fRemoved;
	BOOL				m_fCrossTalk;

	BYTE				m_SubType;
	HANDLE				m_Handle;
	DWORD				m_dwPort;

	DWORD				m_dwPacketNumber;
	DWORD				m_dwSlot[2];
	DWORD				m_TestComplete;
	int	*				m_piTestOrder;

private:
	HANDLE				m_hCountDownMutex;
	int					m_TestCountDown;

};

// check which version we're building
#if  (MUCOUNTZERO)
#define		MUCOUNT 0
#elif defined (MUCOUNTONE)
#define		MUCOUNT 1
#elif defined (MUCOUNTTWO)
#define		MUCOUNT 2
#endif

#endif // __testcontrols_h_
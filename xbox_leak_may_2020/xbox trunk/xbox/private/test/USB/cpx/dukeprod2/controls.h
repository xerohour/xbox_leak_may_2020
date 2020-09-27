/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    controller.h

Abstract:

    function declarations, type definitions, includes, etc, for peripheral controls

Author:

    Dennis Krueger <a-denkru>   May 2000

Revision History:

--*/


#ifndef __controls_h_
#define __controls_h_

#include <xtl.h>

// controller types
#define STANDARD_TYPE		1
#define ACHIBONO_TYPE		2
#define WHEEL_TYPE			10
#define ARCADE_STICK_TYPE	20

class CController;  // forward

class CControl
{
public:
	CControl(CController * pController) { m_pController = pController;};
	virtual ~CControl() { ; } ;

	CController * m_pController;
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


class CAnalogButton:public CControl
{
public:
	CAnalogButton(CController * pController, int index);
	virtual ~CAnalogButton() { ; };
	BYTE GetState();
	BYTE	m_Resolution;
	int		m_Type;
	
};


class CDigitalButton:public CControl
{
public:
	CDigitalButton(CController * pController, WORD wType);
	virtual ~CDigitalButton() {;};
	BOOL GetState();

	WORD m_wType;

};


class CThumbstick:public CControl
{
public:
	CThumbstick(CController * pController, WORD wThumbType);
	virtual ~CThumbstick();

	SHORT GetX();
	SHORT GetY();
	BOOL  GetButtonState();

	BOOL  ExistsX() { return m_Axis[0].GetResolution() ? FALSE : TRUE; };
	BOOL  ExistsY() { return m_Axis[1].GetResolution() ? FALSE : TRUE; };
	BOOL  ExistsButton() { return m_pButton ? FALSE : TRUE; };


	WORD			m_wThumbType;
	CDigitalButton *	m_pButton;
	CAxis			m_Axis[2];  // x,y
};


class CDPad:public CControl
{
public:
	CDPad(CController * pController);
	virtual ~CDPad();

	CDigitalButton * m_pButton[4]; // UP,DOWN,LEFT,RIGHT
	BOOL GetUp();
	BOOL GetDown();
	BOOL GetLeft();
	BOOL GetRight();

};



class CController
{
public:
	CController();
	virtual ~CController();
	virtual DWORD Init(DWORD dwPort);
	void Refresh();
	
	XINPUT_STATE m_InputState;
	XINPUT_CAPABILITIES m_InputCaps;


// control members
	BYTE				m_SubType;
	HANDLE				m_Handle;
	DWORD				m_dwPort;
	CThumbstick	*		m_pThumbStick[2]; // Left and Right Thumbsticks
	CDPad	*			m_pDPad;
	CDigitalButton *	m_pStartButton;
	CDigitalButton	*	m_pBackButton;
	CAnalogButton	*	m_pAnalogButton[8]; // indexes are the same as the xinput_gamepad defines

private:

	DWORD			m_dwPacketNumber;

};

#endif
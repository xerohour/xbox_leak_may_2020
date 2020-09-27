//-----------------------------------------------------------------------------
// File: TestControls.cpp
//
// Desc: Implementation of Peripheral Tests based on the CController and CControl Classes
//
// Hist: Oct 08 2001 - Created
//
//
// Author:  Dennis Krueger <a-denkru>
//
// 
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#include "TestControls.h"

#ifdef __cplusplus
extern "C" {
#endif
int DebugPrint(char* format, ...);
#ifdef __cplusplus
}
#endif

// screen position by control type for Standard Type

CPoint DukeStart[esMax];
CPoint AkiStart[esMax];

WCHAR * Labels[] = 
{
	L"A",
	L"B",
	L"X",
	L"Y",
	L"Black",
	L"White",
	L"T1",
	L"T2",
	L"Left Stick",
	L"DPad",
	L"Back",
	L"Start",
	L"Right Stick"
};

WCHAR * ControllerLabels[] =
{
	L"Standard Game Pad",
	L"Alternate Game Pad",
	L"Steering Wheel",
	L"Arcade Stick",
	L"Flight Stick",
	L"Board Controller"
};

DWORD CurrColor[] =
{
	BlueColor,
	YellowColor,
	GreenColor,
	RedColor,
	GrayColor
};

XINPUT_FEEDBACK gFeedback;


void InitAkiLocs()
{
	AkiStart[esLTrigger].SetXY(50,70); // Left Trigger 0
	AkiStart[esRTrigger].SetXY(520,70); // Right Trigger 1
	AkiStart[esLThumb].SetXY(50,160); // Left Thumb 2
	AkiStart[esDPad].SetXY(115,335); // DPad 3
	AkiStart[esBackButton].SetXY(60,250); // Back Button 4
	AkiStart[esStartButton].SetXY(105,250); // Start Button 5
	AkiStart[esRThumb].SetXY(340,360); // Right Thumb 6
	AkiStart[esX].SetXY(410,200); // X 7 
	AkiStart[esY].SetXY(450,170); // Y 8
	AkiStart[esWhite].SetXY(445,310); // White 9
	AkiStart[esA].SetXY(450,230); // A 10
	AkiStart[esB].SetXY(490,200); // B 11
	AkiStart[esBlack].SetXY(490,290);  // Black 12
}

void InitDukeLocs()
{

	DukeStart[esLTrigger].SetXY(50,70); // Left Trigger 0
	DukeStart[esRTrigger].SetXY(520,70); // Right Trigger 1
	DukeStart[esLThumb].SetXY(50,160); // Left Thumb 2
	DukeStart[esDPad].SetXY(115,335); // DPad 3
	DukeStart[esBackButton].SetXY(240,400); // Back Button 4
	DukeStart[esStartButton].SetXY(280,400); // Start Button 5
	DukeStart[esRThumb].SetXY(340,360); // Right Thumb 6
	DukeStart[esX].SetXY(410,210); // X 7 
	DukeStart[esY].SetXY(450,210); // Y 8
	DukeStart[esWhite].SetXY(490,210); // White 9
	DukeStart[esA].SetXY(410,260); // A 10
	DukeStart[esB].SetXY(450,260); // B 11
	DukeStart[esBlack].SetXY(490,260);  // Black 12
}


CTestAnalogButton::CTestAnalogButton(CTestController * pController, int index) 
						:CTestControl(pController)
{
	m_pTestController = pController;
	m_iSize = 20; // set size of box
	ZeroMemory(m_bTestResults,sizeof(m_bTestResults));
	m_Resolution = pController->m_pInputCaps->In.Gamepad.bAnalogButtons[index];
	m_iType = index;
}

void
CTestAnalogButton::Draw()
{
	CRect ThisRect;
	CPoint BottomRight;
	CPoint Offset;
	DWORD dwColor;
// 0 value is 4x10 rect
	ThisRect.SetTopLeft(m_Location);
	BottomRight.SetX(m_Location.GetX() + 4);
	BottomRight.SetY(m_Location.GetY() + m_iSize);
	ThisRect.SetBottomRight(BottomRight);
	if(m_pTestController->m_fFlash && m_bTestResults[0] == eTesting)
		dwColor = GrayColor;
	else
		dwColor = CurrColor[m_bTestResults[0]]; // get current color
	ThisRect.Draw(dwColor);

// setup for 16 2x10 rects	
	Offset.SetX(m_Location.GetX() + 4);
	Offset.SetY(m_Location.GetY());
	ThisRect.SetTopLeft(Offset);
	Offset.SetX(Offset.GetX()+4);
	Offset.SetY(Offset.GetY()+m_iSize);
	ThisRect.SetBottomRight(Offset);
	Offset.SetX(2);
	Offset.SetY(0);
	for(int i = 1; i<TESTREGIONS+1; i++)
	{
		if(m_pTestController->m_fFlash && m_bTestResults[i] == eTesting)
			dwColor = GrayColor;
		else
			dwColor = CurrColor[m_bTestResults[i]]; // get current color
		ThisRect.Draw(dwColor);
		ThisRect.OffsetRect(Offset);
	}

	// expand ending 255 value rect back to 4
	Offset = ThisRect.GetTopLeft(); // get current position
	Offset.SetX(Offset.GetX()+4);
	ThisRect.SetBottomRight(Offset);
	if(m_pTestController->m_fFlash && m_bTestResults[TESTREGIONS+1] == eTesting)
		dwColor = GrayColor;
	else
		dwColor = CurrColor[m_bTestResults[TESTREGIONS+1]]; // get current color
	ThisRect.Draw(dwColor);

	Offset.SetX(m_Location.GetX() + 5);
	Offset.SetY(m_Location.GetY() + m_iSize + 5);
	ThisRect.SetTopLeft(Offset);
	Offset.SetX(Offset.GetX() + 10);
	Offset.SetY(Offset.GetY() + 10);
	ThisRect.SetBottomRight(Offset);

	// put in "return to zero" button
	if(m_pTestController->m_fFlash && m_bTestResults[TESTREGIONS+2] == eTesting)
		dwColor = GrayColor;
	else
		dwColor = CurrColor[m_bTestResults[TESTREGIONS+2]]; // get current color
	
	ThisRect.Draw(dwColor);

	ThisRect.SetTopLeft(m_Location);  // reset to fron
	drSetSize(50, 100, 0x0000ffff, 0xff000000); //set the text color
	drPrintf((float)m_Location.GetX(),(float) m_Location.GetY() - 14, Labels[m_iType]);
	if(TRUE == m_fUnderTest)
	{
		drPrintf(70,30,L"%s = 0x%02X",Labels[m_iType],TGetState());
	}

}

BYTE
CTestAnalogButton::TGetState()
{
	return m_pTestController->m_pInputState->Gamepad.bAnalogButtons[m_iType];
}


BOOL
CTestAnalogButton::Test()
{
	
	DWORD	dwResult;
	BOOL	fResult = FALSE;
	// set test results to active
	for(int i = 0; i< TESTREGIONS+3; i++)
	{
		m_bTestResults[i] = eTesting;
	}
	m_fUnderTest = TRUE;
	m_bTestResults[1] = eGood;  // note: not gathering in the 1-32 range, too difficult to hit
	m_pTestController->SetCountDown(200); // 20 second countdown
	BYTE bCurrValue = 0;
	BOOL fFinalFlag = FALSE;
	BOOL fNotDone = TRUE;
	while(m_pTestController->GetCountDown())
	{
		bCurrValue = TGetState(); // get current value of control
		// test for cross talk each time we get the analog button state
		int ct = 0;
		for(int i = 0; i<8; i++)
		{
			if(0 < m_pTestController->m_pInputState->Gamepad.bAnalogButtons[i])
				ct++;
		}
		if(ct > 2) // got three or more positive analog buttons, fail crosstalk
			m_pTestController->m_fCrossTalk = TRUE;

		// distribute values among test results
		switch(bCurrValue)
		{
			// first test edge cases
			case 0:
				m_bTestResults[0] = eGood;
				if(TRUE == fFinalFlag)  // test for return to zero as final value
				{
					m_bTestResults[TESTREGIONS+2] = eGood;
					fNotDone = FALSE;
				}
				break;
			case 255:
				m_bTestResults[TESTREGIONS+1] = eGood;
			// distribute all other results among the TESTREGIONS elements up to 254
			default:
				m_bTestResults[(bCurrValue/(255/TESTREGIONS))+1] = eGood;
				break;
		}
		// check for completion
		fFinalFlag = TRUE;
		for(int i = 0; i<TESTREGIONS+2; i++)
		{
			if(eTesting == m_bTestResults[i])
			{
				fFinalFlag = FALSE;
				break; // no need to test further
			}
		}
		if(FALSE == fNotDone)
		{
			break;  // break out of while, we're complete
		}
		if(esLTrigger == m_iType || esRTrigger == m_iType)
		{
			if(bCurrValue)
			{
				// let's rumble
				ZeroMemory(&gFeedback,sizeof(XINPUT_FEEDBACK));
				if(esLTrigger == m_iType)
				{
					// make proportional to 64K
					gFeedback.Rumble.wLeftMotorSpeed = (bCurrValue<<8) + bCurrValue;
				} else
				{
					gFeedback.Rumble.wRightMotorSpeed = (bCurrValue<<8) +bCurrValue;
				}
				dwResult = XInputSetState(m_pTestController->m_Handle,&gFeedback);
				if(ERROR_IO_PENDING == dwResult)
				{
					// wait out i/o async pending condition
					while(dwResult == gFeedback.Header.dwStatus) ;
				}
			}
		}

		Sleep(0); // release quantum
	} // while
	
	// set any remaining incomplete test segments to fail after exiting timeout loop
	for(int i = 0; i <TESTREGIONS+3; i++)
	{
		if(eTesting == m_bTestResults[i])
			m_bTestResults[i] = eBad;
	}
				
	// ensure rumble pack turned of if handling triggers
	if(esLTrigger == m_iType || esRTrigger == m_iType)
	{
		if(esLTrigger == m_iType)
		{
			gFeedback.Rumble.wLeftMotorSpeed = 0;
		} else
		{
			gFeedback.Rumble.wRightMotorSpeed = 0;
		}
		dwResult = XInputSetState(m_pTestController->m_Handle,&gFeedback);
		if(ERROR_IO_PENDING == dwResult)
		{
			// wait out i/o async pending condition
			while(ERROR_IO_PENDING == gFeedback.Header.dwStatus) ;
		}
	}
	for(int i = 0; i < TESTREGIONS+3; i++)	
	{
		// return error if any test segments are bad
		if(eBad == m_bTestResults[i])
		{
			fResult = TRUE;
			break;
		}
	}
	m_fUnderTest = FALSE;
	return fResult;
}


BOOL
CTestDigitalButton::TGetState()
{
	
	return m_pTestController->m_pInputState->Gamepad.wButtons & m_wType ? TRUE : FALSE;
}


CTestDigitalButton::~CTestDigitalButton()
{
	;
}

CTestDigitalButton::CTestDigitalButton(CTestController * pController, WORD wType) 
							:CTestControl(pController)
{
	m_iSize = 20; // set size of box
	m_bTestResult = eNotyet;
	m_wType = wType;
}


void
CTestDigitalButton::Draw()
{
	int		LabelIndex;
	CRect	ThisRect;
	CPoint	BottomRight;
	DWORD	dwColor;

	// make a square with m_size sides
	ThisRect.SetTopLeft(m_Location);
	BottomRight.SetX(m_Location.GetX() + m_iSize);
	BottomRight.SetY(m_Location.GetY() + m_iSize);
	ThisRect.SetBottomRight(BottomRight);
	if(m_pTestController->m_fFlash && m_bTestResult == eTesting)
		dwColor = GrayColor;
	else
		dwColor = CurrColor[m_bTestResult]; // get current color
	ThisRect.Draw(dwColor);


	// start and back are the only discreet digital buttons, the others are either part of the dpad
	// ... or components of the thumbstick
	if(XINPUT_GAMEPAD_START == m_wType)
	{
		LabelIndex = esStartButton;
	} else
	{
		LabelIndex = esBackButton;
	}
	drSetSize(50, 100, 0x0000ffff, 0xff000000); //set the text color
	drPrintf((float)m_Location.GetX(),(float) m_Location.GetY() - 14, Labels[LabelIndex]);
	if(eTesting == m_bTestResult)
		drPrintf(70,30,L"Digital Button Status = 0x%04X",m_pTestController->m_pInputState->Gamepad.wButtons);

}

BOOL
CTestDigitalButton::Test()
{
	BOOL	fResult = FALSE;	// test complete
	BOOL	fGotOn = FALSE;		// detected set status
	BOOL	fButtonState;
	// set testing status
	m_bTestResult = eTesting;
	m_pTestController->SetCountDown(50); // allow 5 secs to get button pressed
	while(m_pTestController->GetCountDown())
	{
		fButtonState = TGetState();
		if(TRUE == fButtonState)
		{
			fGotOn = TRUE;
		} else if(TRUE == fGotOn) // if we've already found set
		{
			m_bTestResult = eGood; // zero after set
			break;
		}
	} // while
	if(eGood != m_bTestResult)
	{
		m_bTestResult = eBad;
		fResult = TRUE;
	}
	return fResult;
}

WORD DPadButtonID[4] = {
	XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_DPAD_DOWN,
	XINPUT_GAMEPAD_DPAD_LEFT,
	XINPUT_GAMEPAD_DPAD_RIGHT
};


BYTE DPadTestMatrix[9];

// testdpad methods

CTestDPad::CTestDPad(CTestController * pController) : CTestControl(pController)
{
	WORD wMask = 1;
	for(int i = 0; i < 4; i++)
	{
		if(pController->m_pInputCaps->In.Gamepad.wButtons & wMask)
		{
			m_pButton[i] = new CTestDigitalButton(pController,DPadButtonID[i]);
		} else
		{
			m_pButton[i] = NULL;
		}
		wMask <<= 1;
	}

	// now assign the test matrix based on the supported buttons
	// first init to false
	for(int i = 0; i < 9; i++)
	{
		DPadTestMatrix[i] = 0;
	}

	DPadTestMatrix[4] = 0; // middle always true if DPad exists

	if(m_pButton[0]) // if up
	{
		DPadTestMatrix[1] = XINPUT_GAMEPAD_DPAD_UP; //up only
		if(m_pButton[2])
			DPadTestMatrix[0] = XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_LEFT; //up and left
		if(m_pButton[3])
			DPadTestMatrix[2] = XINPUT_GAMEPAD_DPAD_UP | XINPUT_GAMEPAD_DPAD_RIGHT; // up and right
	}
	if(m_pButton[1])
	{
		DPadTestMatrix[7] = XINPUT_GAMEPAD_DPAD_DOWN; // down
		if(m_pButton[2])
			DPadTestMatrix[6] = XINPUT_GAMEPAD_DPAD_DOWN | XINPUT_GAMEPAD_DPAD_LEFT; // down and left
		if(m_pButton[3])
			DPadTestMatrix[8] = XINPUT_GAMEPAD_DPAD_DOWN | XINPUT_GAMEPAD_DPAD_RIGHT; // down and right
	}
	if(m_pButton[2])
		DPadTestMatrix[3] = XINPUT_GAMEPAD_DPAD_LEFT;  // left
	if(m_pButton[3])
		DPadTestMatrix[5] = XINPUT_GAMEPAD_DPAD_RIGHT; // right

// test and draw truth matrix now set	
	CRect TempRect;
	CPoint TempPoint,TempPoint1,OffsetPoint;

	// init initial values of the 9 test locations
	m_iSize = 20;
	m_pTestController = pController;
	
	TempPoint.SetXY(0,0); TempPoint1.SetXY(m_iSize,m_iSize);

	TempRect.Set(TempPoint,TempPoint1);

	// set first row across
	m_TestRect[0] = TempRect;
	OffsetPoint.SetXY(m_iSize,0);
	TempRect.OffsetRect(OffsetPoint);
	m_TestRect[1] = TempRect;
	TempRect.OffsetRect(OffsetPoint);
	m_TestRect[2] = TempRect;

	// set second row across
	TempPoint.SetXY(0,m_iSize); TempPoint1.SetXY(m_iSize,m_iSize*2);
	TempRect.Set(TempPoint,TempPoint1);
	m_TestRect[3] = TempRect;
	TempRect.OffsetRect(OffsetPoint);
	m_TestRect[4] = TempRect;
	TempRect.OffsetRect(OffsetPoint);
	m_TestRect[5] = TempRect;

	// set third row across
	TempPoint.SetXY(0,m_iSize*2); TempPoint1.SetXY(m_iSize,m_iSize*3);
	TempRect.Set(TempPoint,TempPoint1);
	m_TestRect[6] = TempRect;
	TempRect.OffsetRect(OffsetPoint);
	m_TestRect[7] = TempRect;
	TempRect.OffsetRect(OffsetPoint);
	m_TestRect[8] = TempRect;

	for(int i = 0; i < 9; i++)
	{
		m_bTestResults[i] = eNotyet;
	}
	// done with init
}

CTestDPad::~CTestDPad()
{
	for(int i = 0; i<4; i++)
	{
		if(m_pButton[i]) delete m_pButton[i];
	}
}


BOOL
CTestDPad::TGetUp()
{
	if(m_pButton[0])
		return m_pButton[0]->TGetState();
	else
		return FALSE;
}

BOOL
CTestDPad::TGetDown()
{
	if(m_pButton[1])
		return m_pButton[1]->TGetState();
	else
		return FALSE;
}

BOOL
CTestDPad::TGetLeft()
{
	if(m_pButton[2])
		return m_pButton[2]->TGetState();
	else
		return FALSE;
}

BOOL
CTestDPad::TGetRight()
{
	if(m_pButton[3])
		return m_pButton[3]->TGetState();
	else
		return FALSE;
}

void
CTestDPad::SetStart(CPoint Start)
{
	m_Location = Start; // save start
	// adjust the 9 boxes by the starting point
	for(int i = 0; i < 9; i++)
	{
		m_TestRect[i].OffsetRect(Start);
	}
}
	

void
CTestDPad::Draw()
{
	DWORD dwColor;
	
	// draw the testable area

	for(int i = 0; i< 9; i++)
	{
		if(DPadTestMatrix[i])
		{
			dwColor = CurrColor[m_bTestResults[i]];
			m_TestRect[i].Draw(dwColor);
		}
	}
	dwColor = CurrColor[m_bTestResults[4]];

	m_TestRect[4].Draw(dwColor); // middle is special case of all zeros

	// now draw label
	drSetSize(50, 100, 0x0000ffff, 0xff000000); //set the text color
	drPrintf((float)m_Location.GetX() + 4,(float) m_Location.GetY() - 14, Labels[esDPad]);
	// draw wButton value if under test
	if(TRUE == m_fUnderTest)
	{
		drPrintf(70,30,L"Digital Button Status = %04x",m_pTestController->m_pInputState->Gamepad.wButtons);
	}


}

int DPadTestOrder[9] = {1,0,3,6,7,8,5,2,4};

BOOL
CTestDPad::Test()
{
	BOOL fResult = FALSE; // set to ok
	int iTestNumber, iCurrTest = 0 ;
	m_pTestController->SetCountDown(200); // give twenty seconds
	m_fUnderTest = TRUE;
	while(m_pTestController->GetCountDown())
	{
		if(9 == iCurrTest) break;
		iTestNumber = DPadTestOrder[iCurrTest];
		if(DPadTestMatrix[iTestNumber] || 4 == iTestNumber)
		{
			m_bTestResults[iTestNumber] = eTesting;
		} else
		{
			iCurrTest++; // go on to next test
			continue;
		}
		if((m_pTestController->m_pInputState->Gamepad.wButtons & 0x0f) == DPadTestMatrix[iTestNumber])
		{
			m_bTestResults[iTestNumber] = eGood;
			iCurrTest++;
		}
	} // while
	for(int i = 0; i< 9; i++)
	{
		if(eGood != m_bTestResults[i])
		{
			m_bTestResults[i] = eBad;
			fResult = TRUE;
		}
	}
	m_fUnderTest = FALSE;
	return fResult;
}	




CTestThumbstick::CTestThumbstick(CTestController * pController, WORD wType) : CTestControl(pController)
{

	CRect TempRect;
	CPoint TempPoint,TempPoint1,OffsetPoint;
	fUnderTest = FALSE;
	// init initial values of the 9 test locations
	m_iSize = 20;

	// discover configuration of this thumbstick or wheel
	m_wThumbType = wType;
	if(XINPUT_GAMEPAD_LEFT_THUMB == wType)
	{
		m_X.SetResolution(pController->m_pInputCaps->In.Gamepad.sThumbLX);
		m_Y.SetResolution(pController->m_pInputCaps->In.Gamepad.sThumbLY);
	} else
	{
		m_X.SetResolution(pController->m_pInputCaps->In.Gamepad.sThumbRX);
		m_Y.SetResolution(pController->m_pInputCaps->In.Gamepad.sThumbRY);
	}

	if(pController->m_pInputCaps->In.Gamepad.wButtons & wType)
	{
		m_pTestButton = new CTestDigitalButton(pController, wType);
	} else
	{
		m_pTestButton = NULL;
	}

	// got configuration, now setup base test rects for drawing
	TempPoint.SetXY(0,0); TempPoint1.SetXY(m_iSize,m_iSize);

	TempRect.Set(TempPoint,TempPoint1);

	// set first row across
	m_TestRect[0] = TempRect;
	OffsetPoint.SetXY(m_iSize,0);
	TempRect.OffsetRect(OffsetPoint);
	m_TestRect[1] = TempRect;
	TempRect.OffsetRect(OffsetPoint);
	m_TestRect[2] = TempRect;

	// set second row across
	TempPoint.SetXY(0,m_iSize); TempPoint1.SetXY(m_iSize,m_iSize*2);
	TempRect.Set(TempPoint,TempPoint1);
	m_TestRect[3] = TempRect;
	TempRect.OffsetRect(OffsetPoint);
	m_TestRect[4] = TempRect;
	TempRect.OffsetRect(OffsetPoint);
	m_TestRect[5] = TempRect;

	// set third row across
	TempPoint.SetXY(0,m_iSize*2); TempPoint1.SetXY(m_iSize,m_iSize*3);
	TempRect.Set(TempPoint,TempPoint1);
	m_TestRect[6] = TempRect;
	TempRect.OffsetRect(OffsetPoint);
	m_TestRect[7] = TempRect;
	TempRect.OffsetRect(OffsetPoint);
	m_TestRect[8] = TempRect;

	m_TestRect[9] = m_TestRect[5];
	OffsetPoint.SetXY(30,-4);
	m_TestRect[9].OffsetRect(OffsetPoint); // move right and slightly up

	if(pController->m_pInputCaps->In.Gamepad.wButtons & wType)
	{
		m_pTestButton = new CTestDigitalButton(pController, wType);
	} else
	{
		m_pTestButton = NULL;
	}


	for(int i = 0; i < 10; i++)
	{
		
		m_bTestResults[i] = eNotyet;
	}
	// do X and Y axis exclusions
	if(0 == m_X.GetResolution() || 0 == m_Y.GetResolution()) // if missing either Axis
	{
		// don't test any corners
		m_bTestResults[0] = eNever;
		m_bTestResults[2] = eNever;
		m_bTestResults[6] = eNever;
		m_bTestResults[8] = eNever;
		if(0 == m_X.GetResolution())
		{
			// if missing X axis
			m_bTestResults[3] = eNever;
			m_bTestResults[5] = eNever;
		}
		if(0 == m_Y.GetResolution())
		{
			// if no Y axis
			m_bTestResults[1] = eNever;
			m_bTestResults[7] = eNever;
		}
	}
	// done with init

}

CTestThumbstick::~CTestThumbstick()
{
	if(m_pTestButton) delete m_pTestButton;
}

SHORT
CTestThumbstick::TGetX()
{
	SHORT sState;
	if(XINPUT_GAMEPAD_LEFT_THUMB == m_wThumbType)
	{
		sState = m_pTestController->m_pInputState->Gamepad.sThumbLX;
	} else
	{
		sState =  m_pTestController->m_pInputState->Gamepad.sThumbRX;
	}
	return sState;
}

SHORT
CTestThumbstick::TGetY()
{
	SHORT sState;
	if(XINPUT_GAMEPAD_LEFT_THUMB == m_wThumbType)
	{
		sState = m_pTestController->m_pInputState->Gamepad.sThumbLY;
	} else
	{
		sState =  m_pTestController->m_pInputState->Gamepad.sThumbRY;
	}
	return sState;
}

BOOL
CTestThumbstick::TGetButtonState()
{
	if(m_pTestButton)
	{
		return m_pTestButton->TGetState();
	} 
	return FALSE;
}



void
CTestThumbstick::SetStart(CPoint Start)
{
	m_Location = Start; // save start
	// adjust the 10 boxes by the starting point
	for(int i = 0; i < 10; i++)
	{
		m_TestRect[i].OffsetRect(Start);
	}
}
// index order of display blocks
// 0 1 2
// 3 4 5
// 6 7 8

short XVal[] =
{
	-24000,		// 0
	0,			// 1
	24000,		// 2
	-32767,		// 3
	0,			// 4
	32767,		// 5
	-24000,		// 6
	0,			// 7
	24000		// 8
};

short YVal[] =
{
	24000,		// 0
	32767,		// 1
	24000,		// 2
	0,			// 3
	0,			// 4
	0,			// 5
	-24000,		// 6
	-32768,		// 7
	-24000		// 8
};
	


void
CTestThumbstick::Draw()
{
	int iLabelIndex;
	DWORD dwColor;

	for(int i = 0; i < 10; i++)
	{
		dwColor = CurrColor[m_bTestResults[i]];  // get the color for the state of this box
		if(TRUE == fUnderTest) 
		{
			// check for flashing only if this control is being tested
			if(eTesting == m_bTestResults[i] && m_pTestController->m_fFlash)
			{
				dwColor = GrayColor;
			}
				
			::drPrintf(70,30,L"X Axis = %05d, Y Axis = %05d",TGetX(),TGetY());
		}
		if(9 == i && ExistsButton() )
		{
			// also draw small rect in middle to represent button
			CRect SmRect;
			CPoint TempPt;
			TempPt = m_TestRect[4].GetTopLeft();
			TempPt.SetX(TempPt.GetX() + 5);
			TempPt.SetY(TempPt.GetY() + 5);
			SmRect.SetTopLeft(TempPt);
			TempPt.SetX(TempPt.GetX() + 10);
			TempPt.SetY(TempPt.GetY() + 10);
			SmRect.SetBottomRight(TempPt);
			SmRect.Draw(dwColor);
		}

		if(eNever != m_bTestResults[i])
		{
			m_TestRect[i].Draw(dwColor);
		}
	}
		// also print x & y values during test

	if(XINPUT_GAMEPAD_LEFT_THUMB == m_wThumbType)
	{
		iLabelIndex = esLThumb;
	} else
	{
		iLabelIndex = esRThumb;
	}
	// draw label
	drSetSize(50, 100, 0x0000ffff, 0xff000000); //set the text color
	drPrintf((float)m_Location.GetX() + 4,(float) m_Location.GetY() - 16, Labels[iLabelIndex]);


}

int TestOrder[] = { 1,0,3,6,7,8,5,2 };

BOOL
CTestThumbstick::Test()
{
	int iTestNumber;
	int iCurrTest = 0;
	short sCurrX,sCurrY;
	BOOL fResult = FALSE;

	m_pTestController->SetCountDown(200); // 20 second countdown
	fUnderTest = TRUE;
	iTestNumber = TestOrder[0];
	while(m_pTestController->GetCountDown() && iCurrTest < 8)
	{
		sCurrX = TGetX();
		sCurrY = TGetY();
		
		if(eNever == m_bTestResults[iTestNumber])
		{
			iCurrTest++; // inc to next test
			iTestNumber = TestOrder[iCurrTest];
			continue; // move on to next test
		}

		if(eNotyet == m_bTestResults[iTestNumber])
			m_bTestResults[iTestNumber] = eTesting;

		BOOL XisOK = FALSE;
		BOOL YisOK = FALSE;

		if(0 == XVal[iTestNumber] && (sCurrX < 16000 && sCurrX > -16000))
		{
			// zero is don't care, only testing y
			XisOK = TRUE;
		} else if( ( (sCurrX  <= XVal[iTestNumber] )  && (XVal[iTestNumber] < 0) ) ||
					( (sCurrX >=XVal[iTestNumber]  ) && (XVal[iTestNumber] > 0) ) )
		{
			XisOK = TRUE;
		}

		
		if(0 == YVal[iTestNumber] && (sCurrY < 16000 && sCurrY > -16000))
		{
			// zero is don't care, only testing y
			YisOK = TRUE;
		} else if( ( (sCurrY <= YVal[iTestNumber])  && (YVal[iTestNumber] < 0) ) ||
					( (sCurrY >= YVal[iTestNumber] ) && (YVal[iTestNumber] > 0) ) )
		{
			YisOK = TRUE;
		}

		if(YisOK && XisOK)
		{
			m_bTestResults[iTestNumber] = eGood;
			iCurrTest++; // inc to next test
			iTestNumber = TestOrder[iCurrTest];
		}

	} // While
	// treat center as a special case
	m_bTestResults[4] = eTesting;
	while(m_pTestController->GetCountDown())
	{
		sCurrX = TGetX();
		sCurrY = TGetY();
		if((sCurrX < 4000 && sCurrX > -4000) &&
			(sCurrY < 4000 && sCurrY > -4000) )
		{
			m_bTestResults[4] = eGood;
			break;
		}
	}
	if(eTesting == m_bTestResults[4]) m_bTestResults[4] = eBad;
		
	// finally test button
	if(FALSE == ExistsButton())
	{
		m_bTestResults[9] = eGood;
	} else
	{
		m_bTestResults[9] = eTesting;
		while(m_pTestController->GetCountDown())
		{
			BOOL fState = m_pTestButton->TGetState();
			if(TRUE == fState )
			{
				m_bTestResults[9] = eGood;
				break;
			}
		}
		if(eGood != m_bTestResults[9])
		{
			m_bTestResults[9] = eBad;
		}
	}

	// reset any untested elements at timeout to bad
	for(int i = 0; i < 10; i++)
	{
		if(!eGood == m_bTestResults[i]  && !eNever == m_bTestResults[i])
		{
			m_bTestResults[i] = eBad;
			fResult = TRUE;
		}
	}
	for(int i = 0; i< 10; i++)
	{
		if(m_bTestResults[i] == eBad)
			fResult = TRUE;
	}

	fUnderTest = FALSE;
	return fResult;
}



// CTestController Class Methods
CTestController::CTestController()
{
	;
}

CTestController::~CTestController()
{
	if(m_pTDPad) delete m_pTDPad;
	for(int i = 0; i<8; i++)
	{
		if(m_pTAnalogButton[i]) delete m_pTAnalogButton[i];
	}
	if(m_pTThumbStick[0]) delete m_pTThumbStick[0];
	if(m_pTThumbStick[1]) delete m_pTThumbStick[1];
	if(m_pTStartButton) delete m_pTStartButton;
	if(m_pTBackButton) delete m_pTBackButton;
	if(m_Handle) XInputClose(m_Handle);
	if(m_pInputState) delete m_pInputState;
	if(m_pInputCaps) delete m_pInputCaps;
}

// order of test execution
int DukeOrder[13] = { 0,1,2,3,4,5,6,7,8,9,10,11,12};
int AkiOrder[13] =  {0,1,2,4,5,3,6,7,8,10,11,9,12 };


DWORD
CTestController::InitTest(DWORD dwPort)
{
	DWORD	dwResult;
	WORD	wMask;
	CPoint * pStartLocs;

	m_dwPacketNumber = 0;
	// set handles and pointers to NULL
	m_Handle = NULL;
	m_pTDPad = NULL;
	m_fCrossTalk = FALSE;

	for(int i = 0; i < 8; i++)
	{
		m_pTAnalogButton[i] = NULL;
	}
	m_pTThumbStick[0] = NULL;
	m_pTThumbStick[1] = NULL;
	m_pTStartButton = NULL;
	m_pTBackButton = NULL;
	m_fRemoved = FALSE;

	m_TestComplete = 0; // no tests are complete
	// first open a handle to this controller
	m_Handle = XInputOpen(XDEVICE_TYPE_GAMEPAD,dwPort,XDEVICE_NO_SLOT,NULL);
	if(NULL == m_Handle)
		return GetLastError(); // failed

	// have open controller, populate controls
	m_dwPort = dwPort; // stash active port for this controller
	m_fFlash = FALSE;
	m_TestCountDown = 0;

	m_pInputState = new XINPUT_STATE;
	if(NULL == m_pInputState)
	{
		return GetLastError();
	}

	m_pInputCaps = new XINPUT_CAPABILITIES;
	if(NULL == m_pInputCaps)
	{
		return GetLastError();
	}
	dwResult = XInputGetCapabilities(m_Handle, m_pInputCaps);
	if(ERROR_SUCCESS != dwResult)
	{
		XInputClose(m_Handle);
		m_Handle = NULL;
		return dwResult;
	}
	// init mutex used to protect the countdown timer
	m_hCountDownMutex = CreateMutex(
						NULL,		// security unused						
						FALSE,		// don't create owned
						NULL		// No name
						);

	if(NULL == m_hCountDownMutex)
	{
		XInputClose(m_Handle);
		dwResult = GetLastError();
		return dwResult;
	}
	m_SubType = m_pInputCaps->SubType;
	// set screen locations for the supported controls
	// setup test array
	switch (m_SubType)
	{
		case ACHIBONO_TYPE:
		{
			InitAkiLocs();
			pStartLocs = &AkiStart[0];
			m_piTestOrder = AkiOrder;
			break;
		}
		case WHEEL_TYPE:
		case ARCADE_STICK_TYPE:
		case STANDARD_TYPE:
		default:
		{
			InitDukeLocs();
			pStartLocs = &DukeStart[0];
			m_piTestOrder = DukeOrder;
			break;
		}

	}

	// got caps, now set existence flags for supported controls
	
	// DPAD
	if(m_pInputCaps->In.Gamepad.wButtons & 0x000f) // test for any portion of dpad
	{
		m_pTDPad = new CTestDPad(this);
		m_pTDPad->SetStart(pStartLocs[esDPad]);
	} else
		m_pTDPad = NULL;
	
	// Start Button
	if(m_pInputCaps->In.Gamepad.wButtons & XINPUT_GAMEPAD_START)
	{
		m_pTStartButton = new CTestDigitalButton(this, XINPUT_GAMEPAD_START);
		m_pTStartButton->SetStart(pStartLocs[esStartButton]);
	} else
		m_pTStartButton = NULL;

	// Back Button
	if(m_pInputCaps->In.Gamepad.wButtons & XINPUT_GAMEPAD_BACK)
	{
		m_pTBackButton = new CTestDigitalButton(this,XINPUT_GAMEPAD_BACK);
		m_pTBackButton->SetStart(pStartLocs[esBackButton]);
	} else
		m_pTBackButton = NULL;

	// All Analog buttons

	for(int i = 0; i < 8 ; i++)
	{
		if(m_pInputCaps->In.Gamepad.bAnalogButtons[i])
		{
			m_pTAnalogButton[i] = new CTestAnalogButton(this,(WORD)i);
			m_pTAnalogButton[i]->SetStart(pStartLocs[i]);
		} else
			m_pTAnalogButton[i] = NULL;
	}

	// Left Thumbstick
	if(m_pInputCaps->In.Gamepad.sThumbLX || 
			m_pInputCaps->In.Gamepad.sThumbLY ||
			(m_pInputCaps->In.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_THUMB)) 
	{
		m_pTThumbStick[0] = new CTestThumbstick(this,XINPUT_GAMEPAD_LEFT_THUMB);
		m_pTThumbStick[0]->SetStart(pStartLocs[esLThumb]);
	} else
		m_pTThumbStick[0] = NULL;

	// Right Thumbstick
	if(m_pInputCaps->In.Gamepad.sThumbRX ||
				m_pInputCaps->In.Gamepad.sThumbRY ||
				(m_pInputCaps->In.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_THUMB))
	{
		m_pTThumbStick[1] = new CTestThumbstick(this,XINPUT_GAMEPAD_RIGHT_THUMB);
		m_pTThumbStick[1]->SetStart(pStartLocs[esRThumb]);
	} else
		m_pTThumbStick[1] = NULL;

	m_dwSlot[0] = FALSE;
	m_dwSlot[1] = FALSE;


	return S_OK;
}

void
CTestController::DecCountDown()
{
	// acquire mutext
	WaitForSingleObject(m_hCountDownMutex,INFINITE);
	if(m_TestCountDown) m_TestCountDown--; // used as flag, don't dec past zero
	ReleaseMutex(m_hCountDownMutex);
}

void
CTestController::SetCountDown(int iVal)
{
	// acquire mutext
	WaitForSingleObject(m_hCountDownMutex,INFINITE);
	m_TestCountDown = iVal;
	ReleaseMutex(m_hCountDownMutex);
}

int
CTestController::GetCountDown()
{
	int iVal;
	// acquire mutext
	WaitForSingleObject(m_hCountDownMutex,INFINITE);
	iVal = m_TestCountDown;
	ReleaseMutex(m_hCountDownMutex);
	return iVal;
}

BOOL
CTestController::Test()
{
	BOOL fTResult;
	BOOL fOResult = FALSE;
	int iTestCounter = 0;
	BOOL fTesting = TRUE;
	// first setup array and order of tests based on controller type
// background tasks now tasking, we can get on with the tests
	// Left Trigger
	fTResult = FALSE; // cumulate results
	while(iTestCounter < 13)
	{
		if(TRUE == m_fRemoved) return FALSE; // bail if controller removed
		switch(m_piTestOrder[iTestCounter])
		{
			case 0:
				if(m_pTAnalogButton[esLTrigger])
					fTResult = m_pTAnalogButton[esLTrigger]->Test();
				break;
			case 1:
				if(m_pTAnalogButton[esRTrigger])
					fTResult = m_pTAnalogButton[esRTrigger]->Test();
				break;
			case 2:
				if(m_pTThumbStick[0])
					fTResult = m_pTThumbStick[0]->Test();
				break;
			case 3:
				if(m_pTDPad)
					fTResult = m_pTDPad->Test();
				break;
			case 4:
				if(m_pTBackButton)
					fTResult = m_pTBackButton->Test();
				break;
			case 5:
				if(m_pTStartButton)
					fTResult = m_pTStartButton->Test();
				break;
			case 6:
				if(m_pTThumbStick[1])
					fTResult = m_pTThumbStick[1]->Test();
				break;
			case 7:
				if(m_pTAnalogButton[esX])
					fTResult = m_pTAnalogButton[esX]->Test();
				break;
			case 8:
				if(m_pTAnalogButton[esY])
					fTResult = m_pTAnalogButton[esY]->Test();
				break;
			case 9:
				if(m_pTAnalogButton[esWhite])
					fTResult = m_pTAnalogButton[esWhite]->Test();
				break;
			case 10:
				if(m_pTAnalogButton[esA])
					fTResult = m_pTAnalogButton[esA]->Test();
				break;
			case 11:
				if(m_pTAnalogButton[esB])
					fTResult = m_pTAnalogButton[esB]->Test();
				break;
			case 12:
				if(m_pTAnalogButton[esBlack])
					fTResult = m_pTAnalogButton[esBlack]->Test();
				break;
		} // switch
		if(TRUE == fTResult)
			fOResult = TRUE;

		iTestCounter++;
	}

	if(TRUE == fOResult)
	{
		m_TestComplete = eBad;// failed at least one test
	} else
	{
		m_TestComplete = eGood; // succeeded
	}
	return fOResult;

}

DWORD CTestController::Refresh()
{
	DWORD dwResult;
	memset(m_pInputState,0x00,sizeof(XINPUT_STATE));
	dwResult = XInputGetState(m_Handle,m_pInputState);
	if(ERROR_SUCCESS != dwResult)
	{
		DebugPrint("GetInputState failed - %x\n",dwResult);
	}
	return dwResult;
}

void
CTestController::Draw()
{
	// advise each supported control to redraw themselves
	if(m_pTThumbStick[0]) m_pTThumbStick[0]->Draw();
	if(m_pTThumbStick[1]) m_pTThumbStick[1]->Draw();
	if(m_pTDPad) m_pTDPad->Draw();
	if(m_pTStartButton) m_pTStartButton->Draw();
	if(m_pTBackButton) m_pTBackButton->Draw();
	if(m_pTAnalogButton[esA]) m_pTAnalogButton[esA]->Draw();
	if(m_pTAnalogButton[esB]) m_pTAnalogButton[esB]->Draw();
	if(m_pTAnalogButton[esX]) m_pTAnalogButton[esX]->Draw();
	if(m_pTAnalogButton[esY]) m_pTAnalogButton[esY]->Draw();
	if(m_pTAnalogButton[esBlack]) m_pTAnalogButton[esBlack]->Draw();
	if(m_pTAnalogButton[esWhite]) m_pTAnalogButton[esWhite]->Draw();
	if(m_pTAnalogButton[esLTrigger]) m_pTAnalogButton[esLTrigger]->Draw();
	if(m_pTAnalogButton[esRTrigger]) m_pTAnalogButton[esRTrigger]->Draw();
	if(m_pTAnalogButton[esA]) m_pTAnalogButton[esA]->Draw();


		// MU's
	if(MUCOUNT)
	{
		drSetSize(50, 100, 0x0000ffff, 0xff000000); //set the color
		drPrintf(345,58, L"Slot 1");
		drBox(230,50,330,80, m_dwSlot[0] ? 0xff00ff00 : 0xfff0f0f0);
		if(2 == MUCOUNT)
		{
			drSetSize(50, 100, 0x0000ffff, 0xff000000); //set the color
			drPrintf(345,98, L"Slot 2");
			drBox(230,90,330,120, m_dwSlot[1] ? 0xff00ff00 : 0xfff0f0f0);
		}
	}
	// label boxes

// draw controller label
	int iLabelIndex = 0;
	switch (m_SubType)
	{
		case ACHIBONO_TYPE:
			iLabelIndex = 1;
			break;
		case WHEEL_TYPE:
			iLabelIndex = 2;
			break;
		case ARCADE_STICK_TYPE:
			iLabelIndex = 3;
			break;
		case FLIGHT_STICK_TYPE:
			iLabelIndex = 4;
			break;
		case BOARD_TYPE:
			iLabelIndex = 5;
			break;
		case STANDARD_TYPE:
		default:
			iLabelIndex = 0;
			break;
	}
	
	drSetSize(50, 100, 0x0000ffff, 0xff000000); //set the text color
	// maintain crosstalk
	DWORD dwColor;
	if(TRUE == m_fCrossTalk)
		dwColor = CurrColor[eBad];
	else
		dwColor = CurrColor[eGood];
	
	CRect CrossTalkRect;
	CPoint TempPt;
	TempPt.SetXY(500,30);
	CrossTalkRect.SetTopLeft(TempPt);
	TempPt.SetXY(510,40);
	CrossTalkRect.SetBottomRight(TempPt);
	CrossTalkRect.Draw(dwColor);
	drPrintf(515,30,L"CrossTalk");

	drPrintf(200,150, ControllerLabels[iLabelIndex]);

	drPrintf(500,400,L"Ver. 1.0"); // version string, should be maintained from resource
	// finally, if test is complet, draw success or failure
	if(m_TestComplete)
	{
		// check current MU Status, Gamepads fail if both slots not populated
		if(2 == MUCOUNT)
		{
				if( FALSE == m_dwSlot[0] || FALSE == m_dwSlot[1])
					m_TestComplete = eBad;
		} else if(1 == MUCOUNT)
		{
				if( FALSE == m_dwSlot[0] && FALSE == m_dwSlot[1])
				{
					m_TestComplete = eBad;
				}
		}
		if(TRUE == m_fCrossTalk)			
		{
			m_TestComplete = eBad; // fail crosstalk detected
		}

		if(eGood == m_TestComplete)
		{
			drSetSize(50, 100, 0xff00ff00, 0xff000000); //set the color
			drPrintf(240,180, L"PASSED");
			drQuad(240, 290, 0xff00ff00, 330,  220, 0xff00ff00, 340,235, 0xff00ff00, 250,300, 0xff00ff00);
			drQuad(250, 300, 0xff00ff00, 260,  290, 0xff00ff00, 230,260, 0xff00ff00, 220,270, 0xff00ff00);
		} else
		{
			drSetSize(50, 100, 0xffff0000, 0xff000000); //set the color
			drQuad(230, 295, 0xffff0000, 335,  220, 0xffff0000, 340, 225, 0xffff0000, 235, 300, 0xffff0000);
			drQuad(335, 300, 0xffff0000, 340,  295, 0xffff0000, 235, 220, 0xffff0000, 230, 225, 0xffff0000);
		}
	}


}


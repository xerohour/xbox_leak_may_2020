//-------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved
// Author: Natalyp
//-------------------------------------------------------------
#include "std.h"
#include "AutomationMsg.h"


//-------------------------------------------------------------
// Function name	: CAutomationMsg::Dump
// Description	    : dump all the data
// Return type		: void 
// Argument         : bool bAll/*=true*/
//-------------------------------------------------------------
void CAutomationMsg::Dump(bool bAll/*=true*/)
{
	switch (m_byMessageId)
	{
		case eControllerInput:
			DbgPrint("CAutomationMsg: ControllerInput message\n==============================\n");			
			DbgPrint("wLastButtons=[%d]\n", m_Data.xbGamepad.wLastButtons);
			DbgPrint("wPressedButtons=[%d]\n", m_Data.xbGamepad.wPressedButtons);
			DbgPrint("==============================\n");			
			break;

		case eHeartBit:
			DbgPrint("CAutomationMsg: HeartBit message\n==============================\n");
			break;
		default:
			// unknown message
			ASSERT(false);
	}
}


//-------------------------------------------------------------
// Function name	: CAutomationMsg::GetSize
// Description	    : get the actual size of the message
// Return type		: int 
//-------------------------------------------------------------
int CAutomationMsg::GetSize()
{
	switch( m_byMessageId )
    {
	case eControllerInput:
		return sizeof(XBGAMEPAD)+ sizeof(m_byMessageId);
		break;
	case eHeartBit:
		return sizeof(m_byMessageId);
		break;
	default: 
		// unknown message 
		ASSERT(false);
		return -1;
		break;
	}
}


//-------------------------------------------------------------
// Function name	: CAutomationMsg::SetGamepadData
// Description	    : 
// Return type		: void 
// Argument         : XBGAMEPAD* pGamePad
//-------------------------------------------------------------
void CAutomationMsg::SetGamepadData(XBGAMEPAD* pGamePad)
{
	// only this message includes valid data for the gamepad structure
	ASSERT(m_byMessageId == eControllerInput);
	*pGamePad = m_Data.xbGamepad;
}
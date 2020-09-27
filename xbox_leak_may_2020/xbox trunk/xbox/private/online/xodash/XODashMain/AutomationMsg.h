//-------------------------------------------------------------
// Copyright (c) Microsoft Corporation.  All rights reserved
// Author: Natalyp
//-------------------------------------------------------------

#pragma once
#include "std.h"
#include "XBInput.h"

enum  eAutoMessage
{
	eControllerInput=1,  // Substitute pereferal input when Automation simulator is connected to the Dash
	eHeartBit,    // mechanism to maintain connection open. Only one simulataneous client is allowed to connect
	// to the Dash. Although it's valid to set multiple bits in XBGAMEPAD structure to simulate input from a 
	// a number of controllers simulteneously
	eNullMsg // for server only, not to be used by client
};


class CSocket;

class CAutomationMsg
{
	friend CSocket;
public:

    explicit CAutomationMsg( BYTE byMessageId = 0 ) : m_byMessageId( byMessageId ) {}
    ~CAutomationMsg() {}
    inline int  GetMaxSize() const { return sizeof(*this); }
	int GetSize();
    inline int GetId() const      { return m_byMessageId; }
	void Dump(bool bAll =true);
	void SetGamepadData(XBGAMEPAD* pGamePad);
	
private:

	char m_byMessageId;
	union Data
	{
		XBGAMEPAD xbGamepad;
	} m_Data;
	bool IsFiltered ()
	{
		if (eControllerInput == m_byMessageId)
		{
			return false;
		}
		return true;
	}
};
/////////////////////////////////////////////////////////////////////////////
// Copyright © 2001 Microsoft.  ALL RIGHTS RESERVED
// Programmer: Sean Wohlgemuth (mailto:seanwo@microsoft.com)
/////////////////////////////////////////////////////////////////////////////

#ifndef __CHANNELS_H_
#define __CHANNELS_H_

#ifndef _WINDOWS_
#include <windows.h>
#endif

#include "qtest.h"

//Channel structure
struct Channel{
	Iengine* pEngine;  //Interface to QTEST engine
	IengineEx* pEngineEx;  //Extended Interface to QTEST engine
	bool fUp;  //Is the channel UP?
};

/////////////////////////////////////////////////////////////////////////////
//  Creates and initialize an array of QTEST Interfaces (Channels)
/////////////////////////////////////////////////////////////////////////////

class Channels{

public:

	//Constructor
	Channels(UINT uicChannels);

	//Constructor Initialized?
	bool IsInit(){return m_fInit;}

	//Destructor
	~Channels();

	//Access to number of channels
	UINT GetNumOfChannels(){return m_uicChannels;}

public:

	Channel* m_rgChannels; //Channels

private:
	UINT m_uicChannels;  //Number of channels
	bool m_fInit;  //Constructor initialized?

};//endclass

#endif
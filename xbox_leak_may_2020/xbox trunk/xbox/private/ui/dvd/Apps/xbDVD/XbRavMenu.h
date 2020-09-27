////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2000-2001 STMicroelectronics, Inc.  All Rights Reserved.         //
// HIGHLY CONFIDENTIAL INFORMATION:  This source code contains                //
// confidential and proprietary information of STMicroelectronics, Inc.       //
// This source code is provided to Microsoft Corporation under a written      //
// confidentiality agreement between STMicroelectronics and Microsoft.  This  //
// software may not be reproduced, distributed, modified, disclosed, used,    //
// displayed, stored in a retrieval system or transmitted in whole or in part,//
// in any form or by any means, electronic, mechanical, photocopying or       //
// otherwise, except as expressly authorized by STMicroelectronics. THE ONLY  //
// PERSONS WHO MAY HAVE ACCESS TO THIS SOFTWARE ARE THOSE PERSONS             //
// AUTHORIZED BY RAVISENT, WHO HAVE EXECUTED AND DELIVERED A                  //
// WRITTEN CONFIDENTIALITY AGREEMENT TO STMicroelectronics, IN THE FORM       //
// PRESCRIBED BY STMicroelectronics.                                          //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#ifndef _XBRAVMENU_
#define _XBRAVMENU_

#pragma warning(disable : 4786)
#pragma warning(disable : 4018)
#pragma warning(disable : 4245)

#include <XBApp.h>
#include <XMenu.h>
#include <vector>
#include <map>

#define RAV_COLORKEY	D3DCOLOR_XRGB(0xFF, 0x80, 0xd0)

typedef struct _XBMenuProfile
{
	WCHAR	title[MAX_PATH];
	DWORD	maxitems;
	DWORD	flags;
	bool	hideparent;
	bool	showbackground;
	bool	showhighlight;
	bool	showtextedge;
	DWORD	topcolor;
	DWORD	bottomcolor;
	DWORD	itemcolor;
	DWORD	seltopcolor;
	DWORD	selbotcolor;
} XBMenuProfile;

using namespace std;

class XBRavMenu : public XMenu
{
private:
	vector <XMenuItem*> m_pItems;

	void UpdateSize(void);
	bool m_bHasTitle;
	bool m_bShowBackground;
	bool m_bShowHighlight;
	bool m_bHideParent;
	bool m_bShowTextEdge;
	void Initialize(WCHAR* title, float x, float y, DWORD maxitems, DWORD flags, DWORD *abortroutine,
						DWORD topcolor, DWORD bottomcolor, DWORD itemcolor, DWORD seltopcolor, DWORD selbotcolor, XBRavMenu * parent = NULL);
	void DrawRect(float x, float y, float w, float h, DWORD dwTopColor, DWORD dwBottomColor);

public:

	XBRavMenu(XBMenuProfile* profile, float x, float y, DWORD *abortroutine, XBRavMenu * parent = NULL);
	XBRavMenu(WCHAR * title, float x, float y, DWORD maxitems, DWORD flags, DWORD *abortroutine, XBRavMenu * parent = NULL);
	XBRavMenu(WCHAR * title, float x, float y, DWORD maxitems, DWORD flags, DWORD *abortroutine,
			  DWORD topcolor, DWORD bottomcolor, DWORD itemcolor, DWORD seltopcolor, DWORD selbotcolor, XBRavMenu * parent = NULL);
	~XBRavMenu(void);
	void AddItem(DWORD flags, WCHAR *string, void *action);
	void AddItem(WCHAR *string, void *action);
	bool DeleteItem(XMenuItem*);
	bool DeleteItem(int);
	int GetItemIndex(XMenuItem* item);
	XMenuItem* GetItem(int item);

	float GetX()
	{
		return this->x;
	}

	float GetY()
	{
		return this->y;
	}

	float GetW()
	{
		return this->w;
	}

	float GetH()
	{
		return this->h;
	}

	float GetBodyY()
	{
		if(m_bHasTitle)
			return this->y + (1.0f * (this->h / (nitems + 1.0f)));
		else
			return GetY();
	}

	float GetBodyH()
	{
		return this->h - (GetBodyY() - GetY());
	}

	void Activate(bool act = true);
	void Render(void);
	static bool IsActive(void);
	DWORD ProcessGamePadInput(XBGAMEPAD *gamepad);
	DWORD ProcessCommand(DWORD command);

	static void SetFont(CXBFont*);

	DWORD Routine(DWORD command);

	// static information for all menus
	static CXBFont* m_pFont;
	static bool m_pFontLoaded;

	// static information for the current active menu.
	static XBRavMenu * m_pCurMenu;
	static int (*m_pCurRoutine)(DWORD cmd, XMenuItem *mi);

	static XBRavMenu * GetActiveMenu(void);
};

#endif //_XBRAVMENU_

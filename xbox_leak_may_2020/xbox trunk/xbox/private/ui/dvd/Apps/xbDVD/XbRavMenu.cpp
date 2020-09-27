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


#include "library/common/prelude.h"
#include "XBRavMenu.h"

#define DEFAULT_MENU_FLAGS	0

CXBFont * XBRavMenu::m_pFont = NULL;
bool XBRavMenu::m_pFontLoaded = false;
XBRavMenu * XBRavMenu::m_pCurMenu = NULL;
int (*XBRavMenu::m_pCurRoutine)(DWORD cmd, XMenuItem *mi) = NULL;

typedef 	vector<XMenuItem*>::iterator itemIt;

void XBRavMenu::Initialize(WCHAR* title, float x, float y, DWORD maxitems, DWORD flags, DWORD *abortroutine,
						DWORD topcolor, DWORD bottomcolor, DWORD itemcolor, DWORD seltopcolor, DWORD selbotcolor, XBRavMenu * parent)
{
	this->maxitems = 0;
	this->maxshow = 0;
	this->nitems = 0;
	this->curitem = 0;
//	this->parent = 0;
	this->parent = parent;
	this->flags = flags;
	this->abortroutine = (DWORD(*)(DWORD, XMenuItem *)) abortroutine;
	this->x = x;
	this->y = y;
	this->w = 100.0f;
	this->topcolor = topcolor;
	this->bottomcolor = bottomcolor;
	this->itemcolor = itemcolor;
	this->seltopcolor = seltopcolor;
	this->selbotcolor = selbotcolor;

	if(title != NULL)
	{
		XMenu_SetTitle(this, title, seltopcolor);
		this->m_bHasTitle = true;
	}
	else
		this->m_bHasTitle = false;

	XMenu_SetMaxShow(this, this->maxitems);		// sets height
}

XBRavMenu::XBRavMenu(XBMenuProfile* profile, float x, float y, DWORD *abortroutine, XBRavMenu * parent) :
	m_bShowBackground(profile->showbackground),
	m_bShowHighlight(profile->showhighlight),
	m_bHideParent(profile->hideparent),
	m_bShowTextEdge(profile->showtextedge)
{
	Initialize(profile->title,x,y,profile->maxitems,profile->flags,abortroutine,profile->topcolor,profile->bottomcolor,profile->itemcolor,profile->seltopcolor,profile->selbotcolor,parent);
}

XBRavMenu::XBRavMenu(WCHAR * title, float x, float y, DWORD maxitems, DWORD flags, DWORD *abortroutine, XBRavMenu * parent) :
	m_bShowBackground(false),
	m_bShowHighlight(false)
{
//	Initialize(title,x,y,maxitems,flags,abortroutine,0xffffffff,0xffffffff,0xff000000,0xffcedfad,0xff84b642);
	m_bHideParent = true;

	Initialize(title,x,y,maxitems,flags,abortroutine,RAV_COLORKEY,RAV_COLORKEY,0xff333333,0xffffffff,0xffffffff,parent);
//	Initialize(title,x,y,maxitems,flags,abortroutine,0x50cedfad,0x5084b642,0xa0111111,0xffffffff,0xffffffff);
}

XBRavMenu::XBRavMenu(WCHAR * title, float x, float y, DWORD maxitems, DWORD flags, DWORD *abortroutine,
					 DWORD topcolor, DWORD bottomcolor, DWORD itemcolor, DWORD seltopcolor, DWORD selbotcolor, XBRavMenu * parent) :
	m_bShowBackground(true),
//	m_bShowBackground(false),
	m_bShowHighlight(true)
{
	if(parent != NULL)
		m_bHideParent = false;
	else
		m_bHideParent = true;

	Initialize(title,x,y,maxitems,flags,abortroutine,topcolor,bottomcolor,itemcolor,seltopcolor,selbotcolor,parent);
}

XBRavMenu::~XBRavMenu(void)
{
	for(itemIt it = m_pItems.begin(); it != m_pItems.end(); )
	{
//		this->DeleteItem(*it);
		XMenuItem * item = *it;
		m_pItems.erase(it);
		delete item;
		this->nitems--;
	}
}

XBRavMenu * XBRavMenu::GetActiveMenu()
{
	return	 XBRavMenu::m_pCurMenu;
}

void XBRavMenu::UpdateSize()
{
	if(m_pItems.size() <= 14)
	{
//		XMenu_SetMaxShow(this,m_pItems.size() + 2*m_bHasTitle);
		XMenu_SetMaxShow(this,m_pItems.size() + m_bHasTitle);
	}
	else
	{
//		XMenu_SetMaxShow(this, 14 + 2*m_bHasTitle );
		XMenu_SetMaxShow(this, 14 + m_bHasTitle );
	}
}

void XBRavMenu::AddItem(DWORD flags, WCHAR *string, void *action)
{
	XMenuItem *mi = new XMenuItem;

	mi->flags = flags;
	mi->action = action;
	mi->val1 = 0;
	mi->val2 = 0;
	mi->menu = this;
	mi->color = this->itemcolor;
	XMenu_SetItemText(mi, (WCHAR*) string);

	this->nitems++;

	m_pItems.push_back(mi);
	this->UpdateSize();
}

void XBRavMenu::AddItem(WCHAR *string, void *action)
{
	DWORD flags = DEFAULT_MENU_FLAGS;
	AddItem(flags, string, action);
}

bool XBRavMenu::DeleteItem(XMenuItem* item)
{
	bool found = false;
	int index = 0;

	for(itemIt it = m_pItems.begin(); it != m_pItems.end(); it++)
	{
		if(*it == item)
		{
			DeleteItem(index);
			found = true;
		}

		index++;
	}

	this->UpdateSize();

	return found;
}

bool XBRavMenu::DeleteItem(int item)
{
	if(item >= m_pItems.size())
		return false;

	delete *(m_pItems.begin() + item);
	m_pItems.erase(m_pItems.begin() + item);
	this->nitems--;
	return true;
}

int XBRavMenu::GetItemIndex(XMenuItem* item)
{
	int count = 0;
	for(itemIt it = m_pItems.begin(); it != m_pItems.end(); it++)
	{
		if(*it == item)
			return count;
		count++;
	}
	return -1;
}

XMenuItem* XBRavMenu::GetItem(int item)
{
	if(item < m_pItems.size())
	{
		return m_pItems[item];
	}
	else
	{
		return NULL;
	}
}


void XBRavMenu::Activate(bool activate)
{
	if(activate)
	{
		m_pCurMenu = this;
	}
	else // deActivate
	{
		if(m_pCurMenu && m_pCurMenu->parent && !(flags&MENU_NOBACK))
		{
			m_pCurMenu = (XBRavMenu*) m_pCurMenu->parent;
		}
		else
			m_pCurMenu = NULL;
	}
}

bool XBRavMenu::IsActive()
{
	return (XBRavMenu::m_pCurMenu != NULL);
}

DWORD XBRavMenu::ProcessGamePadInput(XBGAMEPAD *gamepad)
{
	DWORD command;
	static int count = 0;

	// get menu command
//	if(XMenu_CurMenu || XMenu_CurRoutine)
//	{
		if(gamepad->bPressedAnalogButtons[XINPUT_GAMEPAD_A] && !(gamepad->wPressedButtons & XINPUT_GAMEPAD_LEFT_THUMB))
			command = MENU_SELECTITEM;
		else if(gamepad->bPressedAnalogButtons[XINPUT_GAMEPAD_WHITE])
			command = MENU_ACTIVATE;
		else if(gamepad->wPressedButtons & XINPUT_GAMEPAD_BACK)
			command = MENU_BACK;
		else if(gamepad->fY1>0.25f ||
			gamepad->wPressedButtons & XINPUT_GAMEPAD_DPAD_UP ||
			gamepad->wPressedButtons & XINPUT_GAMEPAD_DPAD_LEFT ||
			((gamepad->wLastButtons & XINPUT_GAMEPAD_DPAD_UP) && ((count % 10) == 0)) ||
			((gamepad->wLastButtons & XINPUT_GAMEPAD_DPAD_LEFT) && ((count % 10) == 0)))
			command = MENU_PREVITEM;
		else if(gamepad->fY1<-0.25f ||
			gamepad->wPressedButtons & XINPUT_GAMEPAD_DPAD_DOWN ||
			gamepad->wPressedButtons & XINPUT_GAMEPAD_DPAD_RIGHT ||
			((gamepad->wLastButtons & XINPUT_GAMEPAD_DPAD_DOWN) && ((count % 10) == 0)) ||
			((gamepad->wLastButtons & XINPUT_GAMEPAD_DPAD_RIGHT) && ((count % 10) == 0)))
			command = MENU_NEXTITEM;
//	}
//	else if(gamepad->bPressedAnalogButtons[XINPUT_GAMEPAD_X])
//		command = MENU_ACTIVATE;
	else
		command = 0;

//	ZeroMemory(gamepad, sizeof(XBGAMEPAD));

//	for(int i = 0; i<8; i++)
//		{
//		gamepad->bPressedAnalogButtons[i] = 0;
//		}
//
//	gamepad->wPressedButtons = 0;

	return command;
}

DWORD XBRavMenu::ProcessCommand(DWORD command)
{
	return this->Routine(command);
}

void XBRavMenu::SetFont(CXBFont* m_pFont)
{
	XBRavMenu::m_pFont = m_pFont;
	XBRavMenu::m_pFontLoaded = true;
}

DWORD XBRavMenu::Routine(DWORD command)
{
	static float menutime = 0.0f;
	float time;
	XBRavMenu *m;
	XMenuItem *mi;
	DWORD status;

	// check for menu routine first
	if(XBRavMenu::m_pCurRoutine)
	{
		status = XBRavMenu::m_pCurRoutine(command, NULL);
		if(status==MROUTINE_DIE)
		{
			XBRavMenu::m_pCurRoutine = NULL;
		}
//		return 0;
	}

	m = XBRavMenu::m_pCurMenu;
	if(!m)
		return 0;
//	else
//		m->Render();

	time = XBUtil_Timer(TIMER_GETABSOLUTETIME);

	if(m->curitem >= m->nitems)
	{
		m->curitem = m->nitems - 1;
	}
	else if(m->curitem < 0)
	{
		m->curitem = 0;
	}

	int looped = 0;

	switch(command)
	{
		case MENU_NEXTITEM:
			if((time-menutime)<0.2f)
				return 0;
			menutime = time;

			if(m->m_pItems.size() == 0)
				break;

			do
			{
				m->curitem++;
				if(m->curitem==m->nitems)
					if(m->flags&MENU_WRAP)
					{
						looped++;
						m->curitem = 0;
					}
					else
					{
						looped++;
						m->curitem = m->nitems-1;
					}

				if( looped >= 2 )
				{
					break;
				}

			} while(m->m_pItems[m->curitem]->flags&(MITEM_SEPARATOR|MITEM_DISABLED));
			break;

		case MENU_PREVITEM:
			if((time-menutime)<0.2f)
				return 0;
			menutime = time;

			if(m->m_pItems.size() == 0)
				break;

			do
			{
				m->curitem--;
				if(m->curitem==-1)
					if(m->flags&MENU_WRAP)
					{
						looped++;
						m->curitem = m->nitems-1;
					}
					else
					{
						looped++;
						m->curitem = 0;
					}

				if( looped >= 2 )
				{
					break;
				}

			} while(m->m_pItems[m->curitem]->flags&(MITEM_SEPARATOR|MITEM_DISABLED));

			menutime = time;
			break;

		case MENU_SELECTITEM:
			if(m->m_pItems.size() == 0 || m->m_pItems[m->curitem]->flags&(MITEM_SEPARATOR|MITEM_DISABLED))
			{
				break;
			}

			mi = m->m_pItems[m->curitem];

			if(mi->flags&MITEM_ROUTINE)
			{
				// do routine
				XBRavMenu::m_pCurRoutine = (int (*)(DWORD, XMenuItem *))mi->action;
				status = XBRavMenu::m_pCurRoutine(MROUTINE_INIT, mi);

				// check return value...
				if(status==MROUTINE_DIE)
				{
					m->Activate(false);			// kill the current menu
					XBRavMenu::m_pCurRoutine = NULL;		// and the routine
					if(m->parent)							// activate parent menu if it exists
						((XBRavMenu*)m->parent)->Activate();
					delete m;
				}
				else if(status==MROUTINE_RETURN)	// just kill the routine
					XBRavMenu::m_pCurRoutine = NULL;
				else if(status==MROUTINE_SLEEP)
					m->Activate(false);			// just kill the current menu
			}
			else
			{
				// activate next menu if it exists
				if(mi->action)
				{
					((XBRavMenu*)mi->action)->Activate();
					XBRavMenu::m_pCurMenu->parent = m;		// the menu that called me
				}
				else
				{
					if(m->abortroutine)
						m->abortroutine(MROUTINE_ABORT, NULL);

					m->Activate(false);			// kill the current menu
				}
			}
			break;

		case MENU_BACK:
			if(!(m->flags&MENU_NOBACK))
			{
				if(m->abortroutine)						// call abort routine if it exists
					m->abortroutine(MROUTINE_ABORT, NULL);

				if(m->parent)							// activate parent menu if it exists
					((XBRavMenu*)m->parent)->Activate();
				else
					m->Activate(false);			// kill the current menu
			}
			break;

		default:
			break;
	}

	return 1;
}

//-----------------------------------------------------------------------------
// Name: DrawRect
// Desc: Draws a gradient filled rectangle
//-----------------------------------------------------------------------------
void XBRavMenu::DrawRect(float x, float y, float w, float h, DWORD dwTopColor, DWORD dwBottomColor)
{
    static LPDIRECT3DVERTEXBUFFER8 XMenu_VB = NULL;
    struct BACKGROUNDVERTEX
	{
		D3DXVECTOR4 p;
		D3DCOLOR color;
	} *v;

    if(XMenu_VB == NULL)
        g_pd3dDevice->CreateVertexBuffer(4*sizeof(BACKGROUNDVERTEX), D3DUSAGE_WRITEONLY,
                                          0L, D3DPOOL_DEFAULT, &XMenu_VB);

    // Setup vertices for a background-covering quad
	XMenu_VB->Lock(0, 0, (BYTE **)&v, NULL);
    v[0].p = D3DXVECTOR4(x, y, 1.0f, 1.0f);
	v[0].color = dwTopColor;
    v[1].p = D3DXVECTOR4(x+w, y, 1.0f, 1.0f);
	v[1].color = dwTopColor;
    v[2].p = D3DXVECTOR4(x, y+h, 1.0f, 1.0f);
	v[2].color = dwBottomColor;
    v[3].p = D3DXVECTOR4(x+w, y+h, 1.0f, 1.0f);
	v[3].color = dwBottomColor;
	XMenu_VB->Unlock();

	// set render states
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

    // Set states
    g_pd3dDevice->SetTexture(0, NULL);
    g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
    g_pd3dDevice->SetVertexShader(D3DFVF_XYZRHW|D3DFVF_DIFFUSE);
    g_pd3dDevice->SetStreamSource(0, XMenu_VB, sizeof(BACKGROUNDVERTEX));

    // Render the quad
    g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// restore render state
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}

void XBRavMenu::Render()
{
	float menux, menuy;
	float rectx, recty, rectw, recth;
	float itemx, itemy, itemh;
	float w, h;
	DWORD i, color;
	DWORD firstitem, nitems;
	WCHAR* str;
	XMenuItem *mi;
	WCHAR ddd[11] = L"--MORE--";


	if(!XBRavMenu::m_pFontLoaded || this->m_pItems.size() == 0)
		return;

	if(!m_bHideParent && ((_xmenu*)this)->parent != NULL)
	{
		((XBRavMenu*)((_xmenu*)this)->parent)->Render();
	}

	// get height of font
	itemh = (float)XBRavMenu::m_pFont->GetFontHeight();

	// draw menu background
	rectx = this->x;
	rectw = this->w;
//	recty = this->y - itemh/2.0f;
	if(m_bHasTitle)
	{
		recty = this->y + itemh;
		recth = this->h - itemh;
	}
	else
	{
		recty = this->y;
		recth = this->h;
	}
	// Outer Rectangle Background of Menu.
	if(m_bShowBackground)
	{
		if(this->flags&(MENU_LEFT|MENU_RIGHT))
		{
//			DrawRect(rectx-8.0f, recty, rectw, recth, this->topcolor, this->bottomcolor);
			OutputDebugString("");
			DrawRect(rectx, recty, rectw, recth, this->topcolor, this->bottomcolor);
		}
		else if(m_bHideParent)
		{
			DrawRect(rectx-(rectw/2.0f), recty, rectw, recth, this->topcolor, this->bottomcolor);
		}
		else //draw to the right of parent menu
		{
			DrawRect(rectx, recty, rectw, recth, this->topcolor, this->bottomcolor);
		}
	}

	menux = this->x;
	menuy = this->y;

	if(this->nitems == this->m_pItems.size())
		this->nitems = this->m_pItems.size();

	// determine which menu items to show (firstitem, nitems)
	if(this->nitems>this->maxshow)
	{
		if(this->curitem<(this->maxshow/2))
			firstitem = 0;
		else if(this->curitem<(this->nitems-(this->maxshow/2)))
			firstitem = this->curitem-(this->maxshow/2)+(!(this->maxshow%2));
		else
			firstitem = this->nitems-this->maxshow;
	}
	else
		firstitem = 0;

	if(this->nitems>this->maxshow)
		nitems = this->maxshow;
	else
		nitems = this->nitems;

	itemy = menuy;

	// draw the menu title
	if(this->m_bHasTitle)
	{
//		itemy += itemh/2.0f;
		float h,w;

		XBRavMenu::m_pFont->GetTextExtent(this->title, &w, &h);

		if(this->flags&MENU_LEFT)
			XBRavMenu::m_pFont->DrawText(menux + 10.f, itemy, this->titlecolor, this->title);
		else if(this->flags&MENU_RIGHT)
			XBRavMenu::m_pFont->DrawText(menux + (this->GetW() - w) - 10.0f, itemy, this->titlecolor, this->title);
		else if(m_bHideParent)
			XBRavMenu::m_pFont->DrawText(menux, itemy, this->titlecolor, this->title, XBFONT_CENTER_X);
		else
			XBRavMenu::m_pFont->DrawText(menux, itemy, this->titlecolor, this->title);

//		itemy += 1.5f*itemh;
//		itemy += 2.0f*itemh * 1.1f;
		itemy += itemh * 1.1f;
	}

	// draw the menu items
	for(i=firstitem; i<nitems+firstitem; i++)
	{
		mi = this->m_pItems[i];

		if(!(mi->flags&MITEM_SEPARATOR))
		{
			str = mi->string;
			if(this->nitems>this->maxshow)
			{
				if((i==firstitem) && (firstitem!=0))
					str = ddd;
				if((i==(firstitem+nitems-1)) && (firstitem!=(this->nitems-this->maxshow)))
					str = ddd;
			}

			//Rectangle around highlighted item
			if(m_bShowHighlight)
			{

//				rectx = this->x;

				if(i==this->curitem)
				{
					if(this->flags&(MENU_LEFT|MENU_RIGHT))
					{
						DrawRect(rectx, itemy, rectw, XBRavMenu::m_pFont->GetFontHeight()+2.0f, this->seltopcolor, this->selbotcolor);
					}
					else if(m_bHideParent)
					{
						OutputDebugString("");
						DrawRect(rectx-(rectw/2.0f), itemy, rectw, XBRavMenu::m_pFont->GetFontHeight()+2.0f, this->seltopcolor, this->selbotcolor);
					}
					else
					{
						DrawRect(rectx, itemy, rectw, XBRavMenu::m_pFont->GetFontHeight()+2.0f, this->seltopcolor, this->selbotcolor);
//						OutputDebugString("showing parent, not centering curitem highlight.\n");
					}
				}
			}

			DWORD negColor;
			float edgeSize;
			if(mi->flags&MITEM_DISABLED)
				{
//				color = (mi->color&0xffffff) | 0x40000000;	// lower alpha
				color = 0xff111111;// & 0xffffff);// | 0x8000000;	// lower alpha
//				negColor = 0xffeeeeee;
				negColor = color;
//				edgeSize = 1.0f;
				edgeSize = 0.5f;
//				edgeSize = 0.0f;
				}
			else if(i==this->curitem)
				{
				color = (0xffffffff - this->seltopcolor) | 0xff000000;
//				color = (0xffffffff - this->seltopcolor) | 0xff000000;
				negColor = 0xff646464;
//				negColor = (0xffffffff - color) | 0xff000000;
				edgeSize = 1.0f;
//				edgeSize = 0.5f;
//				edgeSize = 0.0f;
				}
			else
				{
				color = mi->color;
//				color = RAV_COLORKEY;
//				negColor = ( (this->seltopcolor / 2) + (mi->color / 2) ) | 0xff000000;
				negColor = 0xff646464;
//				edgeSize = 1.0f;
				edgeSize = 0.5f;
//				edgeSize = 0.0f;
				}

			if(this->flags&MENU_LEFT)
			{
				itemx = menux + 10.0f;
				if(m_bShowTextEdge)
				{
					XBRavMenu::m_pFont->DrawText(itemx - edgeSize, itemy - edgeSize, negColor, str);
					XBRavMenu::m_pFont->DrawText(itemx + edgeSize, itemy + edgeSize, negColor, str);
					XBRavMenu::m_pFont->DrawText(itemx - edgeSize, itemy + edgeSize, negColor, str);
					XBRavMenu::m_pFont->DrawText(itemx + edgeSize, itemy - edgeSize, negColor, str);
				}
				XBRavMenu::m_pFont->DrawText(itemx, itemy, color, str);
			}
			else if(this->flags&MENU_RIGHT)
			{
				XBRavMenu::m_pFont->GetTextExtent(str, &w, &h);
				itemx = menux + (this->GetW() - w) - 10.0f;
				if(m_bShowTextEdge)
				{
					XBRavMenu::m_pFont->DrawText(itemx - edgeSize, itemy - edgeSize, negColor, str);
					XBRavMenu::m_pFont->DrawText(itemx + edgeSize, itemy + edgeSize, negColor, str);
					XBRavMenu::m_pFont->DrawText(itemx - edgeSize, itemy + edgeSize, negColor, str);
					XBRavMenu::m_pFont->DrawText(itemx + edgeSize, itemy - edgeSize, negColor, str);
				}
				XBRavMenu::m_pFont->DrawText(itemx, itemy, color, str);
			}
			else if(m_bHideParent)
			{
				XBRavMenu::m_pFont->GetTextExtent(str, &w, &h);
				itemx = menux;
//				itemx = menux-(w/2.0f);
				if(m_bShowTextEdge)
				{
					XBRavMenu::m_pFont->DrawText(itemx - edgeSize, itemy - edgeSize, negColor, str, XBFONT_CENTER_X);
					XBRavMenu::m_pFont->DrawText(itemx + edgeSize, itemy + edgeSize, negColor, str, XBFONT_CENTER_X);
					XBRavMenu::m_pFont->DrawText(itemx - edgeSize, itemy + edgeSize, negColor, str, XBFONT_CENTER_X);
					XBRavMenu::m_pFont->DrawText(itemx + edgeSize, itemy - edgeSize, negColor, str, XBFONT_CENTER_X);
				}
				XBRavMenu::m_pFont->DrawText(itemx, itemy, color, str, XBFONT_CENTER_X);
			}
			else
			{
				XBRavMenu::m_pFont->GetTextExtent(str, &w, &h);
				itemx = menux+(this->GetW()/2.0f);
//				itemx = menux;
				if(m_bShowTextEdge)
				{
					XBRavMenu::m_pFont->DrawText(itemx - edgeSize, itemy - edgeSize, negColor, str, XBFONT_CENTER_X);
					XBRavMenu::m_pFont->DrawText(itemx + edgeSize, itemy + edgeSize, negColor, str, XBFONT_CENTER_X);
					XBRavMenu::m_pFont->DrawText(itemx - edgeSize, itemy + edgeSize, negColor, str, XBFONT_CENTER_X);
					XBRavMenu::m_pFont->DrawText(itemx + edgeSize, itemy - edgeSize, negColor, str, XBFONT_CENTER_X);
				}
				XBRavMenu::m_pFont->DrawText(itemx, itemy, color, str, XBFONT_CENTER_X);
			}
		}

		itemy += XBRavMenu::m_pFont->GetFontHeight() * 1.1f;
	}

}

void XMenu_SetMaxShow(XMenu *m, DWORD maxshow)
{
	m->maxshow = maxshow;
	if(m->nitems > maxshow)
		m->h = (float)(m->maxshow+1) * XBRavMenu::m_pFont->GetFontHeight() * 1.1f;
	else
		m->h = (float)(m->maxshow) * XBRavMenu::m_pFont->GetFontHeight() * 1.1f;
}

void XMenu_SetTitle(XMenu *m, WCHAR *string, DWORD color)
{
	float w, h;

	m->titlecolor = color;

	if(string)
	{
		// set string
//		_tcsncpy(m->title, string, MITEM_STRINGLEN-1);
		wcsncpy(m->title, string, MITEM_STRINGLEN-1);

		// calculate new menu width based on this item
		XBRavMenu::m_pFont->GetTextExtent(string, &w, &h);
		if(m->w<(w+16.0f))
		{
			if(w < 200)
			{
				m->w = w+16.0f;
			}
			else
			{
				m->w = 216;
			}
		}

		m->h = (float)(m->maxshow+3) * XBRavMenu::m_pFont->GetFontHeight();
	}
	else
		m->h = (float)(m->maxshow+1) * XBRavMenu::m_pFont->GetFontHeight();
}

void XMenu_SetItemText(XMenuItem *mi, WCHAR *string)
{
	float w, h;

	if(!string || (mi->flags&MITEM_SEPARATOR))
		return;

	// set string
//	_tcsncpy(mi->string, string, MITEM_STRINGLEN-1);
	wcsncpy(mi->string, string, MITEM_STRINGLEN-1);

	// calculate new menu width based on this item
	XBRavMenu::m_pFont->GetTextExtent(string, &w, &h);
	if(mi->menu->w<(w+16.0f))
		mi->menu->w = w+16.0f;
}

#include <xtl.h>
#include "assert.h"
#include "XBUtil.h"
#include "XBInput.h"
#include "xmenu.h"

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

CXBFont *XMenu_Font = NULL;				// pointer to font
DWORD XMenu_FontLoaded = FALSE;			// font loaded flag
XMenu *XMenu_CurMenu = NULL;
int (*XMenu_CurRoutine)(DWORD cmd, XMenuItem *mi);			// called when item selected

void DrawRect(float x, float y, float w, float h, DWORD dwTopColor, DWORD dwBottomColor);

void XMenu_SetFont(CXBFont *font)
{
	XMenu_Font = font;
}

XMenu *XMenu_Init(float x, float y, DWORD maxitems, DWORD flags, DWORD (*abortroutine)(DWORD, XMenuItem *))
{
	XMenu *m;
	BYTE *mem;

	assert(XMenu_Font!=NULL);				// make sure font is set

	// allocate memory for menu
	mem = new BYTE[sizeof(XMenu) + maxitems*sizeof(XMenuItem)];
	memset(mem, 0, sizeof(XMenu) + maxitems*sizeof(XMenuItem));

	m = (XMenu *)mem;
	m->items = (XMenuItem *)(mem+sizeof(XMenu));
	m->flags = flags;
	m->topcolor = 0xff000000;
	m->bottomcolor = 0xff0000c0;
	m->seltopcolor = 0xff000000;
	m->selbotcolor = 0xffc00000;
	m->maxitems = maxitems;
	m->abortroutine = abortroutine;
	m->itemcolor = 0xffffffff;

	m->x = x;
	m->y = y;
	m->w = 100.0f;

	XMenu_SetMaxShow(m, m->maxitems);		// sets height

	return m;
}

void XMenu_Delete(XMenu *m)
{
	delete m;
}

void XMenu_SetMaxShow(XMenu *m, DWORD maxshow)
{
	m->maxshow = maxshow;
	m->h = (float)(m->maxshow+1) * XMenu_Font->GetFontHeight();
}

XMenuItem *XMenu_AddItem(XMenu *m, DWORD flags, WCHAR *string, void *action)
{
	XMenuItem *mi;

	assert(m->nitems<m->maxitems);		// make sure we dont overstep our allocation
	mi = &m->items[m->nitems];

	mi->flags = flags;
	mi->action = action;
	mi->val1 = 0;
	mi->val2 = 0;
	mi->menu = m;
	mi->color = m->itemcolor;
	XMenu_SetItemText(&m->items[m->nitems], string);

	m->nitems++;

	return &m->items[m->nitems-1];
}

void XMenu_DeleteItem(XMenuItem *mi)
{
	XMenu *m;
	DWORD i;
	
	// get pointer to the menu that owns this item
	m = mi->menu;

	// find the item to delete
	for(i=0; i<m->nitems; i++)
		if(&m->items[i]==mi)
		{
			// shift em down
			memcpy(&m->items[i], &m->items[i+1], (m->nitems-i-1)*sizeof(XMenuItem));
			m->nitems--;
			break;
		}
}

void XMenu_SetTitle(XMenu *m, WCHAR *string, DWORD color)
{
	float w, h;

	m->titlecolor = color;

	if(string)
	{
		// set string
		wcsncpy(m->title, string, MITEM_STRINGLEN-1);

		// calculate new menu width based on this item
		XMenu_Font->GetTextExtent(string, &w, &h);
		if(m->w<(w+16.0f))
			m->w = w+16.0f;
	
		m->h = ((float)m->maxshow+2.5f) * XMenu_Font->GetFontHeight();
	}
	else
		m->h = (float)(m->maxshow+1) * XMenu_Font->GetFontHeight();
}

void XMenu_SetItemText(XMenuItem *mi, WCHAR *string)
{
	float w, h;

	if(!string || (mi->flags&MITEM_SEPARATOR))
		return;

	// set string
	wcsncpy(mi->string, string, MITEM_STRINGLEN-1);

	// calculate new menu width based on this item
	XMenu_Font->GetTextExtent(string, &w, &h);
	if(mi->menu->w<(w+16.0f))
		mi->menu->w = w+16.0f;
}

DWORD XMenu_Activate(XMenu *m)
{
	XMenu_CurMenu = m;

	return 1;
}

DWORD XMenu_GetCommand(XBGAMEPAD *gamepad)
{
	DWORD command;

	command = 0;

	// get menu command
	if(XMenu_CurMenu || XMenu_CurRoutine)
	{
		if(gamepad->bPressedAnalogButtons[XINPUT_GAMEPAD_A] ||
		   gamepad->wPressedButtons&XINPUT_GAMEPAD_START)
			command = MENU_SELECTITEM;
		else if(gamepad->bPressedAnalogButtons[XINPUT_GAMEPAD_B] ||
		   gamepad->wPressedButtons&XINPUT_GAMEPAD_BACK)
			command = MENU_BACK;
		else if(gamepad->fY1>0.25f)
			command = MENU_PREVITEM;
		else if(gamepad->fY1<-0.25f)
			command = MENU_NEXTITEM;
	}
	else if(gamepad->wPressedButtons&XINPUT_GAMEPAD_START)
		command = MENU_ACTIVATE;

	return command;
}

DWORD XMenu_Routine(DWORD command)
{
	float menux, menuy;
	float rectx, recty, rectw, recth;
	float itemy, itemh;
	DWORD i, color;
	static float menutime = 0.0f;
	float time;
	XMenu *m;
	XMenuItem *mi;
	DWORD firstitem, nitems;
	WCHAR ddd[16] = L"[--MORE--]";
	WCHAR *str;
	DWORD status;

	// check for menu routine first
	if(XMenu_CurRoutine)
	{
		status = XMenu_CurRoutine(command, NULL);
		if(status==MROUTINE_DIE)
			XMenu_CurRoutine = NULL;
		return 0;
	}

	// no routine so do menu processing
	m = XMenu_CurMenu;
	if(!m)
		return 0;

	// get height of font
	itemh = (float)XMenu_Font->GetFontHeight();

	// draw menu background
	rectx = m->x;
	recty = m->y - itemh/2.0f;
	rectw = m->w;
	recth = m->h;

	if(m->flags&(MENU_LEFT|MENU_RIGHT))
		DrawRect(rectx-8.0f, recty, rectw, recth, m->topcolor, m->bottomcolor);
	else
		DrawRect(rectx-(rectw/2.0f), recty, rectw, recth, m->topcolor, m->bottomcolor);

	menux = m->x;
	menuy = m->y;

	// determine which menu items to show (firstitem, nitems)
	if(m->nitems>m->maxshow)
	{
		if(m->curitem<(m->maxshow/2))
			firstitem = 0;
		else if(m->curitem<(m->nitems-(m->maxshow/2)))
			firstitem = m->curitem-(m->maxshow/2)+1;
		else
			firstitem = m->nitems-m->maxshow;
	}
	else
		firstitem = 0;

	if(m->nitems>m->maxshow)
		nitems = m->maxshow;
	else
		nitems = m->nitems;

	itemy = menuy;

	// draw the menu title
	if(m->title[0])
	{
		if(m->flags&MENU_LEFT)
			XMenu_Font->DrawText(menux, itemy, m->titlecolor, m->title);
		else if(m->flags&MENU_RIGHT)
			XMenu_Font->DrawText(menux, itemy, m->titlecolor, m->title, XBFONT_RIGHT);
		else
			XMenu_Font->DrawText(menux, itemy, m->titlecolor, m->title, XBFONT_CENTER_X);
			
		itemy += 1.5f*itemh;
	}

	// draw the menu items
	for(i=firstitem; i<nitems+firstitem; i++)
	{
		mi = &m->items[i];

		if(!(mi->flags&MITEM_SEPARATOR))
		{
			// do menu scroll
			str = mi->string;
			if(m->nitems>m->maxshow)
			{
				if((i==firstitem) && (firstitem!=0))
					str = ddd;
				if((i==(firstitem+nitems-1)) && (firstitem!=(m->nitems-m->maxshow)))
					str = ddd;
			}

			// draw menu hilight bar
			if(i==m->curitem)
			{
				if(m->flags&(MENU_LEFT|MENU_RIGHT))
					DrawRect(rectx-8.0f, itemy-2.0f, rectw, XMenu_Font->GetFontHeight()+4.0f, m->seltopcolor, m->selbotcolor);
				else
					DrawRect(rectx-(rectw/2.0f), itemy-2.0f, rectw, XMenu_Font->GetFontHeight()+4.0f, m->seltopcolor, m->selbotcolor);
			}

			// set item color
			if(mi->flags&MITEM_DISABLED)
				color = (mi->color&0xffffff) | 0x40000000;	// lower alpha
			else
				color = mi->color;
	
			// draw the menu text
			if(!(m->flags&(MENU_LEFT|MENU_RIGHT)))
				XMenu_Font->DrawText(menux, itemy, color, str, XBFONT_CENTER_X);
			else if(m->flags&MENU_LEFT)
				XMenu_Font->DrawText(menux, itemy, color, str);
			else if(m->flags&MENU_RIGHT)
				XMenu_Font->DrawText(menux, itemy, color, str, XBFONT_RIGHT);
		}

		itemy += XMenu_Font->GetFontHeight();
	}

	time = XBUtil_Timer(TIMER_GETABSOLUTETIME);

	switch(command)
	{
		case MENU_NEXTITEM:
			if((time-menutime)<0.2f)
				return 0;
			menutime = time;

			do
			{
				m->curitem++;
				if(m->curitem==m->nitems)
					if(m->flags&MENU_WRAP)
						m->curitem = 0;
					else
						m->curitem = m->nitems-1;
			} while(m->items[m->curitem].flags&(MITEM_SEPARATOR|MITEM_DISABLED));
			break;

		case MENU_PREVITEM:
			if((time-menutime)<0.2f)
				return 0;
			menutime = time;

			do 
			{
				m->curitem--;
				if(m->curitem==-1)
					if(m->flags&MENU_WRAP)
						m->curitem = m->nitems-1;
					else
						m->curitem = 0;
			} while(m->items[m->curitem].flags&(MITEM_SEPARATOR|MITEM_DISABLED));

			menutime = time;
			break;

		case MENU_SELECTITEM:
			mi = &m->items[m->curitem];

			if(mi->flags&MITEM_ROUTINE)
			{
				// do routine
				XMenu_CurRoutine = (int (*)(DWORD, XMenuItem *))mi->action;
				status = XMenu_CurRoutine(MROUTINE_INIT, mi);

				// check return value... 
				if(status==MROUTINE_DIE)
				{
					XMenu_Activate(NULL);			// kill the current menu
					XMenu_CurRoutine = NULL;		// and the routine
				}
				else if(status==MROUTINE_RETURN)	// just kill the routine
					XMenu_CurRoutine = NULL;
				else if(status==MROUTINE_SLEEP)
					XMenu_Activate(NULL);			// just kill current menu
			}
			else
			{	
				// activate next menu if it exists
				if(mi->action)
				{
					XMenu_Activate((XMenu *)mi->action);
					XMenu_CurMenu->parent = m;		// the menu that called me
				}
				else
				{
					if(m->abortroutine)
						m->abortroutine(MROUTINE_ABORT, NULL);

					XMenu_Activate(NULL);
				}
			}
			break;

		case MENU_BACK:
			if(!(m->flags&MENU_NOBACK))
			{
				if(m->abortroutine)						// call abort routine if it exists
					m->abortroutine(MROUTINE_ABORT, NULL);

				if(m->parent)							// activate parent menu if it exists
					XMenu_Activate(m->parent);
				else
					XMenu_Activate(NULL);
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
void DrawRect(float x, float y, float w, float h, DWORD dwTopColor, DWORD dwBottomColor)
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
	XMenu_VB->Lock(0, 0, (BYTE **)&v, 0L);
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




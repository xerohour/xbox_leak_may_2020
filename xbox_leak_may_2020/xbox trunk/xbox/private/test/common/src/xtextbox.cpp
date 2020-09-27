#include <xtl.h>
#include <assert.h>

#include "xtextbox.h"

extern LPDIRECT3DDEVICE8 g_pd3dDevice;

CXBFont *XTextBox_Font=NULL;
DWORD XTextBox_FontLoaded = FALSE;
void XTextBox_DrawRect(float x, float y, float w, float h, DWORD dwTopColor, DWORD dwBottomColor);

void XTextBox_SetFont(CXBFont *font)
{
	XTextBox_Font = font;
}

XTextBox *XTextBox_Init(float x, float y, float w, DWORD maxitems, DWORD flags)
{
	XTextBox *xbox;		// pun intended
	BYTE *mem;

	assert(XTextBox_Font!=NULL);

	// bump maxitems for circular array padding
	maxitems++;

	// allocate memory for textbox
	mem = new BYTE[sizeof(XTextBox) + maxitems*sizeof(XTextBoxItem)];
	memset(mem, 0, sizeof(XTextBox) + maxitems*sizeof(XTextBoxItem));

	xbox = (XTextBox *)mem;
	xbox->items = (XTextBoxItem *)(mem+sizeof(XTextBox));
	xbox->flags = flags;

	xbox->maxitems = maxitems;
	xbox->maxshow = (maxitems-1);
	xbox->textcolor = 0xffffffff;

	xbox->topcolor = 0x40000000;
	xbox->bottomcolor = 0x40000000;

	xbox->x = x;
	xbox->y = y;
	xbox->w = w;
	xbox->h = (float)(xbox->maxshow+1)*XTextBox_Font->GetFontHeight();

	return xbox;
}

XTextBoxItem *XTextBox_AddItem(XTextBox *xbox, WCHAR *string)
{
	XTextBoxItem *item;

	if(!string)
		return NULL;

	// copy string to tail
	item = &xbox->items[xbox->tail];
	wcsncpy(item->string, string, TBOX_STRINGLEN-1);

	// set color
	item->color = xbox->textcolor;

	// set expire time
	item->expiretime = GetTickCount() + xbox->expiretime;
	
	// bump tail pointer
	xbox->tail++;
	if(xbox->tail==xbox->maxitems)
		xbox->tail = 0;

	// bump head if necessary
	if(xbox->head==xbox->tail)
	{
		xbox->head++;
		if(xbox->head==xbox->maxitems)
			xbox->head = 0;
	}

	// return pointer to the item we just added
	return item;
}

// Draw the textbox
// Only draw the background if it has color specified.
// If TBOX_POPUP is specified, only draw background if there is visible text.
void XTextBox_Display(XTextBox *xbox)
{
	DWORD idx, cnt=0;
	float itemx, itemy;

	// expire stale entries
	if(xbox->expiretime>0)
		XTextBox_Expire(xbox);

	// check for empty textbox and draw background if appropriate
	if(xbox->head==xbox->tail)
	{
		if(!(xbox->flags&TBOX_POPUP) && (xbox->topcolor!=0 && xbox->bottomcolor!=0))
			XTextBox_DrawRect(xbox->x, xbox->y, xbox->w, xbox->h, xbox->topcolor, xbox->bottomcolor);
		return;		// return if no items to display
	}
	else if(xbox->topcolor!=0 && xbox->bottomcolor!=0)
		XTextBox_DrawRect(xbox->x, xbox->y, xbox->w, xbox->h, xbox->topcolor, xbox->bottomcolor);

	// draw the text
	itemx = xbox->x + 8.0f;
	itemy = xbox->y + XTextBox_Font->GetFontHeight()*0.5f;

	idx = xbox->head;
	do
	{
		XTextBox_Font->DrawText(itemx, itemy, xbox->items[idx].color, xbox->items[idx].string);
		itemy += XTextBox_Font->GetFontHeight();
		
		// bump index & count
		idx++;
		if(idx==xbox->maxitems)
			idx = 0;
		cnt++;

	} while((idx!=xbox->tail) && (cnt<xbox->maxshow));
}

void XTextBox_Expire(XTextBox *xbox)
{
	DWORD time;

	if(xbox->head==xbox->tail)
		return;

	time = GetTickCount();

	while((time>xbox->items[xbox->head].expiretime) && (xbox->head!=xbox->tail))
	{
		xbox->head++;
		if(xbox->head==xbox->maxitems)
			xbox->head = 0;
	}
}

//-----------------------------------------------------------------------------
// Name: DrawRect
// Desc: Draws a gradient filled rectangle
//-----------------------------------------------------------------------------
void XTextBox_DrawRect(float x, float y, float w, float h, DWORD dwTopColor, DWORD dwBottomColor)
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


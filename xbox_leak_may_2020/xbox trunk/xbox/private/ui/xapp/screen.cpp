#include "std.h"
#include "xapp.h"
#include "Node.h"
#include "Screen.h"
#include "Lerper.h"

IMPLEMENT_NODE("Screen", CScreen, CNode)

START_NODE_PROPS(CScreen, CNode)
	NODE_PROP(pt_integer, CScreen, width)
	NODE_PROP(pt_integer, CScreen, height)
	NODE_PROP(pt_boolean, CScreen, fullScreen)
	NODE_PROP(pt_boolean, CScreen, showFramerate)
	NODE_PROP(pt_string, CScreen, title)
	NODE_PROP(pt_string, CScreen, icon)
	NODE_PROP(pt_string, CScreen, border)
	NODE_PROP(pt_boolean, CScreen, letterbox)
	NODE_PROP(pt_boolean, CScreen, wideScreen)
	NODE_PROP(pt_number, CScreen, brightness)
END_NODE_PROPS()


CScreen::CScreen()
{
	m_width = 640;
	m_height = 480;
	m_fullScreen = false;
	m_title = NULL;
	m_icon = NULL;
	m_border = NULL;
	m_showFramerate = false;
	m_letterbox = false;
	m_wideScreen = false;
	m_brightness = 1.0f;
	m_lastBrightness = 1.0f;

	if (theApp.m_pScreen == NULL)
		theApp.m_pScreen = this;

	m_bSizeDirty = false;
	m_bTitleDirty = false;
}

CScreen::~CScreen()
{
	delete [] m_title;
	delete [] m_icon;
	delete [] m_border;

	if (theApp.m_pScreen == this)
		theApp.m_pScreen = NULL;
}

bool CScreen::OnSetProperty(const PRD* pprd, const void* pvValue)
{
	if ((int)pprd->pbOffset == offsetof(m_width) ||
		(int)pprd->pbOffset == offsetof(m_height))
	{
		m_bSizeDirty = true;
	}
	else if ((int)pprd->pbOffset == offsetof(m_title))
	{
		m_bTitleDirty = true;
	}
	else if ((int)pprd->pbOffset == offsetof(m_brightness))
	{
		float nBrightness = *(float*)pvValue;
//		SetBrightness(*(float*)pvValue);
		CLerper::RemoveObject(this);

		if (nBrightness != 1.0f)
		{
			new CLerper(this, &m_brightness, nBrightness, 2.0f);
			return false;
		}
	}

	return CNode::OnSetProperty(pprd, pvValue);
}

void CScreen::Advance(float nSeconds)
{
	CNode::Advance(nSeconds);

	if (m_bSizeDirty)
	{
#ifdef _WINDOWS
		if (theApp.m_pScreen == this)
		{
			RECT rect;
			SetRect(&rect, 0, 0, m_width, m_height);

			DWORD dwStyle = GetWindowLong(theApp.m_hWnd, GWL_STYLE);
			DWORD dwExStyle = GetWindowLong(theApp.m_hWnd, GWL_EXSTYLE);
			AdjustWindowRectEx(&rect, dwStyle, false, dwExStyle);

			int nWidth = rect.right - rect.left;
			int nHeight = rect.bottom - rect.top;

			GetWindowRect(theApp.m_hWnd, &rect);
			int nLeft = rect.left;
			int nTop = rect.top;

			int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
			int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

			if (nLeft + nWidth > nScreenWidth)
				nLeft = (nScreenWidth - nWidth) / 2;

			if (nTop + nHeight > nScreenHeight)
				nTop = (nScreenHeight - nHeight) / 2;

			SetWindowPos(theApp.m_hWnd, NULL, nLeft, nTop, nWidth, nHeight, SWP_NOZORDER | SWP_NOACTIVATE);
		}
#endif

		m_bSizeDirty = false;
	}

	if (m_bTitleDirty)
	{
#ifdef _WINDOWS
		if (theApp.m_pScreen == this)
		{
			SetWindowText(theApp.m_hWnd, m_title);
		}
#endif

		m_bTitleDirty = false;
	}

	if (m_brightness != m_lastBrightness)
	{
		SetBrightness(m_brightness);
		m_lastBrightness = m_brightness;
	}
}

void CScreen::SetBrightness(float nBrightness)
{
#ifdef _XBOX
    D3DGAMMARAMP ramp;

	for (int i = 0; i < 256; i += 1)
	{
        ramp.red[i] = (BYTE)((float)i * nBrightness);
        ramp.green[i] = (BYTE)((float)i * nBrightness);
        ramp.blue[i] = (BYTE)((float)i * nBrightness);
	}

    XAppGetD3DDev()->SetGammaRamp(0, &ramp);
#endif
}

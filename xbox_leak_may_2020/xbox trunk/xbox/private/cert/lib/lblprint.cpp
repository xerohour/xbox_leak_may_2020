/*
 *
 * lblprint.h
 *
 * Label printing
 *
 */

#include "precomp.h"

CLabelPrinter *CLabelPrinter::PprCreate(LPCSTR szName, int nOff)
{
    CLabelPrinter *ppr;
    HANDLE hpr;
    DEVMODE *pdm;
    int cb;

    /* Get the document properties for this printer */
    if(!OpenPrinter((LPSTR)szName, &hpr, NULL))
        return NULL;
    cb = DocumentProperties(NULL, hpr, NULL, NULL, NULL, 0);
    if(cb <= 0)
        goto noprop0;
    pdm = (DEVMODE *)malloc(cb);
    if(DocumentProperties(NULL, hpr, NULL, pdm, NULL, DM_OUT_BUFFER) < 0)
        goto noprop1;
    ClosePrinter(hpr);

    /* Set up to run in landscape mode */
    pdm->dmFields = DM_ORIENTATION;
    pdm->dmOrientation = DMORIENT_LANDSCAPE;

    /* Remember what we've got */
    ppr = new CLabelPrinter;
    ppr->m_szName = _strdup(szName);
    ppr->m_pdm = pdm;
    ppr->m_yOffBase = nOff;
    return ppr;
noprop1:
    free(pdm);
noprop0:
    ClosePrinter(hpr);
    return NULL;
}

CLabelPrinter::~CLabelPrinter()
{
    if(m_hdc)
        EndLabel();
    if(m_hfont)
        DeleteObject(m_hfont);
    if(m_szName)
        free(m_szName);
    if(m_pdm)
        free(m_pdm);
}

BOOL CLabelPrinter::FStartLabel(void)
{
    DOCINFO di;
    int yPitch;

    m_hdc = CreateDC("WINSPOOL", m_szName, NULL, m_pdm);
    if(!m_hdc)
        return FALSE;

    m_iLine = 0;
    yPitch = GetDeviceCaps(m_hdc, LOGPIXELSY) / 6;
    /* See if we can figure out how big the page is */
    m_ptOff.x = GetDeviceCaps(m_hdc, PHYSICALWIDTH);
    m_ptOff.y = GetDeviceCaps(m_hdc, PHYSICALHEIGHT);
    DPtoLP(m_hdc, &m_ptOff, 1);
    m_ptOff.x = GetDeviceCaps(m_hdc, PHYSICALOFFSETX);
    m_ptOff.y = GetDeviceCaps(m_hdc, PHYSICALOFFSETY);
    DPtoLP(m_hdc, &m_ptOff, 1);

    /* If we have no cached font, or if the cached font is the wrong size,
     * we need to build a font that we'll be able to use */
    if(m_hfont && yPitch != m_yPitch) {
        DeleteObject(m_hfont);
        goto newfont;
    }
    if(!m_hfont) {
newfont:
        m_hfont = CreateFont(yPitch, 0, 0, 0, 0, FALSE, FALSE, FALSE,
            ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH, "Helvetica");
    }
    if(!m_hfont) {
        DeleteDC(m_hdc);
        return FALSE;
    }
    m_yPitch = yPitch;
    SelectObject(m_hdc, m_hfont);

    memset(&di, 0, sizeof di);
    di.cbSize = sizeof di;
    di.lpszDocName = "LABEL";
    if(StartDoc(m_hdc, &di) <= 0) {
        DeleteDC(m_hdc);
        return FALSE;
    }
    if(StartPage(m_hdc) <= 0) {
        EndDoc(m_hdc);
        DeleteDC(m_hdc);
        return FALSE;
    }
    return TRUE;
}

void CLabelPrinter::PrintLine(LPCSTR szFormat, ...)
{
    char *pchStart, *pchEnd;
    char szStr[4096];
    va_list va;

    va_start(va, szFormat);
    _vsnprintf(szStr, sizeof szStr, szFormat, va);
    va_end(va);
    szStr[sizeof szStr - 1] = 0;
    for(pchStart = szStr; *pchStart; pchStart = pchEnd) {
        for(pchEnd = pchStart; *pchEnd && *pchEnd != '\n'; ++pchEnd);
        TextOut(m_hdc, m_ptOff.x, m_ptOff.y + m_yOffBase +
            (m_iLine + 1) * m_yPitch, pchStart, pchEnd - pchStart);
        while(*pchEnd == '\n') {
            ++m_iLine;
            ++pchEnd;
        }
    }
}

void CLabelPrinter::EndLabel(void)
{
    if(m_hdc) {
        EndPage(m_hdc);
        EndDoc(m_hdc);
        DeleteDC(m_hdc);
        m_hdc = NULL;
    }
}

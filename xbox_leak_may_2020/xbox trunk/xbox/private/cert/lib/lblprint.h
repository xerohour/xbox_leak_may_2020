/*
 *
 * lblprint.h
 *
 * Label printing
 *
 */

#ifndef _LBLPRINT_H
#define _LBLPRINT_H

class CLabelPrinter
{
public:
    static CLabelPrinter *PprCreate(LPCSTR szName, int nOff=0);
    BOOL FStartLabel(void);
    void PrintLine(LPCSTR szFormat, ...);
    void EndLabel(void);
    ~CLabelPrinter();
private:
    CLabelPrinter(void) {}

    HDC m_hdc;
    HFONT m_hfont;
    LPSTR m_szName;
    DEVMODE *m_pdm;
    int m_iLine;
    POINT m_ptOff;
    int m_yPitch;
    int m_yOffBase;
};

#endif // _LBLPRINT_H
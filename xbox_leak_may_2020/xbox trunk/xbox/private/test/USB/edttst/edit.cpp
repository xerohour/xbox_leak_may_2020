#include <xtl.h>
#include <xkbd.h>
#include "draw.h"
#include "edit.h"

#define FIXED_FONT_WIDTH  7
#define FIXED_FONT_HEIGHT 10

CEditBox::CEditBox(int iLeft, int iTop, int iWidth, int iHeight, CDraw &Draw) :
        m_iLeft(iLeft),
        m_iTop(iTop),
        m_iWidth(iWidth),
        m_iHeight(iHeight),
        m_iLines(0),
        m_iColumns(0),
        m_iLineSpacing(0),
        m_iCurrentLine(0),
        m_iCurrentPos(0),
        m_LineBuffer(0)
{
    int iCharWidth, iCharHeight;
    int i;
    //
    // Figure out Line Count and Spacing
    //
#ifdef propfont
    Draw.GetTextExtent((PDWORD)&iCharWidth, (PDWORD)&iCharHeight, L"A");
    iCharWidth += 2;  //Leave Space Between Lines
    if(0==iCharHeight) iCharHeight = FIXED_FONT_HEIGHT;
    if(0==iCharWidth) iCharWidth = FIXED_FONT_WIDTH;
#else
    iCharHeight = FIXED_FONT_HEIGHT;
    iCharWidth = FIXED_FONT_WIDTH;
#endif

    m_iLineSpacing = iCharHeight;
    m_iLines = (m_iHeight)/m_iLineSpacing;
    
    //
    //  Figure out maximum columns
    //
#ifdef propfont
    Draw.GetTextExtent((PDWORD)&iCharWidth, (PDWORD)&iCharHeight, L" ");
    if(0==iCharHeight) iCharHeight = FIXED_FONT_HEIGHT;
    if(0==iCharWidth) iCharWidth = FIXED_FONT_WIDTH;
#else
    iCharHeight = FIXED_FONT_HEIGHT;
    iCharWidth = FIXED_FONT_WIDTH;
#endif
    m_iColumns = (m_iWidth)/iCharWidth + 1;  //allocate an extra character on the end to terminate.
    
    //
    //  Allocate the line buffers
    //
    m_LineBuffer = new PWCHAR[m_iLines];
    for(i = 0; i < m_iLines; i++)
    {
        m_LineBuffer[i] = new WCHAR[m_iColumns];
        memset(m_LineBuffer[i],0, sizeof(WCHAR)*m_iColumns);
    }
}

void CEditBox::ProcessInput(CDraw &Draw)
{
    XINPUT_DEBUG_KEYSTROKE Keystroke;
    int iLineWidth, iBogus;
    int i;
    while(ERROR_SUCCESS==XInputDebugGetKeystroke(/*NULL,*/ &Keystroke))
    {
        //
        //  If the key has an ascii value place it in the buffer
        //
        if(Keystroke.Ascii)
        {
           switch(Keystroke.Ascii)
           {    
                case '\n':
                    NextLine();
                    break;
                case '\b':
                    if(0==m_iCurrentPos)
                    {
                        PreviousLine();
                    } else
                    {
                        m_LineBuffer[m_iCurrentLine][--m_iCurrentPos] = 0;
                    }
                    break;
                case '\t':
                    for(i=0; i<4; i++)
                    {
                        m_LineBuffer[m_iCurrentLine][m_iCurrentPos++] = 32;
                    }
                    break;
                default:
                    if(Keystroke.Ascii >= 32)
                    {
                        // Check for line buffer overrun
                        if(m_iCurrentPos == (m_iColumns+1))
                        {
                            NextLine();
                        }
                        // Add the character
                        m_LineBuffer[m_iCurrentLine][m_iCurrentPos++] = Keystroke.Ascii;
                        // Check for line render overrun
                        #ifdef propfont
                        Draw.GetTextExtent((PDWORD)&iLineWidth, (PDWORD)&iBogus, m_LineBuffer[m_iCurrentLine]);
                        if(iLineWidth >= m_iWidth)
                        {
                            // Move the character down to the next line
                            m_LineBuffer[m_iCurrentLine][--m_iCurrentPos] = 0;
                            NextLine();
                            m_LineBuffer[m_iCurrentLine][m_iCurrentPos++] = Keystroke.Ascii;
                        }
                        #endif
                    }
                    break;
           }
        }
    }
}

void CEditBox::Draw(CDraw &Draw)
{
    int i;
    // Clear the box out to total blackness
    Draw.FillRect(m_iLeft, m_iTop, m_iWidth, m_iHeight, 0);
    // Draw Each Line.
    for(i=0;i<m_iLines;i++)
    {
        Draw.DrawText(m_LineBuffer[i], m_iLeft, (m_iTop+m_iLineSpacing*i), 0xff7f7f7f);
    }
    // Draw Cursor
    if(GetTickCount()%1024 < 512)
    {
        //
        //  Find the horizontal cursor position
        //
        int iHorizCursorPos, iBogus;
        #ifdef propfont
        WCHAR TempChar;
        TempChar = m_LineBuffer[m_iCurrentLine][m_iCurrentPos];
        m_LineBuffer[m_iCurrentLine][m_iCurrentPos] = 0;
        Draw.GetTextExtent((PDWORD)&iHorizCursorPos,(PDWORD)&iBogus, m_LineBuffer[m_iCurrentLine]);
        m_LineBuffer[m_iCurrentLine][m_iCurrentPos] = TempChar;
        //
        //  Draw the cursor
        //
        Draw.FillRect(m_iLeft+iHorizCursorPos, (m_iTop+m_iLineSpacing*m_iCurrentLine), 2, m_iLineSpacing, 0xffffffff);
        #endif
    }
}

void CEditBox::NextLine()
{
    int i;
    //
    //  If we are on the bottom line then we scroll up
    //
    WCHAR *pTempLine;
    int iNextLine = m_iCurrentLine+1;
    if(iNextLine == m_iLines)
    {
        pTempLine = m_LineBuffer[0];
        for(i = 0; i < m_iCurrentLine; i++)
        {
            m_LineBuffer[i] = m_LineBuffer[i+1];
        }
        memset(pTempLine, 0, sizeof(WCHAR)*m_iColumns); 
        m_LineBuffer[m_iCurrentLine] = pTempLine;
        m_iCurrentPos = 0;
    } else
    //
    //  Otherwise, just move the current position
    //
    {
        m_iCurrentLine = iNextLine;
        m_iCurrentPos = 0;
    }
}

void CEditBox::PreviousLine()
{
    //
    //  If we are on not on top line go up a line.
    //
    if(0!=m_iCurrentLine)
    {
        m_iCurrentLine--;
        // Find the end of the line
        m_iCurrentPos = 0;
        while(m_LineBuffer[m_iCurrentLine][m_iCurrentPos]) m_iCurrentPos++;
    }
}
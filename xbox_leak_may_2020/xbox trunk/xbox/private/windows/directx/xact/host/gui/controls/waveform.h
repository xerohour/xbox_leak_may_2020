/***************************************************************************
 *
 *  Copyright (C) 1/30/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       waveform.h
 *  Content:    Renders a waveform bitmap.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  1/30/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __WAVEFORM_H__
#define __WAVEFORM_H__

//
// Waveform renderer color data
//

BEGIN_DEFINE_STRUCT()
    COLORREF    rgbBackground;          // Window background color
    COLORREF    rgbInactiveBackground;  // Window background color
    COLORREF    rgbWaveform;            // Waveform color
END_DEFINE_STRUCT(WAVEFORMCOLORS);

//
// Waveform renderer wave data
//

BEGIN_DEFINE_STRUCT()
    LPCWAVEFORMATEX pwfxFormat;     // Data format (PCM only)
    LPCVOID         pvWaveData;     // Wave data
    UINT            cbWaveData;     // Wave data size, in bytes
END_DEFINE_STRUCT(WAVEFORMDATA);

//
// Waveform render state
//

typedef enum
{
    WAVEFORM_RENDER_NONE = 0,       // Nothing needs to be redrawn
    WAVEFORM_RENDER_REDRAW,         // The waveform needs to be redrawn
    WAVEFORM_RENDER_DRAWINGTOOLS,   // The objects used to draw the waveform need to be recreated
    WAVEFORM_RENDER_CREATEBITMAP,   // The off-screen bitmap needs to be recreated
    WAVEFORM_RENDER_FULL,           // All render operations need to be done
} WAVEFORM_RENDER_STATE;

#ifdef __cplusplus

//
// Waveform renderer object
//

class CWaveformRenderer
{
public:
    static const UINT       m_nDefaultScale;            // Default scaling factor
    static const UINT       m_nDefaultResolution;       // Default resolution factor

protected:
    WAVEFORM_RENDER_STATE   m_nState;                   // Render state
    HWND                    m_hWndDisplay;              // Window we're drawing on
    RECT                    m_rcDisplay;                // Drawing rectangle
    int                     m_nWidth;                   // Drawing rectangle width
    int                     m_nHeight;                  // Drawing rectangle height
    HDC                     m_hdcDraw;                  // Offscreen drawing device context
    HBRUSH                  m_hbrActiveBackground;      // Background brush
    HBRUSH                  m_hbrInactiveBackground;    // Background brush
    HPEN                    m_hpnBackground;            // Background pen
    HPEN                    m_hpnWaveform;              // Waveform pen
    WAVEFORMCOLORS          m_colors;                   // Bitmap colors
    WAVEFORMATEX            m_wfxFormat;                // Data format
    LPCVOID                 m_pvWaveData;               // PCM wave data
    UINT                    m_cbWaveData;               // Wave data size
    UINT                    m_cbHorizOffset;            // Offset into the wave data
    UINT                    m_nScale;                   // Scaling factor
    UINT                    m_nResolution;              // Resolution factor

public:
    CWaveformRenderer(void);
    virtual ~CWaveformRenderer(void);

public:
    // Initialization
    virtual BOOL Initialize(HWND hWnd);

    // Renderer properties
    virtual BOOL SetRect(LPCRECT prcDisplay);
    virtual BOOL SetColors(LPCWAVEFORMCOLORS pColors);
    virtual BOOL SetWaveData(LPCWAVEFORMDATA pWaveData);
    virtual BOOL SetOffset(UINT cbOffset);
    virtual BOOL SetScale(UINT nScale);
    virtual BOOL SetResolution(UINT nResolution);

    // Renderer methods
    virtual BOOL Render(HDC hdcDisplay, LPCRECT prcUpdate);

protected:
    // Renderer state
    virtual void SetRenderState(WAVEFORM_RENDER_STATE nState);

    // Renderer methods
    virtual BOOL CreateBitmap(HDC hdcDisplay);
    virtual BOOL CreateDrawingTools(void);
    virtual BOOL DrawBackground(void);
    virtual BOOL DrawWaveform(void);
    virtual BOOL DisplayBitmap(HDC hdcDisplay, LPCRECT prcUpdate);

private:
    // Renderer methods
    template <class SampleType> void RenderWaveform(const SampleType *pWaveData, UINT nWaveRange, BOOL fSigned);
};

__inline void CWaveformRenderer::SetRenderState(WAVEFORM_RENDER_STATE nState)
{
    if(nState > m_nState)
    {
        m_nState = nState;
    }
}

__inline BOOL CWaveformRenderer::SetResolution(UINT nResolution)
{
    m_nResolution = nResolution;
    return TRUE;
}

//
// Waveform window
//

class CWaveformControl
    : public CWindow, public CRefCount, protected CWaveformRenderer
{
public:
    static const LPCTSTR    m_pszClassName;     // Window class name

protected:
    static const UINT       m_nSectionDivisor;  // Number of sections in a page
    UINT                    m_nPageSize;        // Visible page size (in logical window units)
    UINT                    m_nSectionSize;     // Section size (in logical window units)

public:
    CWaveformControl(void);
    virtual ~CWaveformControl(void);

public:
    // Reference count
    virtual ULONG STDMETHODCALLTYPE AddRef(void);
    virtual ULONG STDMETHODCALLTYPE Release(void);

    // Creation
    virtual BOOL Create(CWindow *pParent, DWORD dwExStyle, int x, int y, UINT nWidth, UINT nHeight, UINT nControlId);

    // Renderer properties
    virtual BOOL SetColors(LPCWAVEFORMCOLORS pColors);
    virtual BOOL SetWaveData(LPCWAVEFORMDATA pWaveData);
    virtual BOOL SetOffset(UINT cbOffset);
    virtual BOOL SetScale(UINT nScale);
    virtual BOOL SetResolution(UINT nResolution);

    // Message handlers
    virtual BOOL OnCreate(LPVOID pvContext, LRESULT *plResult);
    virtual BOOL OnSize(UINT nType, UINT nWidth, UINT nHeight, LRESULT *plResult);
    virtual BOOL OnEraseBackground(HDC hdc, LRESULT *plResult);
    virtual BOOL OnPaint(LRESULT *plResult);
    virtual BOOL OnScroll(UINT nScrollBarType, HWND hWndScrollBar, INT nPosition, LRESULT *plResult);

protected:
    // Setup
    virtual void SetupOffsetScrollbar(void);
    virtual void SetupScaleScrollbar(void);
};

__inline ULONG CWaveformControl::AddRef(void)
{
    return CRefCount::AddRef();
}

__inline ULONG CWaveformControl::Release(void)
{
    return CRefCount::Release();
}

__inline BOOL CWaveformControl::SetColors(LPCWAVEFORMCOLORS pColors)
{
    return CWaveformRenderer::SetColors(pColors);
}

__inline BOOL CWaveformControl::SetResolution(UINT nResolution)
{
    return CWaveformRenderer::SetResolution(nResolution);
}

#endif // __cplusplus

#endif // __WAVEFORM_H__

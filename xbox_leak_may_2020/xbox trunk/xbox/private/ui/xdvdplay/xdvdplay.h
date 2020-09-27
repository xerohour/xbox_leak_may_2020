//----------------------------------------------------------------------------
//
// File: xdvdplay.h
//
// Copyright (C) Microsoft Corporation, 1999.
//
//----------------------------------------------------------------------------

#ifndef __XDVDPLAY_H__
#define __XDVDPLAY_H__

class XDvdPlayer
{
public:
    XDvdPlayer();
    ~XDvdPlayer();

    HRESULT Initialize(HWND hWnd);
    LRESULT CALLBACK WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

private:
    void SetFullScreen();
    void ToggleClosedCaption();
    void SetParentalLevel();
    BOOL OnKeyDown(HWND hWnd, WPARAM wParam);
    void OnDVDPlayEvent(WPARAM wParam, LPARAM lParam);
    void Exit(HWND hWnd);
    void Play();
    void Pause();
    void Stop();
    void Scan(double dSpeed, BOOL fForward);

private:
    IDvdGraphBuilder *m_pDvdGB;
    IDvdControl      *m_pDvdC;
    IMediaControl    *m_pMC;
    IMediaEventEx    *m_pME;
    BOOL              m_bMenuOn;
    BOOL              m_bScanning;
};

#endif // __XDVDPLAY_H__

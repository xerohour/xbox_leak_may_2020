//----------------------------------------------------------------------------
//
// File: xdvdplay.cpp
//
// Copyright (C) Microsoft Corporation, 1999.
//
//----------------------------------------------------------------------------

#include "pch.cpp"
#pragma hdrstop

#define RELEASE(x)          if (x) x->Release()
#define WM_DVDPLAY_EVENT    (WM_USER+100)

enum
{
    RATING_EARLY_CHILDHOOD,
    RATING_EVERYONE,
    RATING_TEEN,
    RATING_MATURE,
    RATING_ADULTS_ONLY,
    RATING_PENDING,
    RATING_ERROR
};

enum
{
    RATING_G = 1,
    RATING_PG = 3,
    RATING_PG13 = 4,
    RATING_R = 6,
    RATING_NC17 = 7
};

DWORD g_adwRating[5] = {RATING_G, RATING_PG, RATING_PG13, RATING_R, RATING_NC17};

XDvdPlayer::XDvdPlayer()
  : m_pDvdGB(NULL),
    m_pDvdC(NULL),
    m_pMC(NULL),
    m_pME(NULL),
    m_bMenuOn(FALSE),
    m_bScanning(FALSE)
{
    CoInitialize(NULL);
}

XDvdPlayer::~XDvdPlayer()
{
    RELEASE(m_pDvdGB);
    RELEASE(m_pDvdC);
    RELEASE(m_pMC);
    RELEASE(m_pME);
    
    CoUninitialize();
}

HRESULT
XDvdPlayer::Initialize(HWND hWnd)
{
    HRESULT hr;

    hr = CoCreateInstance(CLSID_DvdGraphBuilder, NULL, CLSCTX_INPROC,
                          IID_IDvdGraphBuilder, (LPVOID *)&m_pDvdGB);

    if (FAILED(hr))
        return hr;

    AM_DVD_RENDERSTATUS Status;
    hr = m_pDvdGB->RenderDvdVideoVolume(NULL, AM_DVD_HWDEC_PREFER, &Status);

    if (FAILED(hr))
        return hr;
    else if (hr == S_FALSE) // No dvd disk inserted.
        return E_FAIL;

    IGraphBuilder *pGraph;

    if (FAILED(hr = m_pDvdGB->GetFiltergraph(&pGraph)))
        return hr;

    pGraph->QueryInterface(IID_IMediaControl, (LPVOID *)&m_pMC);
    pGraph->QueryInterface(IID_IMediaEventEx, (LPVOID *)&m_pME);
    pGraph->Release();

    m_pME->SetNotifyWindow((OAHWND)hWnd, WM_DVDPLAY_EVENT, 0);

    m_pDvdGB->GetDvdInterface(IID_IDvdControl, (LPVOID *)&m_pDvdC);

    SetFullScreen();

    // CC is on by DShow's default. We need to change the default to off.
    ToggleClosedCaption();

    SetParentalLevel();

    Play();

    return S_OK;
}

void
XDvdPlayer::SetFullScreen()
{
    IVideoWindow *pVW;

    if (FAILED(m_pDvdGB->GetDvdInterface(IID_IVideoWindow, (LPVOID *)&pVW)))
        return;

    pVW->put_WindowStyle(0);

    LONG  lScrnWidth  = GetSystemMetrics(SM_CXSCREEN);
    LONG  lScrnHeight = GetSystemMetrics(SM_CYSCREEN);

    pVW->SetWindowPosition(0, 0, lScrnWidth, lScrnHeight);
    pVW->HideCursor(OATRUE);

    pVW->Release();
}

void
XDvdPlayer::ToggleClosedCaption()
{
    IAMLine21Decoder *pL21Dec;
    m_pDvdGB->GetDvdInterface(IID_IAMLine21Decoder, (LPVOID *)&pL21Dec);
    if (pL21Dec)
    {
        AM_LINE21_CCSTATE state;
        pL21Dec->GetServiceState(&state);
        pL21Dec->SetServiceState(state? AM_L21_CCSTATE_Off : AM_L21_CCSTATE_On);
        pL21Dec->Release();
    }
}

void
XDvdPlayer::SetParentalLevel()
{
    HKEY hKey;
    DWORD uSize, uType, uAllowedRating;
    ULONG ulLevel = 0xffffffff; // Parental management disabled.

    if (SUCCEEDED(RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                               "Software\\Microsoft\\XBox\\Configuration",
                               0, KEY_ALL_ACCESS, &hKey)))
    {
        if (SUCCEEDED(RegQueryValueEx(hKey, "AllowedRating", 0, &uType,
                                      (BYTE *)&uAllowedRating, &uSize)))
        {
            if (uAllowedRating < RATING_PENDING)
            {
                ulLevel = g_adwRating[uAllowedRating];
            }
        }

        RegCloseKey(hKey);
    }

    m_pDvdC->ParentalLevelSelect(ulLevel);
}

LRESULT CALLBACK
XDvdPlayer::WndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
    switch(uMessage)
    {
        case WM_KEYDOWN:
            OnKeyDown(hWnd, wParam);
            break;

        case WM_DVDPLAY_EVENT:
            OnDVDPlayEvent(wParam, lParam);
            break;

        default:
            return DefWindowProc(hWnd, uMessage, wParam, lParam);
    }

    return 1;
}

BOOL
XDvdPlayer::OnKeyDown(HWND hWnd, WPARAM wParam)
{
    switch (wParam)
    {
        case VK_ESCAPE:
            Exit(hWnd);
            break;

        case 'A':
            Play();
            break;
        
        case 'B':
            Pause();
            break;

        case 'C':
            ToggleClosedCaption();
            break;

        case 'D':
            m_pDvdC->MenuCall(DVD_MENU_Root);
            break;

        case VK_SPACE:
            Stop();
            break;

        case VK_UP:
            if (m_bMenuOn)
                m_pDvdC->UpperButtonSelect();
            else
                Scan(8.0, TRUE);
            break;

        case VK_RIGHT:
            if (m_bMenuOn)
                m_pDvdC->RightButtonSelect();
            else
                Scan(2.0, TRUE);
            break;

        case VK_DOWN:
            if (m_bMenuOn)
                m_pDvdC->LowerButtonSelect();
            else
                Scan(8.0, FALSE);
            break;

        case VK_LEFT:
            if (m_bMenuOn)
                m_pDvdC->LeftButtonSelect();
            else
                Scan(2.0, FALSE);
            break;

        case VK_RETURN:
            if (m_bMenuOn)
                m_pDvdC->ButtonActivate();
            break;

        default:
            return FALSE;
    }

    return TRUE;
}

void
XDvdPlayer::OnDVDPlayEvent(WPARAM wParam, LPARAM lParam)
{
    LONG lEvent, lParam1, lParam2;

    while (SUCCEEDED(m_pME->GetEvent(&lEvent, &lParam1, &lParam2, 0)))
    {
        switch (lEvent)
        {
            case EC_DVD_ERROR:
                m_pMC->Stop();
                break;

            case EC_DVD_DOMAIN_CHANGE:
                switch (lParam1)
                {
                    case DVD_DOMAIN_FirstPlay:
                    case DVD_DOMAIN_Stop:
                        break;

                    case DVD_DOMAIN_VideoManagerMenu:
                    case DVD_DOMAIN_VideoTitleSetMenu:
                        m_bMenuOn = TRUE;
                        m_bScanning = FALSE;
                        break;

                    case DVD_DOMAIN_Title:
                        m_bMenuOn = FALSE;
                        break;
                }
                break;

            case EC_COMPLETE:
                m_pMC->Stop();
                break;
        }

        m_pME->FreeEventParams(lEvent, lParam1, lParam2);
    }
}

void
XDvdPlayer::Exit(HWND hWnd)
{
    Stop();
    PostQuitMessage(0);
    DestroyWindow(hWnd);
}

void
XDvdPlayer::Play()
{
    m_pMC->Run();
    m_pDvdC->ForwardScan(1.0);
    m_bScanning = FALSE;
}

void
XDvdPlayer::Stop()
{
    m_pMC->Stop();
}

void
XDvdPlayer::Pause()
{
    m_pMC->Pause();
}

void
XDvdPlayer::Scan(double dSpeed, BOOL fForward)
{
    // Need to press "Play" button between different scans,
    // otherwise it doesn't scan at the right speed.

    if (!m_bScanning)
    {
        if (fForward)
            m_pDvdC->ForwardScan(dSpeed);
        else 
            m_pDvdC->BackwardScan(dSpeed);

        m_bScanning = TRUE;
    }
}

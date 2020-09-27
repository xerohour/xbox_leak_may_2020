#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>

#include "dsptest.h"
#include "fximg.h"

static const float SPECTRUM_LEFT_X = 250.;
static const float SPECTRUM_BOTTOM_Y = 460.;
static const float SPECTRUM_X_SIZE = 300.;
static const float SPECTRUM_Y_SIZE = 100.;

static const float SPECTRUM_NUM_BINS = 64.;

static const DWORD FFT_WINDOW_SIZE = 256;

static const float PEAK_MONITOR_LEFT_X = 20.;
static const float PEAK_MONITOR_BOTTOM_Y = 150.;

static const float PEAK_MONITOR_X_SIZE = 300.;
static const float PEAK_MONITOR_Y_SIZE = 100.;

static const float PEAK_MONITOR_BAR_WIDTH = 40.;
static const float PEAK_MONITOR_BAR_SPACING = 30.;


HRESULT
CXBoxSample::FourierTransform()
{
    //
    // produce N frequency and phase bins from a looking at current audio output
    //

    HRESULT hr = S_OK;
    DWORD dwBin, k;
    DOUBLE arg, sign = -1.; /* sign = -1 -> FFT, 1 -> iFFT */
    DOUBLE cosPart;
    DOUBLE sinPart;
    DOUBLE fSample;
    DWORD dwTransformLength = FFT_WINDOW_SIZE;
    LONG lSample;

    //
    // take a snapshot of the current delay buffer
    //

    memcpy(m_pdwAudioData,
           m_pEffectsImageDesc->aEffectMaps[m_dwEffectIndex].lpvScratchSegment,
           dwTransformLength*sizeof(DWORD));

    for (dwBin = 0; dwBin <= dwTransformLength/2; dwBin++) {

        cosPart = sinPart = 0.;
        for (k = 0; k < dwTransformLength; k++) {
        

            arg = 2.0*(float)dwBin*M_PI*(float)k/(float)dwTransformLength;
            lSample = (LONG)m_pdwAudioData[k];
            fSample = ((DOUBLE)lSample) / 2147483648.0;

            sinPart += fSample * sign * sin(arg);
            cosPart += fSample * cos(arg);

        }

        m_aMagnitudeBins[dwBin] = 20.0 * log10( 2.0 * sqrt(sinPart*sinPart + cosPart*cosPart) / (float)dwTransformLength);
        m_aPhaseBins[dwBin] = 180.0*atan2(sinPart, cosPart) / M_PI - 90.0;
        m_aFrequencyBins[dwBin] = (float)dwBin * 48000 / (float)dwTransformLength;
         
        m_fMaxMagnitude = max(abs((LONG)m_aMagnitudeBins[dwBin]),m_fMaxMagnitude);

    } 

    return hr;
}


//-----------------------------------------------------------------------------
// Name: Render
// Desc: Renders the spectrum
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::RenderSpectrum()
{
    WCHAR szString[256];
    DWORD dwValue=0,i;
    FLOAT fXPos = SPECTRUM_LEFT_X;
    FLOAT fYPos = SPECTRUM_BOTTOM_Y;

    FLOAT fWidth = SPECTRUM_X_SIZE;
    FLOAT fHeight = SPECTRUM_Y_SIZE;

    FLOAT fNumBins = SPECTRUM_NUM_BINS;
    DWORD dwBinsToBarsFactor = FFT_WINDOW_SIZE/(2*(DWORD)SPECTRUM_NUM_BINS);

    FLOAT fBinSpacing = 2.0;
    FLOAT fBinWidth = (fWidth - fBinSpacing*(fNumBins+2))/fNumBins;
    
    FLOAT x1, x2;
    FLOAT y1, y2;

    FLOAT fValue = 0.;

    // Clear the zbuffer
    m_pd3dDevice->Clear( 0L, NULL, D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL,
                         0x00000000, 1.0f, 0L );

    DirectSoundDoWork();

    // Begin the scene
    m_pd3dDevice->BeginScene();    

    // Draw a gradient filled background
    RenderGradientBackground( 0xff404040, 0xff404080 );

    if (m_bDoDFT) {

        for (DWORD i=0;i<(DWORD)fNumBins;i++) {
    
            x1 = fXPos+fBinSpacing+i*(fBinSpacing+fBinWidth);
            x2 = x1+fBinWidth;
            y2 = fYPos;
    
            
            fValue = 0;
            for (DWORD k=0;k<dwBinsToBarsFactor;k++) {
    
                fValue += (FLOAT)m_aMagnitudeBins[i*dwBinsToBarsFactor+k];
    
            }        
            
            y1 = y2-(((FLOAT)m_fMaxMagnitude*dwBinsToBarsFactor+fValue)/(FLOAT)(m_fMaxMagnitude*dwBinsToBarsFactor))*fHeight;
    
    
            struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
            BACKGROUNDVERTEX v[4];
            v[0].p = D3DXVECTOR4( x1, y1, 1.0f, 1.0f ); v[0].color = 0xffffffff;
            v[1].p = D3DXVECTOR4( x2, y1, 1.0f, 1.0f ); v[1].color = 0xffffffff;
            v[2].p = D3DXVECTOR4( x1, y2, 1.0f, 1.0f ); v[2].color = 0xffff0000;
            v[3].p = D3DXVECTOR4( x2, y2, 1.0f, 1.0f ); v[3].color = 0xffff0000;
    
            m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
            m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );
    
        }

    }

    //
    // get the data from the effect X-memory area
    //

    LPCDSFX_RMS_PARAMS pRmsParams = (LPCDSFX_RMS_PARAMS)m_pEffectsImageDesc->aEffectMaps[RMS_CHAIN_RMS].lpvStateSegment; 

    fYPos = PEAK_MONITOR_BOTTOM_Y+20 ;

    for (i=0;i<6;i++) {

        fValue = (FLOAT)(pRmsParams->dwRMSValues[i]/pow(2,23));
    
        m_fMaxLevels[i] = (FLOAT)(pRmsParams->dwPeakValues[i]/pow(2,23));

        fXPos = PEAK_MONITOR_LEFT_X+PEAK_MONITOR_BAR_SPACING+i*(PEAK_MONITOR_BAR_SPACING+PEAK_MONITOR_BAR_WIDTH);
        swprintf (szString,L"%2.2f",20.0*log10(fValue));
        m_Font.DrawText( fXPos-10, fYPos, 0xFFFFFF00, szString);


        swprintf (szString,L"%2.2f",20.0*log10(m_fMaxLevels[i]));
        m_Font.DrawText( fXPos-10, fYPos+20, 0xFFFFFF00, szString);


        m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );

        // Draw left level
        {
            struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
            BACKGROUNDVERTEX v[4];
            x1 = PEAK_MONITOR_LEFT_X + PEAK_MONITOR_BAR_SPACING + i*(PEAK_MONITOR_BAR_SPACING+PEAK_MONITOR_BAR_WIDTH);
            x2 = x1 + PEAK_MONITOR_BAR_WIDTH;
            y2 = PEAK_MONITOR_BOTTOM_Y;
            y1 = y2 - PEAK_MONITOR_Y_SIZE*fValue;
    
            v[0].p = D3DXVECTOR4( x1, y1, 1.0f, 1.0f ); v[0].color = 0xffffffff;
            v[1].p = D3DXVECTOR4( x2, y1, 1.0f, 1.0f ); v[1].color = 0xffffffff;
            v[2].p = D3DXVECTOR4( x1, y2, 1.0f, 1.0f ); v[2].color = 0xff0000ff;
            v[3].p = D3DXVECTOR4( x2, y2, 1.0f, 1.0f ); v[3].color = 0xff0000ff;
    
            m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
            m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );
        }

        // Draw current max
        {
            struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
            BACKGROUNDVERTEX v[4];
            x1 = PEAK_MONITOR_LEFT_X + PEAK_MONITOR_BAR_SPACING + i*(PEAK_MONITOR_BAR_SPACING+PEAK_MONITOR_BAR_WIDTH);
            x2 = x1 + PEAK_MONITOR_BAR_WIDTH;
            y1 = PEAK_MONITOR_BOTTOM_Y - PEAK_MONITOR_Y_SIZE*m_fMaxLevels[i];
            y2 = y1+3;

    
            v[0].p = D3DXVECTOR4( x1, y1, 1.0f, 1.0f ); v[0].color = 0xffffff00;
            v[1].p = D3DXVECTOR4( x2, y1, 1.0f, 1.0f ); v[1].color = 0xffffff00;
            v[2].p = D3DXVECTOR4( x1, y2, 1.0f, 1.0f ); v[2].color = 0xffff00ff;
            v[3].p = D3DXVECTOR4( x2, y2, 1.0f, 1.0f ); v[3].color = 0xffff00ff;
    
            m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );
            m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );
        }


    }

    m_dwCount++;

    if (m_dwCount>10000) {

        memset((PVOID)&pRmsParams->dwPeakValues[0],0,sizeof(pRmsParams->dwPeakValues));
        m_dwCount = 0;
    }


    m_Font.Begin();
    m_Font.DrawText( 450, 50, 0xffffff00, m_strFrameRate );
    m_Font.End();




    // End the scene
    m_pd3dDevice->EndScene();
    m_pd3dDevice->Present( NULL, NULL, NULL, NULL );

    return S_OK;
}


#include <XBApp.h>
#include <XBFont.h>
#include <XBHelp.h>
#include <xgraphics.h>

// CHANGED - include DMConsole.h instead
// #include "dsptest.h"
#include "DMConsole.h"
#include "dsstdfx.h"
// END NEW


static const float SPECTRUM_LEFT_X = 250.;
static const float SPECTRUM_BOTTOM_Y = 300.;
static const float SPECTRUM_X_SIZE = 300.;
static const float SPECTRUM_Y_SIZE = 100.;

static const float SPECTRUM_NUM_BINS = 32.;

static const DWORD FFT_WINDOW_SIZE = 512;

// constants for graphical LED display
static const float PEAK_MONITOR_BAR_WIDTH = 20.;
static const float PEAK_MONITOR_BAR_SPACING = 35.;
static const float PEAK_MONITOR_DIGITAL_ANALOG_SPACING = 50;
static const float PEAK_MONITOR_LABEL_SPACING_Y = 10.0;
static const float PEAK_DISPLAY_BAR_HEIGHT = 2.0;
static const float PEAK_DISPLAY_BAR_OFFSET = 10.0;
static const float PEAK_MONITOR_LEFT_X = 100.;
static const float PEAK_MONITOR_BOTTOM_Y = 400.;
static const float PEAK_MONITOR_X_SIZE = 300.;
//static const float PEAK_MONITOR_Y_SIZE = 100.;
static const float PEAK_MONITOR_Y_MAX = 220.0;
static const float PEAK_MONITOR_MIN_DB = -60.0;
static const float PEAK_MONITOR_DB_AXIS_SCALE = 5.;
static const float PEAK_MONITOR_Y_HIGHVOLUME = (FLOAT)(PEAK_MONITOR_BOTTOM_Y-(.9*PEAK_MONITOR_Y_MAX)); 
static const float PEAK_MONITOR_Y_MIDVOLUME = (FLOAT)(PEAK_MONITOR_BOTTOM_Y-(.7*PEAK_MONITOR_Y_MAX));
//static const float PEAK_NORMALIZE_DISPLAY = 0.004f;


// based on 20 log(2) = 6.0206 , times 23 bits of amplitude for Xbox
// static const float XBOX_REFERENCE_DB = 138.47379800543134979831989157327;

static const float DISPLAY_DECAY_CONSTANT = 10.0;


//-----------------------------------------------------------------------------
// Name: UpdatePeakLevels
// Desc: Updates current level values (from RMS effect)
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::UpdatePeakLevels()
{
    

    // NEW turned levels into array, and coded to PEAK_NUM_BINS
    // FLOAT fLeftLevel=0.0, fRightLevel = 0.0, fValue = 0.;
    FLOAT fValue = 0.;
    // NEW - show LEDs for all six channels
    FLOAT fPeakLevels[PEAK_NUM_BINS];
    // Level to display ("smoothed out")
    FLOAT fHistoryBufferAverage_Previous;

    DirectSoundDoWork();

    // NEW - we're displaying with other AudConsole content, so don't start the scene or redraw

    //
    // get the data from dsound
    //

    DSOUTPUTLEVELS dsOutputLevels;
    memset(&dsOutputLevels, 0 ,sizeof(dsOutputLevels));

    m_pDSound->GetOutputLevels(&dsOutputLevels, FALSE);

    //
    // digital peak levels
    //

    fPeakLevels[DSMIXBIN_FRONT_LEFT] = (FLOAT) dsOutputLevels.dwDigitalFrontLeftPeak;
    fPeakLevels[DSMIXBIN_FRONT_RIGHT] = (FLOAT) dsOutputLevels.dwDigitalFrontRightPeak;
    fPeakLevels[DSMIXBIN_BACK_LEFT] = (FLOAT) dsOutputLevels.dwDigitalBackLeftPeak;
    fPeakLevels[DSMIXBIN_BACK_RIGHT] = (FLOAT) dsOutputLevels.dwDigitalBackRightPeak;
    fPeakLevels[DSMIXBIN_FRONT_CENTER] = (FLOAT) dsOutputLevels.dwDigitalFrontCenterPeak;
    fPeakLevels[DSMIXBIN_LOW_FREQUENCY] = (FLOAT) dsOutputLevels.dwDigitalLowFrequencyPeak;

    //
    // analog peak levels
    //

    fPeakLevels[DSMIXBIN_FRONT_LEFT+6] = (FLOAT) dsOutputLevels.dwAnalogLeftTotalPeak;
    fPeakLevels[DSMIXBIN_FRONT_RIGHT+6] = (FLOAT) dsOutputLevels.dwAnalogRightTotalPeak;


    for (DWORD k=0; k < PEAK_NUM_BINS; k ++)
    {
        fPeakLevels[k] = (FLOAT)(fPeakLevels[k]/pow(2,23));
        if (fPeakLevels[k] > m_fMaxLevels[k]) {
            m_fMaxLevels[k] = fPeakLevels[k];
        }

    // new - fill in history buffer and use for display
        fHistoryBufferAverage_Previous = m_fHistoryBufferAverage[k];

        m_fHistoryBufferAverage[k] += (fPeakLevels[k] - m_fLevelsHistoryBuffer[k][m_dwHistoryBufferIndex]) / HISTORY_BUFFER_SIZE;
        m_fLevelsHistoryBuffer[k][m_dwHistoryBufferIndex] = fPeakLevels[k];


/* Option 1 for current level display: Average of buffers (slow response, and doesn't show peaks)*/
        if (m_fHistoryBufferAverage[k] < fHistoryBufferAverage_Previous)
        // new level is lower than stored, so 'decay'
        {
            m_fDisplayLevel[k] = (FLOAT)(m_fHistoryBufferAverage[k]* exp(-(fHistoryBufferAverage_Previous-m_fHistoryBufferAverage[k])/DISPLAY_DECAY_CONSTANT));
        }
        else
        {
            m_fDisplayLevel[k] = m_fHistoryBufferAverage[k];
        }

/* Option 2 for current level display: Current buffer value (jittery) 
            m_fDisplayLevel[k] = fLevels[k];*/

    }

    m_dwHistoryBufferIndex = (++m_dwHistoryBufferIndex)%(HISTORY_BUFFER_SIZE);

   return S_OK;
}


//-----------------------------------------------------------------------------
// Name: RenderPeakLevels
// Desc: Draw the current level values (from RMS effect)
//-----------------------------------------------------------------------------
HRESULT
CXBoxSample::RenderPeakLevels()
{
    WCHAR szString[256];
    WCHAR *PeakLabels[]={L"L", L"R", L"C", L"LFE", L"LS", L"RS", L"aLeft", L"aRight"};
    FLOAT fXPos, fYPos;
    FLOAT x1, x2;
    FLOAT y1, y2;

    // calculations for display levels are in UpdatePeakLevels(), running in separate thread
    for (DWORD k=0; k<PEAK_NUM_BINS; k++)
    {
        fXPos = PEAK_MONITOR_LEFT_X + PEAK_MONITOR_BAR_WIDTH/2 + (PEAK_MONITOR_BAR_WIDTH+PEAK_MONITOR_BAR_SPACING)*k;
		if (k>=6)
			fXPos += PEAK_MONITOR_DIGITAL_ANALOG_SPACING;
        fYPos = PEAK_MONITOR_BOTTOM_Y + PEAK_MONITOR_LABEL_SPACING_Y;

        m_FontSmall.DrawText( fXPos, fYPos, 0xffffff00, PeakLabels[k], XBFONT_CENTER_X);

        if (m_fMaxLevels[k] >= .9999f)
        {
            swprintf (szString,L"CLIP");
            m_FontSmall.DrawText( fXPos, fYPos+20, 0xffff2e43, szString, XBFONT_CENTER_X);
        }
        else 
        {
            if (m_fMaxLevels[k] > 0.0001f)
                swprintf (szString,L"%2.2f", 20.0f * log10(m_fMaxLevels[k]));
            else swprintf (szString, L"-Inf");

            m_FontSmall.DrawText( fXPos, fYPos+20, 0xffffff00, szString, XBFONT_CENTER_X);
        }

    }
	// draw dB meter labels (Y-axis)
	for (FLOAT fLabel_counter=0; fLabel_counter >= PEAK_MONITOR_MIN_DB; fLabel_counter-=PEAK_MONITOR_DB_AXIS_SCALE)
	{
		swprintf (szString, L"%2.0f", fLabel_counter);
		m_FontSmall.DrawText (PEAK_MONITOR_LEFT_X - 2.0f * PEAK_DISPLAY_BAR_OFFSET, PEAK_MONITOR_BOTTOM_Y - (PEAK_MONITOR_Y_MAX - PEAK_MONITOR_Y_MAX * fLabel_counter / PEAK_MONITOR_MIN_DB), 0xffffff00, szString, XBFONT_RIGHT | XBFONT_CENTER_Y);
	}
		
	//
    // display current peak levels - better in above loop with render switching?
    //

    m_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_DISABLE );
    m_pd3dDevice->SetVertexShader( D3DFVF_XYZRHW|D3DFVF_DIFFUSE );


    // NEW - draw all current levels

    for (k=0; k < PEAK_NUM_BINS; k++)
    {
        FLOAT fDisplayLevel;
        struct BACKGROUNDVERTEX { D3DXVECTOR4 p; D3DCOLOR color; };
        BACKGROUNDVERTEX v[4];
        
        //
        // normalize level so its range is reasonable
        //

        // Hardware can only produce -60 dB, so only display values larger than that
        if (m_fDisplayLevel[k] > 0.0)
        {
            fDisplayLevel = (FLOAT)(PEAK_MONITOR_Y_MAX - ((20.0f * log10(m_fDisplayLevel[k])) * PEAK_MONITOR_Y_MAX / PEAK_MONITOR_MIN_DB) );
            if (fDisplayLevel < 0.0)
                fDisplayLevel = 0.0;
        }
        else
            fDisplayLevel = 0.0;
        
        x1 = PEAK_MONITOR_LEFT_X + (PEAK_MONITOR_BAR_WIDTH + PEAK_MONITOR_BAR_SPACING)*k;
		if (k>=6)
			x1 += PEAK_MONITOR_DIGITAL_ANALOG_SPACING;
        x2 = x1 + PEAK_MONITOR_BAR_WIDTH;
        y2 = PEAK_MONITOR_BOTTOM_Y;
        y1 = y2 - fDisplayLevel;

        if (y1 < PEAK_MONITOR_Y_HIGHVOLUME) // red for top-most portion
        {
            v[0].p = D3DXVECTOR4( x1, y1, 1.0f, 1.0f ); v[0].color = 0xffff0000;
            v[1].p = D3DXVECTOR4( x2, y1, 1.0f, 1.0f ); v[1].color = 0xffff0000;
            v[2].p = D3DXVECTOR4( x1, PEAK_MONITOR_Y_HIGHVOLUME, 1.0f, 1.0f ); v[2].color = 0xff5f0000;
            v[3].p = D3DXVECTOR4( x2, PEAK_MONITOR_Y_HIGHVOLUME, 1.0f, 1.0f ); v[3].color = 0xff5f0000;

            m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );

            y1 = PEAK_MONITOR_Y_HIGHVOLUME;
        }
        if (y1 < PEAK_MONITOR_Y_MIDVOLUME) // yellow for mid-range
        {
            v[0].p = D3DXVECTOR4( x1, y1, 1.0f, 1.0f ); v[0].color = 0xffffff00;
            v[1].p = D3DXVECTOR4( x2, y1, 1.0f, 1.0f ); v[1].color = 0xffffff00;
            v[2].p = D3DXVECTOR4( x1, PEAK_MONITOR_Y_MIDVOLUME, 1.0f, 1.0f ); v[2].color = 0xff5f5f00;
            v[3].p = D3DXVECTOR4( x2, PEAK_MONITOR_Y_MIDVOLUME, 1.0f, 1.0f ); v[3].color = 0xff5f5f00;

            m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );
            y1 = PEAK_MONITOR_Y_MIDVOLUME;
        }
        // green for bottom portion
        v[0].p = D3DXVECTOR4( x1, y1, 1.0f, 1.0f ); v[0].color = 0xff00ff00;
        v[1].p = D3DXVECTOR4( x2, y1, 1.0f, 1.0f ); v[1].color = 0xff00ff00;
        v[2].p = D3DXVECTOR4( x1, y2, 1.0f, 1.0f ); v[2].color = 0xff005f00;
        v[3].p = D3DXVECTOR4( x2, y2, 1.0f, 1.0f ); v[3].color = 0xff005f00;

        m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );


        // plot peak value (solid line slightly wider than volume bars)
        fDisplayLevel = (m_fMaxLevels[k]) * (FLOAT)PEAK_MONITOR_Y_MAX;
        if (m_fMaxLevels[k] == 0)
            fDisplayLevel = 0.0;
        else
            fDisplayLevel = (FLOAT)(PEAK_MONITOR_Y_MAX - ((20.0f * log10(m_fMaxLevels[k])) * PEAK_MONITOR_Y_MAX / PEAK_MONITOR_MIN_DB));
        if (fDisplayLevel < 0.0)
            fDisplayLevel = 0;

        x1 = PEAK_MONITOR_LEFT_X + (PEAK_MONITOR_BAR_WIDTH + PEAK_MONITOR_BAR_SPACING)*k - PEAK_DISPLAY_BAR_OFFSET;
		if (k>=6)
			x1 += PEAK_MONITOR_DIGITAL_ANALOG_SPACING;

        x2 = x1 + PEAK_MONITOR_BAR_WIDTH + 2*PEAK_DISPLAY_BAR_OFFSET;
        y2 = PEAK_MONITOR_BOTTOM_Y - fDisplayLevel; 
        y1 = y2 - PEAK_DISPLAY_BAR_HEIGHT;

        v[0].p = D3DXVECTOR4( x1, y1, 1.0f, 1.0f );
        v[1].p = D3DXVECTOR4( x2, y1, 1.0f, 1.0f );
        v[2].p = D3DXVECTOR4( x1, y2, 1.0f, 1.0f );
        v[3].p = D3DXVECTOR4( x2, y2, 1.0f, 1.0f );
        // color line based on range
        for (int r=0; r<4; r++)
        {
            if (y2 < PEAK_MONITOR_Y_HIGHVOLUME)
                v[r].color = 0xffff0000;
            else if (y2 < PEAK_MONITOR_Y_MIDVOLUME)
                v[r].color = 0xffffff00;
            else
                v[r].color = 0xff00ff00;
        }
        
        m_pd3dDevice->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, v, sizeof(v[0]) );

    }

    return S_OK;
}
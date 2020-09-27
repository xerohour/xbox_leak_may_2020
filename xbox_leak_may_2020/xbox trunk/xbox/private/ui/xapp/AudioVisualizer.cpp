#include "std.h"

#include "xapp.h"
#include "Node.h"
#include "Audio.h"
#include "surfx.h"
#include "fft.h"

class CAudioVisualizer : public CNode
{
	DECLARE_NODE(CAudioVisualizer, CNode)
public:
	CAudioVisualizer();
	~CAudioVisualizer();

//	int m_size;
	TCHAR* m_type; // line, spiner, circle
	TCHAR* m_channel; // left, center, right
	CNode* m_source;
	float m_scale;
	float m_offset;

protected:
	short* GetMonoPCM();
	void UpdateSpectrum();

	void RenderDynamicTexture(CSurfx* pSurfx);
	void Advance(float nSeconds);
	void CalcSpectrum(short* pcm, short* fft);

	void RenderEffect1(CSurfx* pSurfx);
	void RenderEffect2(CSurfx* pSurfx);

	// the last n stereo samples
	short m_pcmLeft [256];
	short m_pcmRight [256];

	// these are calculated from m_left and m_right as requested...
	short m_fftLeft [128];
	short m_fftRight [128];
	bool m_bFFTValid;
	short m_pcmMono [256];
	bool m_bMonoValid;

	DECLARE_NODE_PROPS()
};

IMPLEMENT_NODE("AudioVisualizer", CAudioVisualizer, CNode)

START_NODE_PROPS(CAudioVisualizer, CNode)
	NODE_PROP(pt_number, CAudioVisualizer, scale)
	NODE_PROP(pt_number, CAudioVisualizer, offset)
	NODE_PROP(pt_string, CAudioVisualizer, type)
	NODE_PROP(pt_string, CAudioVisualizer, channel)
	NODE_PROP(pt_node, CAudioVisualizer, source)
END_NODE_PROPS()


CAudioVisualizer::CAudioVisualizer() :
	m_type(NULL),
	m_source(NULL),
	m_scale(1.0f),
	m_offset(0.0f),
	m_channel(NULL)
{
}

CAudioVisualizer::~CAudioVisualizer()
{
	delete [] m_type;
	delete [] m_channel;

	if (m_source != NULL)
		m_source->Release();
}

void CAudioVisualizer::Advance(float nSeconds)
{
	CNode::Advance(nSeconds);

	if (m_source != NULL)
		m_source->Advance(nSeconds);
}


short* CAudioVisualizer::GetMonoPCM()
{
	if (!m_bMonoValid)
	{
		for (int i = 0; i < 256; i += 1)
		{
			m_pcmMono[i] = (short)(((int)m_pcmLeft[i] + (int)m_pcmRight[i]) / 2);
		}

		m_bMonoValid = true;
	}

	return m_pcmMono;
}

void CAudioVisualizer::CalcSpectrum(short* pcm, short* fft)
{
	static fft_state *state = NULL;
	float buf [FFT_BUFFER_SIZE / 2 + 1];
	
	if (!state)
		state = fft_init();

	fft_perform(pcm, buf, state);
	
	// BLOCK: Scale the fft data
	{
//		float scale = 8.0f;
		for (int i = 0; i < FFT_BUFFER_SIZE / 2 + 1; i += 1)
		{
//			buf[i] = logf(sqrtf(buf[i]) / FFT_BUFFER_SIZE) * scale;
			buf[i] = sqrtf(buf[i]) / FFT_BUFFER_SIZE;
			fft[i] = (short)buf[i];
		}
	}
/*
	// BLOCK: Move to byte sized buffer...
	{
		for (int i = 0; i < 128; i++)
		{
			short n = (short)buf[i + 1];

			if (n < 0)
				n = 0;
			else if (n > 255)
				n = 255;

			fft[i] = (BYTE)n;
		}
	}
*/
}

void CAudioVisualizer::UpdateSpectrum()
{
	if (m_bFFTValid)
		return;

	CalcSpectrum(m_pcmLeft, m_fftLeft);
	CalcSpectrum(m_pcmRight, m_fftRight);

	m_bFFTValid = true;
}


#define mag(s) ((float)samples[(int)(s * nSamples) * 2 + lrc] / 32767.0f)


void CAudioVisualizer::RenderDynamicTexture(CSurfx* pSurfx)
{
	CAudioClip* pAudioClip = (CAudioClip*)m_source;
	if (pAudioClip == NULL || pAudioClip->GetNodeClass() != NODE_CLASS(CAudioClip) || pAudioClip->m_transportMode != TRANSPORT_PLAY)
		return;

	short* samples = (short*)pAudioClip->GetSampleBuffer();
	if (samples == NULL)
		return;

	int nSamples = pAudioClip->GetSampleBufferSize() / 4;

	if (nSamples <= 0)
		return;

	// BLOCK: Setup our local buffers...
	{
		if (nSamples > 256)
			nSamples = 256;

		for (int i = 0; i < nSamples; i += 1)
		{
			m_pcmLeft[i] = samples[i * 2];
			m_pcmRight[i] = samples[i * 2 + 1];
		}

#ifdef _DEBUG
		if (i < 256)
			TRACE(_T("CAudioVisualizer:: fewer than 256 samples!\n"));
#endif

		for (; i < 256; i += 1)
		{
			m_pcmLeft[i] = 0;
			m_pcmRight[i] = 0;
		}

		m_bMonoValid = false;
		m_bFFTValid = false;
	}

	////////////////////////////////////////////////////////////////////////

	if (nSamples > pSurfx->m_nHeight)
		nSamples = pSurfx->m_nHeight;

	int lrc = 0;
	if (m_channel != NULL)
	{
		if (m_channel[0] == 'r' || m_channel[0] == 'R')
			lrc = 1;
	}

	int nType = 0;
	if (m_type != NULL)
	{
		switch (m_type[0])
		{
		case 's':
		case 'S':
			nType = 1; // spinner;
			break;

		case 'c':
		case 'C':
			nType = 2; // circle scope
			break;

		case 'a':
		case 'A':
			nType = 3; // spectrum analyzer
			break;
		}
	}

	float t = (float) XAppGetNow();

	switch (nType)
	{
	case 0:
		// Line Scope
		{
			int xCenter = pSurfx->m_nWidth / 2;
			int xp = xCenter;
			int yp = 0;

			for (int y = 0; y < nSamples; y += 1)
			{
				long s = (((long)samples[y * 2 + lrc]) * pSurfx->m_nWidth) >> 16;
				int x = xCenter + s;

				pSurfx->Line(xp, yp, x, y, 255);
				xp = x;
				yp = y;
			}
		}
		break;

	case 1: // Spinner Scope
		{
			int xCenter = pSurfx->m_nWidth / 2;
			int yCenter = pSurfx->m_nHeight / 2;

			float step = 1.0f / (float)pSurfx->m_nWidth;

			float firstX, firstY;
			float prevX, prevY;

			float A0 = 0.5f + rnd(0.2f); // rotation rate

			float B0 = cosf(t * 0.2f);
			float B1 = sinf(t * 0.2f);

			for (float s = 0.0f; s <= 1.0; s += step)
			{
				float C0 = mag(s) * m_scale + m_offset;// * 0.25f + 0.2f;
				float C1 = 2.1f * (s - 0.5f);

				float X0 = B0 * C1 + B1 * C0;
				float Y0 = -B0 * C0 + B1 * C1;

				float X1 = B0 * C1 - B1 * C0;
				float Y1 = B0 * C0 + B1 * C1;

				if (s == 0.0f)
				{
					firstX = X0;
					firstY = Y0;
				}
				else
				{
					pSurfx->Line(xCenter + (int)(prevX * xCenter), yCenter + (int)(prevY * yCenter),
						xCenter + (int)(X0 * xCenter), yCenter + (int)(Y0 * yCenter), 255);
				}

				prevX = X0;
				prevY = Y0;
			}

	//		pSurfx->Line(xCenter + (int)(prevX * xCenter), yCenter + (int)(prevY * yCenter),
	//			xCenter + (int)(firstX * xCenter), yCenter + (int)(firstY * yCenter), 255);
		}
		break;

#if 0
	case 2: // Circle Scope
		{
			int xCenter = pSurfx->m_nWidth / 2;
			int yCenter = pSurfx->m_nHeight / 2;

			float step = 1.0f / (float)pSurfx->m_nWidth;

			float firstX, firstY;
			float prevX, prevY;

			int i = 0;
			for (float s = 0.0f; s <= 1.0; s += step, i += 1)
			{
				float sample = (float)samples[i * 2 + lrc] / 32767.0f;

				float C0 = sample * m_scale + m_offset; // radius
				float C1 = s * D3DX_PI; // angle

				float X0 = C0 * sinf(C1);
				float Y0 = C0 * cosf(C1);

				int x = xCenter + (int)(X0 * xCenter);
				int y = yCenter + (int)(Y0 * yCenter);

#if 0
				if (x >= 0 && x < pSurfx->m_nWidth && y >= 0 && y < pSurfx->m_nHeight)
					*pSurfx->Pixel(x, y) = 255;
				x = xCenter - (int)(X0 * xCenter);
				if (x >= 0 && x < pSurfx->m_nWidth && y >= 0 && y < pSurfx->m_nHeight)
					*pSurfx->Pixel(x, y) = 255;
#else
				if (i == 0)
				{
					firstX = X0;
					firstY = Y0;
				}
				else
				{
					pSurfx->Line(xCenter + (int)(prevX * xCenter), yCenter + (int)(prevY * yCenter),
						xCenter + (int)(X0 * xCenter), yCenter + (int)(Y0 * yCenter), 255);
					pSurfx->Line(xCenter - (int)(prevX * xCenter), yCenter + (int)(prevY * yCenter),
						xCenter - (int)(X0 * xCenter), yCenter + (int)(Y0 * yCenter), 255);
				}

				prevX = X0;
				prevY = Y0;
#endif
			}

	//		pSurfx->Line(xCenter + (int)(prevX * xCenter), yCenter + (int)(prevY * yCenter),
	//			xCenter + (int)(firstX * xCenter), yCenter + (int)(firstY * yCenter), 255);
		}
		break;
#endif

	case 3: // spectrum analyzer
		{
			UpdateSpectrum();

			static int peak_buf [256];
			BYTE spectrum [256];

			for (int i = 0; i < 128; i += 1)
			{
				float n = logf(m_fftLeft[127 - i]) * 8.0f;
				if (n <= 0.0f)
					spectrum[i] = 0;
				else if (n >= 255.0f)
					spectrum[i] = 255;
				else
					spectrum[i] = (BYTE)n;
			}

			for (; i < 256; i += 1)
			{
				float n = logf(m_fftRight[i - 128]) * 8.0f;
				if (n <= 0.0f)
					spectrum[i] = 0;
				else if (n >= 255.0f)
					spectrum[i] = 255;
				else
					spectrum[i] = (BYTE)n;
			}


			for (i = 0; i < 256; i += 1)
			{
				if (peak_buf[i] > 2)
					peak_buf[i] -= 2;
				else
					peak_buf[i] = 0;

				int y = spectrum[i] * 2;
				if (peak_buf[i] < y)
					peak_buf[i] = y;
			}

			int nWidth = pSurfx->m_nWidth;
			for (int x = 0; x < nWidth; x += 1)
			{
				float nHeight = ((((float)spectrum[(x * 256) / nWidth])) * (float)pSurfx->m_nHeight) / 256.0f;
				ASSERT((int)nHeight <= pSurfx->m_nHeight);

				BYTE bColor = 255;
				for (int y = pSurfx->m_nHeight - (int)nHeight; y < pSurfx->m_nHeight; y += 1, bColor -= 1)
					*pSurfx->Pixel(x, y) = bColor;

				nHeight = (float)peak_buf[(x * 256) / nWidth];
				nHeight /= 2.0f;
				nHeight = (nHeight * (float)pSurfx->m_nHeight) / 256.0f;
				*pSurfx->Pixel(x, pSurfx->m_nHeight - (int)nHeight) = 255;
			}

			RenderEffect1(pSurfx);
			RenderEffect2(pSurfx);
		}
		break;
	}
}

#define BASS_EXT_MEMORY 10

struct bass_info {
	int max_recent;
	int max_old;
	int time_last_max;
	int min_recent;
	int min_old;
	int time_last_min;
	int activated;
} bass_info;


void CAudioVisualizer::RenderEffect1(CSurfx* pSurfx)
{
	static int t = 0;

	int bass = 0;
	const int step = 5;
	for (int i = 0; i < step; i += 1)
		bass += (m_fftLeft[i] >> 4) + (m_fftRight[i] >> 4);
	bass /= (step * 2);

	if (bass > bass_info.max_recent)
		bass_info.max_recent = bass;

	if (bass < bass_info.min_recent)
		bass_info.min_recent = bass;

	if (t - bass_info.time_last_max > BASS_EXT_MEMORY)
	{
		bass_info.max_old = bass_info.max_recent;
		bass_info.max_recent = 0;
		bass_info.time_last_max = t;
	}

	if (t - bass_info.time_last_min > BASS_EXT_MEMORY)
	{
		bass_info.min_old = bass_info.min_recent;
		bass_info.min_recent = 0;
		bass_info.time_last_min = t;
	}

	if (bass > (bass_info.max_old * 6 + bass_info.min_old * 4) / 10 && bass_info.activated == 0)
	{
//		TRACE(_T("BEAT!\n"));
		FillMemory(pSurfx->m_pels, pSurfx->m_nWidth * pSurfx->m_nHeight, 255);
		bass_info.activated = 1;
	}

	if (bass < (bass_info.max_old * 4 + bass_info.min_old * 6) / 10 && bass_info.activated == 1)
		bass_info.activated = 0;

	t += 1;
}

////////////////////////////////////////////////////////////////////////////


#define PI D3DX_PI

struct sincos {
	int i;
	float *f;
};
static struct sincos cosw = { 0, NULL };
static struct sincos sinw = { 0, NULL };

int spectral_amplitude = 50;
int spectral_shift = 30;
int mode_spectre = -1;
BYTE spectral_color = 128;

void SetPixel(CSurfx* pSurfx, int x, int y, BYTE color)
{
	if (x < 0 || y < 0 || x >= pSurfx->m_nWidth || y >= pSurfx->m_nHeight)
		return;
	*pSurfx->Pixel(x, y) = color;
}

void SetPixel2(CSurfx* pSurfx, int x, int y, BYTE color)
{
	SetPixel(pSurfx, x, y, color);
	SetPixel(pSurfx, x + 1, y, color);
	SetPixel(pSurfx, x + 1, y + 1, color);
	SetPixel(pSurfx, x, y + 1, color);
}

void CAudioVisualizer::RenderEffect2(CSurfx* pSurfx)
{
	int halfheight, halfwidth;
	float old_y1, old_y2;
	float y1 = (float)((((m_pcmLeft[0] + m_pcmRight[0]) >> 9) * spectral_amplitude * pSurfx->m_nHeight) >> 12);
	float y2 = (float)((((m_pcmLeft[0] + m_pcmRight[0]) >> 9) * spectral_amplitude * pSurfx->m_nHeight) >> 12);
	const int density_lines = 5;
	const int step = 4;
	const int shift = (spectral_shift * pSurfx->m_nHeight) >> 8;

	static XTIME timeToChange = 0.0f;
	if (mode_spectre < 0 || XAppGetNow() >= timeToChange)
	{
		mode_spectre += 1;
		timeToChange = XAppGetNow() + 3.0f + rnd(5.0f);
	}

	if ((UINT)mode_spectre > 4)
		mode_spectre = 0;

	if (cosw.i != pSurfx->m_nWidth || sinw.i != pSurfx->m_nWidth)
	{
		delete [] cosw.f;
		delete [] sinw.f;
		sinw.f = cosw.f = NULL;
		sinw.i = cosw.i = 0;
	}

	if (cosw.i == 0 || cosw.f == NULL)
	{
	 	const float halfPI  = (float)PI / 2;
		cosw.i = pSurfx->m_nWidth;
		cosw.f = new float [pSurfx->m_nWidth];
		for (int i = 0; i < pSurfx->m_nWidth; i += step)
			cosw.f[i] = cosf((float)i / pSurfx->m_nWidth * PI + halfPI);
	}

	if (sinw.i == 0 || sinw.f == NULL)
	{
	 	const float halfPI = (float)PI / 2;
		sinw.i = pSurfx->m_nWidth;
		sinw.f = new float [pSurfx->m_nWidth];
		for (int i = 0; i < pSurfx->m_nWidth; i += step)
			sinw.f[i] = sinf((float)i / pSurfx->m_nWidth * PI + halfPI);
	}

	if (mode_spectre == 3)
	{
		if (y1 < 0)
			y1 = 0;
		if (y2 < 0)
			y2 = 0;
	}
	
	halfheight = pSurfx->m_nHeight >> 1;
	halfwidth  = pSurfx->m_nWidth >> 1;

	for (int i = step; i < pSurfx->m_nWidth; i += step)
	{
		old_y1 = y1;
		old_y2 = y2;

		y1 = (float)(((m_pcmRight[(i << 8) / pSurfx->m_nWidth / density_lines] >> 8) * spectral_amplitude * pSurfx->m_nHeight) >> 12);
		y2 = (float)((( m_pcmLeft[(i << 8) / pSurfx->m_nWidth / density_lines] >> 8) * spectral_amplitude * pSurfx->m_nHeight) >> 12);

		switch (mode_spectre)
		{
		case 0:
			pSurfx->Line(i-step,(int)(halfheight+shift+old_y2),
			     i,(int)(halfheight+shift+y2),
			     spectral_color);
			break;

		case 1:
			pSurfx->Line(i-step,(int)(halfheight+shift+old_y1),
			     i,(int)(halfheight+shift+y1),
			     spectral_color);
			pSurfx->Line(i-step,(int)(halfheight-shift+old_y2),
			     i,(int)(halfheight-shift+y2),
			     spectral_color);
			break;

		case 2:
			pSurfx->Line(i-step,(int)(halfheight+shift+old_y1),
			     i,(int)(halfheight+shift+y1),
			     spectral_color);
			pSurfx->Line(i-step,(int)(halfheight-shift+old_y1),
			     i,(int)(halfheight-shift+y1),
			     spectral_color);
			pSurfx->Line((int)(halfwidth+shift+old_y2),i-step,
			     (int)(halfwidth+shift+y2),i,
			     spectral_color);
			pSurfx->Line((int)(halfwidth-shift+old_y2),i-step,
			     (int)(halfwidth-shift+y2),i,
			     spectral_color);	
			break;

		case 3:
			if (y1<0)
				y1=0;
			if (y2<0)
				y2=0;
			// FALL THROUGH

		case 4:
			pSurfx->Line(
				(int)(halfwidth  + cosw.f[i - step] * (shift + old_y1)),
				(int)(halfheight + sinw.f[i - step] * (shift + old_y1)),
				(int)(halfwidth  + cosw.f[i]        * (shift + y1)),
				(int)(halfheight + sinw.f[i]        * (shift + y1)),
				spectral_color);
			pSurfx->Line(
				(int)(halfwidth  - cosw.f[i - step] * (shift + old_y2)),
				(int)(halfheight + sinw.f[i - step] * (shift + old_y2)),
				(int)(halfwidth  - cosw.f[i]        * (shift + y2)),
				(int)(halfheight + sinw.f[i]        * (shift + y2)),
				spectral_color);			
			break;
		}
	}

	if (mode_spectre == 3 || mode_spectre == 4)
	{
		pSurfx->Line(
			(int)(halfwidth  + cosw.f[pSurfx->m_nWidth - step] * (shift+y1)),
			(int)(halfheight + sinw.f[pSurfx->m_nWidth - step] * (shift+y1)),
			(int)(halfwidth  - cosw.f[pSurfx->m_nWidth - step] * (shift+y2)),
			(int)(halfheight + sinw.f[pSurfx->m_nWidth - step] * (shift+y2)),
			spectral_color);
	}

#define curve_color 255
#define curve_amplitude 50
static int x_curve = 0;

	// BLOCK: Curve...
	{
		int i, j, k;
		float v, vr;
		float x, y;
		float amplitude = (float)curve_amplitude / 256;

		for (j = 0; j < 2; j += 1)
		{
			v = 80;
			vr = 0.001f;
			k = x_curve;
			for (i = 0; i < 64; i += 1)
			{
				x = cosf((float)(k) / (v + v * j * 1.34f)) * pSurfx->m_nHeight * amplitude;
				y = sinf((float)(k) / (1.756f * (v + v * j * 0.93f))) * pSurfx->m_nHeight * amplitude;
				SetPixel2(pSurfx, (int)(x * cosf((float)k * vr) + y * sinf((float)k * vr) + pSurfx->m_nWidth / 2), (int)(x * sinf((float)k * vr) - y * cosf((float)k * vr) + pSurfx->m_nHeight / 2), curve_color);
				k++;
			}
		}

		x_curve = k;
	}
}

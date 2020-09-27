#ifndef __WAVE_STRIPLAYER_H_
#define __WAVE_STRIPLAYER_H_

class CWaveStrip;


class CWaveStripLayer
{
friend class CWaveStrip;

public:
	CWaveStripLayer( CWaveStrip* pWaveStrip );
	virtual ~CWaveStripLayer();

private:
	CWaveStrip*		m_pWaveStrip;
	BOOL			m_fSelected;
};

#endif // __WAVE_STRIPLAYER_H_

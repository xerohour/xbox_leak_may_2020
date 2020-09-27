#ifndef __FADE_H
#define __FADE_H

struct Fade
{
	int         m_fade;
	int         m_rate;
	int         m_r;
	int         m_g;
	int         m_b;

	void        m_Draw(void);
	void        m_Draw(int x,int y,int w,int h);

	inline bool m_FadedOut(void) { return(m_fade==255); }
	inline bool m_FadedIn (void) { return(m_fade==0); }
	inline void m_SetFade(int r) { m_fade = r; }
	inline void m_SetFadedOut(void) { m_fade = 255; }
	inline void m_SetFadedIn (void) { m_fade = 0; }
	inline void m_SetRate(int r) { m_rate = r; }
	inline void m_SetColor(int r,int g,int b) { m_r=r; m_g=g; m_b=b; }

	void        m_OneFrameUpkeep(void);
};

#define STANDARD_FADE_RATE 12

#endif
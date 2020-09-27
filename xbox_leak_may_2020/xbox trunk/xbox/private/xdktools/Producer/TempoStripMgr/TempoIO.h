#ifndef __TEMPOIO_H_
#define __TEMPOIO_H_

#include "PropTempo.h"
#ifndef MUSIC_TIME
#include <dmusici.h>
#endif

typedef struct _PROD_IO_TEMPO_ITEM
{
    MUSIC_TIME    lTime;
    double        dblTempo;
	MUSIC_TIME    lOffset;
} PROD_IO_TEMPO_ITEM;

class CTempoItem : public CPropTempo
{
public:
	CTempoItem();
	CTempoItem(const CTempoItem&);

	BOOL After(const CTempoItem& Tempo);
	BOOL Before(const CTempoItem& Tempo);

//	Used to track the selection of the tempo in display.
	BOOL		m_fSelected;	// This tempo is currently selected.
	CString		m_strText;		// Textual representation of the tempo
};

#endif // __TEMPOIO_H_

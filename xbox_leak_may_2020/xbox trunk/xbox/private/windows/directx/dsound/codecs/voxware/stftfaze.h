#ifndef _STFTPHASE_H
#define _STFTPHASE_H

/*----------------------------------------------------------------------------
* $Header:   /export/phobos/pvcs/r_and_d/archives/scaleable/encode/STFTfaze.h_v   1.0   06 Feb 1998 10:06:50   johnp  $
----------------------------------------------------------------------------*/

unsigned short VoxSTFTPhase ( float *pfSWR,
                              float *pfSWI,
                              int   iVEVphases,
                              float *pfVEVphase,
                              float fPitchDFT,
                              short WinShift
                            );
unsigned short VoxCodePhase ( float fPhase,
                      int   iNlevels,
                      float *pfPhase_q
                    );

#endif /* _STFTPHASE_H */


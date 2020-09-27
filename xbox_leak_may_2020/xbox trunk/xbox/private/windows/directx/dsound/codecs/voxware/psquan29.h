#ifndef PSQUAN29_H
#define PSQUAN29_H

#ifndef _VEM_RETCODE_
#define _VEM_RETCODE_
typedef unsigned short VEM_RETCODE; /* Voxware Core Interface error type.     */

#endif  /** #ifndef _VEM_RETCODE_ **/

/* main functions for init and free */
VEM_RETCODE PsInitQuan(void *pvCodecBlkEnc, void *pvCodecBlkDec);

#endif /*PSQUAN29_H */


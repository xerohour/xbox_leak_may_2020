/*
 * nv_local.h
 *     placeholder for customer/OEM definitions for required
 *     macros for devinit() code.  It is hoped that all customer
 *     changes can be confined to this file.
 */

/*
 * read/write memory
 */

#define REG_WR32(a,d)   
#define REG_RD32(a)       0
#define REG_WR08(a,d)   
#define REG_RD08(a)       0

#define PRMCIO_REG_WR32(b,o,d)   
#define PRMCIO_REG_RD32(b,o)     0

#define CRTC_WR(i,d)    
#define CRTC_RD(i,d)       0


#ifdef DEBUG
#include <stdout.h>
#define DBG_PRINT_STR(s)  fprintf(stderr, (s))
#else
#define DBG_PRINT_STR(s)  
#endif

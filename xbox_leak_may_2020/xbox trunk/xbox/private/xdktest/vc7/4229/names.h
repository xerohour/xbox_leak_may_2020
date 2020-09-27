#ifndef __NAMES_H
#define __NAMES_H

/* NOTE: FINAL_ROM is now defined 1/0 by the MSDev configuration */

/*#if defined(_LANGUAGE_C) || defined(_LANGUAGE_C_PLUS_PLUS)*/
#  if !FINAL_ROM
/* special version per person/station */
/* these values are set based on person.h */
/* do NOT include person.h! */
extern bool g_brian;
extern bool g_derek;
extern bool g_chris;
extern bool g_richard;
extern bool g_rob;
extern bool g_colin;
extern bool g_chad;
extern bool g_msBuild;
extern bool g_testersBuild;
extern bool g_artistsBuild;
#  endif
/*#endif*/

#endif

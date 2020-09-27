#ifndef _CVWMEMMAP
#define _CVWMEMMAP

#ifdef __cplusplus
extern "C" {
#endif

void FAR * cvw3_fmalloc( size_t );
void _HUGE_ * CDECL cvw3_halloc( long, size_t );
size_t cvw3_fmsize(void FAR *buffer);
void FAR *cvw3_frealloc(void FAR *buffer, size_t size);
void cvw3_ffree( void FAR * );
void cvw3_hfree( void _HUGE_ * );

#ifdef __cplusplus
} //extern "C"
#endif

#endif	// !_CVWMEMMAP

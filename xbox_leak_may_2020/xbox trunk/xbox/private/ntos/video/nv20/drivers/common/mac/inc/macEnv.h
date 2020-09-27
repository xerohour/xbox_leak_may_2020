#ifndef __MACENV__
#define __MACENV__


#ifdef __cplusplus
extern "C" {
#endif



char * GetEnv( char * inKey );
void SetEnv( char * inKey, char * inValue );



#ifdef __cplusplus
}
#endif

#endif /* __MACENV__ */

/*
 *  Copyright (c) Microsoft Corporation.  All rights reserved.
 *
 */



extern "C"  do_sos_init_return(void);
extern "C"	sos_main(void);
extern "C"  put_fifo(unsigned char);
extern "C"  dev_cleanup(void);

#ifdef  __cplusplus
extern "C" {
#endif

// All boot sound functions must be called at passive level

void BootSound_Start(); // Call once to start boot sound
void BootSound_Stop(void); // Call once to stop boot sound





#ifdef  __cplusplus
};


#endif

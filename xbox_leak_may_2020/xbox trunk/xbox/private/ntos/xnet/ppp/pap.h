#ifndef __PAP_H__
#define __PAP_H__

// PAP packet codes from PAP spec.
#define PAPCODE_Req 1
#define PAPCODE_Ack 2
#define PAPCODE_Nak 3

#define MAX_PAP_CODE  3

typedef enum _PAP_STATE {
    PS_Initial,
    PS_RequestSent,
    PS_WaitForRequest,
    PS_WaitForAuthenticationToComplete,
    PS_Done
} PAP_STATE;

#endif // __PAP_H__


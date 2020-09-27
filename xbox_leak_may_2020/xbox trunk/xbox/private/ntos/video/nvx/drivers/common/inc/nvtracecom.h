#ifndef __NV_TRACECOM_H
#define __NV_TRACECOM_H
// This struct is used for debug code.

typedef struct nvFunRec {
    char *uname;
    char *name;
    int class;
    int level;
    struct nvFunRec *next;
    int count;
    struct {
        int msh, lsh;
    } time;
} nvFunRec;

extern int nvDebugOptions;
extern int nvDebugMask;
extern int nvDebugLevel;
extern int nvControlOptions;

#endif __NV_TRACECOM_H

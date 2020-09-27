//**************************************************************************
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NOTICE TO USER:   The source code  is copyrighted under  U.S. and
//     international laws.  Users and possessors of this source code are
//     hereby granted a nonexclusive,  royalty-free copyright license to
//     use this code in individual and commercial software.
//
//     Any use of this source code must include,  in the user documenta-
//     tion and  internal comments to the code,  notices to the end user
//     as follows:
//
//       Copyright 1993-1999 NVIDIA, Corporation.  All rights reserved.
//
//     NVIDIA, CORPORATION MAKES NO REPRESENTATION ABOUT THE SUITABILITY
//     OF  THIS SOURCE  CODE  FOR ANY PURPOSE.  IT IS  PROVIDED  "AS IS"
//     WITHOUT EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORPOR-
//     ATION DISCLAIMS ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,
//     INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY, NONINFRINGE-
//     MENT,  AND FITNESS  FOR A PARTICULAR PURPOSE.   IN NO EVENT SHALL
//     NVIDIA, CORPORATION  BE LIABLE FOR ANY SPECIAL,  INDIRECT,  INCI-
//     DENTAL, OR CONSEQUENTIAL DAMAGES,  OR ANY DAMAGES  WHATSOEVER RE-
//     SULTING FROM LOSS OF USE,  DATA OR PROFITS,  WHETHER IN AN ACTION
//     OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,  ARISING OUT OF
//     OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOURCE CODE.
//
//     U.S. Government  End  Users.   This source code  is a "commercial
//     item,"  as that  term is  defined at  48 C.F.R. 2.101 (OCT 1995),
//     consisting  of "commercial  computer  software"  and  "commercial
//     computer  software  documentation,"  as such  terms  are  used in
//     48 C.F.R. 12.212 (SEPT 1995)  and is provided to the U.S. Govern-
//     ment only as  a commercial end item.   Consistent with  48 C.F.R.
//     12.212 and  48 C.F.R. 227.7202-1 through  227.7202-4 (JUNE 1995),
//     all U.S. Government End Users  acquire the source code  with only
//     those rights set forth herein.
//
// **************************************************************************
//
//  Module: nvGLBackend.c
//        gl immediate mode entry points for the patch shared library
//
//  History:
//        Daniel Rohrer    (drohrer)      23Jun00      created
//
// **************************************************************************

/*** OpenGL immediate mode backend ***/

static void GLImm_BeginPrimitive(void *info, NV_PATCH_PRIMITIVE_TYPE primType)
{
    __GLcontext *gc = (__GLcontext *)((NV_PATCH_INFO *)info)->context;

    switch (primType) {
    case NV_PATCH_PRIMITIVE_TSTRIP:
        (*gc->dispatchState->dispatch.Begin)(GL_TRIANGLE_STRIP);
        break;
    case NV_PATCH_PRIMITIVE_TFAN:
        (*gc->dispatchState->dispatch.Begin)(GL_TRIANGLE_FAN);
        break;
    }
}

static void GLImm_EndPrimitive(void *info)
{
    __GLcontext *gc = (__GLcontext *)((NV_PATCH_INFO *)info)->context;

    (*gc->dispatchState->dispatch.End)();
}

#if defined(COMPILE_DEBUG_LIBRARY)
// to check that proper unique vertices are generated
#define MAX_UNIQUE_CHECK    (16*16) //512
int enableUniqueCheck = 0;
int nUniqueVertices = 0;
float   uniqueVertices[MAX_UNIQUE_CHECK][4];

#define EQUAL_VERTEX4FV(p0, p1) ( \
           (EV_AS_INT(p0[0]) == EV_AS_INT(p1[0])) \
        && (EV_AS_INT(p0[1]) == EV_AS_INT(p1[1])) \
        && (EV_AS_INT(p0[2]) == EV_AS_INT(p1[2])) \
        && (EV_AS_INT(p0[3]) == EV_AS_INT(p1[3])) \
    ) \

void addUniqueVertex4fv(float *v)
{
    int ii;

    if (nvDebugLevel < 2) {
        return;
    }
    for (ii = 0;  ii < nUniqueVertices;  ii++) {
        if (EQUAL_VERTEX4FV(uniqueVertices[ii], v)) {
            // found match
            return;
        }
    }
    assert(nUniqueVertices < MAX_UNIQUE_CHECK);
    if (nUniqueVertices >= MAX_UNIQUE_CHECK) {
        return;
    }
    uniqueVertices[nUniqueVertices][0] = v[0];
    uniqueVertices[nUniqueVertices][1] = v[1];
    uniqueVertices[nUniqueVertices][2] = v[2];
    uniqueVertices[nUniqueVertices][3] = v[3];
    nUniqueVertices++;
}

void drawUniqueVertices(void)
{
    int ii;
    float *p;

    glColor3f(0.5f, 0.f, 0.f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);

    glPointSize(16);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBegin(GL_POINTS);
    //ffglBegin(GL_TRIANGLES);
    for (ii = 0;  ii < nUniqueVertices;  ii++) {
        p = uniqueVertices[ii];
        TPRINTF(("%2d) x%08X x%08X x%08X : %g %g %g\n", ii,
                    EV_AS_INT(p[0]), EV_AS_INT(p[1]), EV_AS_INT(p[2]),
                    p[0], p[1], p[2]));
        glVertex4fv(p);

        //ffglVertex3f(p[0],        p[1],       p[2]);
        //ffglVertex3f(p[0]+.05,    p[1],       p[2]);
        //ffglVertex3f(p[0]+.05,    p[1]+.05,   p[2]);
        //ffglVertex3f(p[0],        p[1],       p[2]);
        //ffglVertex3f(p[0]+.05,    p[1],       p[2]);
        //ffglVertex3f(p[0]+.05,    p[1]-.05,   p[2]);
    }
    glEnd();
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
}
#endif

// This won't work quite right until we have state aliasing for vertex programs.
static void GLImm_SendVertex(void *info,
                             NV_PATCH_EVAL_OUTPUT *pData, int index)
{
    __GLcontext *gc = (__GLcontext *)((NV_PATCH_INFO *)info)->context;
    unsigned int evalEnables = ((NV_PATCH_INFO *)info)->evalEnables;
    float *attribs = &pData->vertexAttribs[index][0][0];
    int i;

#if defined(COMPILE_DEBUG_LIBRARY)
    if (enableUniqueCheck) {
        addUniqueVertex4fv(&attribs[4*__GL_ATTRIB_POSITION]);
        NV_TRACE_COND(TR_EVAL, 35,
            tprintf("vert x%08X x%08X x%08X : %g %g %g\n",
                    EV_AS_INT(attribs[4*__GL_ATTRIB_POSITION+0]),
                    EV_AS_INT(attribs[4*__GL_ATTRIB_POSITION+1]),
                    EV_AS_INT(attribs[4*__GL_ATTRIB_POSITION+2]),
                    attribs[4*__GL_ATTRIB_POSITION+0],
                    attribs[4*__GL_ATTRIB_POSITION+1],
                    attribs[4*__GL_ATTRIB_POSITION+2]);
        );
    }
#endif

    if (gc->state.enables.general & __GL_VERTEX_PROGRAM_ENABLE) {
        for (i = 1; i < __GL_NUMBER_OF_VERTEX_ATTRIBS; i++) {
            if (evalEnables & (1 << i)) {
                (*gc->dispatchState->extensions.VertexAttrib4fvNV)(i, &attribs[4*i]);
            }
        }
        (*gc->dispatchState->extensions.VertexAttrib4fvNV)(0, &attribs[4*0]);
    } else {
        if (evalEnables & (1 << __GL_ATTRIB_VERTEX_WEIGHT)) {
            (*gc->dispatchState->extensions.VertexWeightfvEXT)
                (&attribs[4*__GL_ATTRIB_VERTEX_WEIGHT]);
        }
        if (evalEnables & (1 << __GL_ATTRIB_NORMAL)) {
            (*gc->dispatchState->dispatch.Normal3fv)
                (&attribs[4*__GL_ATTRIB_NORMAL]);
        }
        if (evalEnables & (1 << __GL_ATTRIB_COLOR)) {
            (*gc->dispatchState->dispatch.Color4fv)
                (&attribs[4*__GL_ATTRIB_COLOR]);
        }
        if (evalEnables & (1 << __GL_ATTRIB_SECONDARY_COLOR)) {
            (*gc->dispatchState->extensions.SecondaryColor3fvEXT)
                (&attribs[4*__GL_ATTRIB_SECONDARY_COLOR]);
        }
        if (evalEnables & (1 << __GL_ATTRIB_FOG_COORD)) {
            (*gc->dispatchState->extensions.FogCoordfvEXT)
                (&attribs[4*__GL_ATTRIB_FOG_COORD]);
        }
        // 2 unused attribs
        for (i = 0; i < __GL_NUMBER_OF_TEXTURE_UNITS; i++) {
            if (evalEnables & (1 << (__GL_ATTRIB_TEXCOORD0 + i))) {
                (*gc->dispatchState->extensions.MultiTexCoord4fvARB)
                    (GL_TEXTURE0_ARB + i, &attribs[4*(__GL_ATTRIB_TEXCOORD0 + i)]);
            }
        }
        // 6 (NV10) or 4 (NV20) unused attribs

        // Must do position (vertex attrib 0) last
        (*gc->dispatchState->dispatch.Vertex4fv)
            (&attribs[4*__GL_ATTRIB_POSITION]);
    }
}

// This allows us to flip the orientation of geometry.
static void GLImm_SetFrontFace(void *info, int reversed)
{
    __GLcontext *gc = (__GLcontext *)((NV_PATCH_INFO *)info)->context;
    unsigned int evalEnables = ((NV_PATCH_INFO *)info)->evalEnables;

    (*gc->dispatchState->dispatch.FrontFace)
        (gc->eval.cachedFrontFaceDirection ^ reversed);
}

static void GLImm_BeginPatch(void *void_info)
{
    NV_PATCH_INFO *info = (NV_PATCH_INFO *)void_info;
    __GLNVstate *state = NV_GET_HWSTATE(info->context);

    assert(!(state->fdEvalMask & NV_PATCH_STATE_OPEN));
    state->fdEvalMask |= NV_PATCH_STATE_OPEN | NV_PATCH_STATE_IN_PB;
}

static void GLImm_EndPatch(void *void_info)
{
    NV_PATCH_INFO *info = (NV_PATCH_INFO *)void_info;
    __GLNVstate *state = NV_GET_HWSTATE(info->context);

    assert(state->fdEvalMask & NV_PATCH_STATE_OPEN);
    state->fdEvalMask &= ~NV_PATCH_STATE_OPEN;
}

static NV_PATCH_BACKEND GLImm_Backend = {
    NULL,
    NULL,
    GLImm_BeginPrimitive,
    GLImm_EndPrimitive,
    GLImm_SendVertex,
    GLImm_SetFrontFace,
    GLImm_BeginPatch,
    GLImm_EndPatch,
};



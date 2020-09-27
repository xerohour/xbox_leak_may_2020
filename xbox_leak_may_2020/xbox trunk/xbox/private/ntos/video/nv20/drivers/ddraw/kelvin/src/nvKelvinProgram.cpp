// **************************************************************************
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
// ********************************* Direct 3D ******************************
//
//  Module: nvKelvinProgram.cpp
//      Routines for management of Kelvin programs
//
// **************************************************************************
//
//  History:
//      Craig Duttweiler        23May00         NV20 development
//
// **************************************************************************
#include "nvprecomp.h"

#if (NVARCH >= 0x20)

extern DWORD dwCelsiusMethodDispatch[11][16][2];

//---------------------------------------------------------------------------

long getout (char *s)
{
    long i=0,add;

    if (strstr(s,"HPOS")) add =  0;
    else if (strstr(s,"COL0")) add =  3;
    else if (strstr(s,"COL1")) add =  4;
    else if (strstr(s,"BFC0")) add =  7;
    else if (strstr(s,"BFC1")) add =  8;
    else if (strstr(s,"FOGC")) add =  5;
    else if (strstr(s,"PSIZ")) add =  6;
    else if (strstr(s,"TEX0")) add =  9;
    else if (strstr(s,"TEX1")) add = 10;
    else if (strstr(s,"TEX2")) add = 11;
    else if (strstr(s,"TEX3")) add = 12;
    else if (strstr(s,"TEX4")) add = 13;
    else if (strstr(s,"TEX5")) add = 14;
    else if (strstr(s,"TEX6")) add = 15;
    else if (strstr(s,"TEX7")) add = 15; /* dev null it */
    else {
        while (s[i] < '0' || s[i] > '9')
            i++;

        add = 0;
        while (s[i] >= '0' && s[i] <= '9') {
            add *= 10;
            add += s[i] - '0';
            i++;
        }
    }

    return(add);
}

//---------------------------------------------------------------------------

long readit
(
    char *s,
    long dest
)
{
    long msk,add,i,neg,mux,indexed,output;
    long xmsk,ymsk,zmsk,wmsk,hold0,hold1,j;

    indexed = 0;
    output = 0;

    if (dest) {
        if (strstr(s,"c[")) {
            mux = MX_C;
            if (strstr(s,"A0"))
                assert(0);
        }
        else if (strstr(s,"o[")) {
            mux = MX_O;
            output = 1;
            add = getout(s);
            if (strstr(s,"A0"))
                assert(0);
        }
        else if (strstr(s,"R")) {
            mux = MX_R;
        }
        else if (strstr(s,"v[")) {
            assert(0);
        }
        else {
            return(0);
        }
    }
    else {
        if (strstr(s,"c[")) {
            mux = MX_C;
            if (strstr(s,"A0"))
                indexed = IDX_MASK;
        }
        else if (strstr(s,"v[")) {
            mux = MX_V;
            if (strstr(s,"A0"))
                assert(0);
        }
        else if (strstr(s,"R")) {
            mux = MX_R;
        }
        else if (strstr(s,"o[")) {
            assert(0);
        }
        else {
            return(0);
        }
    }

    neg = 0;
    if (s[0] == '-')
        neg = SGN_MASK;

    i = 0;
    if (output) {
        while (s[i] != ']')
            i++;
    }
    else {
        while (s[i] < '0' || s[i] > '9')
            i++;

        if (indexed && s[i-1] == 'A' && s[i] == '0') {
            i++;
            while (s[i] < '0' || s[i] > '9')
                i++;
        }

        /* else address */
        add = 0;
        while (s[i] >= '0' && s[i] <= '9') {
            add *= 10;
            add += s[i] - '0';
            i++;
        }
    }
    if (add < 0 || add > 511)
        assert(0);
    add &= 0xff;

    hold0 = hold1 = 0;
    for (j=i; s[j] != '\0'; j++) {
        if (s[j] == '.') hold0 = j;
        if (s[j] == ']') hold1 = j+1;
    }
    i = (hold1 > hold0) ?  hold1 : hold0;

    /* write mask */
    if (dest) {
        msk = 15;
        if (s[i++] == '.') {
            msk = 0;
            while (s[i]=='x' || s[i]=='y' || s[i]=='z' || s[i]=='w') {
                if (s[i]=='x') {
                    if (msk & 0xf)
                        assert(0);
                    msk |= 8;
                }
                if (s[i]=='y') {
                    if (msk & 0x7)
                        assert(0);
                    msk |= 4;
                }
                if (s[i]=='z') {
                    if (msk & 0x3)
                        assert(0);
                    msk |= 2;
                }
                if (s[i]=='w') {
                    if (msk & 0x1)
                        assert(0);
                    msk |= 1;
                }
                i++;
            }
        }
    }

    /* swizzle mask */
    else {
        xmsk = 0;
        ymsk = 1;
        zmsk = 2;
        wmsk = 3;
        if (s[i++] == '.') {
            if (s[i] == 'x' && (s[i+1] < 'w' || s[i+1] > 'z'))
                xmsk = ymsk = zmsk = wmsk = CSW_X;
            else if (s[i] == 'y' && (s[i+1] < 'w' || s[i+1] > 'z'))
                xmsk = ymsk = zmsk = wmsk = CSW_Y;
            else if (s[i] == 'z' && (s[i+1] < 'w' || s[i+1] > 'z'))
                xmsk = ymsk = zmsk = wmsk = CSW_Z;
            else if (s[i] == 'w' && (s[i+1] < 'w' || s[i+1] > 'z'))
                xmsk = ymsk = zmsk = wmsk = CSW_W;
            else {
                switch (s[i++]) {
                    case 'x': xmsk = CSW_X; break;
                    case 'y': xmsk = CSW_Y; break;
                    case 'z': xmsk = CSW_Z; break;
                    case 'w': xmsk = CSW_W; break;
                    default: assert(0);
                }
                switch (s[i++]) {
                    case 'x': ymsk = CSW_X; break;
                    case 'y': ymsk = CSW_Y; break;
                    case 'z': ymsk = CSW_Z; break;
                    case 'w': ymsk = CSW_W; break;
                    default: assert(0);
                }
                switch (s[i++]) {
                    case 'x': zmsk = CSW_X; break;
                    case 'y': zmsk = CSW_Y; break;
                    case 'z': zmsk = CSW_Z; break;
                    case 'w': zmsk = CSW_W; break;
                    default: assert(0);
                }
                switch (s[i++]) {
                    case 'x': wmsk = CSW_X; break;
                    case 'y': wmsk = CSW_Y; break;
                    case 'z': wmsk = CSW_Z; break;
                    case 'w': wmsk = CSW_W; break;
                    default: assert(0);
                }
            }
        }

        msk = (wmsk<<6) | (zmsk<<4) | (ymsk<<2) | (xmsk<<0);
    }

    return(indexed | neg | (msk<<12) | (mux<<8) | add);
}

//---------------------------------------------------------------------------

void opcode
(
    PKELVIN_MICRO_PROGRAM pProgram,
    char *s1,
    char *s2
)
{
    long i,j,iflag,mflag,flog,loop,d1,d2,pp[10],pm,mux;
    long neg,msk,add,dubl,faddr;
    long count,start;
    char a[256];
    t_CheopsUcode *ucode;

    ucode = &(pProgram->ucode[pProgram->length]);

    // default fields
    ucode->mac = NV_IGRAPH_XF_V_NOP;
    ucode->ilu = NV_IGRAPH_XF_S_NOP;
    ucode->ca  = 0;
    ucode->va  = 0;
    ucode->ane = 0;
    ucode->axs = CSW_X;
    ucode->ays = CSW_Y;
    ucode->azs = CSW_Z;
    ucode->aws = CSW_W;
    ucode->amx = MX_R;
    ucode->arr = 0;
    ucode->bne = 0;
    ucode->bxs = CSW_X;
    ucode->bys = CSW_Y;
    ucode->bzs = CSW_Z;
    ucode->bws = CSW_W;
    ucode->bmx = MX_R;
    ucode->brr = 0;
    ucode->cne = 0;
    ucode->cxs = CSW_X;
    ucode->cys = CSW_Y;
    ucode->czs = CSW_Z;
    ucode->cws = CSW_W;
    ucode->cmx = MX_R;
    ucode->crr = 0;
    ucode->rw  = 7;
    ucode->rwm = 0;
    ucode->oc  = 0x1ff;
    ucode->om  = OM_MAC;
    ucode->eos = 0;
    ucode->cin = 0;
    ucode->swm = 0;

    for (i=0; i<10; i++)
        pp[i] = 0;

    /********************************/
    /* one or two instructions      */
    /********************************/
    dubl = 0;
    if (s1[0] != '\0' && s2[0] != '\0')
        dubl = 1;

    /********************************/
    /* s1 processing                */
    /********************************/
    if (s1[0] != '\0')
        for (pm=0,i=3,loop=0; loop<5; loop++) {
            flog = j = 0;
            while (s1[i] != ',' && s1[i] != '\0') {
                if (s1[i] == '{')
                    pm = DST_MASK;
                if (s1[i] == '}') {
                    flog = DST_MASK;
                    pm = 0;
                }
                if (s1[i] == '{' || s1[i] == '}')
                    i++;
                else
                    a[j++] = s1[i++];

                if (j >= 200)
                    assert(0);
            }
            a[j] = '\0';

            d1 = readit(a,(pm?1:0)|(loop==0)|(flog?1:0));

            if (d1 != 0) {
                d1 |= pm | flog;
                if (loop == 0)
                    d1 |= DST_MASK;
            }

            pp[loop] = d1;
            if (s1[i] != '\0')
                i++;
        }

    /********************************/
    /* s2 processing                */
    /********************************/
    if (s2[0] != '\0')
        for (pm=0,i=3,loop=6; loop<10; loop++) {
            flog = j = 0;
            while (s2[i] != ',' && s2[i] != '\0') {
                if (s2[i] == '{')
                    pm = DST_MASK;
                if (s2[i] == '}') {
                    flog = DST_MASK;
                    pm = 0;
                }
                if (s2[i] == '{' || s2[i] == '}')
                    i++;
                else
                    a[j++] = s2[i++];

                if (j >= 200)
                    assert(0);
            }
            a[j] = '\0';

            d2 = readit(a,(pm?1:0)|(loop==6)|(flog?1:0));

            if (d2 != 0) {
                d2 |= pm | flog;
                if (loop == 6)
                    d2 |= DST_MASK;
            }

            pp[loop] = d2;
            if (s2[i] != '\0')
                i++;
        }

    /*** sign check ***/
    for (i=0; i<10; i++)
        if ((pp[i]&DST_MASK) && (pp[i]&SGN_MASK))
            assert(0);

        /********************************/
        /* mac/ilu processing           */
        /********************************/
    mflag = 0;
    iflag = 0;
    if (strstr(s1,"NOP")) {
        SET_MAC(ucode,NV_IGRAPH_XF_V_NOP);
    }
    else if (strstr(s1,"MOV")) {
        SET_MAC(ucode,NV_IGRAPH_XF_V_MOV); mflag = 1;
    }
    else if (strstr(s1,"RCP")) {
        SET_ILU(ucode,NV_IGRAPH_XF_S_RCP); iflag = 1;
    }
    else if (strstr(s1,"RCC")) {
        SET_ILU(ucode,NV_IGRAPH_XF_S_RCC); iflag = 1;
    }
    else if (strstr(s1,"RSQ")) {
        SET_ILU(ucode,NV_IGRAPH_XF_S_RSQ); iflag = 1;
    }
    else if (strstr(s1,"MUL")) {
        SET_MAC(ucode,NV_IGRAPH_XF_V_MUL); mflag = 1;
    }
    else if (strstr(s1,"ADD")) {
        SET_MAC(ucode,NV_IGRAPH_XF_V_ADD); mflag = 1;
    }
    else if (strstr(s1,"MAD")) {
        SET_MAC(ucode,NV_IGRAPH_XF_V_MAD); mflag = 1;
    }
    else if (strstr(s1,"DP3")) {
        SET_MAC(ucode,NV_IGRAPH_XF_V_DP3); mflag = 1;
    }
    else if (strstr(s1,"DPH")) {
        SET_MAC(ucode,NV_IGRAPH_XF_V_DPH); mflag = 1;
    }
    else if (strstr(s1,"DP4")) {
        SET_MAC(ucode,NV_IGRAPH_XF_V_DP4); mflag = 1;
    }
    else if (strstr(s1,"DST")) {
        SET_MAC(ucode,NV_IGRAPH_XF_V_DST); mflag = 1;
    }
    else if (strstr(s1,"MIN")) {
        SET_MAC(ucode,NV_IGRAPH_XF_V_MIN); mflag = 1;
    }
    else if (strstr(s1,"MAX")) {
        SET_MAC(ucode,NV_IGRAPH_XF_V_MAX); mflag = 1;
    }
    else if (strstr(s1,"SLT")) {
        SET_MAC(ucode,NV_IGRAPH_XF_V_SLT); mflag = 1;
    }
    else if (strstr(s1,"SGE")) {
        SET_MAC(ucode,NV_IGRAPH_XF_V_SGE); mflag = 1;
    }
    else if (strstr(s1,"ARL")) {
        SET_MAC(ucode,NV_IGRAPH_XF_V_ARL); mflag = 1;
    }
    else if (strstr(s1,"EXP")) {
        SET_ILU(ucode,NV_IGRAPH_XF_S_EXP); iflag = 1;
    }
    else if (strstr(s1,"LOG")) {
        SET_ILU(ucode,NV_IGRAPH_XF_S_LOG); iflag = 1;
    }
    else if (strstr(s1,"LIT")) {
        SET_ILU(ucode,NV_IGRAPH_XF_S_LIT); iflag = 1;
    }
    else if (strstr(s1,"IMV")) {
        SET_ILU(ucode,NV_IGRAPH_XF_S_MOV); iflag = 1;
    }
    else assert(0);

    if (iflag && dubl)
        assert(0);

    if (s2[0] != '\0') {
        if (strstr(s2,"NOP")) {
            SET_ILU(ucode,NV_IGRAPH_XF_S_NOP); iflag =1;
        }
        else if (strstr(s2,"RCP")) {
            SET_ILU(ucode,NV_IGRAPH_XF_S_RCP); iflag =1;
        }
        else if (strstr(s2,"RCC")) {
            SET_ILU(ucode,NV_IGRAPH_XF_S_RCC); iflag =1;
        }
        else if (strstr(s2,"RSQ")) {
            SET_ILU(ucode,NV_IGRAPH_XF_S_RSQ); iflag =1;
        }
        else if (strstr(s2,"EXP")) {
            SET_ILU(ucode,NV_IGRAPH_XF_S_EXP); iflag =1;
        }
        else if (strstr(s2,"LOG")) {
            SET_ILU(ucode,NV_IGRAPH_XF_S_LOG); iflag =1;
        }
        else if (strstr(s2,"LIT")) {
            SET_ILU(ucode,NV_IGRAPH_XF_S_LIT); iflag =1;
        }
        else if (strstr(s2,"IMV")) {
            SET_ILU(ucode,NV_IGRAPH_XF_S_MOV); iflag =1;
        }
        else assert(0);
    }

    // VA
    for (flog=i=0; i<10; i++) {
        add = (pp[i]>> 0) & 0xff;

        if (((pp[i] & MUX_MASK)>>8) == MX_V) {
            if (pp[i] & DST_MASK)
                assert(0);
            if (flog && add != faddr)
                assert(0);

            SET_VA(ucode,add);
            flog = 1;
            faddr = add;
        }
    }

    /*** CA,CIN ***/
    for (flog=i=0; i<10; i++) {
        add = pp[i] & ADD_MASK;

        if (((pp[i] & MUX_MASK)>>8) == MX_C && !(pp[i] & DST_MASK)) {
            if (flog && add != faddr)
                assert(0);

            SET_CA(ucode,add);
            if (pp[i]&IDX_MASK)
                SET_CIN(ucode,1);
            flog = 1;
            faddr = add;
        }
    }

    /*** AMUX ***/
    if (mflag) {
        for (count=i=0; i<6; i++) {
            add =  pp[i] & ADD_MASK;
            mux = (pp[i] & MUX_MASK)>>8;
            msk = (pp[i] & SWZ_MASK)>>12;
            neg = (pp[i] & SGN_MASK) ? 1 : 0;

            if (pp[i] && !(pp[i]&DST_MASK)) {
                count++;

                if (count == 1) {
                    SET_AMX(ucode,mux);
                    SET_AXS(ucode,(msk>>0)&3);
                    SET_AYS(ucode,(msk>>2)&3);
                    SET_AZS(ucode,(msk>>4)&3);
                    SET_AWS(ucode,(msk>>6)&3);
                    SET_ANE(ucode,neg);
                    if (mux == MX_R)
                        SET_ARR(ucode,add);
                }
            }
        }
    }

    /*** BMX, CMX ***/
    if (mflag) {
        for (count=0,i=0; i<6; i++) {
            add =  pp[i] & ADD_MASK;
            mux = (pp[i] & MUX_MASK)>>8;
            msk = (pp[i] & SWZ_MASK)>>12;
            neg = (pp[i] & SGN_MASK) ? 1 : 0;

            if (pp[i] && !(pp[i]&DST_MASK)) {
                count++;

                if (count == 2) {
                    if (strstr(s1,"MUL") || strstr(s1,"MAD") || strstr(s1,"DP3") || strstr(s1,"DPH") || strstr(s1,"DP4") ||
                        strstr(s1,"DST") || strstr(s1,"MIN") || strstr(s1,"MAX") || strstr(s1,"SLT") || strstr(s1,"SGE")) {
                        SET_BMX(ucode,mux);
                        SET_BXS(ucode,(msk>>0)&3);
                        SET_BYS(ucode,(msk>>2)&3);
                        SET_BZS(ucode,(msk>>4)&3);
                        SET_BWS(ucode,(msk>>6)&3);
                        SET_BNE(ucode,neg);
                        if (mux == MX_R)
                            SET_BRR(ucode,add);
                    }
                }

                if ((count == 2 && strstr(s1,"ADD")) || (count == 3 && (strstr(s1,"MAD")))) {
                    SET_CMX(ucode,mux);
                    SET_CXS(ucode,(msk>>0)&3);
                    SET_CYS(ucode,(msk>>2)&3);
                    SET_CZS(ucode,(msk>>4)&3);
                    SET_CWS(ucode,(msk>>6)&3);
                    SET_CNE(ucode,neg);
                    if (mux == MX_R)
                        SET_CRR(ucode,add);
                }
            }
        }
    }

    /*** CMX ***/
    if (iflag) {
        start = (dubl ? 6 : 0);

        for (i=start; i<10; i++) {
            add =  pp[i] & ADD_MASK;
            mux = (pp[i] & MUX_MASK)>>8;
            msk = (pp[i] & SWZ_MASK)>>12;
            neg = (pp[i] & SGN_MASK) ? 1 : 0;

            if (pp[i] && !(pp[i]&DST_MASK)) {
                SET_CMX(ucode,mux);
                SET_CXS(ucode,(msk>>0)&3);
                SET_CYS(ucode,(msk>>2)&3);
                SET_CZS(ucode,(msk>>4)&3);
                SET_CWS(ucode,(msk>>6)&3);
                SET_CNE(ucode,neg);
                if (mux == MX_R)
                    SET_CRR(ucode,add);

                break;
            }
        }
    }

    /*** RW, RWM, SWM ***/
    for (count=i=0; i<10; i++) {
        add =  pp[i] & ADD_MASK;
        mux = (pp[i] & MUX_MASK)>>8;
        msk = (pp[i] & SWZ_MASK)>>12;

        if (pp[i] && (pp[i]&DST_MASK) && (mux == MX_R)) {
            if (i<6) {
                if (count > 1)
                    assert(0);
                count++;

                SET_RW(ucode,add);

                if (mflag)
                    SET_RWM(ucode,msk);
                else if (iflag)
                    SET_SWM(ucode,msk);
                else
                    assert(0);
            }
            else {
                if (!dubl)
                    assert(0);
                if (add != 1)
                    assert(0);

                SET_SWM(ucode,msk);
            }
        }
    }

    /*** OC, OM, OWM ***/
    for (count=i=0; i<10; i++) {
        mux = (pp[i] & MUX_MASK)>>8;

        if ((pp[i]&DST_MASK) && (mux != MX_R)) {
            add =  pp[i] & ADD_MASK;
            msk = (pp[i] & SWZ_MASK)>>12;

            if (count)
                assert(0);

            SET_OWM(ucode,msk);
            if (i < 6 && mflag)
                SET_OM(ucode,OM_MAC);
            else
                SET_OM(ucode,OM_ILU);

            if (mux == MX_O) {
                count = 1;
                pProgram->outwri = 1;
                SET_OC(ucode,(0x100|add));
            }
            else if (mux == MX_C) {
                count = 1;
                pProgram->ctxwri = 1;
                SET_OC(ucode,add);
            }
            else
                assert(0);
        }
    }

    (pProgram->length)++;
}

//---------------------------------------------------------------------------

void nvKelvinInitProgram
(
    PKELVIN_MICRO_PROGRAM pProgram
)
{
    pProgram->ctxwri = 0;
    pProgram->outwri = 0;
    pProgram->length = 0;
}

//---------------------------------------------------------------------------

void nvKelvinParseProgram
(
    PKELVIN_MICRO_PROGRAM pProgram,
    // GLenum pType,
    // GLuint nid,
    char *s
)
{
    char  t[256], u[256];
    DWORD i, j;

    // check for header %!VP1.0
    if ((s[0] != '%') ||
        (s[1] != '!') ||
        (s[2] != 'V') ||
        (s[3] != 'P') ||
        (s[4] != '1') ||
        (s[5] != '.') ||
        (s[6] != '0')) {
        nvAssert(0);
    }

    // parse program
    for (i=7; s[i]!='\0'; i++) {

        for (j=0; s[i]!=':'&&s[i]!=';'&&s[i]!='\0'; i++)
            if (s[i]!=' ' && s[i]!='\t')
                t[j++] = s[i];

        t[j++] = ';';
        t[j++] = '\0';

        u[0] = '\0';
        if (s[i] == ':') {
            i++;
            for (j=0; s[i]!=';'&&s[i]!='\0'; i++)
                if (s[i]!=' ' && s[i]!='\t')
                    u[j++] = s[i];

            u[j++] = ';';
            u[j++] = '\0';
        }

        for (; (s[i]<'A' || s[i]>'Z') && s[i]!='\0'; i++)
            ;
        i--;

        opcode (pProgram, t, u);
    }

    // set end bit
    pProgram->ucode[pProgram->length-1].eos = 1;

    if (pProgram->length > 128) {
        nvAssert(0);
    }
}

//---------------------------------------------------------------------------

void nvKelvinDownloadProgram
(
    PNVD3DCONTEXT         pContext,
    PKELVIN_MICRO_PROGRAM pProgram,
    DWORD                 dwStartAddress
)
{
    long i;
    t_CheopsUcode *ucode;

    // program load address
    pContext->hwState.kelvin.set1 (NV097_SET_TRANSFORM_PROGRAM_LOAD, dwStartAddress);

    // load program
    for (i=0; i < pProgram->length; i++) {
        ucode = &(pProgram->ucode[i]);
        pContext->hwState.kelvin.set4 (NV097_SET_TRANSFORM_PROGRAM(0),
                                       PGM_UWORDX(ucode), PGM_UWORDY(ucode),
                                       PGM_UWORDZ(ucode), PGM_UWORDW(ucode));
    }
}

//---------------------------------------------------------------------------

void nvKelvinDownloadProgram
(
    PNVD3DCONTEXT        pContext,
    VertexProgramOutput *pProgramOutput,
    DWORD                dwStartAddress
)
{
    nvAssert(pContext);
    nvAssert(pProgramOutput);

    vtxpgmInstPacked *pInst = (vtxpgmInstPacked *) pProgramOutput->residentProgram;

    // program load address
    pContext->hwState.kelvin.set1 (NV097_SET_TRANSFORM_PROGRAM_LOAD, dwStartAddress);

    // load program
    for (int i = 0; i < pProgramOutput->residentNumInstructions; i++) {
        pContext->hwState.kelvin.set4 (NV097_SET_TRANSFORM_PROGRAM(0),
                                    pInst[i].x, pInst[i].y,
                                    pInst[i].z, pInst[i].w);
    }
}

//---------------------------------------------------------------------------

// load constants for use by the user vertex program

void nvKelvinDownloadConstants
(
    PNVD3DCONTEXT pContext
)
{
    pContext->hwState.kelvin.set1 (NV097_SET_TRANSFORM_CONSTANT_LOAD, __GL_KELVIN_FIRST_USER_CONSTANT_REGISTER);
    for (DWORD i=0; i < D3DVS_CONSTREG_MAX_V1_1; i++) {
        nvAssert(pContext->pVShaderConsts);
        VSHADERREGISTER *pConst = &pContext->pVShaderConsts->vertexShaderConstants[i];
        pContext->hwState.kelvin.set4f (NV097_SET_TRANSFORM_CONSTANT(0),
                                        pConst->x, pConst->y, pConst->z, pConst->w);
    }
}

//---------------------------------------------------------------------------

// parse D3D-style vertex shader code into kelvin microcode

DWORD KELVIN_BASE_REG(DWORD type)
{
    switch (type & D3DSP_REGTYPE_MASK)
    {
        case D3DSPR_INPUT:
            return VA_0;
            break;
        case D3DSPR_RASTOUT:
            if ((type & D3DSP_REGNUM_MASK )==0) {
                return VR_0;
            } else {
                return VR_4;
            }
            break;
        case D3DSPR_ATTROUT:
            return VR_1;
            break;
        case D3DSPR_TEXCRDOUT:
            return VR_7;
            break;
        case D3DSPR_ADDR:
            return ARL;
            break;
        case D3DSPR_CONST:
            return PR_0;
            break;
        case D3DSPR_TEMP:
            return TR_0;
            break;
        default:
            return 0xDEADBEEF;
            break;
    }
}

Register_t KELVIN_ENUMERATED_REG (DWORD token)
{
    DWORD final;
    if ((token & D3DSP_REGTYPE_MASK) == D3DSPR_RASTOUT) {
        // psz, fog, pts
        if ((token & D3DSP_REGNUM_MASK) == 0) {
            final = VR_0;
        } else {
            final = VR_4;
        }
        final += (DWORD)(token & D3DSP_REGNUM_MASK);
    } else {
        // everything else
        final = KELVIN_BASE_REG(token);
        final += (DWORD)(token & D3DSP_REGNUM_MASK);
        if (final > ARL) {
            DPF("Vertex Shader register index out of range");
            nvAssert(0);
            final = 0;
        }
    }
#ifdef DEBUG
    // debug stuff
    if (final <= VA_15) {
        DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "VA_%02d", final - VA_0);
    } else if (final <= VR_14) {
        DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "VR_%02d", final - VR_0);
    } else if (final <= PR_95) {
        DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "PR_%02d", final - PR_0);
    } else if (final <= TR_15) {
        DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "TR_%02d", final - TR_0);
    } else if (final == ARL) {
        DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "ARL");
    } else {
        DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "Unknown reg %d", final);
    }
#endif
    return (Register_t)final;
}

static char dbgMaskChar[4] = { 'x', 'y', 'z', 'w' };

char getDbgScalarMask(DWORD mask)
{
    return dbgMaskChar[mask & 0x03];
}

DWORD* getDbgVectorMask(DWORD mask)
{
    static DWORD dwResult;
    dwResult = (dbgMaskChar[(mask >> 0) & 0x03] << 0)
             | (dbgMaskChar[(mask >> 2) & 0x03] << 8)
             | (dbgMaskChar[(mask >> 4) & 0x03] << 16)
             | (dbgMaskChar[(mask >> 6) & 0x03] << 24);
    return &dwResult;
}

DWORD* getDbgWriteMask(DWORD mask)
{
    static DWORD dwResult;
    dwResult = (((mask & 1) ? dbgMaskChar[0] : ' ') << 0)
             | (((mask & 2) ? dbgMaskChar[1] : ' ') << 8)
             | (((mask & 4) ? dbgMaskChar[2] : ' ') << 16)
             | (((mask & 8) ? dbgMaskChar[3] : ' ') << 24);
    return &dwResult;
}

#define KELVIN_SET_SWIZZLED_SRC_REG(swizreg, token)                                                        \
{                                                                                                          \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, ", ");                                                        \
    swizreg.Signed      = BYTE((token & D3DSP_SRCMOD_MASK) >> D3DSP_SRCMOD_SHIFT);                         \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%c", (swizreg.Signed ? '-' : '+'));                          \
    swizreg.reg.reg     = KELVIN_ENUMERATED_REG(token);                                                    \
    swizreg.reg.AddrReg = BYTE((token & D3DVS_ADDRESSMODE_MASK) >> D3DVS_ADDRESSMODE_SHIFT);               \
    if (swizreg.reg.AddrReg) { swizreg.reg.addrRegOffset = swizreg.reg.reg - PR_0;                         \
                               DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "[+a0]"); }                        \
    swizreg.com4        = BYTE((token & D3DVS_SWIZZLE_MASK) >> D3DVS_SWIZZLE_SHIFT);                       \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, ".%4.4s", getDbgVectorMask(swizreg.com4));                    \
}

#define KELVIN_SET_SCALAR_SRC_REG_X(scalarreg, token)                                                      \
{                                                                                                          \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, ", ");                                                        \
    scalarreg.Signed      = BYTE((token & D3DSP_SRCMOD_MASK) >> D3DSP_SRCMOD_SHIFT);                       \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%c", (scalarreg.Signed ? '-' : '+'));                        \
    scalarreg.reg.reg     = KELVIN_ENUMERATED_REG(token);                                                  \
    scalarreg.reg.AddrReg = BYTE((token & D3DVS_ADDRESSMODE_MASK) >> D3DVS_ADDRESSMODE_SHIFT);             \
    if (scalarreg.reg.AddrReg) { scalarreg.reg.addrRegOffset = scalarreg.reg.reg - PR_0;                   \
                                 DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "[+a0]"); }                      \
    scalarreg.com         = Component_t((((token & D3DVS_SWIZZLE_MASK) >> D3DVS_SWIZZLE_SHIFT) >> 0) & 3); \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, ".%-4c", getDbgScalarMask(scalarreg.com));                    \
}

#define KELVIN_SET_SCALAR_SRC_REG_Y(scalarreg, token)                                                      \
{                                                                                                          \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, ", ");                                                        \
    scalarreg.Signed      = BYTE((token & D3DSP_SRCMOD_MASK) >> D3DSP_SRCMOD_SHIFT);                       \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%c", (scalarreg.Signed ? '-' : '+'));                        \
    scalarreg.reg.reg     = KELVIN_ENUMERATED_REG(token);                                                  \
    scalarreg.reg.AddrReg = BYTE((token & D3DVS_ADDRESSMODE_MASK) >> D3DVS_ADDRESSMODE_SHIFT);             \
    if (scalarreg.reg.AddrReg) { scalarreg.reg.addrRegOffset = scalarreg.reg.reg - PR_0;                   \
                                 DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "[+a0]"); }                      \
    scalarreg.com         = Component_t((((token & D3DVS_SWIZZLE_MASK) >> D3DVS_SWIZZLE_SHIFT) >> 2) & 3); \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, ".%-4c", getDbgScalarMask(scalarreg.com));                    \
}

#define KELVIN_SET_SCALAR_SRC_REG_Z(scalarreg, token)                                                      \
{                                                                                                          \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, ", ");                                                        \
    scalarreg.Signed      = BYTE((token & D3DSP_SRCMOD_MASK) >> D3DSP_SRCMOD_SHIFT);                       \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%c", (scalarreg.Signed ? '-' : '+'));                        \
    scalarreg.reg.reg     = KELVIN_ENUMERATED_REG(token);                                                  \
    scalarreg.reg.AddrReg = BYTE((token & D3DVS_ADDRESSMODE_MASK) >> D3DVS_ADDRESSMODE_SHIFT);             \
    if (scalarreg.reg.AddrReg) { scalarreg.reg.addrRegOffset = scalarreg.reg.reg - PR_0;                   \
                                 DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "[+a0]"); }                      \
    scalarreg.com         = Component_t((((token & D3DVS_SWIZZLE_MASK) >> D3DVS_SWIZZLE_SHIFT) >> 4) & 3); \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, ".%-4c", getDbgScalarMask(scalarreg.com));                    \
}

#define KELVIN_SET_SCALAR_SRC_REG_W(scalarreg, token)                                                      \
{                                                                                                          \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, ", ");                                                        \
    scalarreg.Signed      = BYTE((token & D3DSP_SRCMOD_MASK) >> D3DSP_SRCMOD_SHIFT);                       \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%c", (scalarreg.Signed ? '-' : '+'));                        \
    scalarreg.reg.reg     = KELVIN_ENUMERATED_REG(token);                                                  \
    scalarreg.reg.AddrReg = BYTE((token & D3DVS_ADDRESSMODE_MASK) >> D3DVS_ADDRESSMODE_SHIFT);             \
    if (scalarreg.reg.AddrReg) { scalarreg.reg.addrRegOffset = scalarreg.reg.reg - PR_0;                   \
                                 DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "[+a0]"); }                      \
    scalarreg.com         = Component_t((((token & D3DVS_SWIZZLE_MASK) >> D3DVS_SWIZZLE_SHIFT) >> 6) & 3); \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, ".%-4c", getDbgScalarMask(scalarreg.com));                    \
}

#define KELVIN_SET_MASKED_DST_REG(dstreg, token)                                                    \
{                                                                                                   \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, " ");                                                  \
    dstreg.reg  = KELVIN_ENUMERATED_REG(token);                                                     \
    dstreg.mask = (BYTE) ((token & D3DSP_WRITEMASK_ALL) >> 16);                                     \
    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, ".%4.4s", getDbgWriteMask(dstreg.mask));               \
}
// should really be:
//    dstreg.mask = (token & D3DSP_WRITEMASK_MASK) >> D3DSP_WRITEMASK_SHIFT;

#define KELVIN_SET_ARL_OP(op,pinstruc,pcode)                                                        \
{                                                                                                   \
    pinstruc->arl_op.opclass = CLASS_ARL;                                                           \
    pinstruc->arl_op.opcode  = op;                                                                  \
    KELVIN_SET_SCALAR_SRC_REG_X (pinstruc->arl_op.src, pcode[1]);                                   \
    pcode += 2;                                                                                     \
}

#define KELVIN_SET_VECTOR_OP(op,pinstruc,pcode)                                                     \
{                                                                                                   \
    pinstruc->vector_op.opclass = CLASS_VECTOR;                                                     \
    pinstruc->vector_op.opcode  = op;                                                               \
    KELVIN_SET_MASKED_DST_REG   (pinstruc->vector_op.dst, pcode[0]);                                \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc->vector_op.src, pcode[1]);                                \
    pcode += 2;                                                                                     \
}

#define KELVIN_SET_SCALAR_OP(op,pinstruc,pcode)                                                     \
{                                                                                                   \
    pinstruc->scalar_op.opclass = CLASS_SCALAR;                                                     \
    pinstruc->scalar_op.opcode  = op;                                                               \
    KELVIN_SET_MASKED_DST_REG (pinstruc->scalar_op.dst, pcode[0]);                                  \
    KELVIN_SET_SCALAR_SRC_REG_W (pinstruc->scalar_op.src, pcode[1]);                                \
    pcode += 2;                                                                                     \
}

#define KELVIN_SET_BINARY_OP(op,pinstruc,pcode)                                                     \
{                                                                                                   \
    pinstruc->bin_op.opclass = CLASS_BIN;                                                           \
    pinstruc->bin_op.opcode  = op;                                                                  \
    KELVIN_SET_MASKED_DST_REG   (pinstruc->bin_op.dst,  pcode[0]);                                  \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc->bin_op.srcA, pcode[1]);                                  \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc->bin_op.srcB, pcode[2]);                                  \
    pcode += 3;                                                                                     \
}

#define KELVIN_SET_TERNARY_OP(op,pinstruc,pcode)                                                    \
{                                                                                                   \
    pinstruc->tri_op.opclass = CLASS_TRI;                                                           \
    pinstruc->tri_op.opcode  = op;                                                                  \
    KELVIN_SET_MASKED_DST_REG   (pinstruc->tri_op.dst,  pcode[0]);                                  \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc->tri_op.srcA, pcode[1]);                                  \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc->tri_op.srcB, pcode[2]);                                  \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc->tri_op.srcC, pcode[3]);                                  \
    pcode += 4;                                                                                     \
}

#define KELVIN_SET_MACRO_4_OP(op,pinstruc,pcode)                                                    \
{                                                                                                   \
    pinstruc[0].bin_op.opclass = CLASS_BIN;                                                         \
    pinstruc[1].bin_op.opclass = CLASS_BIN;                                                         \
    pinstruc[2].bin_op.opclass = CLASS_BIN;                                                         \
    pinstruc[3].bin_op.opclass = CLASS_BIN;                                                         \
    pinstruc[0].bin_op.opcode  = op;                                                                \
    pinstruc[1].bin_op.opcode  = op;                                                                \
    pinstruc[2].bin_op.opcode  = op;                                                                \
    pinstruc[3].bin_op.opcode  = op;                                                                \
    KELVIN_SET_MASKED_DST_REG   (pinstruc[0].bin_op.dst,  pcode[0] & ~D3DSP_WRITEMASK_ALL | D3DSP_WRITEMASK_0); \
    KELVIN_SET_MASKED_DST_REG   (pinstruc[1].bin_op.dst,  pcode[0] & ~D3DSP_WRITEMASK_ALL | D3DSP_WRITEMASK_1); \
    KELVIN_SET_MASKED_DST_REG   (pinstruc[2].bin_op.dst,  pcode[0] & ~D3DSP_WRITEMASK_ALL | D3DSP_WRITEMASK_2); \
    KELVIN_SET_MASKED_DST_REG   (pinstruc[3].bin_op.dst,  pcode[0] & ~D3DSP_WRITEMASK_ALL | D3DSP_WRITEMASK_3); \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[0].bin_op.srcA, pcode[1]);                                \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[1].bin_op.srcA, pcode[1]);                                \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[2].bin_op.srcA, pcode[1]);                                \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[3].bin_op.srcA, pcode[1]);                                \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[0].bin_op.srcB, pcode[2] + 0);                            \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[1].bin_op.srcB, pcode[2] + 1);                            \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[2].bin_op.srcB, pcode[2] + 2);                            \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[3].bin_op.srcB, pcode[2] + 3);                            \
    pcode += 3;                                                                                     \
}

#define KELVIN_SET_MACRO_3_OP(op,pinstruc,pcode)                                                    \
{                                                                                                   \
    pinstruc[0].bin_op.opclass = CLASS_BIN;                                                         \
    pinstruc[1].bin_op.opclass = CLASS_BIN;                                                         \
    pinstruc[2].bin_op.opclass = CLASS_BIN;                                                         \
    pinstruc[0].bin_op.opcode  = op;                                                                \
    pinstruc[1].bin_op.opcode  = op;                                                                \
    pinstruc[2].bin_op.opcode  = op;                                                                \
    KELVIN_SET_MASKED_DST_REG   (pinstruc[0].bin_op.dst,  pcode[0] & ~D3DSP_WRITEMASK_ALL | D3DSP_WRITEMASK_0); \
    KELVIN_SET_MASKED_DST_REG   (pinstruc[1].bin_op.dst,  pcode[0] & ~D3DSP_WRITEMASK_ALL | D3DSP_WRITEMASK_1); \
    KELVIN_SET_MASKED_DST_REG   (pinstruc[2].bin_op.dst,  pcode[0] & ~D3DSP_WRITEMASK_ALL | D3DSP_WRITEMASK_2); \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[0].bin_op.srcA, pcode[1]);                                \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[1].bin_op.srcA, pcode[1]);                                \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[2].bin_op.srcA, pcode[1]);                                \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[0].bin_op.srcB, pcode[2] + 0);                            \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[1].bin_op.srcB, pcode[2] + 1);                            \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[2].bin_op.srcB, pcode[2] + 2);                            \
    pcode += 3;                                                                                     \
}

#define KELVIN_SET_MACRO_2_OP(op,pinstruc,pcode)                                                    \
{                                                                                                   \
    pinstruc[0].bin_op.opclass = CLASS_BIN;                                                         \
    pinstruc[1].bin_op.opclass = CLASS_BIN;                                                         \
    pinstruc[0].bin_op.opcode  = op;                                                                \
    pinstruc[1].bin_op.opcode  = op;                                                                \
    KELVIN_SET_MASKED_DST_REG   (pinstruc[0].bin_op.dst,  pcode[0] & ~D3DSP_WRITEMASK_ALL | D3DSP_WRITEMASK_0); \
    KELVIN_SET_MASKED_DST_REG   (pinstruc[1].bin_op.dst,  pcode[0] & ~D3DSP_WRITEMASK_ALL | D3DSP_WRITEMASK_1); \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[0].bin_op.srcA, pcode[1]);                                \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[1].bin_op.srcA, pcode[1]);                                \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[0].bin_op.srcB, pcode[2] + 0);                            \
    KELVIN_SET_SWIZZLED_SRC_REG (pinstruc[1].bin_op.srcB, pcode[2] + 1);                            \
    pcode += 3;                                                                                     \
}

void MarkResultReg (unsigned char* pos, MaskedDstReg_Rec dst)
{
    if (dst.reg >= VR_0 && dst.reg <= VR_14)
    {
        pos[dst.reg-VR_0] |= dst.mask;
    }
}

void ExpMacro (PKELVIN_PROGRAM pProgram, DWORD dwInstruction, DWORD *pcode)
{
    pProgram->code[dwInstruction + 0].scalar_op.opclass         = CLASS_SCALAR;
    pProgram->code[dwInstruction + 0].scalar_op.opcode          = OP_EXP;
    KELVIN_SET_MASKED_DST_REG   (pProgram->code[dwInstruction + 1].vector_op.dst, pcode[0]);
    KELVIN_SET_SCALAR_SRC_REG_W (pProgram->code[dwInstruction + 0].scalar_op.src, pcode[1]);
    pProgram->code[dwInstruction + 0].scalar_op.dst.reg         = TR_8;
    pProgram->code[dwInstruction + 0].scalar_op.dst.mask        = 4; // write z only

    pProgram->code[dwInstruction + 1].scalar_op.opclass         = CLASS_VECTOR;
    pProgram->code[dwInstruction + 1].scalar_op.opcode          = OP_MOV;
    pProgram->code[dwInstruction + 1].vector_op.src.reg.reg     = TR_8;
    pProgram->code[dwInstruction + 1].vector_op.src.reg.AddrReg = 0x0;
    pProgram->code[dwInstruction + 1].vector_op.src.com4        = 0xAA; // get from z
    pProgram->code[dwInstruction + 1].vector_op.src.Signed      = 0x0;
}

void LogMacro (PKELVIN_PROGRAM pProgram, DWORD dwInstruction, DWORD *pcode)
{
    pProgram->code[dwInstruction + 0].scalar_op.opclass         = CLASS_SCALAR;
    pProgram->code[dwInstruction + 0].scalar_op.opcode          = OP_LOG;
    KELVIN_SET_MASKED_DST_REG   (pProgram->code[dwInstruction + 1].vector_op.dst, pcode[0]);
    KELVIN_SET_SCALAR_SRC_REG_W (pProgram->code[dwInstruction + 0].scalar_op.src, pcode[1]);
    pProgram->code[dwInstruction + 0].scalar_op.dst.reg         = TR_8;
    pProgram->code[dwInstruction + 0].scalar_op.dst.mask        = 4; // write z only

    pProgram->code[dwInstruction + 1].scalar_op.opclass         = CLASS_VECTOR;
    pProgram->code[dwInstruction + 1].scalar_op.opcode          = OP_MOV;
    pProgram->code[dwInstruction + 1].vector_op.src.reg.reg     = TR_8;
    pProgram->code[dwInstruction + 1].vector_op.src.reg.AddrReg = 0x0;
    pProgram->code[dwInstruction + 1].vector_op.src.com4        = 0xAA; // get from z
    pProgram->code[dwInstruction + 1].vector_op.src.Signed      = 0x0;
}

DWORD FrcMacro (PKELVIN_PROGRAM pProgram, DWORD dwInstruction, DWORD *pCode)
{
    if (pCode[0] & D3DSP_WRITEMASK_0) { // if x is written
        pProgram->code[dwInstruction].scalar_op.opclass         = CLASS_SCALAR;
        pProgram->code[dwInstruction].scalar_op.opcode          = OP_EXP;
        KELVIN_SET_SCALAR_SRC_REG_X (pProgram->code[dwInstruction].scalar_op.src, pCode[1]);
        pProgram->code[dwInstruction].scalar_op.dst.reg         = TR_8;
        pProgram->code[dwInstruction].scalar_op.dst.mask        = 2; // write y only
        dwInstruction++;
        pProgram->code[dwInstruction].scalar_op.opclass         = CLASS_VECTOR;
        pProgram->code[dwInstruction].scalar_op.opcode          = OP_MOV;
        pProgram->code[dwInstruction].vector_op.src.reg.reg     = TR_8;
        pProgram->code[dwInstruction].vector_op.src.reg.AddrReg = 0x0;
        pProgram->code[dwInstruction].vector_op.src.com4        = 0x55; // get from y
        pProgram->code[dwInstruction].vector_op.src.Signed      = 0x0;
        KELVIN_SET_MASKED_DST_REG   (pProgram->code[dwInstruction].vector_op.dst, pCode[0]);
        pProgram->code[dwInstruction].vector_op.dst.mask        = 0x1; // write x only
        dwInstruction++;
    }

    if (pCode[0] & D3DSP_WRITEMASK_1) { // if y is written
        pProgram->code[dwInstruction].scalar_op.opclass         = CLASS_SCALAR;
        pProgram->code[dwInstruction].scalar_op.opcode          = OP_EXP;
        KELVIN_SET_SCALAR_SRC_REG_Y (pProgram->code[dwInstruction].scalar_op.src, pCode[1]);
        KELVIN_SET_MASKED_DST_REG   (pProgram->code[dwInstruction].vector_op.dst, pCode[0]);
        pProgram->code[dwInstruction].scalar_op.dst.mask        = 2; // write y only
        dwInstruction++;
    }

/* MS doesn't want these anymore (for now)
    if (pCode[0] & D3DSP_WRITEMASK_2) { // if z is written
        pProgram->code[dwInstruction].scalar_op.opclass         = CLASS_SCALAR;
        pProgram->code[dwInstruction].scalar_op.opcode          = OP_EXP;
        KELVIN_SET_SCALAR_SRC_REG_Z (pProgram->code[dwInstruction].scalar_op.src, pCode[1]);
        pProgram->code[dwInstruction].scalar_op.dst.reg         = TR_8;
        pProgram->code[dwInstruction].scalar_op.dst.mask        = 2; // write y only
        dwInstruction++;
        pProgram->code[dwInstruction].scalar_op.opclass         = CLASS_VECTOR;
        pProgram->code[dwInstruction].scalar_op.opcode          = OP_MOV;
        pProgram->code[dwInstruction].vector_op.src.reg.reg     = TR_8;
        pProgram->code[dwInstruction].vector_op.src.reg.AddrReg = 0x0;
        pProgram->code[dwInstruction].vector_op.src.com4        = 0x55; // get from y
        pProgram->code[dwInstruction].vector_op.src.Signed      = 0x0;
        KELVIN_SET_MASKED_DST_REG   (pProgram->code[dwInstruction].vector_op.dst, pCode[0]);
        pProgram->code[dwInstruction].vector_op.dst.mask       &= 0x4; // write z only
        dwInstruction++;
    }

    if (pCode[0] & D3DSP_WRITEMASK_3) { // if w is written
        pProgram->code[dwInstruction].scalar_op.opclass         = CLASS_SCALAR;
        pProgram->code[dwInstruction].scalar_op.opcode          = OP_EXP;
        KELVIN_SET_SCALAR_SRC_REG_W (pProgram->code[dwInstruction].scalar_op.src, pCode[1]);
        pProgram->code[dwInstruction].scalar_op.dst.reg         = TR_8;
        pProgram->code[dwInstruction].scalar_op.dst.mask        = 2; // write y only
        dwInstruction++;
        pProgram->code[dwInstruction].scalar_op.opclass         = CLASS_VECTOR;
        pProgram->code[dwInstruction].scalar_op.opcode          = OP_MOV;
        pProgram->code[dwInstruction].vector_op.src.reg.reg     = TR_8;
        pProgram->code[dwInstruction].vector_op.src.reg.AddrReg = 0x0;
        pProgram->code[dwInstruction].vector_op.src.com4        = 0x55; // get from y
        pProgram->code[dwInstruction].vector_op.src.Signed      = 0x0;
        KELVIN_SET_MASKED_DST_REG   (pProgram->code[dwInstruction].vector_op.dst, pCode[0]);
        pProgram->code[dwInstruction].vector_op.dst.mask       &= 0x8; // write w only
        dwInstruction++;
    }
*/
    return dwInstruction - 1;
}

// parse D3D-style vertex shader code into intermediate compiled code

void nvKelvinParseVertexShaderCode
(
    PKELVIN_PROGRAM pProgram,
    ParsedProgram *parsed,
    DWORD *pCode,
    DWORD dwCodeSize
)
{
    DWORD dwHeader, dwToken, dwInstruction;
    Instruction *pInstruction;
    BOOL bDone;

    // read the header
    dwHeader = *pCode;
    pCode++;

    // clear written regs
    for (int i = 0; i < __GL_NUMBER_OF_RESULT_REGISTERS; i++) {
        parsed->resultRegsWritten[i] = DST_NONE_BITS;
    }

    bDone = FALSE;
    dwInstruction = 0;

    while (!bDone) {

        dwToken = *pCode;
        pCode++;
        pInstruction = &(pProgram->code[dwInstruction]);

        switch (dwToken & D3DSI_OPCODE_MASK) {

            case D3DSIO_NOP:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d NOP ", dwInstruction);
                pInstruction->end_op.opclass = CLASS_END;
                pInstruction->end_op.opcode  = OP_NOP;
                break;
            case D3DSIO_MOV:
                if (KELVIN_BASE_REG(pCode[0]) == ARL) {
                    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d ARL ", dwInstruction);
                    KELVIN_SET_ARL_OP (OP_ARL, pInstruction, pCode);
                } else {
                    DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d MOV ", dwInstruction);
                    KELVIN_SET_VECTOR_OP (OP_MOV, pInstruction, pCode);
                    MarkResultReg (parsed->resultRegsWritten, pInstruction->vector_op.dst);
                }
                break;
            case D3DSIO_ADD:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d ADD ", dwInstruction);
                KELVIN_SET_BINARY_OP (OP_ADD, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction->bin_op.dst);
                break;
            case D3DSIO_MAD:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d MAD ", dwInstruction);
                KELVIN_SET_TERNARY_OP (OP_MAD, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction->tri_op.dst);
                break;
            case D3DSIO_MUL:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d MUL ", dwInstruction);
                KELVIN_SET_BINARY_OP (OP_MUL, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction->bin_op.dst);
                break;
            case D3DSIO_RCP:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d RCP ", dwInstruction);
                KELVIN_SET_SCALAR_OP (OP_RCP, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction->scalar_op.dst);
                break;
            case D3DSIO_RSQ:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d RSQ ", dwInstruction);
                KELVIN_SET_SCALAR_OP (OP_RSQ, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction->scalar_op.dst);
                break;
            case D3DSIO_DP3:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d DP3 ", dwInstruction);
                KELVIN_SET_BINARY_OP (OP_DP3, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction->bin_op.dst);
                break;
            case D3DSIO_DP4:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d DP4 ", dwInstruction);
                KELVIN_SET_BINARY_OP (OP_DP4, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction->bin_op.dst);
                break;
            case D3DSIO_MIN:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d MIN ", dwInstruction);
                KELVIN_SET_BINARY_OP (OP_MIN, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction->bin_op.dst);
                break;
            case D3DSIO_MAX:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d MAX ", dwInstruction);
                KELVIN_SET_BINARY_OP (OP_MAX, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction->bin_op.dst);
                break;
            case D3DSIO_SLT:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d SLT ", dwInstruction);
                KELVIN_SET_BINARY_OP (OP_SLT, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction->bin_op.dst);
                break;
            case D3DSIO_SGE:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d SGE ", dwInstruction);
                KELVIN_SET_BINARY_OP (OP_SGE, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction->bin_op.dst);
                break;
            case D3DSIO_EXP:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d EXP ", dwInstruction);
                ExpMacro(pProgram, dwInstruction, pCode);
                pCode += 2;
                dwInstruction ++;
                break;
            case D3DSIO_LOG:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d LOG ", dwInstruction);
                LogMacro(pProgram, dwInstruction, pCode);
                pCode += 2;
                dwInstruction ++;
                break;
            case D3DSIO_LIT:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d LIT ", dwInstruction);
                KELVIN_SET_VECTOR_OP (OP_LIT, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction->vector_op.dst);
                break;
            case D3DSIO_DST:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d DST ", dwInstruction);
                KELVIN_SET_BINARY_OP (OP_DST, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction->bin_op.dst);
                break;
            case D3DSIO_M4x4:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d M4x4", dwInstruction);
                KELVIN_SET_MACRO_4_OP (OP_DP4, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction[0].bin_op.dst);
                MarkResultReg (parsed->resultRegsWritten, pInstruction[1].bin_op.dst);
                MarkResultReg (parsed->resultRegsWritten, pInstruction[2].bin_op.dst);
                MarkResultReg (parsed->resultRegsWritten, pInstruction[3].bin_op.dst);
                dwInstruction += 3;
                break;
            case D3DSIO_M3x4:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d M3x4", dwInstruction);
                KELVIN_SET_MACRO_4_OP (OP_DP3, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction[0].bin_op.dst);
                MarkResultReg (parsed->resultRegsWritten, pInstruction[1].bin_op.dst);
                MarkResultReg (parsed->resultRegsWritten, pInstruction[2].bin_op.dst);
                MarkResultReg (parsed->resultRegsWritten, pInstruction[3].bin_op.dst);
                dwInstruction += 3;
                break;
            case D3DSIO_M4x3:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d M4x3", dwInstruction);
                KELVIN_SET_MACRO_3_OP (OP_DP4, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction[0].bin_op.dst);
                MarkResultReg (parsed->resultRegsWritten, pInstruction[1].bin_op.dst);
                MarkResultReg (parsed->resultRegsWritten, pInstruction[2].bin_op.dst);
                dwInstruction += 2;
                break;
            case D3DSIO_M3x3:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d M3x3", dwInstruction);
                KELVIN_SET_MACRO_3_OP (OP_DP3, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction[0].bin_op.dst);
                MarkResultReg (parsed->resultRegsWritten, pInstruction[1].bin_op.dst);
                MarkResultReg (parsed->resultRegsWritten, pInstruction[2].bin_op.dst);
                dwInstruction += 2;
                break;
            case D3DSIO_M3x2:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d M3x2", dwInstruction);
                KELVIN_SET_MACRO_2_OP (OP_DP3, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction[0].bin_op.dst);
                MarkResultReg (parsed->resultRegsWritten, pInstruction[1].bin_op.dst);
                dwInstruction ++;
                break;
            case D3DSIO_FRC:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d FRC ", dwInstruction);
                dwInstruction = FrcMacro(pProgram, dwInstruction, pCode);
                pCode += 2;
                break;
            case D3DSIO_EXPP:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d EXPP", dwInstruction);
                KELVIN_SET_SCALAR_OP (OP_EXP, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction->scalar_op.dst);
                break;
            case D3DSIO_LOGP:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d LOGP", dwInstruction);
                KELVIN_SET_SCALAR_OP (OP_LOG, pInstruction, pCode);
                MarkResultReg (parsed->resultRegsWritten, pInstruction->scalar_op.dst);
                break;
            case D3DSIO_COMMENT:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d COMMENT (TODO)", dwInstruction);
                nvAssert(0);
                break;
            case D3DSIO_END:
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "%2d END\r\n", dwInstruction);
                pInstruction->end_op.opclass = CLASS_END;
                pInstruction->end_op.opcode  = OP_END;
                bDone = TRUE;
                break;
            default:
                // unhandled instruction
                DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "Unknown opcode (%d)", dwToken & D3DSI_OPCODE_MASK);
                nvAssert(0);

        }  // switch

        DPF_LEVEL_PLAIN(NVDBG_LEVEL_VSHADER_INS, "\r\n");
        dwInstruction++;
    }

    // check special case output registers
    if (parsed->resultRegsWritten[VA_FOGC]) {
        // fog may only use .x
        nvAssert(parsed->resultRegsWritten[VA_FOGC] == 1);

        // force it to .x
        parsed->resultRegsWritten[VA_FOGC] = 1;
    }

/*
    // sanity check program output
    for (i = 0; i < 11; i++)
    {
        DWORD dwRegWriteMask = parsed->resultRegsWritten[i];

        // data in this stream?
        if (dwRegWriteMask)
        {
            // valid combination of output registers written?
            if (dwCelsiusMethodDispatch[i][dwRegWriteMask][0] == 0xDEADBEEF) {
                DPF("Vertex Shader - invalid write mask for register %d reset", i);
                nvAssert(0);
                // set the regs written mask to 0 (disable output for this register)
                parsed->resultRegsWritten[i] = 0; // do this for emulation only
            }
        }
    }
*/
    pProgram->dwNumInstructions = dwInstruction;
}
 
#ifdef KPFS

void CKelvinProgramMan::create(DWORD dwInstructions)
{ 
    memset(this,0,sizeof(CKelvinProgramMan));
    m_dwInstructions = dwInstructions;
    if (!ins)
        ins=new CInstr[dwInstructions]();
    nvAssert(ins);
    memset(ins,0,sizeof(CInstr)*dwInstructions);
    for (DWORD i=0;i<dwInstructions;i++) {
        ins[i].index=i;
        ins[i].len=dwInstructions-i;
    }
    DPF_LEVEL(NVDBG_LEVEL_VSHADER_MAN,"shaderman: created\n");
}

void CKelvinProgramMan::destroy()
{ 
    nvAssert(ins);
    if (ins) 
        delete[] ins;
    DPF_LEVEL(NVDBG_LEVEL_VSHADER_MAN,"shaderman: deleted\n");
}

void CKelvinProgramMan::flush()
{
    DWORD   n= m_dwInstructions;
    CInstr* t= ins ? ins : new CInstr[m_dwInstructions]();
    memset(this,0,sizeof(CKelvinProgramMan));
    ins = t; m_dwInstructions = n;
    for (DWORD i=0;i<m_dwInstructions;i++) {
        ins[i].index=i;
        ins[i].len=m_dwInstructions-i;
        ins[i].next = 0;
    }
    DPF_LEVEL(NVDBG_LEVEL_VSHADER_MAN,"shaderman: flushed\n");
}

bool CKelvinProgramMan::find(DWORD handle, DWORD num_instr)
{
    CInstr *f = &ins[0];
    while (f){
        if (f->handle==handle){   
            //check
            if (f->next) {
                nvAssert((f->next->index) == (f->index+f->len));
                nvAssert(f->len ==num_instr);
            }
            m_dwOffset=f->index;
            return true;
        }
        f=f->next;
    }
    return false;
}

void CKelvinProgramMan::add(DWORD handle, DWORD num_instr) 
{
    DWORD    i,  x, sx, px = ~0;
    CInstr  *t, *n, *a, *f = &ins[0];
    //find a place to insert
    while (f) {
        //found a free block   
        if (!f->handle && f->len >= num_instr){
            sx=f->len;
            n=f->next;
            m_dwOffset = f->index;
            goto free_block;
        }
        
        //slide window and compute cost
        t=f; x=0;
        while(num_instr > x && t) {
            x+=t->len;
            t=t->next;
        }
        
        if (x<px && num_instr <= x) {
            sx=px=x;
            n=t;
            m_dwOffset=f->index;
        }
        
        f=f->next;
    }
    
    //insert
    f = &ins[m_dwOffset]; 
    free_block:
    f->handle = handle;
    f->len = num_instr;
    i = f->index+f->len;
    
    if (i<m_dwInstructions) {
        a = &ins[i];
        f->next = a;
        
        if (f->next!=n) {
            a->next = n;
            a->len=sx-f->len;
            a->handle=0;
        }
    }
    else f->next=NULL;
}   

void CKelvinProgramMan::display() 
{
    CInstr* t=&ins[0];
    while(t) {
        for(DWORD i=0;i<t->len;i++) {
            DPF_PLAIN("%3x",t->handle);        
        }       
        t=t->next;
    }
    DPF_PLAIN("\n"); 
}


#define SET_PROGRAM_TYPE(type)                                              \
{                                                                           \
    pContext->hwState.kelvin.set1(NV097_SET_TRANSFORM_EXECUTION_MODE,       \
    DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _MODE, _PROGRAM) |          \
    DRF_DEF(097, _SET_TRANSFORM_EXECUTION_MODE, _RANGE_MODE, type));        \
    pContext->hwState.kelvin.set1(NV097_SET_TRANSFORM_PROGRAM_CXT_WRITE_EN, \
    DRF_DEF(097, _SET_TRANSFORM_PROGRAM_CXT_WRITE_EN, _V, _READ_ONLY));     \
}


void CKelvinProgramMan::download_special(PNVD3DCONTEXT pContext, PKELVIN_MICRO_PROGRAM pProgram, DWORD handle) {

    DWORD num_instr = pProgram->length;
    nvAssert(num_instr<=m_dwInstructions);
    
    if (!m_pContext)
        m_pContext=pContext;
    else {
        if (m_pContext!=pContext) {
            m_pContext=pContext;
            m_dwOffset=0;
            flush();
            goto fromzero;
        }
        else {
            //early out
            if ((m_dwLastHandle==handle) && (m_pPrevContext == pContext)) { 
                if (m_dwDirty) {
                    SET_PROGRAM_TYPE(_PRIV);
                    m_dwDirty=0;
                }
                return;
            }
                
            m_dwLastHandle=handle;
            m_pPrevContext = pContext; 
        }
    }

    SET_PROGRAM_TYPE(_PRIV);

    //shader already loaded
    if (find(handle,num_instr)) {
        pContext->hwState.kelvin.set1 (NV097_SET_TRANSFORM_PROGRAM_START, m_dwOffset);
        DPF_LEVEL(NVDBG_LEVEL_VSHADER_MAN,"shaderman: Is %d, %d",m_dwOffset,num_instr);
    }
    else {
        add(handle,num_instr);
        fromzero:
        pContext->hwState.kelvin.set1 (NV097_SET_TRANSFORM_PROGRAM_START, m_dwOffset);
        nvKelvinDownloadProgram(pContext, pProgram,m_dwOffset);
        DPF_LEVEL(NVDBG_LEVEL_VSHADER_MAN,"shaderman: Es %d, %d",m_dwOffset,num_instr);
    }
}

void CKelvinProgramMan::download(PNVD3DCONTEXT pContext,  CVertexShader* pVS) {
    
    DWORD num_instr = pVS->m_ProgramOutput.residentNumInstructions;
    DWORD handle = pVS->getHandle();
    
    nvAssert(num_instr<=m_dwInstructions);
    nvAssert(handle!=0xFFFFFFFF && handle!=0xFFFFFFFE); //reserved for passthrough and aa shaders, bug florin if this ever triggers

    if (!m_pContext)
        m_pContext=pContext;
    else {
        if (m_pContext!=pContext) {
            m_pContext=pContext;
            m_dwOffset=0;
            flush();
            goto fromzero;
        }
        else
        {
            //early out
            if ((m_dwLastHandle==handle) && (m_pPrevContext == pContext)) {
                if (m_dwDirty) {
                    SET_PROGRAM_TYPE(_USER);
                    m_dwDirty=0;
                }
                nvKelvinDownloadConstants (pContext);
                return;
            }
            m_dwLastHandle=handle;
            m_pPrevContext = pContext; 
        }
    }

    SET_PROGRAM_TYPE(_USER);
    
    //shader already loaded
    if (find(handle,num_instr)) {
        pContext->hwState.kelvin.set1 (NV097_SET_TRANSFORM_PROGRAM_START, m_dwOffset);
        DPF_LEVEL(NVDBG_LEVEL_VSHADER_MAN,"I %d, %d",m_dwOffset,num_instr);
    }
    else {
        add(handle,num_instr);
        fromzero:
        pContext->hwState.kelvin.set1 (NV097_SET_TRANSFORM_PROGRAM_START, m_dwOffset);
        nvKelvinDownloadProgram (pContext, &pVS->m_ProgramOutput, m_dwOffset);
        DPF_LEVEL(NVDBG_LEVEL_VSHADER_MAN,"E %d, %d",m_dwOffset,num_instr);
    }
    
    nvKelvinDownloadConstants (pContext);
}
#endif

#endif  // NVARCH >= 0x020

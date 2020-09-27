 /***************************************************************************\
|*                                                                           *|
|*        Copyright (c) 1993-2000 NVIDIA, Corp.  All rights reserved.        *|
|*                                                                           *|
|*     NOTICE TO USER:   The source code  is copyrighted under  U.S. and     *|
|*     international laws.   NVIDIA, Corp. of Sunnyvale, California owns     *|
|*     the copyright  and as design patents  pending  on the design  and     *|
|*     interface  of the NV chips.   Users and possessors of this source     *|
|*     code are hereby granted  a nonexclusive,  royalty-free  copyright     *|
|*     and  design  patent license  to use this code  in individual  and     *|
|*     commercial software.                                                  *|
|*                                                                           *|
|*     Any use of this source code must include,  in the user documenta-     *|
|*     tion and  internal comments to the code,  notices to the end user     *|
|*     as follows:                                                           *|
|*                                                                           *|
|*     Copyright (c) 1993-2000  NVIDIA, Corp.    NVIDIA  design  patents     *|
|*     pending in the U.S. and foreign countries.                            *|
|*                                                                           *|
|*     NVIDIA, CORP.  MAKES  NO REPRESENTATION ABOUT  THE SUITABILITY OF     *|
|*     THIS SOURCE CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT     *|
|*     EXPRESS OR IMPLIED WARRANTY OF ANY KIND.  NVIDIA, CORP. DISCLAIMS     *|
|*     ALL WARRANTIES  WITH REGARD  TO THIS SOURCE CODE,  INCLUDING  ALL     *|
|*     IMPLIED   WARRANTIES  OF  MERCHANTABILITY  AND   FITNESS   FOR  A     *|
|*     PARTICULAR  PURPOSE.   IN NO EVENT SHALL NVIDIA, CORP.  BE LIABLE     *|
|*     FOR ANY SPECIAL, INDIRECT, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES,     *|
|*     OR ANY DAMAGES  WHATSOEVER  RESULTING  FROM LOSS OF USE,  DATA OR     *|
|*     PROFITS,  WHETHER IN AN ACTION  OF CONTRACT,  NEGLIGENCE OR OTHER     *|
|*     TORTIOUS ACTION, ARISING OUT  OF OR IN CONNECTION WITH THE USE OR     *|
|*     PERFORMANCE OF THIS SOURCE CODE.                                      *|
|*                                                                           *|
 \***************************************************************************/

/************************Object to manage scratch Qs*************************\
*                                                                           *
* Module: CCommandQ.h														*
*   Object to command Q reads												*
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*                                                                           *
\***************************************************************************/

#if !defined _CCOMMANDQH_
#define _CCOMMANDQH_

#define COMMAND_Q_SIZE      0x1000
#define COMMAND_Q_INCR      0x4
#define COMMAND_Q_INCREMENT(x)   (x) = ((x) + COMMAND_Q_INCR) & (COMMAND_Q_SIZE -1)

class CHalBase;

class CCommandQ
{
public:
    enum Q_TYPE
    {
        Q_TYPE_EP,
        Q_TYPE_GP
    };

    CCommandQ(CHalBase *pHal)
    {
        m_uPut = 0;
        m_uGet = 0;
        m_CommandQ.pLinAddr = NULL;
        m_CommandQ.Size = COMMAND_Q_SIZE;
        m_pHal = pHal;
    }
    ~CCommandQ();

    RM_STATUS Initialize(Q_TYPE);
    BOOL Read(U032 *);

    VOID *GetLinAddr() { return m_CommandQ.pLinAddr; }
    
private:
    // register offsets
    U032        m_uPut;
    U032        m_uGet;
    U032        m_uQ;

    MEM_DESC    m_CommandQ;
    CHalBase    *m_pHal;
};



#endif
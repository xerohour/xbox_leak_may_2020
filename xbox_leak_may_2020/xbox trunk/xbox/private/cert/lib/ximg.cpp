/*
 *
 * ximg.cpp
 *
 * ximage.dat processing
 *
 */

#include "precomp.h"

enum { mdDataBeforeImage, mdDataImage, mdDataBeforeAP, mdDataAP };

BOOL FProcessXImage(int ilyr, DXL *pdxl, int idxr, CTapeDevice *ptapSrc,
    CTapeDevice *ptapDst, PFN_HANDLEXBE pfnXbe, PFN_HANDLEAP pfnAP)
{
    int ixbi;
    int ixph;
    DWORD iblkCur;
    DWORD cblkCur;
    DWORD cblk;
    DWORD cblkRem;
    DWORD cb;
    DXR *pdxr = &pdxl->rgdxrAreas[idxr];
    CFileStmSeq *pstmTo;
    CDiskFile *pstmXBE = NULL;
    CXBEFile *pxbe = NULL;
    BYTE rgb[32768];
    int mdData = mdDataBeforeImage;

    /* We're going to loop over all of the data in the source.  For non-image
     * data, we'll just copy it.  For image data, we'll build an XBE on disk
     * and process it */
    ixph = ixbi = 0;
    iblkCur = 0;
    do {
        /* First figure out how much we're going to read from the tape */
        switch(mdData) {
        case mdDataImage:
            MsgOut("Reading XBE file\n");
            cblkCur = pdxl->pxbi[ixbi++].cpsnSize;
            pstmXBE = PstmCreateTemp("sign.xbe", GENERIC_WRITE);
            if(!pstmXBE->FIsOpen()) {
                fprintf(stderr, "error: could not open temporary XBE file\n");
                return FALSE;
            }
            pstmTo = pstmXBE;
            break;
        case mdDataAP:
            MsgOut("%s placeholder data\n", pfnAP ? "Reading" : "Copying");
            cblkCur = 4096;
            if(pfnAP) {
                if(!pfnAP(ilyr, ixph, ptapDst))
                    return FALSE;
                pstmTo = NULL;
            } else
                pstmTo = ptapDst;
            ++ixph;
            break;
        case mdDataBeforeAP:
            mdData = mdDataBeforeImage;
            // fall through
        case mdDataBeforeImage:
            /* Find the next image */
            cblkCur = (ixbi == pdxl->ixbiMac ? pdxr->cpsnSize :
                pdxl->pxbi[ixbi].lsnStart);
            /* If we have an AP region coming up before the next
             * image, we need to stop short */
            if(ixph < pdxl->cxph && cblkCur > pdxl->rgxph[ixph].lsnStart) {
                cblkCur = pdxl->rgxph[ixph].lsnStart;
                mdData = mdDataBeforeAP;
            }
            /* We don't need to verify that the images and placeholders
             * are nonoverlapping, since our dx2ml input data doesn't allow
             * for that possibility */
            cblkCur -= iblkCur;
            pstmTo = ptapDst;
            MsgOut("Copying %d blocks of data\n", cblkCur);
            break;
        }

        if(cblkCur) {
            /* Now read the tape data */
            cblkRem = cblkCur;
            while(cblkRem) {
                /* We'll try to read up to the next physical block */
                cblk = iblkCur & 15;
                if(cblk == 0)
                    cblk = sizeof rgb / 2048;
                else
                    cblk = sizeof rgb / 2048 - cblk;
                /* If this is more data than we want, cut down our data
                 * size */
                if(cblk > cblkRem)
                    cblk = cblkRem;
                cb = cblk * 2048;
                if(ptapSrc->CbRead(cb, rgb) != cb) {
                    fprintf(stderr, "error: reading source tape %d\n", ilyr);
                    goto fatal;
                }
                switch(mdData) {
                case mdDataBeforeImage:
                case mdDataBeforeAP:
                    /* Add these bytes into the media checksums */
                    xsumBlk.SumBytes(rgb, cb);
                    xsumRed.SumBytes(rgb, cb);
                    xsumGrn.SumBytes(rgb, cb);
                    // fall through
                case mdDataImage:
                    /* These go into the input media checksum */
                    xsumMediaIn.SumBytes(rgb, cb);
                    break;
                }
                if(pstmTo && pstmTo->CbWrite(cb, rgb) != cb) {
                    if(pstmTo != pstmXBE) {
                        fprintf(stderr, "error: writing destination tape %d\n",
                            ilyr);
                        goto fatal;
                    }
                    fprintf(stderr, "error: writing temporary XBE file\n");
                    goto fatal;
                }
                cblkRem -= cblk;
                iblkCur += cblk;
            }
        }

        switch(mdData) {
        case mdDataImage:
            /* We've read the image file; now we need to sign it and copy it
             * to the destination tape */
            if(!pstmXBE->FReopen(GENERIC_READ)) {
                fprintf(stderr, "error: reading temporary XBE file\n");
                goto fatal;
            }
            pxbe = new CXBEFile(pstmXBE);
            if(!pfnXbe(ilyr, pxbe, cblkCur, ptapDst))
                goto fatal;
            delete pxbe;
            pxbe = NULL;
            delete pstmXBE;
            pstmXBE = NULL;
            // fall through
        case mdDataAP:
            mdData = mdDataBeforeImage;
            break;
        case mdDataBeforeAP:
            mdData = mdDataAP;
            break;
        case mdDataBeforeImage:
            mdData = mdDataImage;
            break;
        }
    } while(iblkCur < pdxr->cpsnSize);

    /* If this file size doesn't actually match what we're expecting on the
     * tape, we're in trouble */
    if(iblkCur != pdxr->cpsnSize || ptapSrc->CbRead(sizeof rgb, rgb) != 0) {
        fprintf(stderr, "error: tape %d XImage file is the wrong size\n", ilyr);
        return FALSE;
    }

    return TRUE;

fatal:
    if(pxbe)
        delete pxbe;
    if(pstmXBE)
        delete pstmXBE;
    return FALSE;
}

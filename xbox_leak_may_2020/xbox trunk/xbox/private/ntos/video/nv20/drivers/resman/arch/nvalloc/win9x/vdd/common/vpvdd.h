

#ifndef _VP_VDD_H_
#define _VP_VDD_H_


U032	vddGetIRQInfo(U032 dwMiniVDDContext, U032 dummy, DDGETIRQINFO* pGetIrgInfo);
U032	vddIsOurIRQ(U032 dwMiniVDDContext, U032 dummy);
U032	vddEnableIRQ(U032 dwMiniVDDContext, DDENABLEIRQINFO* pEnableIrqInfo, U032 dummy);
U032	vddSkipNextField(U032 dwMiniVDDContext, DDSKIPINFO* pSkipInfo, U032 dummy);
U032	vddBobNextField(U032 dwMiniVDDContext, DDBOBINFO* pBobInfo, U032 dummy);
U032	vddSetState(U032 dwMiniVDDContext, DDSTATEININFO* pSetStateInfo, DDSTATEOUTINFO* pSetStateInfoOut);
U032	vddLock(U032 dwMiniVDDContext, DDLOCKININFO* pLockInfoIn, DDLOCKOUTINFO* pLockInfoOut);
U032	vddFlipOverlay(U032 dwMiniVDDContext, DDFLIPOVERLAYINFO* pFlipOverlayInfo, U032 dummy);


U032	vddGetFieldPolarity(U032 dwMiniVDDContext, DDPOLARITYININFO* pGetPolarityInInfo, DDPOLARITYOUTINFO* pGetPolarityOutInfo);



// for whatever reason the "real" definition won't work here so no big deal since we don't
// really USE this function, but it goes to show that MS headers are CRAP
//U032	vddFlipVideoPort(U032 dwMiniVDDContext, DDFLIPVIDEOPORT* pFlipVideoPort,U032 dummy);
U032	vddFlipVideoPort(U032 dwMiniVDDContext, U032* pFlipVideoPort, U032 dummy);


U032	vddSetSkipPattern(U032 dwMiniVDDContext, DDSETSKIPINFO* pSetSkipInfo, U032 dummy);
U032	vddGetCurrentAutoFlip(U032 dwMiniVDDContext, DDGETAUTOFLIPININFO* pGetAutoFlipInfoIn, DDGETAUTOFLIPOUTINFO* pGetAutoFlipInfoOut);
U032	vddGetPreviousAutoFlip(U032 dwMiniVDDContext, DDGETAUTOFLIPININFO* pGetAutoFlipInfoIn, DDGETAUTOFLIPOUTINFO* pGetAutoFlipInfoOut);


U032	vddTransfer(U032 dwMiniVddContext, DDTRANSFERININFO* pTransferInInfo, DDTRANSFEROUTINFO* pTransferOutInfo );

U032 vddGetTransferStatus(U032 dwMiniVddContext, U032 dummy, DDGETTRANSFERSTATUSOUTINFO* pGetTransStatusInfo );


#endif //_VP_VDD_H_
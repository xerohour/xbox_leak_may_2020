///////////////////////////////////////////////////////////////////////////
//
//  Copyright (C) 1999 Microsoft Corporation.  All Rights Reserved.
//
//  File:       s3tchelp.h
//  Content:    S3 texture compression helpper functions
//
///////////////////////////////////////////////////////////////////////////

void XXEncodeBlockRGB(S3TC_COLOR colorSrc[S3TC_BLOCK_PIXELS], S3TCBlockRGB *blockdst);
void XXDecodeBlockRGB(S3TCBlockRGB *blocksrc, S3TC_COLOR colordst[S3TC_BLOCK_PIXELS]);

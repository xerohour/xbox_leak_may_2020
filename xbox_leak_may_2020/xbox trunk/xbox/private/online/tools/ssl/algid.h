/*-----------------------------------------------------------------------------
* Copyright (C) Microsoft Corporation, 1995 - 1996.
* All rights reserved.
*
* This file is part of the Microsoft Private Communication Technology 
* reference implementation, version 1.0
* 
* The Private Communication Technology reference implementation, version 1.0 
* ("PCTRef"), is being provided by Microsoft to encourage the development and 
* enhancement of an open standard for secure general-purpose business and 
* personal communications on open networks.  Microsoft is distributing PCTRef 
* at no charge irrespective of whether you use PCTRef for non-commercial or 
* commercial use.
*
* Microsoft expressly disclaims any warranty for PCTRef and all derivatives of
* it.  PCTRef and any related documentation is provided "as is" without 
* warranty of any kind, either express or implied, including, without 
* limitation, the implied warranties or merchantability, fitness for a 
* particular purpose, or noninfringement.  Microsoft shall have no obligation
* to provide maintenance, support, upgrades or new releases to you or to anyone
* receiving from you PCTRef or your modifications.  The entire risk arising out 
* of use or performance of PCTRef remains with you.
* 
* Please see the file LICENSE.txt, 
* or http://pct.microsoft.com/pct/pctlicen.txt
* for more information on licensing.
* 
* Please see http://pct.microsoft.com/pct/pct.htm for The Private 
* Communication Technology Specification version 1.0 ("PCT Specification")
*
* 1/23/96
*----------------------------------------------------------------------------*/ 

#ifndef _ALGID_H_
#define _ALGID_H_

/* ALG_ID crackers */
#define GET_ALG_CLASS(x)                (x & (3 << 14))
#define GET_ALG_TYPE(x)                 (x & (15 << 10))
#define GET_ALG_SID(x)                  (x & (511))

/* Algorithm classes */

#define ALG_CLASS_SIGNATURE             (0 << 14)
#define ALG_CLASS_MSG_ENCRYPT           (1 << 14)
#define ALG_CLASS_DATA_ENCRYPT          (2 << 14)
#define ALG_CLASS_HASH                  (3 << 14)
#define ALG_CLASS_KEY_EXCHANGE          (4 << 14)

/* Algorithm types */

#define ALG_TYPE_ANY                    (0)
#define ALG_TYPE_DSA                    (1 << 10)
#define ALG_TYPE_RSA                    (2 << 10)
#define ALG_TYPE_BLOCK                  (3 << 10)
#define ALG_TYPE_STREAM                 (4 << 10)

/* Some RSA sub-ids */

#define ALG_SID_RSA_ANY                         0
#define ALG_SID_RSA_PKCS                        1
#define ALG_SID_RSA_MSATWORK                    2
#define ALG_SID_RSA_ENTRUST                     3
#define ALG_SID_RSA_PGP                         4

/* Some DSS sub-ids */

#define ALG_SID_DSS_ANY                         0
#define ALG_SID_DSS_PKCS                        1
#define ALG_SID_DSS_DMS                         2

/* Block cipher sub ids */
/* DES sub_ids */
#define ALG_SID_DES_ECB                         0
#define ALG_SID_DES_CBC                         1
#define ALG_SID_DES_CFB                         2
#define ALG_SID_DES_OFB                         3

/* RC2 sub-ids */
#define ALG_SID_RC2_ECB                         4
#define ALG_SID_RC2_CBC                         5
#define ALG_SID_RC2_CFB                         6
#define ALG_SID_RC2_OFB                         7

/* Stream cipher sub-ids */
#define ALG_SID_RC4                             0
#define ALG_SID_SEAL                            1

/* Hash sub ids */
#define ALG_SID_MD2                                     0
#define ALG_SID_MD4                                     1
#define ALG_SID_MD5                                     2
#define ALG_SID_SHA                                     3

/* Our silly example sub-id */    

#define ALG_SID_EXAMPLE         80

typedef int ALG_ID;


#define MD2                                     ALG_CLASS_HASH | ALG_TYPE_ANY | ALG_SID_MD2
#define MD4                                     ALG_CLASS_HASH | ALG_TYPE_ANY | ALG_SID_MD4
#define MD5                                     ALG_CLASS_HASH | ALG_TYPE_ANY | ALG_SID_MD5
#define SHA                                     ALG_CLASS_HASH | ALG_TYPE_ANY | ALG_SID_SHA
#define RSA_SIGNATURE           ALG_CLASS_SIGNATURE | ALG_TYPE_RSA | ALG_SID_RSA_ANY
#define DSS_SIGNATURE           ALG_CLASS_SIGNATURE | ALG_TYPE_DSA | ALG_SID_DSA_ANY
#define RSA_KEYEXCHANGE         ALG_CLASS_KEY_EXCHANGE | ALG_TYPE_RSA | ALG_SID_RSA_ANY
#define DES_ECB                         ALG_CLASS_DATA_ENCRYPT | ALG_TYPE_BLOCK | ALG_SID_DES_ECB
#define DES_CBC                         ALG_CLASS_DATA_ENCRYPT | ALG_TYPE_BLOCK | ALG_SID_DES_CBC
#define DES_CFB                         ALG_CLASS_DATA_ENCRYPT | ALG_TYPE_BLOCK | ALG_SID_DES_CFB
#define DES_OFB                         ALG_CLASS_DATA_ENCRYPT | ALG_TYPE_BLOCK | ALG_SID_DES_OFB
#define RC2_ECB                         ALG_CLASS_DATA_ENCRYPT | ALG_TYPE_BLOCK | ALG_SID_RC2_ECB
#define RC2_CBC                         ALG_CLASS_DATA_ENCRYPT | ALG_TYPE_BLOCK | ALG_SID_RC2_CBC
#define RC2_CFB                         ALG_CLASS_DATA_ENCRYPT | ALG_TYPE_BLOCK | ALG_SID_RC2_CFB
#define RC2_OFB                         ALG_CLASS_DATA_ENCRYPT | ALG_TYPE_BLOCK | ALG_SID_RC2_OFB
#define RC4                                     ALG_CLASS_DATA_ENCRYPT | ALG_TYPE_STREAM | ALG_SID_RC4
#define SEAL                            ALG_CLASS_DATA_ENCRYPT | ALG_TYPE_STREAM | ALG_SID_SEAL



#define MAXNAMELEN                      0x60

#define BASIC_RSA       0
#define MD2_WITH_RSA    1
#define MD5_WITH_RSA    2
#define RC4_STREAM      3

#endif /* _ALGID_H_ */

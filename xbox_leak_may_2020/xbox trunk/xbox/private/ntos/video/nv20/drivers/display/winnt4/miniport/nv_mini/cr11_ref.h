
/* NVidia Corporation */ 
/* basis: crush11 manuals 
built on Thu Feb 22 04:10:06 PST 2001*/

#ifndef _CR_REF_H_
#define _CR_REF_H_

#define CR_DRF_DEF(d,r,f,c)  ((CR ## d ## r ## f ## c)<<DRF_SHIFT(CR ## d ## r ## f))
#define CR_DRF_NUM(d,r,f,n)  (((n)&DRF_MASK(CR ## d ## r ## f))<<DRF_SHIFT(CR ## d ## r ## f))
#define CR_DRF_VAL(d,r,f,v)  (((v)>>DRF_SHIFT(CR ## d ## r ## f))&DRF_MASK(CR ## d ## r ## f))

/* dev_cr.ref */

#define CR_CMC_CFG0                                      0x8000017c /* RW-4R */
#define CR_CMC_CFG0_TOP_OF_FB				       10:6 /* RWIVF */
#define CR_CMC_CFG0_TOP_OF_FB_1MB                        0x00000000 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_2MB                        0x00000001 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_3MB                        0x00000002 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_4MB                        0x00000003 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_5MB                        0x00000004 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_6MB                        0x00000005 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_7MB                        0x00000006 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_8MB                        0x00000007 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_9MB                        0x00000008 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_10MB                       0x00000009 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_11MB                       0x0000000A /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_12MB                       0x0000000B /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_13MB                       0x0000000C /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_14MB                       0x0000000D /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_15MB                       0x0000000E /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_16MB                       0x0000000F /* RWI-V */
#define CR_CMC_CFG0_TOP_OF_FB_17MB                       0x00000010 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_18MB                       0x00000011 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_19MB                       0x00000012 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_20MB                       0x00000013 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_21MB                       0x00000014 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_22MB                       0x00000015 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_23MB                       0x00000016 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_24MB                       0x00000017 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_25MB                       0x00000018 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_26MB                       0x00000019 /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_27MB                       0x0000001A /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_28MB                       0x0000001B /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_29MB                       0x0000001C /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_30MB                       0x0000001D /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_31MB                       0x0000001E /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB_32MB                       0x0000001F /* RW--V */
#define CR_CMC_CFG0_TOP_OF_FB__PROD                      0x0000001F /* RW--V */

#endif /* _CR_REF_H_ */


/***                                                                  ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                      ***/
/***                                                                  ***/
/***   This software is supplied under the terms of a license         ***/
/***   agreement or nondisclosure agreement with Intel Corporation    ***/
/***   and may not be copied or disclosed except in accordance with   ***/
/***   the terms of that agreement.                                   ***/
/***   Copyright (c) 1992,1993,1994,1995  Intel Corporation.          ***/
/***                                                                  ***/

#ifndef IA_DECODER_H
#define IA_DECODER_H
typedef enum IA_Decoder_Inst_Id_e
{
	IA_DECODER_INST_NONE=0,
	X86_ALIAS,
	X86_FIRST_INST,
	X86_AAA,						/**  opcode:  37        **/
	X86_AAD,						/**  opcode:  D5        **/
	X86_AAM,						/**  opcode:  D4        **/
	X86_AAS,						/**  opcode:  3F        **/
	X86_ADCB_MI_IMB,				/**  opcode:  80 /2     **/
	X86_ADCB_MI_ALIAS,				/**  opcode:  82 /2     **/
	X86_ADCB_RI_AL,					/**  opcode:  14        **/
	X86_ADCW_RI_AX,					/**  opcode:  15        **/
	X86_ADCL_RI_EAX,				/**  opcode:  15        **/
	X86_ADCW_MI,					/**  opcode:  81 /2     **/
	X86_ADCL_MI,					/**  opcode:  81 /2     **/
	X86_ADCW_MI_B,					/**  opcode:  83 /2     **/
	X86_ADCL_MI_B,					/**  opcode:  83 /2     **/
	X86_ADCB_MR,					/**  opcode:  10        **/
	X86_ADCW_MR_RMR16,				/**  opcode:  11        **/
	X86_ADCL_MR,					/**  opcode:  11        **/
	X86_ADCB_RM,					/**  opcode:  12        **/
	X86_ADCW_RM_RRM16,				/**  opcode:  13        **/
	X86_ADCL_RM,					/**  opcode:  13        **/
	X86_ADDB_RI_AL,					/**  opcode:  04        **/
	X86_ADDW_RI_AX,					/**  opcode:  05        **/
	X86_ADDL_RI_EAX,				/**  opcode:  05        **/
	X86_ADDB_MI_IMB,				/**  opcode:  80 /0     **/
	X86_ADDB_MI_ALIAS,				/**  opcode:  82 /0     **/
	X86_ADDW_MI,					/**  opcode:  81 /0     **/
	X86_ADDL_MI,					/**  opcode:  81 /0     **/
	X86_ADDW_MI_B,					/**  opcode:  83 /0     **/
	X86_ADDL_MI_B,					/**  opcode:  83 /0     **/
	X86_ADDB_MR,					/**  opcode:  00        **/
	X86_ADDW_MR_RMR16,				/**  opcode:  01        **/
	X86_ADDL_MR,					/**  opcode:  01        **/
	X86_ADDB_RM,					/**  opcode:  02        **/
	X86_ADDW_RM_RRM16,				/**  opcode:  03        **/
	X86_ADDL_RM,					/**  opcode:  03        **/
	X86_ANDB_RI_AL,					/**  opcode:  24        **/
	X86_ANDW_RI_AX,					/**  opcode:  25        **/
	X86_ANDL_RI_EAX,				/**  opcode:  25        **/
	X86_ANDB_MI_IMB,				/**  opcode:  80 /4     **/
	X86_ANDB_MI_ALIAS,				/**  opcode:  82 /4     **/
	X86_ANDW_MI,					/**  opcode:  81 /4     **/
	X86_ANDL_MI,					/**  opcode:  81 /4     **/
	X86_ANDW_MI_B,					/**  opcode:  83 /4     **/
	X86_ANDL_MI_B,					/**  opcode:  83 /4     **/
	X86_ANDB_MR,					/**  opcode:  20        **/
	X86_ANDW_MR_RMR16,				/**  opcode:  21        **/
	X86_ANDL_MR,					/**  opcode:  21        **/
	X86_ANDB_RM,					/**  opcode:  22        **/
	X86_ANDW_RM_RRM16,				/**  opcode:  23        **/
	X86_ANDL_RM,					/**  opcode:  23        **/
	X86_ARPL_MR_RMR16,				/**  opcode:  63        **/
	X86_BOUNDW_RM,					/**  opcode:  62        **/
	X86_BOUNDL_RM,					/**  opcode:  62        **/
	X86_BSFW_RM_RRM16,				/**  opcode:  0F BC     **/
	X86_BSFL_RM,					/**  opcode:  0F BC     **/
	X86_BSRW_RM_RRM16,				/**  opcode:  0F BD     **/
	X86_BSRL_RM,					/**  opcode:  0F BD     **/
	X86_BSWAP_R_R32_OP1,			/**  opcode:  0F C8     **/
	X86_BTW_MR_RMR16,				/**  opcode:  0F A3     **/
	X86_BTL_MR,						/**  opcode:  0F A3     **/
	X86_BTW_MI_IMB,					/**  opcode:  0F BA /4  **/
	X86_BTL_MI_IMB,					/**  opcode:  0F BA /4  **/
	X86_BTCW_MR_RMR16,				/**  opcode:  0F BB     **/
	X86_BTCL_MR,					/**  opcode:  0F BB     **/
	X86_BTCW_MI_IMB,				/**  opcode:  0F BA /7  **/
	X86_BTCL_MI_IMB,				/**  opcode:  0F BA /7  **/
	X86_BTRW_MR_RMR16,				/**  opcode:  0F B3     **/
	X86_BTRL_MR,					/**  opcode:  0F B3     **/
	X86_BTRW_MI_IMB,				/**  opcode:  0F BA /6  **/
	X86_BTRL_MI_IMB,				/**  opcode:  0F BA /6  **/
	X86_BTSW_MR_RMR16,				/**  opcode:  0F AB     **/
	X86_BTSL_MR,					/**  opcode:  0F AB     **/
	X86_BTSW_MI_IMB,				/**  opcode:  0F BA /5  **/
	X86_BTSL_MI_IMB,				/**  opcode:  0F BA /5  **/
	X86_CALLFW_M,					/**  opcode:  FF /3     **/
	X86_CALLFL_M,					/**  opcode:  FF /3     **/
	X86_CALLNW_M_RM,				/**  opcode:  FF /2     **/
	X86_CALLNL_M_RM,				/**  opcode:  FF /2     **/
	X86_CALLNW,						/**  opcode:  E8        **/
	X86_CALLNL,						/**  opcode:  E8        **/
	X86_CALLFW,						/**  opcode:  9A        **/
	X86_CALLFL,						/**  opcode:  9A        **/
	X86_CBW,						/**  opcode:  98        **/
	X86_CWDE,						/**  opcode:  98        **/
	X86_CFLSH,						/**  opcode:  0F 0A     **/
	X86_CLC,						/**  opcode:  F8        **/
	X86_CLD,						/**  opcode:  FC        **/
	X86_CLI,						/**  opcode:  FA        **/
	X86_CLTS,						/**  opcode:  0F 06     **/
	X86_CMC,						/**  opcode:  F5        **/
	X86_CMOVAW,						/**  opcode:  0F 47     **/
	X86_CMOVAEW,					/**  opcode:  0F 43     **/
	X86_CMOVBW,						/**  opcode:  0F 42     **/
	X86_CMOVBEW,					/**  opcode:  0F 46     **/
	X86_CMOVEW,						/**  opcode:  0F 44     **/
	X86_CMOVGW,						/**  opcode:  0F 4F     **/
	X86_CMOVGEW,					/**  opcode:  0F 4D     **/
	X86_CMOVLW,						/**  opcode:  0F 4C     **/
	X86_CMOVLEW,					/**  opcode:  0F 4E     **/
	X86_CMOVNEW,					/**  opcode:  0F 45     **/
	X86_CMOVNOW,					/**  opcode:  0F 41     **/
	X86_CMOVNPW,					/**  opcode:  0F 4B     **/
	X86_CMOVNSW,					/**  opcode:  0F 49     **/
	X86_CMOVOW,						/**  opcode:  0F 40     **/
	X86_CMOVAW_W,					/**  opcode:  0F 4A     **/
	X86_CMOVSW,						/**  opcode:  0F 48     **/
	X86_CMOVAL,						/**  opcode:  0F 47     **/
	X86_CMOVAEL,					/**  opcode:  0F 43     **/
	X86_CMOVBL,						/**  opcode:  0F 42     **/
	X86_CMOVBEL,					/**  opcode:  0F 46     **/
	X86_CMOVEL,						/**  opcode:  0F 44     **/
	X86_CMOVGL,						/**  opcode:  0F 4F     **/
	X86_CMOVGEL,					/**  opcode:  0F 4D     **/
	X86_CMOVLL,						/**  opcode:  0F 4C     **/
	X86_CMOVLEL,					/**  opcode:  0F 4E     **/
	X86_CMOVNEL,					/**  opcode:  0F 45     **/
	X86_CMOVNOL,					/**  opcode:  0F 41     **/
	X86_CMOVNPL,					/**  opcode:  0F 4B     **/
	X86_CMOVNSL,					/**  opcode:  0F 49     **/
	X86_CMOVOL,						/**  opcode:  0F 40     **/
	X86_CMOVAL_L,					/**  opcode:  0F 4A     **/
	X86_CMOVSL,						/**  opcode:  0F 48     **/
	X86_CMPB_RI_AL,					/**  opcode:  3C        **/
	X86_CMPW_RI_AX,					/**  opcode:  3D        **/
	X86_CMPL_RI_EAX,				/**  opcode:  3D        **/
	X86_CMPB_MI_IMB,				/**  opcode:  80 /7     **/
	X86_CMPB_MI_ALIAS,				/**  opcode:  82 /7     **/
	X86_CMPW_MI,					/**  opcode:  81 /7     **/
	X86_CMPL_MI,					/**  opcode:  81 /7     **/
	X86_CMPW_MI_B,					/**  opcode:  83 /7     **/
	X86_CMPL_MI_B,					/**  opcode:  83 /7     **/
	X86_CMPB_MR,					/**  opcode:  38        **/
	X86_CMPW_MR_RMR16,				/**  opcode:  39        **/
	X86_CMPL_MR,					/**  opcode:  39        **/
	X86_CMPB_RM,					/**  opcode:  3A        **/
	X86_CMPW_RM_RRM16,				/**  opcode:  3B        **/
	X86_CMPL_RM,					/**  opcode:  3B        **/
	X86_CMPSB,						/**  opcode:  A6        **/
	X86_CMPSW,						/**  opcode:  A7        **/
	X86_CMPSL,						/**  opcode:  A7        **/
	X86_CMPXCHGB_MR,				/**  opcode:  0F B0     **/
	X86_CMPXCHGW_MR_RMR16,			/**  opcode:  0F B1     **/
	X86_CMPXCHGL_MR,				/**  opcode:  0F B1     **/
	X86_CMPXCHG8B,					/**  opcode:  0f C7 /1  **/
	X86_CPUID,						/**  opcode:  0F A2     **/
	X86_CWD,						/**  opcode:  99        **/
	X86_CDQ,						/**  opcode:  99        **/
	X86_DAA,						/**  opcode:  27        **/
	X86_DAS,						/**  opcode:  2F        **/
	X86_DECW_M_R16_OP1,				/**  opcode:  48        **/
	X86_DECL_M_R32_OP1,				/**  opcode:  48        **/
	X86_DECB_M_RM,					/**  opcode:  FE /1     **/
	X86_DECW_M_RM,					/**  opcode:  FF /1     **/
	X86_DECL_M_RM,					/**  opcode:  FF /1     **/
	X86_DIVB_RM_AL,					/**  opcode:  F6 /6     **/
	X86_DIVW_RM_AX,					/**  opcode:  F7 /6     **/
	X86_DIVL_RM_EAX,				/**  opcode:  F7 /6     **/
	X86_ENTER,						/**  opcode:  C8        **/
	X86_HLT,						/**  opcode:  F4        **/
	X86_IDIVB_RM_AL,				/**  opcode:  F6 /7     **/
	X86_IDIVW_RM_AX,				/**  opcode:  F7 /7     **/
	X86_IDIVL_RM_EAX,				/**  opcode:  F7 /7     **/
	X86_IMULB_M_RM,					/**  opcode:  F6 /5     **/
	X86_IMULW_M_RM,					/**  opcode:  F7 /5     **/
	X86_IMULL_M_RM,					/**  opcode:  F7 /5     **/
	X86_IMULW_RM_RRM16,				/**  opcode:  0F AF     **/
	X86_IMULL_RM,					/**  opcode:  0F AF     **/
	X86_IMULW_RMI_RRM16I8S,			/**  opcode:  6B        **/
	X86_IMULL_RMI_RRM32I8S,			/**  opcode:  6B        **/
	X86_IMULW_RMI_RRM16I,			/**  opcode:  69        **/
	X86_IMULL_RMI,					/**  opcode:  69        **/
	X86_INB_I_AL,					/**  opcode:  E4        **/
	X86_INW_I_AX,					/**  opcode:  E5        **/
	X86_INL_I_EAX,					/**  opcode:  E5        **/
	X86_INB_R_AL,					/**  opcode:  EC        **/
	X86_INW_R_AX,					/**  opcode:  ED        **/
	X86_INL_R_EAX,					/**  opcode:  ED        **/
	X86_INCB_M_RM,					/**  opcode:  FE /0     **/
	X86_INCW_M_RM,					/**  opcode:  FF /0     **/
	X86_INCL_M_RM,					/**  opcode:  FF /0     **/
	X86_INCW_M_R16_OP1,				/**  opcode:  40        **/
	X86_INCL_M_R32_OP1,				/**  opcode:  40        **/
	X86_INSB,						/**  opcode:  6C        **/
	X86_INSW,						/**  opcode:  6D        **/
	X86_INSL,						/**  opcode:  6D        **/
	X86_INT1,						/**  opcode:  F1        **/
	X86_INT3,						/**  opcode:  CC        **/
	X86_INT,						/**  opcode:  CD        **/
	X86_INTO,						/**  opcode:  CE        **/
	X86_INVD,						/**  opcode:  0F 08     **/
	X86_INVLPG,						/**  opcode:  0F 01 /7  **/
	X86_IRET,						/**  opcode:  CF        **/
	X86_IRETD,						/**  opcode:  CF        **/
	X86_JA,							/**  opcode:  77        **/
	X86_JAE,						/**  opcode:  73        **/
	X86_JB,							/**  opcode:  72        **/
	X86_JBE,						/**  opcode:  76        **/
	X86_JCXZ,						/**  opcode:  E3        **/
	X86_JE,							/**  opcode:  74        **/
	X86_JG,							/**  opcode:  7F        **/
	X86_JGE,						/**  opcode:  7D        **/
	X86_JLE,						/**  opcode:  7E        **/
	X86_JNGE,						/**  opcode:  7C        **/
	X86_JNO,						/**  opcode:  71        **/
	X86_JNP,						/**  opcode:  7B        **/
	X86_JNS,						/**  opcode:  79        **/
	X86_JNZ,						/**  opcode:  75        **/
	X86_JO,							/**  opcode:  70        **/
	X86_JP,							/**  opcode:  7A        **/
	X86_JS,							/**  opcode:  78        **/
	X86_JAFW,						/**  opcode:  0F 87     **/
	X86_JAEFW,						/**  opcode:  0F 83     **/
	X86_JBFW,						/**  opcode:  0F 82     **/
	X86_JBEFW,						/**  opcode:  0F 86     **/
	X86_JEFW,						/**  opcode:  0F 84     **/
	X86_JGFW,						/**  opcode:  0F 8F     **/
	X86_JGEFW,						/**  opcode:  0F 8D     **/
	X86_JLEFW,						/**  opcode:  0F 8E     **/
	X86_JNGEFW,						/**  opcode:  0F 8C     **/
	X86_JNOFW,						/**  opcode:  0F 81     **/
	X86_JNPFW,						/**  opcode:  0F 8B     **/
	X86_JNSFW,						/**  opcode:  0F 89     **/
	X86_JNZFW,						/**  opcode:  0F 85     **/
	X86_JOFW,						/**  opcode:  0F 80     **/
	X86_JPFW,						/**  opcode:  0F 8A     **/
	X86_JSFW,						/**  opcode:  0F 88     **/
	X86_JAFL,						/**  opcode:  0F 87     **/
	X86_JAEFL,						/**  opcode:  0F 83     **/
	X86_JBFL,						/**  opcode:  0F 82     **/
	X86_JBEFL,						/**  opcode:  0F 86     **/
	X86_JEFL,						/**  opcode:  0F 84     **/
	X86_JGFL,						/**  opcode:  0F 8F     **/
	X86_JGEFL,						/**  opcode:  0F 8D     **/
	X86_JLEFL,						/**  opcode:  0F 8E     **/
	X86_JNGEFL,						/**  opcode:  0F 8C     **/
	X86_JNOFL,						/**  opcode:  0F 81     **/
	X86_JNPFL,						/**  opcode:  0F 8B     **/
	X86_JNSFL,						/**  opcode:  0F 89     **/
	X86_JNZFL,						/**  opcode:  0F 85     **/
	X86_JOFL,						/**  opcode:  0F 80     **/
	X86_JPFL,						/**  opcode:  0F 8A     **/
	X86_JSFL,						/**  opcode:  0F 88     **/
	X86_JMPB,						/**  opcode:  EB        **/
	X86_JMPW,						/**  opcode:  E9        **/
	X86_JMPL,						/**  opcode:  E9        **/
	X86_JMPWP,						/**  opcode:  EA        **/
	X86_JMPLP,						/**  opcode:  EA        **/
	X86_JMPW_M_RM,					/**  opcode:  FF /4     **/
	X86_JMPL_M_RM,					/**  opcode:  FF /4     **/
	X86_JMPWI,						/**  opcode:  FF /5     **/
	X86_JMPLI,						/**  opcode:  FF /5     **/
	X86_JMPEW_M_RM,					/**  opcode:  0F 00 /6  **/
	X86_JMPEL_M_RM,					/**  opcode:  0F 00 /6  **/
	X86_JMPEW,						/**  opcode:  0F B8     **/
	X86_JMPEL,						/**  opcode:  0F B8     **/
	X86_LAHF,						/**  opcode:  9F        **/
	X86_LARW_M_RRM16,				/**  opcode:  0F 02     **/
	X86_LARL_M,						/**  opcode:  0F 02     **/
	X86_LDSW,						/**  opcode:  C5        **/
	X86_LDSL,						/**  opcode:  C5        **/
	X86_LESW,						/**  opcode:  C4        **/
	X86_LESL,						/**  opcode:  C4        **/
	X86_LSSW,						/**  opcode:  0F B2     **/
	X86_LSSL,						/**  opcode:  0F B2     **/
	X86_LFSW,						/**  opcode:  0F B4     **/
	X86_LFSL,						/**  opcode:  0F B4     **/
	X86_LGSW,						/**  opcode:  0F B5     **/
	X86_LGSL,						/**  opcode:  0F B5     **/
	X86_LEAW,						/**  opcode:  8D        **/
	X86_LEAL,						/**  opcode:  8D        **/
	X86_LEAVEW,						/**  opcode:  C9        **/
	X86_LEAVEL,						/**  opcode:  C9        **/
	X86_LGDT,						/**  opcode:  0F 01 /2  **/
	X86_LIDT,						/**  opcode:  0F 01 /3  **/
	X86_LLDT_M,						/**  opcode:  0F 00 /2  **/
	X86_LMSW_M,						/**  opcode:  0F 01 /6  **/
	X86_LODSB,						/**  opcode:  AC        **/
	X86_LODSW,						/**  opcode:  AD        **/
	X86_LODSL,						/**  opcode:  AD        **/
	X86_LOOP,						/**  opcode:  E2        **/
	X86_LOOPZ,						/**  opcode:  E1        **/
	X86_LOOPNZ,						/**  opcode:  E0        **/
	X86_LSLW_RRM16,					/**  opcode:  0F 03     **/
	X86_LSLL,						/**  opcode:  0F 03     **/
	X86_LTRW_M,						/**  opcode:  0F 00 /3  **/
	X86_MOVB_RM_AL,					/**  opcode:  A0        **/
	X86_MOVW_RM_AX,					/**  opcode:  A1        **/
	X86_MOVL_RM_EAX,				/**  opcode:  A1        **/
	X86_MOVB_MR_AL,					/**  opcode:  A2        **/
	X86_MOVW_MR_AX,					/**  opcode:  A3        **/
	X86_MOVL_MR_EAX,				/**  opcode:  A3        **/
	X86_MOVB_MR,					/**  opcode:  88        **/
	X86_MOVW_MR_RMR16,				/**  opcode:  89        **/
	X86_MOVL_MR,					/**  opcode:  89        **/
	X86_MOVB_RM,					/**  opcode:  8A        **/
	X86_MOVW_RM_RRM16,				/**  opcode:  8B        **/
	X86_MOVL_RM,					/**  opcode:  8B        **/
	X86_MOVW_SM_RMS16,				/**  opcode:  8C        **/
	X86_MOVW_SM_RMS32,				/**  opcode:  8C        **/
	X86_MOVW_MS_SRM16,				/**  opcode:  8E        **/
	X86_MOVB_RI,					/**  opcode:  B0        **/
	X86_MOVW_RI,					/**  opcode:  B8        **/
	X86_MOVL_RI,					/**  opcode:  B8        **/
	X86_MOVB_MI_IMB,				/**  opcode:  C6 /0     **/
	X86_MOVW_MI,					/**  opcode:  C7 /0     **/
	X86_MOVL_MI,					/**  opcode:  C7 /0     **/
	X86_MOVL_CR,					/**  opcode:  0F 22     **/
	X86_MOVL_RC,					/**  opcode:  0F 20     **/
	X86_MOVL_DR,					/**  opcode:  0F 23     **/
	X86_MOVL_RD,					/**  opcode:  0F 21     **/
	X86_MOVSB,						/**  opcode:  A4        **/
	X86_MOVSW,						/**  opcode:  A5        **/
	X86_MOVSL,						/**  opcode:  A5        **/
	X86_MOVSXBW_M,					/**  opcode:  0F BE     **/
	X86_MOVSXBL_M,					/**  opcode:  0F BE     **/
	X86_MOVSXWW_M,					/**  opcode:  0F BF     **/
	X86_MOVSXWL_M,					/**  opcode:  0F BF     **/
	X86_MOVZXBW_M,					/**  opcode:  0F B6     **/
	X86_MOVZXBL_M,					/**  opcode:  0F B6     **/
	X86_MOVZXWW_M,					/**  opcode:  0F B7     **/
	X86_MOVZXWL_M,					/**  opcode:  0F B7     **/
	X86_MULB_RM_AL,					/**  opcode:  F6 /4     **/
	X86_MULW_RM_AX,					/**  opcode:  F7 /4     **/
	X86_MULL_RM_EAX,				/**  opcode:  F7 /4     **/
	X86_NEGB_M_RM,					/**  opcode:  F6 /3     **/
	X86_NEGW_M_RM,					/**  opcode:  F7 /3     **/
	X86_NEGL_M_RM,					/**  opcode:  F7 /3     **/
	X86_NOTB_M_RM,					/**  opcode:  F6 /2     **/
	X86_NOTW_M_RM,					/**  opcode:  F7 /2     **/
	X86_NOTL_M_RM,					/**  opcode:  F7 /2     **/
	X86_ORB_RI_AL,					/**  opcode:  0C        **/
	X86_ORW_RI_AX,					/**  opcode:  0D        **/
	X86_ORL_RI_EAX,					/**  opcode:  0D        **/
	X86_ORB_MI_IMB,					/**  opcode:  80 /1     **/
	X86_ORB_MI_ALIAS,				/**  opcode:  82 /1     **/
	X86_ORW_MI,						/**  opcode:  81 /1     **/
	X86_ORL_MI,						/**  opcode:  81 /1     **/
	X86_ORW_MI_B,					/**  opcode:  83 /1     **/
	X86_ORL_MI_B,					/**  opcode:  83 /1     **/
	X86_ORB_MR,						/**  opcode:  08        **/
	X86_ORW_MR_RMR16,				/**  opcode:  09        **/
	X86_ORL_MR,						/**  opcode:  09        **/
	X86_ORB_RM,						/**  opcode:  0A        **/
	X86_ORW_RM_RRM16,				/**  opcode:  0B        **/
	X86_ORL_RM,						/**  opcode:  0B        **/
	X86_OUTB_I_AL,					/**  opcode:  E6        **/
	X86_OUTW_I_AX,					/**  opcode:  E7        **/
	X86_OUTL_I_EAX,					/**  opcode:  E7        **/
	X86_OUTB_R_AL,					/**  opcode:  EE        **/
	X86_OUTW_R_AX,					/**  opcode:  EF        **/
	X86_OUTL_R_EAX,					/**  opcode:  EF        **/
	X86_OUTSB,						/**  opcode:  6E        **/
	X86_OUTSW,						/**  opcode:  6F        **/
	X86_OUTSL,						/**  opcode:  6F        **/
	X86_POPW_M_RM,					/**  opcode:  8F /0     **/
	X86_POPL_M_RM,					/**  opcode:  8F /0     **/
	X86_POPW_R_R16_OP1,				/**  opcode:  58        **/
	X86_POPL_R_R32_OP1,				/**  opcode:  58        **/
	X86_POPW_DS_RDS,				/**  opcode:  1F        **/
	X86_POPL_DS_RDS,				/**  opcode:  1F        **/
	X86_POPW_ES,					/**  opcode:  07        **/
	X86_POPL_ES,					/**  opcode:  07        **/
	X86_POPW_FS,					/**  opcode:  17        **/
	X86_POPL_FS,					/**  opcode:  17        **/
	X86_POPW_DS_RFS,				/**  opcode:  0F A1     **/
	X86_POPL_DS_RFS,				/**  opcode:  0F A1     **/
	X86_POPW_GS_RGS,				/**  opcode:  0F A9     **/
	X86_POPL_GS_RGS,				/**  opcode:  0F A9     **/
	X86_POPAW,						/**  opcode:  61        **/
	X86_POPAL,						/**  opcode:  61        **/
	X86_POPFW,						/**  opcode:  9D        **/
	X86_POPFL,						/**  opcode:  9D        **/
	X86_PUSHW_M_RM,					/**  opcode:  FF /6     **/
	X86_PUSHL_M_RM,					/**  opcode:  FF /6     **/
	X86_PUSHW_R_R16_OP1,			/**  opcode:  50        **/
	X86_PUSHL_R_R32_OP1,			/**  opcode:  50        **/
	X86_PUSHW_DS_RDS,				/**  opcode:  1E        **/
	X86_PUSHL_DS_RDS,				/**  opcode:  1E        **/
	X86_PUSHW_ES,					/**  opcode:  06        **/
	X86_PUSHL_ES,					/**  opcode:  06        **/
	X86_PUSHW_SS,					/**  opcode:  16        **/
	X86_PUSHL_SS,					/**  opcode:  16        **/
	X86_PUSHW_FS_RFS,				/**  opcode:  0F A0     **/
	X86_PUSHL_FS_RFS,				/**  opcode:  0F A0     **/
	X86_PUSHW_GS_RGS,				/**  opcode:  0F A8     **/
	X86_PUSHL_GS_RGS,				/**  opcode:  0F A8     **/
	X86_PUSHW_CS_RCS,				/**  opcode:  0E        **/
	X86_PUSHL_CS_RCS,				/**  opcode:  0E        **/
	X86_PUSHBW_I,					/**  opcode:  6A        **/
	X86_PUSHB_I,					/**  opcode:  6A        **/
	X86_PUSHW_I,					/**  opcode:  68        **/
	X86_PUSHL_I,					/**  opcode:  68        **/
	X86_PUSHAW,						/**  opcode:  60        **/
	X86_PUSHAL,						/**  opcode:  60        **/
	X86_PUSHFW,						/**  opcode:  9C        **/
	X86_PUSHFL,						/**  opcode:  9C        **/
	X86_RCLB_MI_SHFT_1,				/**  opcode:  D0 /2     **/
	X86_RCLB_MR,					/**  opcode:  D2 /2     **/
	X86_RCLB_MI_IMB,				/**  opcode:  C0 /2     **/
	X86_RCLW_MI_SHFT_1,				/**  opcode:  D1 /2     **/
	X86_RCLW_MR,					/**  opcode:  D3 /2     **/
	X86_RCLW_MI_IMB,				/**  opcode:  C1 /2     **/
	X86_RCLL_MI_SHFT_1,				/**  opcode:  D1 /2     **/
	X86_RCLL_MR,					/**  opcode:  D3 /2     **/
	X86_RCLL_MI_IMB,				/**  opcode:  C1 /2     **/
	X86_RCRB_MI_SHFT_1,				/**  opcode:  D0 /3     **/
	X86_RCRB_MR,					/**  opcode:  D2 /3     **/
	X86_RCRB_MI_IMB,				/**  opcode:  C0 /3     **/
	X86_RCRW_MI_SHFT_1,				/**  opcode:  D1 /3     **/
	X86_RCRW_MR,					/**  opcode:  D3 /3     **/
	X86_RCRW_MI_IMB,				/**  opcode:  C1 /3     **/
	X86_RCRL_MI_SHFT_1,				/**  opcode:  D1 /3     **/
	X86_RCRL_MR,					/**  opcode:  D3 /3     **/
	X86_RCRL_MI_IMB,				/**  opcode:  C1 /3     **/
	X86_ROLB_MI_SHFT_1,				/**  opcode:  D0 /0     **/
	X86_ROLB_MR,					/**  opcode:  D2 /0     **/
	X86_ROLB_MI_IMB,				/**  opcode:  C0 /0     **/
	X86_ROLW_MI_SHFT_1,				/**  opcode:  D1 /0     **/
	X86_ROLW_MR,					/**  opcode:  D3 /0     **/
	X86_ROLW_MI_IMB,				/**  opcode:  C1 /0     **/
	X86_ROLL_MI_SHFT_1,				/**  opcode:  D1 /0     **/
	X86_ROLL_MR,					/**  opcode:  D3 /0     **/
	X86_ROLL_MI_IMB,				/**  opcode:  C1 /0     **/
	X86_RORB_MI_SHFT_1,				/**  opcode:  D0 /1     **/
	X86_RORB_MR,					/**  opcode:  D2 /1     **/
	X86_RORB_MI_IMB,				/**  opcode:  C0 /1     **/
	X86_RORW_MI_SHFT_1,				/**  opcode:  D1 /1     **/
	X86_RORW_MR,					/**  opcode:  D3 /1     **/
	X86_RORW_MI_IMB,				/**  opcode:  C1 /1     **/
	X86_RORL_MI_SHFT_1,				/**  opcode:  D1 /1     **/
	X86_RORL_MR,					/**  opcode:  D3 /1     **/
	X86_RORL_MI_IMB,				/**  opcode:  C1 /1     **/
	X86_RDMSR,						/**  opcode:  0F 32     **/
	X86_RDPMC,						/**  opcode:  0F 33     **/
	X86_RDTSC,						/**  opcode:  0F 31     **/
	X86_RSM,						/**  opcode:  0F AA     **/
	X86_RETN,						/**  opcode:  C3        **/
	X86_RETF,						/**  opcode:  CB        **/
	X86_RETN_I,						/**  opcode:  C2        **/
	X86_RETF_I,						/**  opcode:  CA        **/
	X86_SAHF,						/**  opcode:  9E        **/
	X86_SARB_MI_SHFT_1,				/**  opcode:  D0 /7     **/
	X86_SARB_MR,					/**  opcode:  D2 /7     **/
	X86_SARB_MI_IMB,				/**  opcode:  C0 /7     **/
	X86_SARW_MI_SHFT_1,				/**  opcode:  D1 /7     **/
	X86_SARW_MR,					/**  opcode:  D3 /7     **/
	X86_SARW_MI_IMB,				/**  opcode:  C1 /7     **/
	X86_SARL_MI_SHFT_1,				/**  opcode:  D1 /7     **/
	X86_SARL_MR,					/**  opcode:  D3 /7     **/
	X86_SARL_MI_IMB,				/**  opcode:  C1 /7     **/
	X86_SHLB_MI_SHFT_1,				/**  opcode:  D0 /4     **/
	X86_SHLB_MR,					/**  opcode:  D2 /4     **/
	X86_SHLB_MI_IMB,				/**  opcode:  C0 /4     **/
	X86_SHLW_MI_SHFT_1,				/**  opcode:  D1 /4     **/
	X86_SHLW_MR,					/**  opcode:  D3 /4     **/
	X86_SHLW_MI_IMB,				/**  opcode:  C1 /4     **/
	X86_SHLL_MI_SHFT_1,				/**  opcode:  D1 /4     **/
	X86_SHLL_MR,					/**  opcode:  D3 /4     **/
	X86_SHLL_MI_IMB,				/**  opcode:  C1 /4     **/
	X86_SHLB_MI_1_ALIAS,			/**  opcode:  D0 /6     **/
	X86_SHLB_MR_ALIAS,				/**  opcode:  D2 /6     **/
	X86_SHLB_MI_I_ALIAS,			/**  opcode:  C0 /6     **/
	X86_SHLW_MI_1_ALIAS,			/**  opcode:  D1 /6     **/
	X86_SHLW_MR_ALIAS,				/**  opcode:  D3 /6     **/
	X86_SHLW_MI_I_ALIAS,			/**  opcode:  C1 /6     **/
	X86_SHLL_MI_1_ALIAS,			/**  opcode:  D1 /6     **/
	X86_SHLL_MR_ALIAS,				/**  opcode:  D3 /6     **/
	X86_SHLL_MI_I_ALIAS,			/**  opcode:  C1 /6     **/
	X86_SHRB_MI_SHFT_1,				/**  opcode:  D0 /5     **/
	X86_SHRB_MR,					/**  opcode:  D2 /5     **/
	X86_SHRB_MI_IMB,				/**  opcode:  C0 /5     **/
	X86_SHRW_MI_SHFT_1,				/**  opcode:  D1 /5     **/
	X86_SHRW_MR,					/**  opcode:  D3 /5     **/
	X86_SHRW_MI_IMB,				/**  opcode:  C1 /5     **/
	X86_SHRL_MI_SHFT_1,				/**  opcode:  D1 /5     **/
	X86_SHRL_MR,					/**  opcode:  D3 /5     **/
	X86_SHRL_MI_IMB,				/**  opcode:  C1 /5     **/
	X86_SBBB_RI_AL,					/**  opcode:  1C        **/
	X86_SBBW_RI_AX,					/**  opcode:  1D        **/
	X86_SBBL_RI_EAX,				/**  opcode:  1D        **/
	X86_SBBB_MI_IMB,				/**  opcode:  80 /3     **/
	X86_SBBB_MI_ALIAS,				/**  opcode:  82 /3     **/
	X86_SBBW_MI,					/**  opcode:  81 /3     **/
	X86_SBBL_MI,					/**  opcode:  81 /3     **/
	X86_SBBW_MI_B,					/**  opcode:  83 /3     **/
	X86_SBBL_MI_B,					/**  opcode:  83 /3     **/
	X86_SBBB_MR,					/**  opcode:  18        **/
	X86_SBBW_MR_RMR16,				/**  opcode:  19        **/
	X86_SBBL_MR,					/**  opcode:  19        **/
	X86_SBBB_RM,					/**  opcode:  1A        **/
	X86_SBBW_RM_RRM16,				/**  opcode:  1B        **/
	X86_SBBL_RM,					/**  opcode:  1B        **/
	X86_SCASB,						/**  opcode:  AE        **/
	X86_SCASW,						/**  opcode:  AF        **/
	X86_SCASL,						/**  opcode:  AF        **/
	X86_SALC,						/**  opcode:  D6        **/
	X86_SETA_M_RM,					/**  opcode:  0F 97     **/
	X86_SETAE_M_RM,					/**  opcode:  0F 93     **/
	X86_SETB_M_RM,					/**  opcode:  0F 92     **/
	X86_SETBE_M_RM,					/**  opcode:  0F 96     **/
	X86_SETE_M_RM,					/**  opcode:  0F 94     **/
	X86_SETG_M_RM,					/**  opcode:  0F 9F     **/
	X86_SETGE_M_RM,					/**  opcode:  0F 9D     **/
	X86_SETL_M_RM,					/**  opcode:  0F 9C     **/
	X86_SETLE_R_RM,					/**  opcode:  0F 9E     **/
	X86_SETNE_M_RM,					/**  opcode:  0F 95     **/
	X86_SETNO_M_RM,					/**  opcode:  0F 91     **/
	X86_SETNP_M_RM,					/**  opcode:  0F 9B     **/
	X86_SETNS_M_RM,					/**  opcode:  0F 99     **/
	X86_SETO_M_RM,					/**  opcode:  0F 90     **/
	X86_SETA_M_P,					/**  opcode:  0F 9A     **/
	X86_SETS_M_RM,					/**  opcode:  0F 98     **/
	X86_SGDT,						/**  opcode:  0F 01 /0  **/
	X86_SIDT,						/**  opcode:  0F 01 /1  **/
	X86_SHLDW_MI,					/**  opcode:  0F A4     **/
	X86_SHLDL_MI,					/**  opcode:  0F A4     **/
	X86_SHLDW_MR,					/**  opcode:  0F A5     **/
	X86_SHLDL_MR,					/**  opcode:  0F A5     **/
	X86_SHRDW_MI,					/**  opcode:  0F AC     **/
	X86_SHRDL_MI,					/**  opcode:  0F AC     **/
	X86_SHRDW_MR,					/**  opcode:  0F AD     **/
	X86_SHRDL_MR,					/**  opcode:  0F AD     **/
	X86_SLDTW_M,					/**  opcode:  0F 00 /0  **/
	X86_SLDTL_M,					/**  opcode:  0F 00 /0  **/
	X86_SMSWW_M,					/**  opcode:  0F 01 /4  **/
	X86_SMSWL_M,					/**  opcode:  0F 01 /4  **/
	X86_STC,						/**  opcode:  F9        **/
	X86_STD,						/**  opcode:  FD        **/
	X86_STI,						/**  opcode:  FB        **/
	X86_STOSB,						/**  opcode:  AA        **/
	X86_STOSW,						/**  opcode:  AB        **/
	X86_STOSL,						/**  opcode:  AB        **/
	X86_STRW_M,						/**  opcode:  0F 00 /1  **/
	X86_STRL_M,						/**  opcode:  0F 00 /1  **/
	X86_SUBB_RI_AL,					/**  opcode:  2C        **/
	X86_SUBW_RI_AX,					/**  opcode:  2D        **/
	X86_SUBL_RI_EAX,				/**  opcode:  2D        **/
	X86_SUBB_MI_IMB,				/**  opcode:  80 /5     **/
	X86_SUBB_MI_ALIAS,				/**  opcode:  82 /5     **/
	X86_SUBW_MI,					/**  opcode:  81 /5     **/
	X86_SUBL_MI,					/**  opcode:  81 /5     **/
	X86_SUBW_MI_B,					/**  opcode:  83 /5     **/
	X86_SUBL_MI_B,					/**  opcode:  83 /5     **/
	X86_SUBB_MR,					/**  opcode:  28        **/
	X86_SUBW_MR_RMR16,				/**  opcode:  29        **/
	X86_SUBL_MR,					/**  opcode:  29        **/
	X86_SUBB_RM,					/**  opcode:  2A        **/
	X86_SUBW_RM_RRM16,				/**  opcode:  2B        **/
	X86_SUBL_RM,					/**  opcode:  2B        **/
	X86_TESTB_RI_AL,				/**  opcode:  A8        **/
	X86_TESTW_RI_AX,				/**  opcode:  A9        **/
	X86_TESTL_RI_EAX,				/**  opcode:  A9        **/
	X86_TESTB_MI_IMB,				/**  opcode:  F6 /0     **/
	X86_TESTB_MI_ALIAS,				/**  opcode:  F6 /1     **/
	X86_TESTW_MI,					/**  opcode:  F7 /0     **/
	X86_TESTW_MI_ALIAS,				/**  opcode:  F7 /1     **/
	X86_TESTL_MI,					/**  opcode:  F7 /0     **/
	X86_TESTL_MI_ALIAS,				/**  opcode:  F7 /1     **/
	X86_TESTB_MR,					/**  opcode:  84        **/
	X86_TESTW_MR_RMR16,				/**  opcode:  85        **/
	X86_TESTL_MR,					/**  opcode:  85        **/
	X86_VERR_M,						/**  opcode:  0F 00 /4  **/
	X86_VERW_M,						/**  opcode:  0F 00 /5  **/
	X86_WAIT,						/**  opcode:  9B        **/
	X86_WBINVD,						/**  opcode:  0F 09     **/
	X86_WRMSR,						/**  opcode:  0F 30     **/
	X86_XADDB_MR,					/**  opcode:  0F C0     **/
	X86_XADDW_MR_RMR16,				/**  opcode:  0F C1     **/
	X86_XADDL_MR,					/**  opcode:  0F C1     **/
	X86_XCHGW_RR_AX,				/**  opcode:  90        **/
	X86_XCHGL_RR_EAX,				/**  opcode:  90        **/
	X86_XCHGB_RM,					/**  opcode:  86        **/
	X86_XCHGW_RM_RRM16,				/**  opcode:  87        **/
	X86_XCHGL_RM,					/**  opcode:  87        **/
	X86_XLATB,						/**  opcode:  D7        **/
	X86_XORB_RI_AL,					/**  opcode:  34        **/
	X86_XORW_RI_AX,					/**  opcode:  35        **/
	X86_XORL_RI_EAX,				/**  opcode:  35        **/
	X86_XORB_MI_IMB,				/**  opcode:  80 /6     **/
	X86_XORB_MI_ALIAS,				/**  opcode:  82 /6     **/
	X86_XORW_MI,					/**  opcode:  81 /6     **/
	X86_XORL_MI,					/**  opcode:  81 /6     **/
	X86_XORW_MI_B,					/**  opcode:  83 /6     **/
	X86_XORL_MI_B,					/**  opcode:  83 /6     **/
	X86_XORB_MR,					/**  opcode:  30        **/
	X86_XORW_MR_RMR16,				/**  opcode:  31        **/
	X86_XORL_MR,					/**  opcode:  31        **/
	X86_XORB_RM,					/**  opcode:  32        **/
	X86_XORW_RM_RRM16,				/**  opcode:  33        **/
	X86_XORL_RM,					/**  opcode:  33        **/
	X86_F2XM1,						/**  opcode:  D9 F0     **/
	X86_FABS,						/**  opcode:  D9 E1     **/
	X86_FADDS_M,					/**  opcode:  D8 /0     **/
	X86_FADDL_M,					/**  opcode:  DC /0     **/
	X86_FADD_0I,					/**  opcode:  D8 C0     **/
	X86_FADD_I0,					/**  opcode:  DC C0     **/
	X86_FADDP_I0,					/**  opcode:  DE C0     **/
	X86_FIADDL_M,					/**  opcode:  DA /0     **/
	X86_FIADDS_M,					/**  opcode:  DE /0     **/
	X86_FBLD,						/**  opcode:  DF /4     **/
	X86_FBSTP,						/**  opcode:  DF /6     **/
	X86_FCHS,						/**  opcode:  D9 E0     **/
	X86_FNCLEX,						/**  opcode:  DB E2     **/
	X86_FCOMS_M,					/**  opcode:  D8 /2     **/
	X86_FCOML_M,					/**  opcode:  DC /2     **/
	X86_FCOM_0I,					/**  opcode:  D8 D0     **/
	X86_FCOM_0I_ALIAS,				/**  opcode:  DC D0     **/
	X86_FCOMPS_M,					/**  opcode:  D8 /3     **/
	X86_FCOMPL_M,					/**  opcode:  DC /3     **/
	X86_FCOMP_0I,					/**  opcode:  D8 D8     **/
	X86_FCOMP_0I_ALIAS1,			/**  opcode:  DC D8     **/
	X86_FCOMP_0I_ALIAS2,			/**  opcode:  DE D0     **/
	X86_FCOMPP,						/**  opcode:  DE D9     **/
	X86_FCOMI_0I,					/**  opcode:  DB F0     **/
	X86_FCOMIP_0I,					/**  opcode:  DF F0     **/
	X86_FCOS,						/**  opcode:  D9 FF     **/
	X86_FDECSTP,					/**  opcode:  D9 F6     **/
	X86_FCMOVB,						/**  opcode:  DA C0     **/
	X86_FCMOVE,						/**  opcode:  DA C8     **/
	X86_FCMOVBE,					/**  opcode:  DA D0     **/
	X86_FCMOVU,						/**  opcode:  DA D8     **/
	X86_FCMOVNB,					/**  opcode:  DB C0     **/
	X86_FCMOVNE,					/**  opcode:  DB C8     **/
	X86_FCMOVNBE,					/**  opcode:  DB D0     **/
	X86_FCMOVNU,					/**  opcode:  DB D8     **/
	X86_FDIVS_M,					/**  opcode:  D8 /6     **/
	X86_FDIVL_M,					/**  opcode:  DC /6     **/
	X86_FDIV_0I,					/**  opcode:  D8 F0     **/
	X86_FDIVR_I0,					/**  opcode:  DC F0     **/
	X86_FDIVRP_I0,					/**  opcode:  DE F0     **/
	X86_FIDIV_M,					/**  opcode:  DE /6     **/
	X86_FIDIVL_M,					/**  opcode:  DA /6     **/
	X86_FDISI,						/**  opcode:  DB E1     **/
	X86_FENI,						/**  opcode:  DB E0     **/
	X86_FDIVRS_M,					/**  opcode:  D8 /7     **/
	X86_FDIVRL_M,					/**  opcode:  DC /7     **/
	X86_FDIVR_0I,					/**  opcode:  D8 F8     **/
	X86_FDIV_I0,					/**  opcode:  DC F8     **/
	X86_FDIVP_I0,					/**  opcode:  DE F8     **/
	X86_FIDIVR_M,					/**  opcode:  DE /7     **/
	X86_FIDIVRL_M,					/**  opcode:  DA /7     **/
	X86_FFREE,						/**  opcode:  DD C0     **/
	X86_FFREEP,						/**  opcode:  DF C0     **/
	X86_FICOM,						/**  opcode:  DE /2     **/
	X86_FICOML,						/**  opcode:  DA /2     **/
	X86_FICOMP,						/**  opcode:  DE /3     **/
	X86_FICOMPL,					/**  opcode:  DA /3     **/
	X86_FILD,						/**  opcode:  DF /0     **/
	X86_FILDL,						/**  opcode:  DB /0     **/
	X86_FILDLL,						/**  opcode:  DF /5     **/
	X86_FINCSTP,					/**  opcode:  D9 F7     **/
	X86_FNINIT,						/**  opcode:  DB E3     **/
	X86_FIST,						/**  opcode:  DF /2     **/
	X86_FISTL,						/**  opcode:  DB /2     **/
	X86_FISTP,						/**  opcode:  DF /3     **/
	X86_FISTPL,						/**  opcode:  DB /3     **/
	X86_FISTPLL,					/**  opcode:  DF /7     **/
	X86_FLDS,						/**  opcode:  D9 /0     **/
	X86_FLDL,						/**  opcode:  DD /0     **/
	X86_FLDT,						/**  opcode:  DB /5     **/
	X86_FLD,						/**  opcode:  D9 C0     **/
	X86_FLD1,						/**  opcode:  D9 E8     **/
	X86_FLDL2T,						/**  opcode:  D9 E9     **/
	X86_FLDL2E,						/**  opcode:  D9 EA     **/
	X86_FLDPI,						/**  opcode:  D9 EB     **/
	X86_FLDLG2,						/**  opcode:  D9 EC     **/
	X86_FLDLN2,						/**  opcode:  D9 ED     **/
	X86_FLDZ,						/**  opcode:  D9 EE     **/
	X86_FLDCW_RM,					/**  opcode:  D9 /5     **/
	X86_FLDENV_W_M14B,				/**  opcode:  D9 /4     **/
	X86_FLDENV_L_M28B,				/**  opcode:  D9 /4     **/
	X86_FMULS_M,					/**  opcode:  D8 /1     **/
	X86_FMULL_M,					/**  opcode:  DC /1     **/
	X86_FMUL_0I,					/**  opcode:  D8 C8     **/
	X86_FMUL_I0,					/**  opcode:  DC C8     **/
	X86_FMULP_I0,					/**  opcode:  DE C8     **/
	X86_FIMULL_M,					/**  opcode:  DA /1     **/
	X86_FIMUL_M,					/**  opcode:  DE /1     **/
	X86_FNOP,						/**  opcode:  D9 D0     **/
	X86_FPATAN,						/**  opcode:  D9 F3     **/
	X86_FPREM,						/**  opcode:  D9 F8     **/
	X86_FPREM1,						/**  opcode:  D9 F5     **/
	X86_FPTAN,						/**  opcode:  D9 F2     **/
	X86_FRNDINT,					/**  opcode:  D9 FC     **/
	X86_FRSTOR_W_RM,				/**  opcode:  DD /4     **/
	X86_FRSTOR_L_RM,				/**  opcode:  DD /4     **/
	X86_FNSAVE_W_RM,				/**  opcode:  DD /6     **/
	X86_FNSAVE_L_RM,				/**  opcode:  DD /6     **/
	X86_FSETPM,						/**  opcode:  DB E4     **/
	X86_FSCALE,						/**  opcode:  D9 FD     **/
	X86_FSIN,						/**  opcode:  D9 FE     **/
	X86_FSINCOS,					/**  opcode:  D9 FB     **/
	X86_FSQRT,						/**  opcode:  D9 FA     **/
	X86_FSTS,						/**  opcode:  D9 /2     **/
	X86_FSTL,						/**  opcode:  DD /2     **/
	X86_FST,						/**  opcode:  DD D0     **/
	X86_FSTPS,						/**  opcode:  D9 /3     **/
	X86_FSTPL,						/**  opcode:  DD /3     **/
	X86_FSTPT,						/**  opcode:  DB /7     **/
	X86_FSTP,						/**  opcode:  DD D8     **/
	X86_FSTP_ALIAS1,				/**  opcode:  D9 D8     **/
	X86_FSTP_ALIAS2,				/**  opcode:  DF D0     **/
	X86_FSTP_ALIAS3,				/**  opcode:  DF D8     **/
	X86_FNSTCW_RM,					/**  opcode:  D9 /7     **/
	X86_FSTENV_W_M14B,				/**  opcode:  D9 /6     **/
	X86_FSTENV_L_M28B,				/**  opcode:  D9 /6     **/
	X86_FNSTSW_RM,					/**  opcode:  DD /7     **/
	X86_FNSTSW_A_16,				/**  opcode:  DF E0     **/
	X86_FSUBS_M,					/**  opcode:  D8 /4     **/
	X86_FSUBL_M,					/**  opcode:  DC /4     **/
	X86_FSUB_0I,					/**  opcode:  D8 E0     **/
	X86_FSUBR_I0,					/**  opcode:  DC E0     **/
	X86_FSUBRP_I0,					/**  opcode:  DE E0     **/
	X86_FISUBL_M,					/**  opcode:  DA /4     **/
	X86_FISUB_M,					/**  opcode:  DE /4     **/
	X86_FSUBRS_M,					/**  opcode:  D8 /5     **/
	X86_FSUBRL_M,					/**  opcode:  DC /5     **/
	X86_FSUBR_0I,					/**  opcode:  D8 E8     **/
	X86_FSUB_I0,					/**  opcode:  DC E8     **/
	X86_FSUBP_I0,					/**  opcode:  DE E8     **/
	X86_FISUBRL_M,					/**  opcode:  DA /5     **/
	X86_FISUBR_M,					/**  opcode:  DE /5     **/
	X86_FTST,						/**  opcode:  D9 E4     **/
	X86_FUCOM,						/**  opcode:  DD E0     **/
	X86_FUCOMP,						/**  opcode:  DD E8     **/
	X86_FUCOMPP,					/**  opcode:  DA E9     **/
	X86_FUCOMI,						/**  opcode:  DB E8     **/
	X86_FUCOMIP,					/**  opcode:  DF E8     **/
	X86_FXAM,						/**  opcode:  D9 E5     **/
	X86_FXCH,						/**  opcode:  D9 C8     **/
	X86_FXCH_ALIAS1,				/**  opcode:  DD C8     **/
	X86_FXCH_ALIAS2,				/**  opcode:  DF C8     **/
	X86_FXTRACT,					/**  opcode:  D9 F4     **/
	X86_FYL2X,						/**  opcode:  D9 F1     **/
	X86_FYL2XP1,					/**  opcode:  D9 F9     **/
	X86_SYSENTER,					/**  opcode:  0F 34     **/
	X86_SYSEXIT,					/**  opcode:  0F 35     **/
	X86_ZALLOC,						/**  opcode:  0F C7 /2  **/
	X86_EMMS_MM,					/**  opcode:  0F 77     **/
	X86_MOVDL_MM,					/**  opcode:  0F 7E     **/
	X86_MOVDL_MRR_MM,				/**  opcode:  0F 6E     **/
	X86_MOVQ_RM_MM,					/**  opcode:  0F 6F     **/
	X86_MOVQ_MR_MM,					/**  opcode:  0F 7F     **/
	X86_PACKSSWB_MM,				/**  opcode:  0F 63     **/
	X86_PACKSSDW_MM,				/**  opcode:  0F 6B     **/
	X86_PACKUSWB_MM,				/**  opcode:  0F 67     **/
	X86_PADDB_MM,					/**  opcode:  0F FC     **/
	X86_PADDW_MM,					/**  opcode:  0F FD     **/
	X86_PADDD_MM,					/**  opcode:  0F FE     **/
	X86_PADDSB_MM,					/**  opcode:  0F EC     **/
	X86_PADDSW_MM,					/**  opcode:  0F ED     **/
	X86_PADDUSB_MM,					/**  opcode:  0F DC     **/
	X86_PADDUSW_MM,					/**  opcode:  0F DD     **/
	X86_PAND_MM,					/**  opcode:  0F DB     **/
	X86_PANDN_MM,					/**  opcode:  0F DF     **/
	X86_PCMPEQB_MM,					/**  opcode:  0F 74     **/
	X86_PCMPEQW_MM,					/**  opcode:  0F 75     **/
	X86_PCMPEQD_MM,					/**  opcode:  0F 76     **/
	X86_PCMPGTB_MM,					/**  opcode:  0F 64     **/
	X86_PCMPGTW_MM,					/**  opcode:  0F 65     **/
	X86_PCMPGTD_MM,					/**  opcode:  0F 66     **/
	X86_PMADDWD_MM,					/**  opcode:  0F F5     **/
	X86_PMULHW_MM,					/**  opcode:  0F E5     **/
	X86_PMULLW_MM,					/**  opcode:  0F D5     **/
	X86_POR_MM,						/**  opcode:  0F EB     **/
	X86_PSLLW_MM,					/**  opcode:  0F F1     **/
	X86_PSLLW_I_MM,					/**  opcode:  0F 71 /6  **/
	X86_PSLLD_MM,					/**  opcode:  0F F2     **/
	X86_PSLLD_I_MM,					/**  opcode:  0F 72 /6  **/
	X86_PSLLQ_MM,					/**  opcode:  0F F3     **/
	X86_PSLLQ_I_MM,					/**  opcode:  0F 73 /6  **/
	X86_PSRAW_MM,					/**  opcode:  0F E1     **/
	X86_PSRAW_I_MM,					/**  opcode:  0F 71 /4  **/
	X86_PSRAD_MM,					/**  opcode:  0F E2     **/
	X86_PSRAD_I_MM,					/**  opcode:  0F 72 /4  **/
	X86_PSRLW__MM,					/**  opcode:  0F D1     **/
	X86_PSRLW_I_MM,					/**  opcode:  0F 71 /2  **/
	X86_PSRLD_MM,					/**  opcode:  0F D2     **/
	X86_PSRLD_I_MM,					/**  opcode:  0F 72 /2  **/
	X86_PSRLQ_MM,					/**  opcode:  0F D3     **/
	X86_PSRLQ_I_MM,					/**  opcode:  0F 73 /2  **/
	X86_PSUBB_MM,					/**  opcode:  0F F8     **/
	X86_PSUBW_MM,					/**  opcode:  0F F9     **/
	X86_PSUBD_MM,					/**  opcode:  0F FA     **/
	X86_PSUBSB_MM,					/**  opcode:  0F E8     **/
	X86_PSUBSW_MM,					/**  opcode:  0F E9     **/
	X86_PSUBUSB_MM,					/**  opcode:  0F D8     **/
	X86_PSUBUSW_MM,					/**  opcode:  0F D9     **/
	X86_PUNPCKLBW_MM,				/**  opcode:  0F 60     **/
	X86_PUNPCKLWD_MM,				/**  opcode:  0F 61     **/
	X86_PUNPCKLDQ_MM,				/**  opcode:  0F 62     **/
	X86_PUNPCKHBW_MM,				/**  opcode:  0F 68     **/
	X86_PUNPCKHWD_MM,				/**  opcode:  0F 69     **/
	X86_PUNPCKHDQ_MM,				/**  opcode:  0F 6A     **/
	X86_PXOR_MM,					/**  opcode:  0F EF     **/
	X86_FXRSTOR_VXF,				/**  opcode:  0F AE /1  **/
	X86_FXSAVE_VXF,					/**  opcode:  0F AE /0  **/
	X86_LDMXCSR_VXF,				/**  opcode:  0F AE /2  **/
	X86_STMXCSR_VXF,				/**  opcode:  0F AE /3  **/
	X86_SFENCE_VXF,					/**  opcode:  0F AE /7  **/
	X86_MOVLPS_VXF,					/**  opcode:  0F 12     **/
	X86_MOVLPS_R_VXF,				/**  opcode:  0F 13     **/
	X86_MOVHPS_VXF,					/**  opcode:  0F 16     **/
	X86_MOVHPS_R_VXF,				/**  opcode:  0F 17     **/
	X86_MOVAPS_VXF,					/**  opcode:  0F 28     **/
	X86_MOVAPS_R_VXF,				/**  opcode:  0F 29     **/
	X86_MOVUPS_VXF,					/**  opcode:  0F 10     **/
	X86_MOVUPS_R_VXF,				/**  opcode:  0F 11     **/
	X86_MOVSS_VXF,					/**  opcode:  0F 10     **/
	X86_MOVSS_R_VXF,				/**  opcode:  0F 11     **/
	X86_MOVMSKPS_VXF,				/**  opcode:  0F 50     **/
	X86_ADDPS_VXF,					/**  opcode:  0F 58     **/
	X86_ADDSS_VXF,					/**  opcode:  0F 58     **/
	X86_ANDPS_VXF,					/**  opcode:  0F 54     **/
	X86_ANDNPS_VXF,					/**  opcode:  0F 55     **/
	X86_COMISS_VXF,					/**  opcode:  0F 2F     **/
	X86_DIVPS_VXF,					/**  opcode:  0F 5E     **/
	X86_DIVSS_VXF,					/**  opcode:  0F 5E     **/
	X86_CVTTPS2PI_VXF,				/**  opcode:  0F 2C     **/
	X86_CVTTSS2SI_VXF,				/**  opcode:  0F 2C     **/
	X86_CVTPI2PS_VXF,				/**  opcode:  0F 2A     **/
	X86_CVTSI2SS_VXF,				/**  opcode:  0F 2A     **/
	X86_MAXPS_VXF,					/**  opcode:  0F 5F     **/
	X86_MAXSS_VXF,					/**  opcode:  0F 5F     **/
	X86_MINPS_VXF,					/**  opcode:  0F 5D     **/
	X86_MINSS_VXF,					/**  opcode:  0F 5D     **/
	X86_MULPS_VXF,					/**  opcode:  0F 59     **/
	X86_MULSS_VXF,					/**  opcode:  0F 59     **/
	X86_CVTPS2PI_VXF,				/**  opcode:  0F 2D     **/
	X86_CVTSS2SI_VXF,				/**  opcode:  0F 2D     **/
	X86_ORPS_VXF,					/**  opcode:  0F 56     **/
	X86_RCPPS_VXF,					/**  opcode:  0F 53     **/
	X86_RCPSS_VXF,					/**  opcode:  0F 53     **/
	X86_RSQRTPS_VXF,				/**  opcode:  0F 52     **/
	X86_RSQRTSS_VXF,				/**  opcode:  0F 52     **/
	X86_SHUFPS_VXF,					/**  opcode:  0F C6     **/
	X86_CMPPS_VXF,					/**  opcode:  0F C2     **/
	X86_CMPSS_VXF,					/**  opcode:  0F C2     **/
	X86_SQRTPS_VXF,					/**  opcode:  0F 51     **/
	X86_SQRTSS_VXF,					/**  opcode:  0F 51     **/
	X86_SUBPS_VXF,					/**  opcode:  0F 5C     **/
	X86_SUBSS_VXF,					/**  opcode:  0F 5C     **/
	X86_UCOMISS_VXF,				/**  opcode:  0F 2E     **/
	X86_UNPCKHPS_VXF,				/**  opcode:  0F 15     **/
	X86_UNPCKLPS_VXF,				/**  opcode:  0F 14     **/
	X86_XORPS_VXF,					/**  opcode:  0F 57     **/
	X86_MASKMOVQ_MME,				/**  opcode:  0F F7     **/
	X86_MOVNTQ_MME,					/**  opcode:  0F E7     **/
	X86_MOVNTPS_MME,				/**  opcode:  0F 2B     **/
	X86_PAVGW_MME,					/**  opcode:  0F E3     **/
	X86_PAVGB_MME,					/**  opcode:  0F E0     **/
	X86_PEXTRW_MME,					/**  opcode:  0F C5     **/
	X86_PINSRW_MME,					/**  opcode:  0F C4     **/
	X86_PMAXSW_MME,					/**  opcode:  0F EE     **/
	X86_PMAXUB_MME,					/**  opcode:  0F DE     **/
	X86_PMINSW_MME,					/**  opcode:  0F EA     **/
	X86_PMINUB_MME,					/**  opcode:  0F DA     **/
	X86_PMOVMSKB_MME,				/**  opcode:  0F D7     **/
	X86_PMULHUW_MME,				/**  opcode:  0F E4     **/
	X86_PSADBW_MME,					/**  opcode:  0F F6     **/
	X86_PSHUFW_MME,					/**  opcode:  0F 70     **/
	X86_PREFETCHT0_MME,				/**  opcode:  0F 18 /1  **/
	X86_PREFETCHT1_MME,				/**  opcode:  0F 18 /2  **/
	X86_PREFETCHT2_MME,				/**  opcode:  0F 18 /3  **/
	X86_PREFETCHNTA_MME,			/**  opcode:  0F 18 /0  **/
	X86_LAST_INST,
	IA_DECODER_LAST_INST = X86_LAST_INST
} IA_Decoder_Inst_Id;

#ifndef _MSC_VER
	#ifndef _UNICODE
	#define _TCHAR char
	#else
	#include <tchar.h>
	#endif
#else
	#include <tchar.h>
#endif

#include <iel.h>

typedef enum
{
    IA_DECODER_NO_ERROR = 0,
    IA_DECODER_RESERVED_OPCODE,
    IA_DECODER_INVALID_PRM_OPCODE,
    IA_DECODER_TOO_LONG_OPCODE,
    IA_DECODER_LOCK_ERR,
    IA_DECODER_OPERAND_ERR = 5,
    IA_DECODER_TOO_SHORT_ERR,
    IA_DECODER_ASSOCIATE_MISS,
    IA_DECODER_FIRST_FATAL_ERROR = 9,
    IA_DECODER_INVALID_INST_ID,
    IA_DECODER_INVALID_CLIENT_ID,
    IA_DECODER_INVALID_MACHINE_MODE,
    IA_DECODER_INVALID_MACHINE_TYPE,
    IA_DECODER_NULL_PTR,
    IA_DECODER_INTERNAL_ERROR,
    IA_DECODER_LAST_ERROR
} IA_Decoder_Err;

typedef enum
{ 
    IA_DECODER_CPU_NO_CHANGE=0,
    IA_DECODER_CPU_DEFAULT,
    IA_DECODER_CPU_PENTIUM = 2,
    IA_DECODER_CPU_P6 = 3,
    IA_DECODER_CPU_P5MM = 5,
    IA_DECODER_CPU_P6MM = 6,
    IA_DECODER_CPU_P6_KATNI=7,
    IA_DECODER_CPU_P7=8,
    IA_DECODER_CPU_LAST=99
} IA_Decoder_Machine_Type;

typedef enum
{
    IA_DECODER_MODE_NO_CHANGE = 0,
    IA_DECODER_MODE_DEFAULT,
    IA_DECODER_MODE_86 = 2,
    IA_DECODER_MODE_V86,
    IA_DECODER_MODE_PROTECTED_16,
    IA_DECODER_MODE_PROTECTED_32,
    IA_DECODER_MODE_BIG_REAL,
    IA_DECODER_MODE_LAST = 8
} IA_Decoder_Machine_Mode;

typedef enum
{
    IA_DECODER_NO_OPER = 0,
    IA_DECODER_REGISTER,
    IA_DECODER_MEMORY,
    IA_DECODER_IMMEDIATE,
    IA_DECODER_IP_RELATIVE,
    IA_DECODER_SEG_OFFSET,
    IA_DECODER_PORT,
    IA_DECODER_PORT_IN_DX,
    IA_DECODER_CONST,
    IA_DECODER_OPERAND_LAST
} IA_Decoder_Operand_Type;

typedef enum
{
    IA_DECODER_NO_REG_TYPE = 0,
    IA_DECODER_INT_REG = 1, 
    IA_DECODER_SEG_REG,
    IA_DECODER_FP_REG,
    IA_DECODER_DEBUG_REG = 4,
    IA_DECODER_CTRL_REG,
    IA_DECODER_TASK_REG,
    IA_DECODER_KER_REG ,
    IA_DECODER_PROC_REG,
    IA_DECODER_SYS_REG,
    IA_DECODER_MM_REG = 15,
    IA_DECODER_XMM_REG,
    IA_DECODER_REG_TYPE_LAST=18
} IA_Decoder_Reg_Type;

typedef enum
{
    IA_DECODER_NO_REG=0,
    IA_DECODER_REG_EAX = 1,
    IA_DECODER_REG_ECX,
    IA_DECODER_REG_EDX,
    IA_DECODER_REG_EBX,
    IA_DECODER_REG_ESP,
    IA_DECODER_REG_EBP,
    IA_DECODER_REG_ESI,
    IA_DECODER_REG_EDI,
    IA_DECODER_REG_ES,
    IA_DECODER_REG_CS,
    IA_DECODER_REG_SS,
    IA_DECODER_REG_DS,
    IA_DECODER_REG_FS,
    IA_DECODER_REG_GS,
    IA_DECODER_REG_EFLAGS,
    IA_DECODER_REG_DR0,
    IA_DECODER_REG_DR1,
    IA_DECODER_REG_DR2,
    IA_DECODER_REG_DR3,
    IA_DECODER_REG_DR4,
    IA_DECODER_REG_DR5,
    IA_DECODER_REG_DR6,
    IA_DECODER_REG_DR7,
    IA_DECODER_REG_CR0,
    IA_DECODER_REG_CR1,
    IA_DECODER_REG_CR2,
    IA_DECODER_REG_CR3,
    IA_DECODER_REG_CR4,
    IA_DECODER_REG_TSSR,
    IA_DECODER_REG_LDTR,
    IA_DECODER_REG_ESR_BASE,
    IA_DECODER_REG_ESR_LIMIT,
    IA_DECODER_REG_CSR_BASE,
    IA_DECODER_REG_CSR_LIMIT,
    IA_DECODER_REG_SSR_BASE,
    IA_DECODER_REG_SSR_LIMIT,
    IA_DECODER_REG_DSR_BASE,
    IA_DECODER_REG_DSR_LIMIT,
    IA_DECODER_REG_FSR_BASE,
    IA_DECODER_REG_FSR_LIMIT,
    IA_DECODER_REG_GSR_BASE,
    IA_DECODER_REG_GSR_LIMIT,
    IA_DECODER_REG_TSSR_BASE,
    IA_DECODER_REG_TSSR_LIMIT,
    IA_DECODER_REG_LDTR_BASE,
    IA_DECODER_REG_LDTR_LIMIT,
    IA_DECODER_REG_GDTR_BASE,
    IA_DECODER_REG_GDTR_LIMIT,
    IA_DECODER_REG_IDTR_BASE,
    IA_DECODER_REG_IDTR_LIMIT,
    IA_DECODER_REG_TR,
    IA_DECODER_REG_TR3,
    IA_DECODER_REG_TR4,
    IA_DECODER_REG_TR5,
    IA_DECODER_REG_TR6,
    IA_DECODER_REG_TR7,
    IA_DECODER_REG_AX,
    IA_DECODER_REG_CX,
    IA_DECODER_REG_DX,
    IA_DECODER_REG_BX,
    IA_DECODER_REG_SP,
    IA_DECODER_REG_BP,
    IA_DECODER_REG_SI,
    IA_DECODER_REG_DI,
    IA_DECODER_REG_AL,
    IA_DECODER_REG_CL,
    IA_DECODER_REG_DL,
    IA_DECODER_REG_BL,
    IA_DECODER_REG_AH,
    IA_DECODER_REG_CH,
    IA_DECODER_REG_DH,
    IA_DECODER_REG_BH,
    IA_DECODER_REG_ST0,
    IA_DECODER_REG_ST1,
    IA_DECODER_REG_ST2,
    IA_DECODER_REG_ST3,
    IA_DECODER_REG_ST4,
    IA_DECODER_REG_ST5,
    IA_DECODER_REG_ST6,
    IA_DECODER_REG_ST7,
    IA_DECODER_REG_MM0 = 81,
    IA_DECODER_REG_MM1,
    IA_DECODER_REG_MM2,
    IA_DECODER_REG_MM3,
    IA_DECODER_REG_MM4,
    IA_DECODER_REG_MM5,
    IA_DECODER_REG_MM6,
    IA_DECODER_REG_MM7,
    IA_DECODER_REG_XMM0 = 89,
    IA_DECODER_REG_XMM1,
    IA_DECODER_REG_XMM2,
    IA_DECODER_REG_XMM3,
    IA_DECODER_REG_XMM4,
    IA_DECODER_REG_XMM5,
    IA_DECODER_REG_XMM6,
    IA_DECODER_REG_XMM7,
    IA_DECODER_REG_MXCSR,
    IA_DECODER_REG_FPCW = 107,
    IA_DECODER_REG_FPSW,
    IA_DECODER_REG_FPTAG,
    IA_DECODER_REG_FPIP_OFF,
    IA_DECODER_REG_FPIP_SEL,
    IA_DECODER_REG_FPOPCODE,
    IA_DECODER_REG_FPDP_OFF,
    IA_DECODER_REG_FPDP_SEL,
    IA_DECODER_REG_EIP,
    IA_DECODER_REG_LAST,
    IA_DECODER_FPST_ALL,
    IA_DECODER_IREG32_ALL,
    IA_DECODER_IREG16_ALL,
    IA_DECODER_MEM_REF,
    IA_DECODER_MEM8,
    IA_DECODER_MEM16,
    IA_DECODER_MEM32,
    IA_DECODER_MEM64,
    IA_DECODER_MEM80,
	IA_DECODER_MEM128
} IA_Decoder_Operand_Name;

/* For vtune source release, fixing static arrays */

#define IA_DECODER_REG_MXT IA_DECODER_NO_REG

typedef enum
{
    IA_DECODER_OPER_NO_SIZE =  0,
    IA_DECODER_OPER_SIZE_1 =   1,
    IA_DECODER_OPER_SIZE_2 =   2,
    IA_DECODER_OPER_SIZE_4 =   4,
    IA_DECODER_OPER_SIZE_8 =   8,
    IA_DECODER_OPER_SIZE_10 = 10,
    IA_DECODER_OPER_SIZE_16 = 16,
    IA_DECODER_OPER_SIZE_20 = 20,
    IA_DECODER_OPER_SIZE_22 = 22,
    IA_DECODER_OPER_SIZE_24 = 24,  
    IA_DECODER_OPER_SIZE_32 = 32,
    IA_DECODER_OPER_SIZE_64 = 64,
    IA_DECODER_OPER_SIZE_80 = 80,
    IA_DECODER_OPER_SIZE_94 = 94,
    IA_DECODER_OPER_SIZE_108 = 108,
    IA_DECODER_OPER_SIZE_128 = 128,
    IA_DECODER_OPER_SIZE_512 = 512
} IA_Decoder_Oper_Size;

typedef IA_Decoder_Operand_Name IA_Decoder_Reg_Name;

typedef struct
{
    int                  valid;
    IA_Decoder_Reg_Type  type;
    IA_Decoder_Reg_Name  name;
    long                 value;
} IA_Decoder_Reg_Info;

typedef enum
{
    IA_DECODER_REP_NONE = 0,
    IA_DECODER_REPE,
    IA_DECODER_REPNE
} IA_Decoder_Rep_Type;

typedef enum
{
    IA_DECODER_OPER_2ND_ROLE_NONE = 0,
    IA_DECODER_OPER_2ND_ROLE_SRC,
    IA_DECODER_OPER_2ND_ROLE_DST
} IA_Decoder_Operand_2nd_Role;


typedef enum
{
    IA_DECODER_OO_NO_OPRNDS             = 0,    /*    No operands         */
    IA_DECODER_OO_1SRC1                 = 1,    /*    1st - src1          */
    IA_DECODER_OO_1DST                  = 2,    /*    1st - dst           */
    IA_DECODER_OO_1DST_SRC              = 3,    /*    1st - src1 & dst    */
    IA_DECODER_OO_1SRC1_2SRC2           = 4,    /*    1st - src1          */
                                                /*    2nd - src2          */
    IA_DECODER_OO_1DST_2SRC1            = 5,    /*    1st - dest          */
                                                /*    2nd - src1          */
    IA_DECODER_OO_1DST_SRC1_2SRC2       = 6,    /*    1st - src1 & dst    */
                                                /*    2nd - src2          */
    IA_DECODER_OO_1DST_2SRC1_3SRC2      = 7,    /*    1st - dest          */
                                                /*    2nd - src1          */
                                                /*    3rd - src2          */
    IA_DECODER_OO_1DST_SRC1_2SRC2_3SRC3 = 8,    /*    1st - src1 & dst    */
                                                /*    2nd - src2          */
                                                /*    3rd - src3          */
    IA_DECODER_OO_1DST1_SRC1_2DST2_SRC2 = 9     /*    1st - dst1 &src1    */
} IA_Decoder_Opers_Order;

typedef struct
{
    long            signed_imm;
    unsigned int    size;
    unsigned long   value;
    U64             val64;
} IA_Decoder_Imm_Info;

typedef struct
{
    IA_Decoder_Reg_Info      mem_seg;
    long                     mem_offset;
    IA_Decoder_Reg_Info      mem_base;
    IA_Decoder_Reg_Info      mem_index;
    unsigned long            mem_scale;
    IA_Decoder_Oper_Size     size;
    IA_Decoder_Imm_Info      mem_off;          /*** offset == displacement   ***/
} IA_Decoder_Mem_Info;

typedef struct
{
    unsigned long         offset;
    unsigned long         segment_number;
} IA_Decoder_Seg_Offset_Info;

typedef struct
{
    IA_Decoder_Operand_Type     type;
    IA_Decoder_Operand_2nd_Role oper_2nd_role;
    IA_Decoder_Reg_Info         reg_info;
    IA_Decoder_Mem_Info         mem_info;
    IA_Decoder_Imm_Info         imm_info;
    long                        ip_relative_offset;
    IA_Decoder_Seg_Offset_Info  seg_offset_info;
    unsigned long               port_number;   
} IA_Decoder_Operand_Info;

typedef struct
{
    IA_Decoder_Rep_Type    repeat_type;
    unsigned char          n_prefixes;
    unsigned char          n_rep_pref;
    unsigned char          n_lock_pref;
    unsigned char          n_seg_pref;
    unsigned char          n_oper_size_pref;
    unsigned char          n_addr_size_pref;
    IA_Decoder_Reg_Name    segment_register;
} IA_Decoder_Prefix_Info;

typedef enum
{
    IA_DECODER_OPCODE_TYPE_NONE=0,
    IA_DECODER_OPCODE_TYPE_OP1,
    IA_DECODER_OPCODE_TYPE_OP1_XOP,
    IA_DECODER_OPCODE_TYPE_OP1_OP2,
    IA_DECODER_OPCODE_TYPE_OP1_OP2_XOP
} IA_Decoder_Opcode_Type;

typedef enum
{
    IA_DECODER_ADDR_NO_SIZE =  0,
    IA_DECODER_ADDR_SIZE_16 = 16,
    IA_DECODER_ADDR_SIZE_32 = 32
} IA_Decoder_Addr_Size;

typedef enum
{
  IA_DECODER_MODRM_NONE = 0,
  IA_DECODER_MODRM_REG,
  IA_DECODER_MODRM_XOP,
  IA_DECODER_MODRM_OP2,
  IA_DECODER_MODRM_OP1
} IA_Decoder_ModRM_Type;

typedef struct
{
    unsigned char         modrm;
    unsigned char         sib;
    IA_Decoder_ModRM_Type mrm_type;
    int                   mrm_opr_size;
}IA_Decoder_ModRM_Info;

typedef struct
{
    int n_imp_srcs;
    int n_imp_dests;
    IA_Decoder_Operand_Name *imp_srcs;
    IA_Decoder_Operand_Name *imp_dests;
}IA_Inst_Imp_Info_t;

typedef struct
{
    IA_Decoder_Inst_Id       inst;
    IA_Decoder_Oper_Size     operand_size;
    IA_Decoder_Oper_Size     implicit_oper_size;
    IA_Decoder_Addr_Size     address_size;
    IA_Decoder_Operand_Info  src1;
    IA_Decoder_Operand_Info  src2;
    IA_Decoder_Operand_Info  dst1; 
    void *                   client_info;
    unsigned long            flags;
    unsigned long            ext_flags;
    unsigned long            fp_opcode;
    IA_Decoder_Opcode_Type   opcode_type;
    IA_Decoder_Prefix_Info   prefix_info;
    unsigned char            size;
    IA_Decoder_ModRM_Info    mrm_info;
    const IA_Inst_Imp_Info_t *     imp_info;
} IA_Decoder_Info;

typedef struct
{
    void *                client_info;
    unsigned long         flags;
} IA_Decoder_Inst_Static_Info;

typedef int IA_Decoder_Id;

IA_Decoder_Id  ia_decoder_open(void);

IA_Decoder_Err ia_decoder_associate_one(const IA_Decoder_Id,
                                        const IA_Decoder_Inst_Id,
                                        const void *);

IA_Decoder_Err ia_decoder_associate_check(const IA_Decoder_Id,
                                          IA_Decoder_Inst_Id *);

IA_Decoder_Err ia_decoder_setenv(const IA_Decoder_Id,
                                 const IA_Decoder_Machine_Type,
                                 const IA_Decoder_Machine_Mode);

IA_Decoder_Err ia_decoder_close(const IA_Decoder_Id);

IA_Decoder_Err ia_decoder_decode(const IA_Decoder_Id,
                                 const unsigned char *,
                                 int,
                                 IA_Decoder_Info *);

IA_Decoder_Err ia_decoder_inst_static_info(const IA_Decoder_Id,
                                           const IA_Decoder_Inst_Id,
                                           IA_Decoder_Inst_Static_Info *);

IA_Decoder_Oper_Size ia_decoder_operand_size(IA_Decoder_Operand_Name name);

const _TCHAR *ia_decoder_ver_str(void);
IA_Decoder_Err  ia_decoder_ver(long *major, long *minor);

#include <EM_tools.h>
 
void  ia_decoder_get_version(EM_library_version_t  * dec_version);
/**************      IA Instruction Flags Related Macros        *************/

#define IA_DECODER_BIT_8086          0x00000001 /* 0 - inst valid in 8086 */
                                                /*     mode               */
                                                 
#define IA_DECODER_BIT_V86           0x00000002 /* 1 - inst valid in  V86 */
                                                /*     mode               */
                                                 
#define IA_DECODER_BIT_P5            0x00000004 /* 2 - inst valid on P5   */
#define IA_DECODER_BIT_P6            0x00000008 /* 3 - inst valid on P6   */
#define IA_DECODER_BIT_P7            0x00000010 /* 4 - inst valid on P7   */
                                                 
#define IA_DECODER_BIT_P5MM          0x40000000 /* 30 - valid on SIMD P5MM */
#define IA_DECODER_BIT_P6MM          0x80000000 /* 31 - valid on SIMD P6MM */
#define IA_DECODER_BIT_PRIVILEGE     0x00000060 /* 5-6 - inst privilige   */
                                                /*       level            */
                                                 
#define IA_DECODER_BIT_PRIV_POSITION 5       
#define IA_DECODER_BIT_LOCK          0x00000080 /* 7 - inst can use lock  */
                                                /*     prefix             */
                                                 
#define IA_DECODER_BIT_OPER_ERR      0x00000100 /* 8 - inst invalid with  */
                                                /*     certain operands   */
                                                 
#define IA_DECODER_BIT_IMPLIED_OPR   0x00000200 /* 9 - inst has implied   */
                                                /*     operands           */
                                                 
#define IA_DECODER_BIT_TYPE          0x00003C00 /* 10-13 - inst type      */
#define IA_DECODER_BIT_TYPE_FLOAT    0x00000400 /* 10 - FP type           */
#define IA_DECODER_BIT_TYPE_ALU      0x00000800 /* 11 - ALU type          */
#define IA_DECODER_BIT_TYPE_COND_JMP 0x00001000 /* 12 - conditional jump  */
#define IA_DECODER_BIT_TYPE_JMP      0x00002000 /* 13 - jump type         */
#define IA_DECODER_BIT_TYPE_MM       0x00000000 /*      SIMD-MM type      */

/* ext_flags bits */
#define IA_DECODER_BIT_COND_MOVE     0x1
#define IA_DECODER_BIT_VX_INT        0x4
#define IA_DECODER_BIT_VX_FP         0x8

/* Flags 39-40 (extended flags 8-9) - How the instruction affect the FP TOS */
#define IA_DECODER_BITS_FP_STACK_MANIPULATION	0x00000180	
#define IA_DECODER_BITS_FP_STACK_POP			0x00000180
#define IA_DECODER_BITS_FP_STACK_POP_TWICE		0x00000100
#define IA_DECODER_BITS_FP_STACK_PUSH			0x00000080

/* Flags 42-44 (extended 11-13) Bistro information */
#define IA_DECODER_BITS_SCOPE_INST	0x00000400
#define IA_DECODER_BITS_SCOPE_BITS	0x00001800
#define IA_DECODER_BITS_CONTINUE_SEQUENTIAL_IN_SCOPE	0x00000000
#define IA_DECODER_BITS_TERMINATE_SEQUENTIAL_IN_SCOPE	0x00001000
#define IA_DECODER_BITS_TERMINATE_SEQUENTIAL_EXIT_SCOPE	0x00000800
#define IA_DECODER_BITS_CONDITIONAL_SEQUENTIAL_ENTER_SCOPE	0x00001800

/* Flag 45 (extended 14) problematic implicit operands */
#define IA_DECODER_BITS_PREFIX_IMPLICIT 0x00002000



#define IA_DECODER_BIT_TYPE_SYS      0x00000C00 
#define IA_DECODER_BIT_INSTRUCTION   0x00004000 /* 14 - instruction (not  */
                                                /*      an alias)         */
                                                 
#define IA_DECODER_BIT_W_NEED_PREFIX 0x00008000 /* 15 - 16-bit needs      */
                                                /*      size-prefix in 32 */
                                                 
#define IA_DECODER_BIT_L_NEED_PREFIX 0x00010000 /* 16 - 32-bit needs      */
                                                /*      size-prefix in 16 */
                                                 
#define IA_DECODER_BIT_STOP_TRANS    0x00020000 /* 17 - stop translation  */
#define IA_DECODER_BIT_STRING_OP     0x00040000 /* 18 - string inst       */
#define IA_DECODER_BIT_READ          0x00080000 /* 19 - explicit memory   */
                                                /*      operand read      */
                                                 
#define IA_DECODER_BIT_WRITE         0x00100000 /* 20 - explicit memory   */
                                                /*      operand write     */
                                                 
#define IA_DECODER_BIT_IMP_MEM_READ  0x00200000 /* 21 - implicit memory   */
                                                /*      operand read      */
                                                 
#define IA_DECODER_BIT_IMP_MEM_WRITE 0x00400000 /* 22 - implicit memory   */
                                                /*      operand write     */

#define IA_DECODER_BIT_IAS_VALID     0x00800000 /* 23 - in SVR4 IAS valid */
                                                /*      instrcuion        */

#define IA_DECODER_BIT_OPRNDS_ORDER  0x0f000000 /* 24-27 operands order   */
#define IA_DECODER_POS_OPRNDS_ORDER          24

#define IA_DECODER_BIT_OPRNDS_PRINT_RVRS \
                                      0x20000000 /* 29   Operands print    */
                                                 /*      order is reverse  */
                                                 /*      from database     */



#define IA_DECODER_VALID_86(di)         ((di)->flags & IA_DECODER_BIT_8086)
#define IA_DECODER_VALID_V86(di)        ((di)->flags & IA_DECODER_BIT_V86)
#define IA_DECODER_VALID_PENTIUM(di)    ((di)->flags & IA_DECODER_BIT_P5)
#define IA_DECODER_VALID_P6(di)         ((di)->flags & IA_DECODER_BIT_P6)
#define IA_DECODER_VALID_P7(di)         ((di)->flags & IA_DECODER_BIT_P7)
#define IA_DECODER_VALID_P5MM(di)       ((di)->flags & IA_DECODER_BIT_P5MM)
#define IA_DECODER_VALID_P6MM(di)       ((di)->flags & IA_DECODER_BIT_P6MM)
#define IA_DECODER_LOCK_IS_VALID(di)    ((di)->flags & IA_DECODER_BIT_LOCK)

#define IA_DECODER_PRIV(di)                                                \
            ((di)->flags & IA_DECODER_BIT_PRIVILEGE) >>                    \
                                            IA_DECODER_BIT_PRIV_POSITION)

#define IA_DECODER_OPER_CAUSE_ERR(di)                                      \
            ((di)->flags & IA_DECODER_BIT_OPER_ERR)

#define IA_DECODER_IMPLIED_OPER(di)                                        \
            ((di)->flags & IA_DECODER_BIT_IMPLIED_OPR)

#define IA_DECODER_JMP(di)                                                 \
            ((di)->flags & IA_DECODER_BIT_TYPE_JMP)

#define IA_DECODER_COND_JMP(di)                                            \
            ((di)->flags & IA_DECODER_BIT_TYPE_COND_JMP)

#define IA_DECODER_INST_TYPE(di)                                           \
            ((di)->flags &  IA_DECODER_BIT_TYPE)

#define IA_DECODER_ALU(di)                                                 \
            (IA_DECODER_INST_TYPE(di) == IA_DECODER_BIT_TYPE_ALU)

#define IA_DECODER_FLOAT(di)                                               \
            (IA_DECODER_INST_TYPE(di) == IA_DECODER_BIT_TYPE_FLOAT)

#define IA_DECODER_SYS(di)                                                 \
            (IA_DECODER_INST_TYPE(di) == IA_DECODER_BIT_TYPE_SYS)
#define IA_DECODER_MM(di)                                                  \
            (IA_DECODER_INST_TYPE(di) == IA_DECODER_BIT_TYPE_MM)

#define IA_DECODER_COND_MOVE(di)((di)->ext_flags & IA_DECODER_BIT_COND_MOVE)

/* is the instruction a KatNI instruction touching mm regs */
#define IA_DECODER_VX_INT(di)((di)->ext_flags & IA_DECODER_BIT_VX_INT)
/* is the instruction a KatNI instruction touching xmm regs */
#define IA_DECODER_VX_FP(di) ((di)->ext_flags & IA_DECODER_BIT_VX_FP)
/* is the instruction a KatNI instruction touching xmm or mm regs */
#define IA_DECODER_VX(di) (IA_DECODER_VX_INT(di) || IA_DECODER_VX_FP(di))

#define IA_DECODER_TRANS_STOPPER(di)                                       \
            ((di)->flags & IA_DECODER_BIT_STOP_TRANS)

#define IA_DECODER_LONG(di)                                                \
            ((di)->flags & IA_DECODER_BIT_L_NEED_PREFIX)

#define IA_DECODER_WORD(di)                                                \
            ((di)->flags & IA_DECODER_BIT_W_NEED_PREFIX)

#define IA_DECODER_STRING_OP(di)                                           \
            ((di)->flags & IA_DECODER_BIT_STRING_OP)

#define IA_DECODER_MEM_READ(di)                                            \
            ((di)->flags & IA_DECODER_BIT_READ)

#define IA_DECODER_MEM_WRITE(di)                                           \
            ((di)->flags & IA_DECODER_BIT_WRITE)

#define IA_DECODER_IMP_MEM_READ(di)                                        \
            ((di)->flags & IA_DECODER_BIT_IMP_MEM_READ)

#define IA_DECODER_IMP_MEM_WRITE(di)                                       \
            ((di)->flags & IA_DECODER_BIT_IMP_MEM_WRITE)

#define IA_DECODER_OPERANDS_ORDER(di)                                      \
 (((di)->flags & IA_DECODER_BIT_OPRNDS_ORDER) >> IA_DECODER_POS_OPRNDS_ORDER)


/* macros for fp instructions which pop/push the register stack */
/****************************************************************/

/* does the instruction manipulate the tos? */
#define IA_DECODER_FP_STACK_MANIPULATED(di)	\
	(0!=((di)->ext_flags & IA_DECODER_BITS_FP_STACK_MANIPULATION))

/* does the instruction execute push? */
#define IA_DECODER_FP_STACK_PUSH(di)		\
	(IA_DECODER_BITS_FP_STACK_PUSH==((di)->ext_flags & IA_DECODER_BITS_FP_STACK_MANIPULATION))

/* does the instruction execute pop? */
#define IA_DECODER_FP_STACK_POP(di)			\
	(IA_DECODER_BITS_FP_STACK_POP==((di)->ext_flags & IA_DECODER_BITS_FP_STACK_MANIPULATION))

/* does the instruction execute pop twice? */
#define IA_DECODER_FP_STACK_POP_TWICE(di)	\
	(IA_DECODER_BITS_FP_STACK_POP_TWICE==((di)->ext_flags & IA_DECODER_BITS_FP_STACK_MANIPULATION))

/* adjust a a register to point to it's value before the instruction executed push */
/* since the stack decrement occures before the execution, no adjustment needed */
#define IA_DECODER_FP_REG_AFTER_PUSH(fpreg) \
	(fpreg)

/* adjust a a register to point to it's value before the instruction executed pop */
#define IA_DECODER_FP_REG_AFTER_POP(fpreg) \
((fpreg)==IA_DECODER_REG_ST0 ? IA_DECODER_REG_ST7 : fpreg-1 )

/* adjust a a register to point to it's value before the instruction executed pop twice */
#define IA_DECODER_FP_REG_AFTER_POP_TWICE(fpreg) \
	(IA_DECODER_FP_REG_AFTER_POP(IA_DECODER_FP_REG_AFTER_POP(fpreg)))

/* adjust a a register to point to it's value before the instruction changed TOS */
#define IA_DECODER_ADJUST_FP_REG(di,fpreg) \
	(IA_DECODER_FP_STACK_MANIPULATED(di) ?													\
		(IA_DECODER_FP_STACK_POP(di)) ? IA_DECODER_FP_REG_AFTER_POP(fpreg) :				\
		(IA_DECODER_FP_STACK_POP_TWICE(di)) ? IA_DECODER_FP_REG_AFTER_POP_TWICE(fpreg) :	\
		(IA_DECODER_FP_STACK_PUSH(di)) ? IA_DECODER_FP_REG_AFTER_PUSH(fpreg) : (IA_DECODER_NO_REG) \
		: (fpreg))
		
/* Is the instruction connected to scope (info for bistro) */
#define IA_DECODER_IS_SCOPE_INST(di)	\
	(IA_DECODER_BITS_SCOPE_INST==((di)->ext_flags & IA_DECODER_BITS_SCOPE_INST))
/* Get scope type information */
#define IA_DECODER_SCOPE_TYPE(di)	\
	((di)->ext_flags & IA_DECODER_BITS_SCOPE_BITS)

/* Does the instruction have implicit operands affected by prefixes */
#define IA_DECODER_PREFIX_IMPLICIT(di)  \
	((di)->ext_flags & IA_DECODER_BITS_PREFIX_IMPLICIT)

typedef enum
{
     no_hint,
     weakly_not_taken,
     strongly_not_taken,
     weakly_taken,
     strongly_taken,
     trace_break /* statically not very predictable */
} branch_hint;

/* this macro will convert the segment prefix to 
   a branch hint enum from enum branch hints.
   di must be a decoder_info structure pointer,
   and hint must be a branch_hint enumerated variable
*/

#define IA_DECODER_PREFIX_HINT(di,_hint) \
	 {													\
		switch ((di)->prefix_info.segment_register)		\
		{												\
		case IA_DECODER_REG_CS:							\
				_hint = weakly_taken;					\
				break;									\
		case IA_DECODER_REG_SS:							\
				_hint = weakly_not_taken;				\
				break;									\
		case IA_DECODER_REG_DS:							\
				_hint = strongly_taken;					\
				break;									\
		case IA_DECODER_REG_ES:							\
				_hint = strongly_not_taken;				\
				break;									\
		case IA_DECODER_REG_FS:							\
				_hint = trace_break;					\
				break;									\
		default:										\
			_hint=no_hint;								\
			break;										\
		}												\
	 }


#endif /*** IA_DECODER_H ***/
/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright (c) 1992,1993,1994,1995,1996,1997,1998 Intel Corporation. ***/
/***                                                                       ***/

#ifndef EM_DECODER_H
#define EM_DECODER_H

#include "inst_ids.h"
#include "emdb_types.h"
#include "EM.h"

#define EM_DECODER_INST_NONE  EM_INST_NONE

typedef Inst_id_t  EM_Decoder_Inst_Id;

typedef unsigned char  EM_Decoder_imp_oper_t;

#include "EM_tools.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum em_cmp_rel_s
{
	EM_CMP_REL_NONE  = 0,
	EM_CMP_REL_GEU   = 1,
	EM_CMP_REL_LTU   = 2,
	EM_CMP_REL_EQ    = 3,
	EM_CMP_REL_NE    = 4,
	EM_CMP_REL_LT    = 5,
	EM_CMP_REL_GE    = 6,
	EM_CMP_REL_GT    = 7,
	EM_CMP_REL_LE    = 8,
    EM_CMP_REL_UNORD = 9,
	EM_CMP_REL_ORD   = 10,
	EM_CMP_REL_NEQ   = 11,
	EM_CMP_REL_NLT   = 12,
	EM_CMP_REL_NLE   = 13,
	EM_CMP_REL_LAST  = 14
} EM_cmp_rel_t;

typedef enum em_fp_precision_s
{
	EM_FP_PRECISION_NONE    = 0,
	EM_FP_PRECISION_SINGLE  = 1,
	EM_FP_PRECISION_DOUBLE  = 2,
	EM_FP_PRECISION_DYNAMIC = 3,
	EM_FP_PRECISION_LAST    = 4
} EM_fp_precision_t;	

typedef enum em_fp_status_s
{
	EM_FP_STATUS_NONE = 0,
	EM_FP_STATUS_S0   = 1,
	EM_FP_STATUS_S1   = 2,
	EM_FP_STATUS_S2   = 3,
	EM_FP_STATUS_S3   = 4,
	EM_FP_STATUS_LAST = 5
} EM_fp_status_t;

typedef enum EM_decoder_imp_operand
{
	EM_DECODER_IMP_OPERAND_NONE = 0,
	EM_DECODER_IMP_OPERAND_AR_LC,
	EM_DECODER_IMP_OPERAND_RR,
    EM_DECODER_IMP_OPERAND_AR_BSPSTORE,
    EM_DECODER_IMP_OPERAND_APP_REG_GRP_HIGH,
    EM_DECODER_IMP_OPERAND_DTR,
    EM_DECODER_IMP_OPERAND_AR_UNAT,
    EM_DECODER_IMP_OPERAND_CR_IIM,
    EM_DECODER_IMP_OPERAND_PSR,
    EM_DECODER_IMP_OPERAND_CFM,
    EM_DECODER_IMP_OPERAND_CR_IFS,
    EM_DECODER_IMP_OPERAND_CR_ISR,
    EM_DECODER_IMP_OPERAND_AR_BSP,
    EM_DECODER_IMP_OPERAND_AR_RSC,
    EM_DECODER_IMP_OPERAND_AR_EC,
    EM_DECODER_IMP_OPERAND_AR_PFS,
    EM_DECODER_IMP_OPERAND_FPSR,
    EM_DECODER_IMP_OPERAND_APP_CCV,
	EM_DECODER_IMP_OPERAND_PR63,
	EM_DECODER_IMP_OPERAND_DCR,
	EM_DECODER_IMP_OPERAND_CR_IIP,
	EM_DECODER_IMP_OPERAND_IPSR,
	EM_DECODER_IMP_OPERAND_CSD,
	EM_DECODER_IMP_OPERAND_IP,
	EM_DECODER_IMP_OPERAND_EIP,
	EM_DECODER_IMP_OPERAND_MSR_XIP,
	EM_DECODER_IMP_OPERAND_MSR_XPSR,
	EM_DECODER_IMP_OPERAND_MSR_XFS,
    EM_DECODER_IMP_OPERAND_AR_ITC,
    EM_DECODER_IMP_OPERAND_AR_RNAT,
    EM_DECODER_IMP_OPERAND_AR_RCS,
    EM_DECODER_IMP_OPERAND_CR_CMCV,
    EM_DECODER_IMP_OPERAND_CR_EOI,
    EM_DECODER_IMP_OPERAND_CR_GPTA,
    EM_DECODER_IMP_OPERAND_CR_IFA,
    EM_DECODER_IMP_OPERAND_CR_IHA,
    EM_DECODER_IMP_OPERAND_CR_IIPA,
    EM_DECODER_IMP_OPERAND_CR_ITIR,
    EM_DECODER_IMP_OPERAND_CR_ITM,
    EM_DECODER_IMP_OPERAND_CR_ITV,
    EM_DECODER_IMP_OPERAND_CR_IVA,
    EM_DECODER_IMP_OPERAND_CR_IVR,
    EM_DECODER_IMP_OPERAND_CR_LID,
    EM_DECODER_IMP_OPERAND_CR_PMV,
    EM_DECODER_IMP_OPERAND_CR_PTA,
    EM_DECODER_IMP_OPERAND_CR_TPR,
	EM_DECODER_IMP_OPERAND_LAST
} EM_Decoder_Imp_Operand;

typedef enum EM_decoder_err
{ 
    EM_DECODER_NO_ERROR = 0,
	EM_DECODER_INVALID_SLOT_BRANCH_INST,
    EM_DECODER_MUST_BE_GROUP_LAST,
	EM_DECODER_BASE_EQUAL_DEST,
	EM_DECODER_EQUAL_DESTS,
	EM_DECODER_ODD_EVEN_DESTS,
	EM_DECODER_WRITE_TO_ZERO_REGISTER,
	EM_DECODER_WRITE_TO_SPECIAL_FP_REGISTER,
	EM_DECODER_REGISTER_VALUE_OUT_OF_RANGE,
	EM_DECODER_REGISTER_RESERVED_VALUE,
	EM_DECODER_IMMEDIATE_VALUE_OUT_OF_RANGE,
	EM_DECODER_IMMEDIATE_INVALID_VALUE,
	EM_DECODER_STACK_FRAME_SIZE_OUT_OF_RANGE,
	EM_DECODER_LOCALS_SIZE_LARGER_STACK_FRAME,
	EM_DECODER_ROTATING_SIZE_LARGER_STACK_FRAME,
	EM_DECODER_HARD_CODED_PREDICATE_INVALID_VALUE,
	EM_DECODER_FIRST_FATAL_INST_ERROR,
    EM_DECODER_INVALID_PRM_OPCODE = EM_DECODER_FIRST_FATAL_INST_ERROR,
	EM_DECODER_INVALID_INST_SLOT,
	EM_DECODER_FIRST_FATAL_ERROR,
	EM_DECODER_INVALID_TEMPLATE = EM_DECODER_FIRST_FATAL_ERROR,
    EM_DECODER_INVALID_CLIENT_ID,
    EM_DECODER_NULL_PTR,
    EM_DECODER_TOO_SHORT_ERR,
    EM_DECODER_ASSOCIATE_MISS,
    EM_DECODER_INVALID_INST_ID,
    EM_DECODER_INVALID_MACHINE_MODE,
    EM_DECODER_INVALID_MACHINE_TYPE,
    EM_DECODER_INTERNAL_ERROR,
    EM_DECODER_LAST_ERROR
} EM_Decoder_Err;

typedef EM_Decoder_Err DecErr();

typedef enum EM_decoder_machine_type
{ 
    EM_DECODER_CPU_NO_CHANGE=0,
    EM_DECODER_CPU_DEFAULT,
    EM_DECODER_CPU_P7 = 4,
	EM_DECODER_CPU_MERCED,
    EM_DECODER_CPU_LAST = 7
} EM_Decoder_Machine_Type;

typedef enum EM_decoder_machine_mode
{
    EM_DECODER_MODE_NO_CHANGE = 0,
    EM_DECODER_MODE_DEFAULT,
	EM_DECODER_MODE_EM = 8,
    EM_DECODER_MODE_LAST = 9
} EM_Decoder_Machine_Mode;

typedef enum EM_decoder_operand_type
{
    EM_DECODER_NO_OPER = 0,
    EM_DECODER_REGISTER,
    EM_DECODER_MEMORY,
    EM_DECODER_IMMEDIATE,
    EM_DECODER_IP_RELATIVE,
	EM_DECODER_REGFILE,
    EM_DECODER_OPERAND_LAST
} EM_Decoder_Operand_Type;

typedef enum EM_decoder_reg_type
{
    EM_DECODER_NO_REG_TYPE = 0,
    EM_DECODER_INT_REG = 7,
    EM_DECODER_FP_REG,
    EM_DECODER_APP_REG,
    EM_DECODER_BR_REG,
    EM_DECODER_PRED_REG = 13,
	EM_DECODER_CR_REG,
	EM_DECODER_APP_CCV_REG,
	EM_DECODER_APP_PFS_REG,
	EM_DECODER_PR_REG,
	EM_DECODER_PR_ROT_REG,
	EM_DECODER_PSR_REG,
	EM_DECODER_PSR_L_REG,
	EM_DECODER_PSR_UM_REG = 20,
	EM_DECODER_IP_REG,            /* IP register type */
    EM_DECODER_REG_TYPE_LAST
} EM_Decoder_Reg_Type;

typedef enum EM_decoder_reg_name
{
    EM_DECODER_NO_REG=0,
    EM_DECODER_REG_R0 = 98,
    EM_DECODER_REG_R1,
    EM_DECODER_REG_R2,
    EM_DECODER_REG_R3,
    EM_DECODER_REG_R4,
    EM_DECODER_REG_R5,
    EM_DECODER_REG_R6,
    EM_DECODER_REG_R7,
    EM_DECODER_REG_R8,
    EM_DECODER_REG_R9,
    EM_DECODER_REG_R10,
    EM_DECODER_REG_R11,
    EM_DECODER_REG_R12,
    EM_DECODER_REG_R13,
    EM_DECODER_REG_R14,
    EM_DECODER_REG_R15,
    EM_DECODER_REG_R16,
    EM_DECODER_REG_R17,
    EM_DECODER_REG_R18,
    EM_DECODER_REG_R19,
    EM_DECODER_REG_R20,
    EM_DECODER_REG_R21,
    EM_DECODER_REG_R22,
    EM_DECODER_REG_R23,
    EM_DECODER_REG_R24,
    EM_DECODER_REG_R25,
    EM_DECODER_REG_R26,
    EM_DECODER_REG_R27,
    EM_DECODER_REG_R28,
    EM_DECODER_REG_R29,
    EM_DECODER_REG_R30,
    EM_DECODER_REG_R31,
    EM_DECODER_REG_R32,
    EM_DECODER_REG_R33,
    EM_DECODER_REG_R34,
    EM_DECODER_REG_R35,
    EM_DECODER_REG_R36,
    EM_DECODER_REG_R37,
    EM_DECODER_REG_R38,
    EM_DECODER_REG_R39,
    EM_DECODER_REG_R40,
    EM_DECODER_REG_R41,
    EM_DECODER_REG_R42,
    EM_DECODER_REG_R43,
    EM_DECODER_REG_R44,
    EM_DECODER_REG_R45,
    EM_DECODER_REG_R46,
    EM_DECODER_REG_R47,
    EM_DECODER_REG_R48,
    EM_DECODER_REG_R49,
    EM_DECODER_REG_R50,
    EM_DECODER_REG_R51,
    EM_DECODER_REG_R52,
    EM_DECODER_REG_R53,
    EM_DECODER_REG_R54,
    EM_DECODER_REG_R55,
    EM_DECODER_REG_R56,
    EM_DECODER_REG_R57,
    EM_DECODER_REG_R58,
    EM_DECODER_REG_R59,
    EM_DECODER_REG_R60,
    EM_DECODER_REG_R61,
    EM_DECODER_REG_R62,
    EM_DECODER_REG_R63,
    EM_DECODER_REG_R64,
    EM_DECODER_REG_R65,
    EM_DECODER_REG_R66,
    EM_DECODER_REG_R67,
    EM_DECODER_REG_R68,
    EM_DECODER_REG_R69,
    EM_DECODER_REG_R70,
    EM_DECODER_REG_R71,
    EM_DECODER_REG_R72,
    EM_DECODER_REG_R73,
    EM_DECODER_REG_R74,
    EM_DECODER_REG_R75,
    EM_DECODER_REG_R76,
    EM_DECODER_REG_R77,
    EM_DECODER_REG_R78,
    EM_DECODER_REG_R79,
    EM_DECODER_REG_R80,
    EM_DECODER_REG_R81,
    EM_DECODER_REG_R82,
    EM_DECODER_REG_R83,
    EM_DECODER_REG_R84,
    EM_DECODER_REG_R85,
    EM_DECODER_REG_R86,
    EM_DECODER_REG_R87,
    EM_DECODER_REG_R88,
    EM_DECODER_REG_R89,
    EM_DECODER_REG_R90,
    EM_DECODER_REG_R91,
    EM_DECODER_REG_R92,
    EM_DECODER_REG_R93,
    EM_DECODER_REG_R94,
    EM_DECODER_REG_R95,
    EM_DECODER_REG_R96,
    EM_DECODER_REG_R97,
    EM_DECODER_REG_R98,
    EM_DECODER_REG_R99,
    EM_DECODER_REG_R100,
    EM_DECODER_REG_R101,
    EM_DECODER_REG_R102,
    EM_DECODER_REG_R103,
    EM_DECODER_REG_R104,
    EM_DECODER_REG_R105,
    EM_DECODER_REG_R106,
    EM_DECODER_REG_R107,
    EM_DECODER_REG_R108,
    EM_DECODER_REG_R109,
    EM_DECODER_REG_R110,
    EM_DECODER_REG_R111,
    EM_DECODER_REG_R112,
    EM_DECODER_REG_R113,
    EM_DECODER_REG_R114,
    EM_DECODER_REG_R115,
    EM_DECODER_REG_R116,
    EM_DECODER_REG_R117,
    EM_DECODER_REG_R118,
    EM_DECODER_REG_R119,
    EM_DECODER_REG_R120,
    EM_DECODER_REG_R121,
    EM_DECODER_REG_R122,
    EM_DECODER_REG_R123,
    EM_DECODER_REG_R124,
    EM_DECODER_REG_R125,
    EM_DECODER_REG_R126,
    EM_DECODER_REG_R127,
    EM_DECODER_REG_F0,
    EM_DECODER_REG_F1,
    EM_DECODER_REG_F2,
    EM_DECODER_REG_F3,
    EM_DECODER_REG_F4,
    EM_DECODER_REG_F5,
    EM_DECODER_REG_F6,
    EM_DECODER_REG_F7,
    EM_DECODER_REG_F8,
    EM_DECODER_REG_F9,
    EM_DECODER_REG_F10,
    EM_DECODER_REG_F11,
    EM_DECODER_REG_F12,
    EM_DECODER_REG_F13,
    EM_DECODER_REG_F14,
    EM_DECODER_REG_F15,
    EM_DECODER_REG_F16,
    EM_DECODER_REG_F17,
    EM_DECODER_REG_F18,
    EM_DECODER_REG_F19,
    EM_DECODER_REG_F20,
    EM_DECODER_REG_F21,
    EM_DECODER_REG_F22,
    EM_DECODER_REG_F23,
    EM_DECODER_REG_F24,
    EM_DECODER_REG_F25,
    EM_DECODER_REG_F26,
    EM_DECODER_REG_F27,
    EM_DECODER_REG_F28,
    EM_DECODER_REG_F29,
    EM_DECODER_REG_F30,
    EM_DECODER_REG_F31,
    EM_DECODER_REG_F32,
    EM_DECODER_REG_F33,
    EM_DECODER_REG_F34,
    EM_DECODER_REG_F35,
    EM_DECODER_REG_F36,
    EM_DECODER_REG_F37,
    EM_DECODER_REG_F38,
    EM_DECODER_REG_F39,
    EM_DECODER_REG_F40,
    EM_DECODER_REG_F41,
    EM_DECODER_REG_F42,
    EM_DECODER_REG_F43,
    EM_DECODER_REG_F44,
    EM_DECODER_REG_F45,
    EM_DECODER_REG_F46,
    EM_DECODER_REG_F47,
    EM_DECODER_REG_F48,
    EM_DECODER_REG_F49,
    EM_DECODER_REG_F50,
    EM_DECODER_REG_F51,
    EM_DECODER_REG_F52,
    EM_DECODER_REG_F53,
    EM_DECODER_REG_F54,
    EM_DECODER_REG_F55,
    EM_DECODER_REG_F56,
    EM_DECODER_REG_F57,
    EM_DECODER_REG_F58,
    EM_DECODER_REG_F59,
    EM_DECODER_REG_F60,
    EM_DECODER_REG_F61,
    EM_DECODER_REG_F62,
    EM_DECODER_REG_F63,
    EM_DECODER_REG_F64,
    EM_DECODER_REG_F65,
    EM_DECODER_REG_F66,
    EM_DECODER_REG_F67,
    EM_DECODER_REG_F68,
    EM_DECODER_REG_F69,
    EM_DECODER_REG_F70,
    EM_DECODER_REG_F71,
    EM_DECODER_REG_F72,
    EM_DECODER_REG_F73,
    EM_DECODER_REG_F74,
    EM_DECODER_REG_F75,
    EM_DECODER_REG_F76,
    EM_DECODER_REG_F77,
    EM_DECODER_REG_F78,
    EM_DECODER_REG_F79,
    EM_DECODER_REG_F80,
    EM_DECODER_REG_F81,
    EM_DECODER_REG_F82,
    EM_DECODER_REG_F83,
    EM_DECODER_REG_F84,
    EM_DECODER_REG_F85,
    EM_DECODER_REG_F86,
    EM_DECODER_REG_F87,
    EM_DECODER_REG_F88,
    EM_DECODER_REG_F89,
    EM_DECODER_REG_F90,
    EM_DECODER_REG_F91,
    EM_DECODER_REG_F92,
    EM_DECODER_REG_F93,
    EM_DECODER_REG_F94,
    EM_DECODER_REG_F95,
    EM_DECODER_REG_F96,
    EM_DECODER_REG_F97,
    EM_DECODER_REG_F98,
    EM_DECODER_REG_F99,
    EM_DECODER_REG_F100,
    EM_DECODER_REG_F101,
    EM_DECODER_REG_F102,
    EM_DECODER_REG_F103,
    EM_DECODER_REG_F104,
    EM_DECODER_REG_F105,
    EM_DECODER_REG_F106,
    EM_DECODER_REG_F107,
    EM_DECODER_REG_F108,
    EM_DECODER_REG_F109,
    EM_DECODER_REG_F110,
    EM_DECODER_REG_F111,
    EM_DECODER_REG_F112,
    EM_DECODER_REG_F113,
    EM_DECODER_REG_F114,
    EM_DECODER_REG_F115,
    EM_DECODER_REG_F116,
    EM_DECODER_REG_F117,
    EM_DECODER_REG_F118,
    EM_DECODER_REG_F119,
    EM_DECODER_REG_F120,
    EM_DECODER_REG_F121,
    EM_DECODER_REG_F122,
    EM_DECODER_REG_F123,
    EM_DECODER_REG_F124,
    EM_DECODER_REG_F125,
    EM_DECODER_REG_F126,
    EM_DECODER_REG_F127,
    EM_DECODER_REG_AR0,
    EM_DECODER_REG_AR1,
    EM_DECODER_REG_AR2,
    EM_DECODER_REG_AR3,
    EM_DECODER_REG_AR4,
    EM_DECODER_REG_AR5,
    EM_DECODER_REG_AR6,
    EM_DECODER_REG_AR7,
    EM_DECODER_REG_AR8,
    EM_DECODER_REG_AR9,
    EM_DECODER_REG_AR10,
    EM_DECODER_REG_AR11,
    EM_DECODER_REG_AR12,
    EM_DECODER_REG_AR13,
    EM_DECODER_REG_AR14,
    EM_DECODER_REG_AR15,
    EM_DECODER_REG_AR16,
    EM_DECODER_REG_AR17,
    EM_DECODER_REG_AR18,
    EM_DECODER_REG_AR19,
    EM_DECODER_REG_AR20,
    EM_DECODER_REG_AR21,
    EM_DECODER_REG_AR22,
    EM_DECODER_REG_AR23,
    EM_DECODER_REG_AR24,
    EM_DECODER_REG_AR25,
    EM_DECODER_REG_AR26,
    EM_DECODER_REG_AR27,
    EM_DECODER_REG_AR28,
    EM_DECODER_REG_AR29,
    EM_DECODER_REG_AR30,
    EM_DECODER_REG_AR31,
    EM_DECODER_REG_AR32,
    EM_DECODER_REG_AR33,
    EM_DECODER_REG_AR34,
    EM_DECODER_REG_AR35,
    EM_DECODER_REG_AR36,
    EM_DECODER_REG_AR37,
    EM_DECODER_REG_AR38,
    EM_DECODER_REG_AR39,
    EM_DECODER_REG_AR40,
    EM_DECODER_REG_AR41,
    EM_DECODER_REG_AR42,
    EM_DECODER_REG_AR43,
    EM_DECODER_REG_AR44,
    EM_DECODER_REG_AR45,
    EM_DECODER_REG_AR46,
    EM_DECODER_REG_AR47,
    EM_DECODER_REG_AR48,
    EM_DECODER_REG_AR49,
    EM_DECODER_REG_AR50,
    EM_DECODER_REG_AR51,
    EM_DECODER_REG_AR52,
    EM_DECODER_REG_AR53,
    EM_DECODER_REG_AR54,
    EM_DECODER_REG_AR55,
    EM_DECODER_REG_AR56,
    EM_DECODER_REG_AR57,
    EM_DECODER_REG_AR58,
    EM_DECODER_REG_AR59,
    EM_DECODER_REG_AR60,
    EM_DECODER_REG_AR61,
    EM_DECODER_REG_AR62,
    EM_DECODER_REG_AR63,
    EM_DECODER_REG_AR64,
    EM_DECODER_REG_AR65,
    EM_DECODER_REG_AR66,
    EM_DECODER_REG_AR67,
    EM_DECODER_REG_AR68,
    EM_DECODER_REG_AR69,
    EM_DECODER_REG_AR70,
    EM_DECODER_REG_AR71,
    EM_DECODER_REG_AR72,
    EM_DECODER_REG_AR73,
    EM_DECODER_REG_AR74,
    EM_DECODER_REG_AR75,
    EM_DECODER_REG_AR76,
    EM_DECODER_REG_AR77,
    EM_DECODER_REG_AR78,
    EM_DECODER_REG_AR79,
    EM_DECODER_REG_AR80,
    EM_DECODER_REG_AR81,
    EM_DECODER_REG_AR82,
    EM_DECODER_REG_AR83,
    EM_DECODER_REG_AR84,
    EM_DECODER_REG_AR85,
    EM_DECODER_REG_AR86,
    EM_DECODER_REG_AR87,
    EM_DECODER_REG_AR88,
    EM_DECODER_REG_AR89,
    EM_DECODER_REG_AR90,
    EM_DECODER_REG_AR91,
    EM_DECODER_REG_AR92,
    EM_DECODER_REG_AR93,
    EM_DECODER_REG_AR94,
    EM_DECODER_REG_AR95,
    EM_DECODER_REG_AR96,
    EM_DECODER_REG_AR97,
    EM_DECODER_REG_AR98,
    EM_DECODER_REG_AR99,
    EM_DECODER_REG_AR100,
    EM_DECODER_REG_AR101,
    EM_DECODER_REG_AR102,
    EM_DECODER_REG_AR103,
    EM_DECODER_REG_AR104,
    EM_DECODER_REG_AR105,
    EM_DECODER_REG_AR106,
    EM_DECODER_REG_AR107,
    EM_DECODER_REG_AR108,
    EM_DECODER_REG_AR109,
    EM_DECODER_REG_AR110,
    EM_DECODER_REG_AR111,
    EM_DECODER_REG_AR112,
    EM_DECODER_REG_AR113,
    EM_DECODER_REG_AR114,
    EM_DECODER_REG_AR115,
    EM_DECODER_REG_AR116,
    EM_DECODER_REG_AR117,
    EM_DECODER_REG_AR118,
    EM_DECODER_REG_AR119,
    EM_DECODER_REG_AR120,
    EM_DECODER_REG_AR121,
    EM_DECODER_REG_AR122,
    EM_DECODER_REG_AR123,
    EM_DECODER_REG_AR124,
    EM_DECODER_REG_AR125,
    EM_DECODER_REG_AR126,
    EM_DECODER_REG_AR127,
    EM_DECODER_REG_P0,
    EM_DECODER_REG_P1,
    EM_DECODER_REG_P2,
    EM_DECODER_REG_P3,
    EM_DECODER_REG_P4,
    EM_DECODER_REG_P5,
    EM_DECODER_REG_P6,
    EM_DECODER_REG_P7,
    EM_DECODER_REG_P8,
    EM_DECODER_REG_P9,
    EM_DECODER_REG_P10,
    EM_DECODER_REG_P11,
    EM_DECODER_REG_P12,
    EM_DECODER_REG_P13,
    EM_DECODER_REG_P14,
    EM_DECODER_REG_P15,
    EM_DECODER_REG_P16,
    EM_DECODER_REG_P17,
    EM_DECODER_REG_P18,
    EM_DECODER_REG_P19,
    EM_DECODER_REG_P20,
    EM_DECODER_REG_P21,
    EM_DECODER_REG_P22,
    EM_DECODER_REG_P23,
    EM_DECODER_REG_P24,
    EM_DECODER_REG_P25,
    EM_DECODER_REG_P26,
    EM_DECODER_REG_P27,
    EM_DECODER_REG_P28,
    EM_DECODER_REG_P29,
    EM_DECODER_REG_P30,
    EM_DECODER_REG_P31,
    EM_DECODER_REG_P32,
    EM_DECODER_REG_P33,
    EM_DECODER_REG_P34,
    EM_DECODER_REG_P35,
    EM_DECODER_REG_P36,
    EM_DECODER_REG_P37,
    EM_DECODER_REG_P38,
    EM_DECODER_REG_P39,
    EM_DECODER_REG_P40,
    EM_DECODER_REG_P41,
    EM_DECODER_REG_P42,
    EM_DECODER_REG_P43,
    EM_DECODER_REG_P44,
    EM_DECODER_REG_P45,
    EM_DECODER_REG_P46,
    EM_DECODER_REG_P47,
    EM_DECODER_REG_P48,
    EM_DECODER_REG_P49,
    EM_DECODER_REG_P50,
    EM_DECODER_REG_P51,
    EM_DECODER_REG_P52,
    EM_DECODER_REG_P53,
    EM_DECODER_REG_P54,
    EM_DECODER_REG_P55,
    EM_DECODER_REG_P56,
    EM_DECODER_REG_P57,
    EM_DECODER_REG_P58,
    EM_DECODER_REG_P59,
    EM_DECODER_REG_P60,
    EM_DECODER_REG_P61,
    EM_DECODER_REG_P62,
    EM_DECODER_REG_P63,
    EM_DECODER_REG_BR0 ,
    EM_DECODER_REG_BR1,
    EM_DECODER_REG_BR2,
    EM_DECODER_REG_BR3,
    EM_DECODER_REG_BR4,
    EM_DECODER_REG_BR5,
    EM_DECODER_REG_BR6,
    EM_DECODER_REG_BR7,
	EM_DECODER_REG_PR,
	EM_DECODER_REG_PR_ROT,
	EM_DECODER_REG_CR0,
	EM_DECODER_REG_CR1,
	EM_DECODER_REG_CR2,
	EM_DECODER_REG_CR3,
	EM_DECODER_REG_CR4,
	EM_DECODER_REG_CR5,
	EM_DECODER_REG_CR6,
	EM_DECODER_REG_CR7,
	EM_DECODER_REG_CR8,
	EM_DECODER_REG_CR9,
	EM_DECODER_REG_CR10,
	EM_DECODER_REG_CR11,
	EM_DECODER_REG_CR12,
	EM_DECODER_REG_CR13,
	EM_DECODER_REG_CR14,
	EM_DECODER_REG_CR15,
	EM_DECODER_REG_CR16,
	EM_DECODER_REG_CR17,
	EM_DECODER_REG_CR18,
	EM_DECODER_REG_CR19,
	EM_DECODER_REG_CR20,
	EM_DECODER_REG_CR21,
	EM_DECODER_REG_CR22,
	EM_DECODER_REG_CR23,
	EM_DECODER_REG_CR24,
	EM_DECODER_REG_CR25,
	EM_DECODER_REG_CR26,
	EM_DECODER_REG_CR27,
	EM_DECODER_REG_CR28,
	EM_DECODER_REG_CR29,
	EM_DECODER_REG_CR30,
	EM_DECODER_REG_CR31,
	EM_DECODER_REG_CR32,
	EM_DECODER_REG_CR33,
	EM_DECODER_REG_CR34,
	EM_DECODER_REG_CR35,
	EM_DECODER_REG_CR36,
	EM_DECODER_REG_CR37,
	EM_DECODER_REG_CR38,
	EM_DECODER_REG_CR39,
	EM_DECODER_REG_CR40,
	EM_DECODER_REG_CR41,
	EM_DECODER_REG_CR42,
	EM_DECODER_REG_CR43,
	EM_DECODER_REG_CR44,
	EM_DECODER_REG_CR45,
	EM_DECODER_REG_CR46,
	EM_DECODER_REG_CR47,
	EM_DECODER_REG_CR48,
	EM_DECODER_REG_CR49,
	EM_DECODER_REG_CR50,
	EM_DECODER_REG_CR51,
	EM_DECODER_REG_CR52,
	EM_DECODER_REG_CR53,
	EM_DECODER_REG_CR54,
	EM_DECODER_REG_CR55,
	EM_DECODER_REG_CR56,
	EM_DECODER_REG_CR57,
	EM_DECODER_REG_CR58,
	EM_DECODER_REG_CR59,
	EM_DECODER_REG_CR60,
	EM_DECODER_REG_CR61,
	EM_DECODER_REG_CR62,
	EM_DECODER_REG_CR63,
	EM_DECODER_REG_CR64,
	EM_DECODER_REG_CR65,
	EM_DECODER_REG_CR66,
	EM_DECODER_REG_CR67,
	EM_DECODER_REG_CR68,
	EM_DECODER_REG_CR69,
	EM_DECODER_REG_CR70,
	EM_DECODER_REG_CR71,
	EM_DECODER_REG_CR72,
	EM_DECODER_REG_CR73,
	EM_DECODER_REG_CR74,
	EM_DECODER_REG_CR75,
	EM_DECODER_REG_CR76,
	EM_DECODER_REG_CR77,
	EM_DECODER_REG_CR78,
	EM_DECODER_REG_CR79,
	EM_DECODER_REG_CR80,
	EM_DECODER_REG_CR81,
	EM_DECODER_REG_CR82,
	EM_DECODER_REG_CR83,
	EM_DECODER_REG_CR84,
	EM_DECODER_REG_CR85,
	EM_DECODER_REG_CR86,
	EM_DECODER_REG_CR87,
	EM_DECODER_REG_CR88,
	EM_DECODER_REG_CR89,
	EM_DECODER_REG_CR90,
	EM_DECODER_REG_CR91,
	EM_DECODER_REG_CR92,
	EM_DECODER_REG_CR93,
	EM_DECODER_REG_CR94,
	EM_DECODER_REG_CR95,
	EM_DECODER_REG_CR96,
	EM_DECODER_REG_CR97,
	EM_DECODER_REG_CR98,
	EM_DECODER_REG_CR99,
	EM_DECODER_REG_CR100,
	EM_DECODER_REG_CR101,
	EM_DECODER_REG_CR102,
	EM_DECODER_REG_CR103,
	EM_DECODER_REG_CR104,
	EM_DECODER_REG_CR105,
	EM_DECODER_REG_CR106,
	EM_DECODER_REG_CR107,
	EM_DECODER_REG_CR108,
	EM_DECODER_REG_CR109,
	EM_DECODER_REG_CR110,
	EM_DECODER_REG_CR111,
	EM_DECODER_REG_CR112,
	EM_DECODER_REG_CR113,
	EM_DECODER_REG_CR114,
	EM_DECODER_REG_CR115,
	EM_DECODER_REG_CR116,
	EM_DECODER_REG_CR117,
	EM_DECODER_REG_CR118,
	EM_DECODER_REG_CR119,
	EM_DECODER_REG_CR120,
	EM_DECODER_REG_CR121,
	EM_DECODER_REG_CR122,
	EM_DECODER_REG_CR123,
	EM_DECODER_REG_CR124,
	EM_DECODER_REG_CR125,
	EM_DECODER_REG_CR126,
	EM_DECODER_REG_CR127,
	EM_DECODER_REG_PSR,
	EM_DECODER_REG_PSR_L,
	EM_DECODER_REG_PSR_UM,
    EM_DECODER_REG_IP,      /* register IP name */ 
	EM_DECODER_EM_REG_LAST,

	EM_DECODER_REG_AR_K0   = EM_DECODER_REG_AR0+EM_AR_KR0,
	EM_DECODER_REG_AR_K1   = EM_DECODER_REG_AR0+EM_AR_KR1,
	EM_DECODER_REG_AR_K2   = EM_DECODER_REG_AR0+EM_AR_KR2,
	EM_DECODER_REG_AR_K3   = EM_DECODER_REG_AR0+EM_AR_KR3,
	EM_DECODER_REG_AR_K4   = EM_DECODER_REG_AR0+EM_AR_KR4, /* added AR_K4-7 */
	EM_DECODER_REG_AR_K5   = EM_DECODER_REG_AR0+EM_AR_KR5,
	EM_DECODER_REG_AR_K6   = EM_DECODER_REG_AR0+EM_AR_KR6,
	EM_DECODER_REG_AR_K7   = EM_DECODER_REG_AR0+EM_AR_KR7,
	EM_DECODER_REG_AR_RSC  = EM_DECODER_REG_AR0+EM_AR_RSC,
	EM_DECODER_REG_AR_BSP  = EM_DECODER_REG_AR0+EM_AR_BSP,
	EM_DECODER_REG_AR_BSPSTORE = EM_DECODER_REG_AR0+EM_AR_BSPSTORE,
	EM_DECODER_REG_AR_RNAT = EM_DECODER_REG_AR0+EM_AR_RNAT,
	EM_DECODER_REG_AR_EFLAG= EM_DECODER_REG_AR0+EM_AR_EFLAG,
   	EM_DECODER_REG_AR_CSD  = EM_DECODER_REG_AR0+EM_AR_CSD,
	EM_DECODER_REG_AR_SSD  = EM_DECODER_REG_AR0+EM_AR_SSD,
	EM_DECODER_REG_AR_CFLG = EM_DECODER_REG_AR0+EM_AR_CFLG,
	EM_DECODER_REG_AR_FSR  = EM_DECODER_REG_AR0+EM_AR_FSR,
	EM_DECODER_REG_AR_FIR  = EM_DECODER_REG_AR0+EM_AR_FIR,
	EM_DECODER_REG_AR_FDR  = EM_DECODER_REG_AR0+EM_AR_FDR,
	EM_DECODER_REG_AR_CCV  = EM_DECODER_REG_AR0+EM_AR_CCV,
	EM_DECODER_REG_AR_UNAT = EM_DECODER_REG_AR0+EM_AR_UNAT,
	EM_DECODER_REG_AR_FPSR = EM_DECODER_REG_AR0+EM_AR_FPSR,
	EM_DECODER_REG_AR_ITC  = EM_DECODER_REG_AR0+EM_AR_ITC,	
	EM_DECODER_REG_AR_PFS  = EM_DECODER_REG_AR0+EM_AR_PFS,
	EM_DECODER_REG_AR_LC   = EM_DECODER_REG_AR0+EM_AR_LC,
	EM_DECODER_REG_AR_EC   = EM_DECODER_REG_AR0+EM_AR_EC,

	EM_DECODER_REG_CR_DCR  = EM_DECODER_REG_CR0+EM_CR_DCR,
	EM_DECODER_REG_CR_ITM  = EM_DECODER_REG_CR0+EM_CR_ITM,
	EM_DECODER_REG_CR_IVA  = EM_DECODER_REG_CR0+EM_CR_IVA,
	EM_DECODER_REG_CR_PTA  = EM_DECODER_REG_CR0+EM_CR_PTA,
	EM_DECODER_REG_CR_GPTA = EM_DECODER_REG_CR0+EM_CR_GPTA,
	EM_DECODER_REG_CR_IPSR = EM_DECODER_REG_CR0+EM_CR_IPSR,
	EM_DECODER_REG_CR_ISR  = EM_DECODER_REG_CR0+EM_CR_ISR,
	EM_DECODER_REG_CR_IIP  = EM_DECODER_REG_CR0+EM_CR_IIP,
	EM_DECODER_REG_CR_IFA  = EM_DECODER_REG_CR0+EM_CR_IFA,
	EM_DECODER_REG_CR_ITIR = EM_DECODER_REG_CR0+EM_CR_ITIR,
	EM_DECODER_REG_CR_IIPA = EM_DECODER_REG_CR0+EM_CR_IIPA,
	EM_DECODER_REG_CR_IFS  = EM_DECODER_REG_CR0+EM_CR_IFS,
	EM_DECODER_REG_CR_IIM  = EM_DECODER_REG_CR0+EM_CR_IIM,
	EM_DECODER_REG_CR_IHA  = EM_DECODER_REG_CR0+EM_CR_IHA,

	EM_DECODER_REG_CR_LID  = EM_DECODER_REG_CR0+EM_CR_LID,
	EM_DECODER_REG_CR_IVR  = EM_DECODER_REG_CR0+EM_CR_IVR,
	EM_DECODER_REG_CR_TPR  = EM_DECODER_REG_CR0+EM_CR_TPR,
	EM_DECODER_REG_CR_EOI  = EM_DECODER_REG_CR0+EM_CR_EOI,
	EM_DECODER_REG_CR_IRR0 = EM_DECODER_REG_CR0+EM_CR_IRR0,
	EM_DECODER_REG_CR_IRR1 = EM_DECODER_REG_CR0+EM_CR_IRR1,
	EM_DECODER_REG_CR_IRR2 = EM_DECODER_REG_CR0+EM_CR_IRR2,
	EM_DECODER_REG_CR_IRR3 = EM_DECODER_REG_CR0+EM_CR_IRR3,
	EM_DECODER_REG_CR_ITV  = EM_DECODER_REG_CR0+EM_CR_ITV,
	EM_DECODER_REG_CR_PMV  = EM_DECODER_REG_CR0+EM_CR_PMV,
	EM_DECODER_REG_CR_LRR0 = EM_DECODER_REG_CR0+EM_CR_LRR0,
	EM_DECODER_REG_CR_LRR1 = EM_DECODER_REG_CR0+EM_CR_LRR1,
	EM_DECODER_REG_CR_CMCV = EM_DECODER_REG_CR0+EM_CR_CMCV,
		
/************************************************************/
    EM_DECODER_REG_LAST
} EM_Decoder_Reg_Name;

typedef struct EM_decoder_reg_info
{
    int                  valid;
    EM_Decoder_Reg_Type  type;
    EM_Decoder_Reg_Name  name;
    long                 value;
} EM_Decoder_Reg_Info;

typedef enum EM_DECODER_regfile_name
{
	EM_DECODER_NO_REGFILE = 0,
	EM_DECODER_REGFILE_PMC,
	EM_DECODER_REGFILE_PMD,
	EM_DECODER_REGFILE_PKR,
	EM_DECODER_REGFILE_RR,
	EM_DECODER_REGFILE_IBR,
	EM_DECODER_REGFILE_DBR,
	EM_DECODER_REGFILE_ITR,
	EM_DECODER_REGFILE_DTR,
	EM_DECODER_REGFILE_MSR,
	EM_DECODER_REGFILE_CPUID,
	EM_DECODER_REGFILE_LAST
} EM_Decoder_Regfile_Name;

typedef enum EM_decoder_operand_2nd_role
{
    EM_DECODER_OPER_2ND_ROLE_NONE = 0,
    EM_DECODER_OPER_2ND_ROLE_SRC,
    EM_DECODER_OPER_2ND_ROLE_DST
} EM_Decoder_Operand_2nd_Role;

typedef enum EM_decoder_oper_size
{
    EM_DECODER_OPER_NO_SIZE =  0,
    EM_DECODER_OPER_SIZE_1 =   1,
    EM_DECODER_OPER_SIZE_2 =   2,
    EM_DECODER_OPER_SIZE_4 =   4,
    EM_DECODER_OPER_SIZE_8 =   8,
    EM_DECODER_OPER_SIZE_10 = 10,
    EM_DECODER_OPER_SIZE_16 = 16,
    EM_DECODER_OPER_SIZE_20 = 20,
    EM_DECODER_OPER_SIZE_22 = 22,
    EM_DECODER_OPER_SIZE_24 = 24,  
    EM_DECODER_OPER_SIZE_32 = 32,
    EM_DECODER_OPER_SIZE_64 = 64
} EM_Decoder_Oper_Size;

typedef enum EM_decoder_imm_type
{
	EM_DECODER_IMM_NONE,
	EM_DECODER_IMM_SIGNED,
	EM_DECODER_IMM_UNSIGNED,
	EM_DECODER_IMM_FCLASS,
	EM_DECODER_IMM_MUX1,
	EM_DECODER_IMM_LAST
} EM_Decoder_Imm_Type;

typedef enum EM_decoder_slot
{
	EM_DECODER_SLOT_0 = 0,
	EM_DECODER_SLOT_1 = 1,
	EM_DECODER_SLOT_2 = 2,
	EM_DECODER_SLOT_LAST = 2
} EM_Decoder_Slot;


/***** EM_decoder Structure Defenition ****/

typedef struct EM_decoder_modifiers_s
{
	EM_cmp_type_t           cmp_type;
	EM_cmp_rel_t            cmp_rel;
	EM_branch_type_t        branch_type;
	EM_branch_hint_t        branch_hint;
	EM_fp_precision_t       fp_precision;
	EM_fp_status_t          fp_status;
	EM_memory_access_hint_t mem_access_hint;
} EM_Decoder_modifiers_t;	


typedef struct EM_decoder_oper_static_s
{
	Operand_role_t role;
	Operand_type_t type;
	unsigned long  flags;
} EM_Decoder_oper_static_t;


#define EM_DECODER_MAX_EXP_DST (2)
#define EM_DECODER_MAX_EXP_SRC (5)
#define EM_DECODER_MAX_IMP_DST (8)
#define EM_DECODER_MAX_IMP_SRC (8)

typedef struct EM_decoder_static_info_s
{
	Mnemonic_t               mnemonic;
	Template_role_t          template_role;
	EM_Decoder_oper_static_t explicit_dst[EM_DECODER_MAX_EXP_DST];
	EM_Decoder_oper_static_t explicit_src[EM_DECODER_MAX_EXP_SRC];
	EM_Decoder_imp_oper_t    implicit_dst[EM_DECODER_MAX_IMP_DST];
	EM_Decoder_imp_oper_t    implicit_src[EM_DECODER_MAX_IMP_SRC];
	EM_Decoder_modifiers_t   modifiers;
	Flags_t                  flags;
} EM_Decoder_static_info_t;


extern const EM_Decoder_static_info_t em_decoder_static_info[];


typedef struct EM_decoder_regfile_info
{
    EM_Decoder_Regfile_Name  name;
    EM_Decoder_Reg_Info      index;
} EM_Decoder_Regfile_Info;


typedef struct EM_decoder_imm_info
{
    EM_Decoder_Imm_Type  imm_type;
    unsigned int         size;
    U64                  val64;
} EM_Decoder_Imm_Info;

typedef struct EM_decoder_mem_info
{
    EM_Decoder_Reg_Info    mem_base;
    EM_Decoder_Oper_Size   size;
} EM_Decoder_Mem_Info;

typedef struct em_decoder_em_bundle_info
{
	EM_template_t           b_template;
	unsigned long		    flags;
} EM_Decoder_EM_Bundle_Info;

typedef struct em_decoder_em_info
{
	EM_Decoder_EM_Bundle_Info  em_bundle_info;
	EM_Decoder_Slot            slot_no;
	Template_role_t            eut;
	unsigned long              em_flags;
} EM_Decoder_EM_Info;

typedef struct EM_decoder_inst_static_info
{
    void *                client_info;
	const EM_Decoder_static_info_t *static_info;
    unsigned long         flags;
} EM_Decoder_Inst_Static_Info;


typedef struct EM_decoder_operand_info
{
    EM_Decoder_Operand_Type     type;
	EM_Decoder_Regfile_Info     regfile_info;
    EM_Decoder_Reg_Info         reg_info;
    EM_Decoder_Mem_Info         mem_info;
    EM_Decoder_Imm_Info         imm_info;
    long                        ip_relative_offset;
    unsigned long               oper_flags;
} EM_Decoder_Operand_Info;


typedef struct em_decoder_info
{
    EM_Decoder_Inst_Id       inst;
    EM_Decoder_Reg_Info      pred;
    EM_Decoder_Operand_Info  src1;
    EM_Decoder_Operand_Info  src2;
    EM_Decoder_Operand_Info  src3;
    EM_Decoder_Operand_Info  src4;
    EM_Decoder_Operand_Info  src5;
    EM_Decoder_Operand_Info  dst1;
    EM_Decoder_Operand_Info  dst2;
	EM_Decoder_EM_Info       EM_info;
    void *                   client_info;
    unsigned long            flags;
    unsigned char            size;
	const EM_Decoder_static_info_t *static_info;
} EM_Decoder_Info;


typedef struct em_decoder_bundle_info
{
	unsigned int          	    inst_num;
	EM_Decoder_EM_Bundle_Info	em_bundle_info;
	EM_Decoder_Err           	error[3];
	EM_Decoder_Info          	inst_info[3];
} EM_Decoder_Bundle_Info;
	

typedef int EM_Decoder_Id;


/***********************************************/
/***          Setup flags                    ***/
/***********************************************/
  
#define EM_DECODER_FLAG_NO_MEMSET       0X00000001


extern const U32 em_decoder_bundle_size;

EM_Decoder_Id  em_decoder_open(void);

EM_Decoder_Err em_decoder_associate_one(const EM_Decoder_Id		 id,
								   const EM_Decoder_Inst_Id	     inst,
								   const void                  * client_info);

EM_Decoder_Err em_decoder_associate_check(const EM_Decoder_Id    id,
									       EM_Decoder_Inst_Id  * inst);

EM_Decoder_Err em_decoder_setenv(const EM_Decoder_Id,
                           const EM_Decoder_Machine_Type,
                           const EM_Decoder_Machine_Mode);

EM_Decoder_Err em_decoder_setup(const EM_Decoder_Id,
                           const EM_Decoder_Machine_Type,
                           const EM_Decoder_Machine_Mode,
						   unsigned long flags);

EM_Decoder_Err em_decoder_close(const EM_Decoder_Id);

EM_Decoder_Err em_decoder_decode(const EM_Decoder_Id    id,
						         const unsigned char  * code,
						         const int			    max_code_size,
						         const EM_IL		    location,
						         EM_Decoder_Info      * decoder_info);

 EM_Decoder_Err em_decoder_inst_static_info(const  EM_Decoder_Id,
                                            const  EM_Decoder_Inst_Id,
                                            EM_Decoder_Inst_Static_Info *);

 const char* em_decoder_ver_str(void);

 void  em_decoder_get_version(EM_library_version_t  * dec_version);

 const char* em_decoder_err_msg(EM_Decoder_Err error);

 EM_Decoder_Err em_decoder_decode_bundle(const EM_Decoder_Id        id,
								         const unsigned char      * code,
								         const int                  max_size,
								         EM_Decoder_Bundle_Info   * bundle_info);

/**********************  GET next IL  *************************/
#define EM_DECODER_NEXT(IL, decoder_info)                       \
{                                                               \
	U32 rem_size;												\
	int slot_no = EM_IL_GET_SLOT_NO(IL),						\
	    size = (decoder_info)->size;							\
	switch (slot_no)											\
	{															\
	  case 0:													\
		break;													\
	  case 1:													\
		if (size < 2)											\
			break;		/*** else fall-through ***/				\
	  case 2:													\
		size = EM_BUNDLE_SIZE - slot_no;						\
	}															\
	IEL_CONVERT1(rem_size, size);								\
	IEL_ADDU(IL, IL, rem_size);                                 \
}

#define EM_DECODER_BUNDLE_NEXT(addr)          	                \
{                                                               \
	IEL_ADDU(addr, addr, em_decoder_bundle_size);               \
}


#define EM_DECODER_ERROR_IS_FATAL(_Err)          \
     ((_Err) >= EM_DECODER_FIRST_FATAL_ERROR)

#define EM_DECODER_ERROR_IS_INST_FATAL(_Err)      \
     (((_Err) >= EM_DECODER_FIRST_FATAL_INST_ERROR) && ((_Err) < EM_DECODER_FIRST_FATAL_ERROR))


/*************   EM Instruction Flags Related Macros   ***************/

/*** EM_decoder and static infos flags ***/

/* Flags that copied directly from EMDB */

#define EM_DECODER_BIT_PREDICATE          EM_FLAG_PRED               /* The instruction can get pred  */
#define EM_DECODER_BIT_PRIVILEGED         EM_FLAG_PRIVILEGED         /* The instruction is privileged */
#define EM_DECODER_BIT_LMEM               EM_FLAG_LMEM               /* The instuction is a load inst */
#define EM_DECODER_BIT_SMEM               EM_FLAG_SMEM               /* The instruction is a store    */
#define EM_DECODER_BIT_CHECK_BASE_EQ_DST  EM_FLAG_CHECK_BASE_EQ_DST  /* Base value must differ from destination's */
#define EM_DECODER_BIT_GROUP_FIRST        EM_FLAG_FIRST_IN_INSTRUCTION_GROUP /* Instruction must be the first in instruction group */
#define EM_DECODER_BIT_GROUP_LAST         EM_FLAG_LAST_IN_INSTRUCTION_GROUP  /* Instruction must be the last in instruction group */
#define EM_DECODER_BIT_CHECK_SAME_DSTS    EM_FLAG_CHECK_SAME_DSTS    /* Two destinations should have different values */
#define EM_DECODER_BIT_SLOT2_ONLY         EM_FLAG_SLOT2_ONLY         /* The instruction is allowed in the last slot of bundle */
#define EM_DECODER_BIT_TWO_SLOT           EM_FLAG_TWO_SLOT           /* The instruction is long (2 slots length) */
#define EM_DECODER_BIT_OK_IN_MLI          EM_FLAG_OK_IN_MLI
#define EM_DECODER_BIT_CHECK_EVEN_ODD_FREGS EM_FLAG_CHECK_EVEN_ODD_FREGS
#define EM_DECODER_BIT_CTYPE_UNC          EM_FLAG_CTYPE_UNC          /* If two destinations are equal the instruction
																		allways causes illegal operation fault */   
 
 /* Others */
#define EM_DECODER_BIT_SPECULATION        (EMDB_LAST_FLAG << 1)      /* Speculative form of instruction */
#define EM_DECODER_BIT_POSTINCREMENT      (EMDB_LAST_FLAG << 2)      /* Post increment form of instruction */
#define EM_DECODER_BIT_FALSE_PRED_EXEC    (EMDB_LAST_FLAG << 3)      /* Instruction executed when predicate is false */
#define EM_DECODER_BIT_BR_HINT            (EMDB_LAST_FLAG << 4)      /* Branch-hint form of instruction */ 
#define EM_DECODER_BIT_BR                 (EMDB_LAST_FLAG << 5)      /* Branch instruction              */
#define EM_DECODER_BIT_ADV_LOAD           (EMDB_LAST_FLAG << 6)      /* Instruction is an advanced or speculative advanced load */
#define EM_DECODER_BIT_CONTROL_TRANSFER   (EMDB_LAST_FLAG << 7)      /* Instruction violates sequential control flow */
#define EM_DECODER_BIT_UNC_ILLEGAL_FAULT  (EMDB_LAST_FLAG << 8)      /* Illegal opcode causes illegal operation fault 
                                                                        undependent on predicate value */
 
/* Flags that depend on the current bundle encoding */

/* in em_flags: */
#define EM_DECODER_BIT_CYCLE_BREAK 0x10000 /*Inst is last in its group */
#define EM_DECODER_BIT_LAST_INST   0x20000 /*Last instruction in bundle   */

/* Static flags (depend only on inst id) */
 
#define EM_DECODER_BIT_LONG_INST   0x40000 /* 2 slots Inst (movl)*/

/* in em_bundle_info flags */
#define EM_DECODER_BIT_BUNDLE_STOP 0x80000 /*Stop bit is set in bundle*/


#define EM_DECODER_PREDICATE(di)                (EM_DECODER_BIT_PREDICATE & ((di)->flags))
#define EM_DECODER_PRIVILEGED(di)               (EM_DECODER_BIT_PRIVILEGED & ((di)->flags))
#define EM_DECODER_LMEM(di)                     (EM_DECODER_BIT_LMEM & ((di)->flags))      
#define EM_DECODER_SMEM(di)                     (EM_DECODER_BIT_SMEM & ((di)->flags))
#define EM_DECODER_CHECK_BASE_EQ_DST(di)        (EM_DECODER_BIT_CHECK_BASE_EQ_DST & ((di)->flags))
#define EM_DECODER_CHECK_SPECULATION(di)        (EM_DECODER_BIT_SPECULATION & ((di)->flags))
#define EM_DECODER_CHECK_POSTINCREMENT(di)      (EM_DECODER_BIT_POSTINCREMENT & ((di)->flags))
#define EM_DECODER_CHECK_FALSE_PRED_EXEC(di)    (EM_DECODER_BIT_FALSE_PRED_EXEC & ((di)->flags))
#define EM_DECODER_CHECK_BR_HINT(di)            (EM_DECODER_BIT_BR_HINT & ((di)->flags))
#define EM_DECODER_CHECK_BR(di)                 (EM_DECODER_BIT_BR & ((di)->flags))
#define EM_DECODER_CHECK_GROUP_FIRST(di)        (EM_DECODER_BIT_GROUP_FIRST & ((di)->flags))
#define EM_DECODER_CHECK_GROUP_LAST(di)         (EM_DECODER_BIT_GROUP_LAST & ((di)->flags))
#define EM_DECODER_CHECK_SAME_DSTS(di)          (EM_DECODER_BIT_CHECK_SAME_DSTS & ((di)->flags))
#define EM_DECODER_CHECK_SLOT2_ONLY(di)         (EM_DECODER_BIT_SLOT2_ONLY & ((di)->flags))
#define EM_DECODER_CHECK_TWO_SLOT(di)           (EM_DECODER_BIT_TWO_SLOT & ((di)->flags))
#define EM_DECODER_CHECK_ADV_LOAD(di)           (EM_DECODER_BIT_ADV_LOAD & ((di)->flags))
#define EM_DECODER_CHECK_CONTROL_TRANSFER(di)   (EM_DECODER_BIT_CONTROL_TRANSFER & ((di)->flags))
#define EM_DECODER_CHECK_UNC_ILLEGAL_FAULT(di)  (EM_DECODER_BIT_CTYPE_UNC & ((di)->flags))
#define EM_DECODER_CHECK_OK_IN_MLI(di)          (EM_DECODER_BIT_OK_IN_MLI & ((di)->flags))

#define EM_DECODER_LONG_INST(di)   		\
		(EM_DECODER_BIT_LONG_INST & (((di)->EM_info).em_flags))

#define EM_DECODER_LAST_INST(di)  				\
		(EM_DECODER_BIT_LAST_INST & (((di)->EM_info).em_flags))

#define EM_DECODER_CYCLE_BREAK(di) 		\
		(EM_DECODER_BIT_CYCLE_BREAK & (((di)->EM_info).em_flags))

#define EM_DECODER_BUNDLE_STOP(di) 		\
		(EM_DECODER_BIT_BUNDLE_STOP & 	\
		 (((di)->EM_info).em_bundle_info.flags))



/************** Operand Related macros ****************/
									
#define EM_DECODER_OPER_2ND_ROLE_SRC_BIT  0x00000001  /* Oper second role:  src */
#define EM_DECODER_OPER_2ND_ROLE_DST_BIT  0x00000002  /* Oper second role: dest */
#define EM_DECODER_OPER_IMM_IREG_BIT      0x00000040  /* Operand type is IREG_NUM */
#define EM_DECODER_OPER_IMM_FREG_BIT      0x00000080  /* Operand type is FREG_NUM */

#define EM_DECODER_OPER_2ND_ROLE_SRC(oi)                                         \
							(((oi)->oper_flags) & EM_DECODER_OPER_2ND_ROLE_SRC_BIT)
#define EM_DECODER_OPER_2ND_ROLE_DST(oi)                                         \
							(((oi)->oper_flags) & EM_DECODER_OPER_2ND_ROLE_DST_BIT)
#define EM_DECODER_OPER_NOT_TRUE_SRC(oi)                                         \
							(((oi)->oper_flags) & EM_DECODER_OPER_NOT_TRUE_SRC_BIT)
#define EM_DECODER_OPER_IMP_ENCODED(oi)                                          \
							(((oi)->oper_flags) & EM_DECODER_OPER_IMP_ENCODED_BIT)

#define EM_DECODER_OPER_IMM_REG(oi) 		                                    \
	(((oi)->oper_flags) & (EM_DECODER_OPER_IMM_IREG_BIT |						\
						   EM_DECODER_OPER_IMM_FREG_BIT))

#define EM_DECODER_OPER_IMM_IREG(oi)                                     \
							(((oi)->oper_flags) & EM_DECODER_OPER_IMM_IREG_BIT)
#define EM_DECODER_OPER_IMM_FREG(oi)                                     \
							(((oi)->oper_flags) & EM_DECODER_OPER_IMM_FREG_BIT)



/************* EM_decoder Static Info Related macros ************/

/****** Macros receive pointer to modifiers ******/

#define EM_DECODER_MODIFIERS_CMP_TYPE(Mo) \
                            ((Mo)->cmp_type)

#define EM_DECODER_MODIFIERS_CMP_REL(Mo) \
                            ((Mo)->cmp_rel)

#define EM_DECODER_MODIFIERS_BRANCH_TYPE(Mo) \
                            ((Mo)->branch_type)

#define EM_DECODER_MODIFIERS_BRANCH_HINT(Mo) \
                            ((Mo)->branch_hint)
 
#define EM_DECODER_MODIFIERS_FP_PRECISION(Mo) \
                            ((Mo)->fp_precision)

#define EM_DECODER_MODIFIERS_FP_STATUS(Mo) \
                            ((Mo)->fp_status)

#define EM_DECODER_MODIFIERS_MEMORY_ACCESS_HINT(Mo) \
                            ((Mo)->mem_access_hint)


/****** Macros receive operand flags value ******/

#define EM_DECODER_OPER_FLAGS_2ND_ROLE_SRC(of) \
							((of) & EM_DECODER_OPER_2ND_ROLE_SRC_BIT)

#define EM_DECODER_OPER_FLAGS_2ND_ROLE_DST(of) \
                            ((of) & EM_DECODER_OPER_2ND_ROLE_DST_BIT)

#define EM_DECODER_OPER_FLAGS_IMM_REG(of) \
            	            ((of) & (EM_DECODER_OPER_IMM_IREG_BIT | \
						    EM_DECODER_OPER_IMM_FREG_BIT))

#define EM_DECODER_OPER_FLAGS_IMM_IREG(of) \
							((of) & EM_DECODER_OPER_IMM_IREG_BIT)

#define EM_DECODER_OPER_FLAGS_IMM_FREG(of) \
							((of) & EM_DECODER_OPER_IMM_FREG_BIT)


/****** Macros receive pointer to operand ******/

#define EM_DECODER_OPER_STAT_2ND_ROLE_SRC(oi) \
							EM_DECODER_OPER_FLAGS_2ND_ROLE_SRC((oi)->flags)

#define EM_DECODER_OPER_STAT_2ND_ROLE_DST(oi) \
                            EM_DECODER_OPER_FLAGS_2ND_ROLE_DST((oi)->flags)

#define EM_DECODER_OPER_STAT_IMM_REG(oi) \
                            EM_DECODER_OPER_FLAGS_IMM_REG((oi)->flags)

#define EM_DECODER_OPER_STAT_IMM_BREG(oi) \
							EM_DECODER_OPER_FLAGS_IMM_BREG((oi)->flags)

#define EM_DECODER_OPER_STAT_IMM_IREG(oi) \
							EM_DECODER_OPER_FLAGS_IMM_IREG((oi)->flags)
												  

#define EM_DECODER_OPER_STAT_IMM_FREG(oi) \
							EM_DECODER_OPER_FLAGS_IMM_FREG((oi)->flags)



#define EM_DECODER_OPER_ROLE(oi) \
                            ((oi)->role)

#define EM_DECODER_OPER_TYPE(oi) \
                            ((oi)->type)

#define EM_DECODER_OPER_FLAGS(oi) \
                            ((oi)->flags)


/****** Macros receive instruction flags value ******/

#define EM_DECODER_FLAGS_FLAG_PRED(if) \
                            ((if) & EM_DECODER_BIT_PREDICATE)

#define EM_DECODER_FLAGS_FLAG_PRIVILEGED(if) \
                            ((if) & EM_DECODER_BIT_PRIVILEGED)

#define EM_DECODER_FLAGS_FLAG_LMEM(if) \
                            ((if) & EM_DECODER_BIT_LMEM)

#define EM_DECODER_FLAGS_FLAG_SMEM(if) \
                            ((if) & EM_DECODER_BIT_SMEM)

#define EM_DECODER_FLAGS_FLAG_CHECK_BASE_EQ_DST(if) \
                            ((if) & EM_DECODER_BIT_CHECK_BASE_EQ_DST)

#define EM_DECODER_FLAGS_FLAG_SPECULATION(if) \
                            ((if) & EM_DECODER_BIT_SPECULATION)

#define EM_DECODER_FLAGS_FLAG_POSTINCREMENT(if) \
                            ((if) & EM_DECODER_BIT_POSTINCREMENT)

#define EM_DECODER_FLAGS_FLAG_FALSE_PRED_EXEC(if) \
                            ((if) & EM_DECODER_BIT_FALSE_PRED_EXEC)

#define EM_DECODER_FLAGS_FLAG_BR_HINT(if) \
                            ((if) & EM_DECODER_BIT_BR_HINT)

#define EM_DECODER_FLAGS_FLAG_BR(if) \
                            ((if) & EM_DECODER_BIT_BR)

#define EM_DECODER_FLAGS_FLAG_ADV_LOAD(if) \
                            ((if) & EM_DECODER_CHECK_ADV_LOAD)

#define EM_DECODER_FLAGS_FLAG_GROUP_FIRST(if) \
                            ((if) & EM_DECODER_BIT_GROUP_FIRST)

#define EM_DECODER_FLAGS_FLAG_GROUP_LAST(if) \
                            ((if) & EM_DECODER_BIT_GROUP_LAST)

#define EM_DECODER_FLAGS_FLAG_CHECK_SAME_DSTS(if) \
                            ((if) & EM_DECODER_BIT_CHECK_SAME_DSTS)

#define EM_DECODER_FLAGS_FLAG_CONTROL_TRANSFER(if) \
                            ((if) & EM_DECODER_BIT_CONTROL_TRANSFER)


/****** Macros receive pointer to EM_decoder static info ******/

#define EM_DECODER_STATIC_MNEMONIC(si) \
                            ((si)->mnemonic)

#define EM_DECODER_STATIC_TEMPLATE_ROLE(si) \
                            ((si)->template_role)


#define EM_DECODER_STATIC_EXP_DST_ROLE(si, n) \
                            EM_DECODER_OPER_ROLE(((si)->explicit_dst) + (n))

#define EM_DECODER_STATIC_EXP_DST_TYPE(si, n) \
                            EM_DECODER_OPER_TYPE(((si)->explicit_dst) + (n))

#define EM_DECODER_STATIC_EXP_DST_FLAGS(si, n) \
                            EM_DECODER_OPER_FLAGS(((si)->explicit_dst) + (n))

#define EM_DECODER_STATIC_EXP_SRC_ROLE(si, n) \
                            EM_DECODER_OPER_ROLE(((si)->explicit_src) + (n))

#define EM_DECODER_STATIC_EXP_SRC_TYPE(si, n) \
                            EM_DECODER_OPER_TYPE(((si)->explicit_src) + (n))

#define EM_DECODER_STATIC_EXP_SRC_FLAGS(si, n) \
                            EM_DECODER_OPER_FLAGS(((si)->explicit_src) + (n))


#define EM_DECODER_STATIC_IMP_DST(si, n) \
                            ((si)->implicit_dst[(n)])

#define EM_DECODER_STATIC_IMP_SRC(si, n) \
                            ((si)->implicit_src[(n)])


#define EM_DECODER_STATIC_CMP_TYPE(si) \
                            EM_DECODER_MODIFIERS_CMP_TYPE(&((si)->modifiers))

#define EM_DECODER_STATIC_CMP_REL(si) \
                            EM_DECODER_MODIFIERS_CMP_REL(&((si)->modifiers))

#define EM_DECODER_STATIC_BRANCH_TYPE(si) \
                            EM_DECODER_MODIFIERS_BRANCH_TYPE(&((si)->modifiers))

#define EM_DECODER_STATIC_BRANCH_HINT(si) \
                            EM_DECODER_MODIFIERS_BRANCH_HINT(&((si)->modifiers))

#define EM_DECODER_STATIC_FP_PRECISION(si) \
                            EM_DECODER_MODIFIERS_FP_PRECISION(&((si)->modifiers))

#define EM_DECODER_STATIC_FP_STATUS(si) \
                            EM_DECODER_MODIFIERS_FP_STATUS(&((si)->modifiers))

#define EM_DECODER_STATIC_MEMORY_ACCESS_HINT(si) \
                            EM_DECODER_MODIFIERS_MEMORY_ACCESS_HINT(&((si)->modifiers))


#define EM_DECODER_STATIC_FLAGS(si) \
                            ((si)->flags)

#define EM_DECODER_STATIC_FLAG_PRED(si) \
                            EM_DECODER_FLAGS_FLAG_PRED((si)->flags)

#define EM_DECODER_STATIC_FLAG_PRIVILEGED(si) \
                            EM_DECODER_FLAGS_FLAG_PRIVILEGED((si)->flags)

#define EM_DECODER_STATIC_FLAG_LMEM(si) \
                            EM_DECODER_FLAGS_FLAG_LMEM((si)->flags)

#define EM_DECODER_STATIC_FLAG_SMEM(si) \
                            EM_DECODER_FLAGS_FLAG_SMEM((si)->flags)

#define EM_DECODER_STATIC_FLAG_CHECK_BASE_EQ_DST(si) \
                            EM_DECODER_FLAGS_FLAG_CHECK_BASE_EQ_DST((si)->flags)

#define EM_DECODER_STATIC_FLAG_SPECULATION(si) \
                            EM_DECODER_FLAGS_FLAG_SPECULATION((si)->flags)

#define EM_DECODER_STATIC_FLAG_POSTINCREMENT(si) \
                            EM_DECODER_FLAGS_FLAG_POSTINCREMENT((si)->flags)

#define EM_DECODER_STATIC_FLAG_FALSE_PRED_EXEC(si) \
                            EM_DECODER_FLAGS_FLAG_FALSE_PRED_EXEC((si)->flags)

#define EM_DECODER_STATIC_FLAG_BR_HINT(si) \
                            EM_DECODER_FLAGS_FLAG_BR_HINT((si)->flags)

#define EM_DECODER_STATIC_FLAG_BR(si) \
                            EM_DECODER_FLAGS_FLAG_BR((si)->flags)

#define EM_DECODER_STATIC_FLAG_GROUP_FIRST(si) \
                            EM_DECODER_FLAGS_FLAG_GROUP_FIRST((si)->flags)

#define EM_DECODER_STATIC_FLAG_GROUP_LAST(si) \
                            EM_DECODER_FLAGS_FLAG_GROUP_LAST((si)->flags)

#define EM_DECODER_STATIC_FLAG_CHECK_SAME_DSTS(si) \
                            EM_DECODER_FLAGS_FLAG_CHECK_SAME_DSTS((si)->flags)

#define EM_DECODER_STATIC_FLAG_CONTROL_TRANSFER(si) \
                            EM_DECODER_FLAGS_FLAG_CONTROL_TRANSFER((si)->flags)

#ifdef __cplusplus
}
#endif

#endif /*** EM_DECODER_H ***/









/***                                                                  ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                      ***/
/***                                                                  ***/
/***   This software is supplied under the terms of a license         ***/
/***   agreement or nondisclosure agreement with Intel Corporation    ***/
/***   and may not be copied or disclosed except in accordance with   ***/
/***   the terms of that agreement.                                   ***/
/***   Copyright (c) 1992-1997  Intel Corporation.                    ***/
/***                                                                  ***/

#ifndef IA_DISASM_H
#define IA_DISASM_H

#include <deceia.h>

typedef enum
{
    IA_DIS_NO_ERROR=0,
    IA_DIS_NULL_PTR,
    IA_DIS_SHORT_ASCII_INST_BUF,
    IA_DIS_SHORT_BIN_INST_BUF,
    IA_DIS_SHORT_SYMBOL_BUF,
    IA_DIS_UNALIGNED_INST,
    IA_DIS_NO_SYMBOL,
    IA_DIS_INVALID_OPCODE,
    IA_DIS_FIRST_FATAL_ERROR,
    IA_DIS_INVALID_MACHINE_TYPE,
    IA_DIS_INVALID_MACHINE_MODE,
    IA_DIS_INVALID_RADIX,
    IA_DIS_INVALID_STYLE,
    IA_DIS_INVALID_ALIASES, 
    IA_DIS_INTERNAL_ERROR,
    IA_DIS_ERROR_LAST
} IA_Dis_Err;

typedef enum
{
    IA_DIS_RADIX_NO_CHANGE=0,
    IA_DIS_RADIX_BINARY,
    IA_DIS_RADIX_OCTAL,
    IA_DIS_RADIX_DECIMAL,
    IA_DIS_RADIX_HEX,	  /* print negative immediate in decimal */
	IA_DIS_RADIX_HEX_FULL,	/* print negative immediate in hex */
    IA_DIS_RADIX_LAST
} IA_Dis_Radix;

typedef enum
{
    IA_DIS_STYLE_NO_CHANGE=0,
    IA_DIS_STYLE_USL,
    IA_DIS_STYLE_MASM,
    IA_DIS_STYLE_LAST
} IA_Dis_Style;

typedef enum
{
    IA_DIS_FIELD_NONE,
    IA_DIS_FIELD_ADDR_SYM,
    IA_DIS_FIELD_ADDR_PLUS,
    IA_DIS_FIELD_ADDR_OFFSET,
    IA_DIS_FIELD_ADDR_HEX,
    IA_DIS_FIELD_ADDR_COLON,
    IA_DIS_FIELD_DISP_SYM,
    IA_DIS_FIELD_DISP_PLUS,
    IA_DIS_FIELD_DISP_OFFSET,
    IA_DIS_FIELD_DISP_VALUE,
    IA_DIS_FIELD_MNEM,
    IA_DIS_FIELD_PREFIX,
    IA_DIS_FIELD_INDIRECT,
    IA_DIS_FIELD_OPER_S_LPARENT,
    IA_DIS_FIELD_OPER_S_RPARENT,
    IA_DIS_FIELD_OPER_SIZE,
    IA_DIS_FIELD_PTR_DIRECT,
    IA_DIS_FIELD_OPER_PLUS,
    IA_DIS_FIELD_OPER_MUL,
    IA_DIS_FIELD_OPER,
    IA_DIS_FIELD_COMMA,
    IA_DIS_FIELD_REMARK,
    IA_DIS_FIELD_OPER_IMM,
    IA_DIS_FIELD_OPER_SEGOVR,
    IA_DIS_FIELD_OPER_COLON,
    IA_DIS_FIELD_OPER_OFS,
    IA_DIS_FIELD_OPER_SCALE,    
    IA_DIS_FIELD_OPER_INDEX,
    IA_DIS_FIELD_OPER_BASE,
    IA_DIS_FIELD_OPER_LPARENT,
    IA_DIS_FIELD_OPER_RPARENT,
    IA_DIS_FIELD_OPER_COMMA
} IA_Dis_Field_Type;
      

typedef struct
{
    IA_Dis_Field_Type  type;
    _TCHAR *           first;
    unsigned int       length;
} IA_Dis_Field;

typedef IA_Dis_Field    IA_Dis_Fields[40];


/*****************************************************/
/***   Disassembler Library Functions Prototypes   ***/
/*****************************************************/

IA_Dis_Err      ia_dis_setup(const IA_Decoder_Machine_Type  type,
                             const IA_Decoder_Machine_Mode  mode,
                             const long                     aliases,
                             const IA_Dis_Radix             radix,
                             const IA_Dis_Style             style,
                             IA_Dis_Err               (*client_gen_sym)(const U64 ,
                                                                        unsigned int,
                                                                        char *,
                                                                        unsigned int *,
                                                                        U64 *));

IA_Dis_Err      client_gen_sym(const U64        address,
                               unsigned int     internal_disp_offset,
                               char *           sym_buf,
                               unsigned int *   sym_buf_length,
                               U64 *            offset);

IA_Dis_Err      ia_dis_inst(const U64 *                    address,
                            const IA_Decoder_Machine_Mode  mode,
                            const unsigned char *          bin_inst_buf,
                            const unsigned int             bin_inst_buf_length,
                            unsigned int *                 actual_inst_length,
                            _TCHAR *                       ascii_inst_buf,
                            unsigned int *                 ascii_inst_buf_length,
                            IA_Dis_Fields *                ascii_inst_fields);

const _TCHAR *ia_dis_ver_str(void);                

IA_Dis_Err     ia_dis_ver(long *major, long *minor);

#include <EM_tools.h>
 
void  ia_dis_get_version(EM_library_version_t  * dec_version);

/**********************************************/
/***   Setup Macros                         ***/
/**********************************************/

#define     IA_DIS_RADIX_DEFAULT     IA_DIS_RADIX_HEX
#define     IA_DIS_STYLE_DEFAULT     IA_DIS_STYLE_USL

#define     IA_DIS_ALIAS_NONE           (0x00000000)


#define     IA_DIS_ALIAS_ALL_REGS       IA_DIS_ALIAS_NONE  

#define     IA_DIS_ALIAS_TOGGLE_SPACE   (0x40000000)
#define     IA_DIS_ALIAS_IMM_TO_SYMBOL  (0x20000000)
#define     IA_DIS_ALIAS_NO_CHANGE      (0x80000000)

#define     IA_DIS_FUNC_NO_CHANGE       (NULL)

/***********************************/
/***   Enviroment Setup Macros   ***/
/***********************************/

#define     IA_DIS_SET_MACHINE_TYPE(type)                                              \
{                                                                                      \
   ia_dis_setup((type), IA_DECODER_MODE_NO_CHANGE, IA_DIS_ALIAS_NO_CHANGE,              \
             IA_DIS_RADIX_NO_CHANGE, IA_DIS_STYLE_NO_CHANGE, IA_DIS_FUNC_NO_CHANGE);   \
}
#define     IA_DIS_SET_MACHINE_MODE(mode)                                              \
{                                                                                      \
   ia_dis_setup(IA_DECODER_CPU_NO_CHANGE, (mode), IA_DIS_ALIAS_NO_CHANGE,             \
             IA_DIS_RADIX_NO_CHANGE, IA_DIS_STYLE_NO_CHANGE, IA_DIS_FUNC_NO_CHANGE);   \
}
#define     IA_DIS_SET_ALIASES(aliases)                                                \
{                                                                                      \
   ia_dis_setup(IA_DECODER_CPU_NO_CHANGE, IA_DECODER_MODE_NO_CHANGE, (aliases),        \
             IA_DIS_RADIX_NO_CHANGE, IA_DIS_STYLE_NO_CHANGE, IA_DIS_FUNC_NO_CHANGE);   \
}
#define     IA_DIS_SET_RADIX(radix)                                                    \
{                                                                                      \
   ia_dis_setup(IA_DECODER_CPU_NO_CHANGE, IA_DECODER_MODE_NO_CHANGE,                   \
             IA_DIS_ALIAS_NO_CHANGE, (radix), IA_DIS_STYLE_NO_CHANGE,                  \
             IA_DIS_FUNC_NO_CHANGE);                                                   \
}
#define     IA_DIS_SET_STYLE(style)                                                    \
{                                                                                      \
   ia_dis_setup(IA_DECODER_CPU_NO_CHANGE, IA_DECODER_MODE_NO_CHANGE,                   \
             IA_DIS_ALIAS_NO_CHANGE, IA_DIS_RADIX_NO_CHANGE, (style),                  \
             IA_DIS_FUNC_NO_CHANGE);                                                   \
}
#define     IA_DIS_SET_CLIENT_FUNC(client_gen_sym)                                     \
{                                                                                      \
   ia_dis_setup(IA_DECODER_CPU_NO_CHANGE, IA_DECODER_MODE_NO_CHANGE,                   \
             IA_DIS_ALIAS_NO_CHANGE, IA_DIS_RADIX_NO_CHANGE, IA_DIS_STYLE_NO_CHANGE,   \
             (client_gen_sym));                                                        \
}

#endif  /*IA_DISASM_H*/




/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright (c) 1992,1993,1994,1995,1996,1997,1998 Intel Corporation. ***/
/***                                                                       ***/

#ifndef EM_DISASM_H
#define EM_DISASM_H

#include "decem.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    EM_DIS_NO_ERROR=0,
    EM_DIS_INVALID_SLOT_BRANCH_INST,
    EM_DIS_MUST_BE_GROUP_LAST,
    EM_DIS_REGISTER_VALUE_OUT_OF_RANGE,
    EM_DIS_BASE_EQUAL_DEST,
    EM_DIS_EQUAL_DESTS,
	EM_DIS_ODD_EVEN_DESTS,
    EM_DIS_REGISTER_RESERVED_VALUE,
    EM_DIS_IMMEDIATE_VALUE_OUT_OF_RANGE,
    EM_DIS_IMMEDIATE_INVALID_VALUE,
	EM_DIS_STACK_FRAME_SIZE_OUT_OF_RANGE,
    EM_DIS_LOCALS_SIZE_LARGER_STACK_FRAME,
    EM_DIS_ROTATING_SIZE_LARGER_STACK_FRAME,
	EM_DIS_WRITE_TO_ZERO_REGISTER,
	EM_DIS_WRITE_TO_SPECIAL_FP_REGISTER,
	EM_DIS_HARD_CODED_PREDICATE_INVALID_VALUE,
    EM_DIS_FIRST_FATAL_ERROR,
    EM_DIS_NULL_PTR = EM_DIS_FIRST_FATAL_ERROR,
    EM_DIS_SHORT_ASCII_INST_BUF,
    EM_DIS_SHORT_BIN_INST_BUF,
    EM_DIS_SHORT_SYMBOL_BUF,
    EM_DIS_UNALIGNED_INST,
    EM_DIS_NO_SYMBOL,
    EM_DIS_INVALID_OPCODE,
    EM_DIS_IGNORED_OPCODE,
    EM_DIS_INVALID_SLOT,
    EM_DIS_INVALID_TEMPLATE,
    EM_DIS_INVALID_MACHINE_TYPE,
    EM_DIS_INVALID_MACHINE_MODE,
    EM_DIS_INVALID_RADIX,
    EM_DIS_INVALID_STYLE,
    EM_DIS_INTERNAL_ERROR,
    EM_DIS_LAST_ERROR
} EM_Dis_Err;

typedef enum
{
    EM_DIS_RADIX_NO_CHANGE=0,
    EM_DIS_RADIX_BINARY,
    EM_DIS_RADIX_OCTAL,
    EM_DIS_RADIX_DECIMAL,
    EM_DIS_RADIX_HEX,
    EM_DIS_RADIX_LAST
} EM_Dis_Radix;

typedef enum
{
    EM_DIS_STYLE_NO_CHANGE=0,
    EM_DIS_STYLE_USL,
    EM_DIS_STYLE_MASM,
    EM_DIS_STYLE_LAST
} EM_Dis_Style;



typedef enum
{
    EM_DIS_FIELD_NONE,
    EM_DIS_FIELD_ADDR_SYM,
    EM_DIS_FIELD_ADDR_PLUS,
    EM_DIS_FIELD_ADDR_OFFSET,
    EM_DIS_FIELD_ADDR_HEX,
    EM_DIS_FIELD_ADDR_COLON,
    EM_DIS_FIELD_DISP_SYM,
    EM_DIS_FIELD_DISP_PLUS,
    EM_DIS_FIELD_DISP_OFFSET,
    EM_DIS_FIELD_DISP_VALUE,
    EM_DIS_FIELD_MNEM,
    EM_DIS_FIELD_OPER,
    EM_DIS_FIELD_COMMA,
    EM_DIS_FIELD_REMARK,
    EM_DIS_FIELD_OPER_IMM,
    EM_DIS_FIELD_OPER_SEGOVR,
    EM_DIS_FIELD_OPER_COLON,
    EM_DIS_FIELD_OPER_OFS,
    EM_DIS_FIELD_OPER_SCALE,    
    EM_DIS_FIELD_OPER_INDEX,
    EM_DIS_FIELD_OPER_BASE,
    EM_DIS_FIELD_OPER_LPARENT,
    EM_DIS_FIELD_OPER_RPARENT,
    EM_DIS_FIELD_OPER_COMMA,

    EM_DIS_FIELD_PREG_LPARENT,
    EM_DIS_FIELD_PREG_RPARENT,
    EM_DIS_FIELD_PREG_REG,
    EM_DIS_FIELD_BUNDLE_START,
    EM_DIS_FIELD_BUNDLE_END,
    EM_DIS_FIELD_GROUP_SEPERATE,
    EM_DIS_FIELD_EQUAL,
    EM_DIS_FIELD_MEM_BASE,
    EM_DIS_FIELD_MEM_LPARENT,
    EM_DIS_FIELD_MEM_RPARENT,
    EM_DIS_FIELD_RF_LPARENT,
    EM_DIS_FIELD_RF_RPARENT,
    EM_DIS_FIELD_RF_REG_FILE,
    EM_DIS_FIELD_RF_INDEX,
    EM_DIS_FIELD_LESS,
    EM_DIS_FIELD_GREATER,
    EM_DIS_FIELD_VALUE,
    EM_DIS_FIELD_LAST
      
} EM_Dis_Field_Type;
      

typedef struct
{
    EM_Dis_Field_Type  type;
    char *             first;
    unsigned int       length;
} EM_Dis_Field;

typedef EM_Dis_Field    EM_Dis_Fields[120];

#ifdef WINNT
#define WIN_CDECL __cdecl
#else
#define WIN_CDECL
#endif

/*****************************************************/
/***   Disassembler Library Functions Prototypes   ***/
/*****************************************************/

EM_Dis_Err      WIN_CDECL em_dis_setup(const EM_Decoder_Machine_Type  type,
                                       const EM_Decoder_Machine_Mode  mode,
                                       const unsigned long            print_flags,
                                       const EM_Dis_Radix             radix,
                                       const EM_Dis_Style             style,
                                       EM_Dis_Err                    (*em_client_gen_sym)(U64 ,
                                                                      char *,
                                                                      int *,
                                                                      U64 *));

EM_Dis_Err      em_client_gen_sym(const U64         address,
                               char *           sym_buf,
                               unsigned int *   sym_buf_length,
                               U64 *            offset);

EM_Dis_Err      WIN_CDECL em_dis_inst(const U64 *                   syl_location,
                                      const EM_Decoder_Machine_Mode mode,
                                      const unsigned char *         bin_inst_buf,
                                      const unsigned int            bin_inst_buf_length,
                                      unsigned int *                actual_inst_length,
                                      char *                        ascii_inst_buf,
                                      unsigned int *                ascii_inst_buf_length,
                                      EM_Dis_Fields *               ascii_inst_fields);

EM_Dis_Err      WIN_CDECL em_dis_bundle(const U64 *                   location,
                                        const EM_Decoder_Machine_Mode mach_mode,
                                        const unsigned char *         bin_bundle_buf,
                                        const unsigned int            bin_bundle_buf_length,
                                        char *                        ascii_bundle_buf,
                                        unsigned int *                ascii_bundle_buf_length,
                                        EM_Dis_Fields *               ascii_bundle_fields);

const char* WIN_CDECL em_dis_err_str(EM_Dis_Err error);

void  WIN_CDECL em_dis_get_version(EM_library_version_t *dis_version);

/**********************************************/
/***   Setup Macros                         ***/
/**********************************************/

#define     EM_DIS_RADIX_DEFAULT     EM_DIS_RADIX_HEX
#define     EM_DIS_STYLE_DEFAULT     EM_DIS_STYLE_USL

#define EM_DIS_FLAG_ALIAS_CR         0x00000001
#define EM_DIS_FLAG_ALIAS_AR         0x00000002
#define EM_DIS_FLAG_ALIAS_R          0x00000004
#define EM_DIS_FLAG_ALIAS_F          0x00000008
#define EM_DIS_FLAG_ALIAS_B          0x00000010
#define EM_DIS_FLAG_C_STYLE_RADIX    0x00000100
#define EM_DIS_FLAG_EXCLUDE_ADDRESS  0x00001000
#define EM_DIS_FLAG_EXCLUDE_BUNDLE   0x00002000
#define EM_DIS_FLAG_EXCLUDE_GROUP    0x00004000
#define EM_DIS_FLAG_PRINT_PREDICATE  0x00008000
#define EM_DIS_FLAG_REG_ALIASES  (EM_DIS_FLAG_ALIAS_CR | EM_DIS_FLAG_ALIAS_AR | \
                                  EM_DIS_FLAG_ALIAS_R  | EM_DIS_FLAG_ALIAS_F  | \
                                  EM_DIS_FLAG_ALIAS_B)
#define EM_DIS_FLAG_DEFAULT          EM_DIS_FLAG_REG_ALIASES
#define EM_DIS_FLAG_EXCLUDE_ADDRESS_CALLBACK 0x00010000
#define EM_DIS_FLAG_NO_CHANGE        0x80000000

#define EM_DIS_FUNC_NO_CHANGE        (NULL)

#define EM_DIS_ERROR_IS_NOT_FATAL(_err)                  \
   (_err < EM_DIS_FIRST_FATAL_ERROR)

/***********************************/
/***   Enviroment Setup Macros   ***/
/***********************************/

#define     EM_DIS_SET_MACHINE_TYPE(type)                                              \
{                                                                                      \
   em_dis_setup((type), EM_DECODER_CPU_NO_CHANGE, EM_DIS_FLAG_NO_CHANGE,               \
             EM_DIS_RADIX_NO_CHANGE, EM_DIS_STYLE_NO_CHANGE, EM_DIS_FUNC_NO_CHANGE);   \
}

#define     EM_DIS_SET_MACHINE_MODE(mode)                                              \
{                                                                                      \
   em_dis_setup(EM_DECODER_MODE_NO_CHANGE, (mode), EM_DIS_FLAG_NO_CHANGE,              \
             EM_DIS_RADIX_NO_CHANGE, EM_DIS_STYLE_NO_CHANGE, EM_DIS_FUNC_NO_CHANGE);   \
}

#define     EM_DIS_SET_PRINT_FLAGS(flags)                                              \
{                                                                                      \
   em_dis_setup(EM_DECODER_MODE_NO_CHANGE, EM_DECODER_CPU_NO_CHANGE, (flags),          \
             EM_DIS_RADIX_NO_CHANGE, EM_DIS_STYLE_NO_CHANGE, EM_DIS_FUNC_NO_CHANGE);   \
}

#define     EM_DIS_SET_RADIX(radix)                                                    \
{                                                                                      \
   em_dis_setup(EM_DECODER_MODE_NO_CHANGE, EM_DECODER_CPU_NO_CHANGE,                   \
             EM_DIS_FLAG_NO_CHANGE, (radix), EM_DIS_STYLE_NO_CHANGE,                   \
             EM_DIS_FUNC_NO_CHANGE);                                                   \
}

#define     EM_DIS_SET_STYLE(style)                                                    \
{                                                                                      \
   em_dis_setup(EM_DECODER_MODE_NO_CHANGE, EM_DECODER_CPU_NO_CHANGE,                   \
             EM_DIS_FLAG_NO_CHANGE, EM_DIS_RADIX_NO_CHANGE, (style),                   \
             EM_DIS_FUNC_NO_CHANGE);                                                   \
}

#define     EM_DIS_SET_CLIENT_FUNC(client_gen_sym)                                     \
{                                                                                      \
   em_dis_setup(EM_DECODER_MODE_NO_CHANGE, EM_DECODER_CPU_NO_CHANGE,                   \
             EM_DIS_FLAG_NO_CHANGE, EM_DIS_RADIX_NO_CHANGE, EM_DIS_STYLE_NO_CHANGE,    \
             (client_gen_sym));                                                        \
}

#define EM_DIS_NEXT(Addr, Size)         \
{                                       \
   U32 syl_size;                        \
   IEL_CONVERT1(syl_size, (Size));      \
   IEL_ADDU((Addr), syl_size, (Addr));  \
}

#define EM_DIS_EM_NEXT(Addr,SlotSize)                   \
{                                                       \
   int slot = EM_IL_GET_SLOT_NO(Addr);                  \
   switch (slot)                                        \
   {                                                    \
     case 0:                                            \
       IEL_INCU(Addr);                                  \
       break;                                           \
     case 1:                                            \
       IEL_INCU(Addr);                                  \
       if ((SlotSize) != 2)                             \
       {                                                \
           break;                                       \
       } /*** else fall-through ***/                    \
     case 2:                                            \
     {                                                  \
         U32 syl_size;                                  \
         IEL_CONVERT1(syl_size, EM_BUNDLE_SIZE-2);      \
         IEL_ADDU((Addr), syl_size, (Addr));            \
     }                                                  \
       break;                                           \
   }                                                    \
}

#ifdef __cplusplus
}
#endif

#endif  /*EM_DISASM_H*/













/***                                                                  ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                      ***/
/***                                                                  ***/
/***   This software is supplied under the terms of a license         ***/
/***   agreement or nondisclosure agreement with Intel Corporation    ***/
/***   and may not be copied or disclosed except in accordance with   ***/
/***   the terms of that agreement.                                   ***/
/***   Copyright (c) 1992,1993,1994,1995,1996,1997 Intel Corporation. ***/
/***                                                                  ***/

#ifndef EM_H
#define EM_H

/*** UPDATED TO 2.4 draft ***/

#include "iel.h"
#include "EM_hints.h"

/*****************************************************************************/
/***                                                                       ***/
/***       Enhanced mode architecture constants and macros                 ***/
/***                                                                       ***/
/***  NOTE: this header files assumes that the following typedef's exist:  ***/
/***        U8, U16, U32, U64. iel.h contains these typedefs, but one      ***/
/***        may want to define them differently.                           ***/
/***                                                                       ***/
/*****************************************************************************/

#define EM_BUNDLE_SIZE              16
#define EM_SYLLABLE_BITS            41
#define EM_DISPERSAL_POS            0
#define EM_DISPERSAL_BITS           5
#define EM_SBIT_POS                 0
#define EM_TEMPLATE_POS             1
#define EM_TEMPLATE_BITS            4
#define EM_NUM_OF_TEMPLATES         (1<<EM_TEMPLATE_BITS) 
                                        /*** including the reserved!!! ***/
#define EM_SYL2_POS                 (EM_DISPERSAL_POS+EM_DISPERSAL_BITS)

#define EM_MAJOR_OPCODE_POS         37
#define EM_MAJOR_OPCODE_BITS        4
#define EM_PREDICATE_POS            0
#define EM_PREDICATE_BITS           6

#define EM_IL_SLOT_BITS             4
#define EM_IL_SLOTS_MASK32      ((1<<EM_IL_SLOT_BITS)-1)

typedef enum
{
    EM_SLOT_0=0,
    EM_SLOT_1=1,
    EM_SLOT_2=2,
    EM_SLOT_LAST=3
} EM_slot_num_t;

/****************************************************************************/
/*** the following macros needs iel. Bundle assumed to be in U128         ***/

/*** Bring syllable binary to bits 0-40 of Syl; DO NOT mask off bits 41   ***/
/*** and on (for movl, can use Slot=2 but then Maj.Op. is in bits 79-82!) ***/

#define EM_GET_SYLLABLE(syl,bundle,slot) \
        IEL_SHR((syl), (bundle), EM_SYL2_POS+(slot)*EM_SYLLABLE_BITS)

#define EM_GET_TEMPLATE(bundle) \
        ((IEL_GETDW0(bundle) >> EM_TEMPLATE_POS) & ((1<<EM_TEMPLATE_BITS)-1))

#define EM_TEMPLATE_IS_RESERVED(templt) \
(((templt)==3)||((templt)==10)||((templt)==13)||((templt)==15))

#define EM_IL_GET_BUNDLE_ADDRESS(il,addr) \
        IEL_CONVERT2((addr),IEL_GETDW0(il) & \
                            (unsigned int)(~EM_IL_SLOTS_MASK32), \
                     IEL_GETDW1(il))

#define EM_IL_GET_SLOT_NO(il)  (IEL_GETDW0(il) & EM_IL_SLOTS_MASK32)

#define EM_IL_SET(il,addr,slot) \
        IEL_CONVERT2((il), IEL_GETDW0(addr) | (slot), IEL_GETDW1(addr))

#define EM_IS_IGNORED_SQUARE(template_role, major_opcode)       \
        (((template_role) == EM_TROLE_BR) &&                    \
         (major_opcode == 3 || major_opcode == 6) ? 1 : 0)

/*****************************************************************************/

typedef U64     EM_IL;     /* Instruction (syllable) Location */

typedef enum em_branch_type_s
{
    EM_branch_type_none          = 0x0,
    EM_branch_type_direct_cond   = 0x1,
    EM_branch_type_direct_wexit  = 0x2,
    EM_branch_type_direct_wtop   = 0x3,   
    EM_branch_type_direct_cloop  = 0x4,   
    EM_branch_type_direct_cexit  = 0x5,   
    EM_branch_type_direct_ctop   = 0x6,   
    EM_branch_type_direct_call   = 0x7,   
    EM_branch_type_direct_last   = 0x8,   
    EM_branch_type_indirect_cond = 0x9,
    EM_branch_type_indirect_ia   = 0xa,
    EM_branch_type_indirect_ret  = 0xb,
    EM_branch_type_indirect_call = 0xc,
    EM_branch_type_last
} EM_branch_type_t;

typedef enum em_cmp_type_s
{
    EM_cmp_type_none     = 0,  /* none (dstT=REL, dstF=!REL)        */
    EM_cmp_type_and      = 1,  /* and (dstT&=REL, dstF&=REL)        */
    EM_cmp_type_or       = 2,  /* or (dstT|=REL, dstF|=REL)         */
    EM_cmp_type_unc      = 3,  /* uncond (dstT=P&REL, dstF=P&!REL)  */
    EM_cmp_type_or_andcm = 4,  /* or.andcm (dstT|=REL, dstF&=!REL)  */
    EM_cmp_type_last
} EM_cmp_type_t;

typedef enum EM_template_e
{
    EM_template_mii   = 0,
    EM_template_mi_i  = 1,
    EM_template_mli   = 2,
    /*** 3  reserved ***/
    EM_template_mmi   = 4,
    EM_template_m_mi  = 5,
    EM_template_mfi   = 6,
    EM_template_mmf   = 7,
    EM_template_mib   = 8,
    EM_template_mbb   = 9,
    /*** 10 reserved ***/
    EM_template_bbb   = 11,
    EM_template_mmb   = 12,
    /*** 13 reserved ***/
    EM_template_mfb   = 14,
    /*** 15 reserved ***/
    EM_template_last 
} EM_template_t;


/***** Misc operands values: fclass, sync (stype), mux *****/

typedef enum em_fclass_bit
{
    EM_fclass_bit_pos    = 0 ,
    EM_fclass_bit_neg    = 1 ,
    EM_fclass_bit_zero   = 2 ,
    EM_fclass_bit_unorm  = 3 ,
    EM_fclass_bit_norm   = 4 ,
    EM_fclass_bit_inf    = 5 ,
    EM_fclass_bit_signan = 6 ,
    EM_fclass_bit_qnan   = 7 ,
    EM_fclass_bit_nat    = 8
} EM_fclass_bit_t;
    
#define EM_FCLASS_POS           (1<<EM_fclass_bit_pos   )  /*** 0x001 ***/
#define EM_FCLASS_NEG           (1<<EM_fclass_bit_neg   )  /*** 0x002 ***/
#define EM_FCLASS_ZERO          (1<<EM_fclass_bit_zero  )  /*** 0x004 ***/
#define EM_FCLASS_UNORM         (1<<EM_fclass_bit_unorm )  /*** 0x008 ***/
#define EM_FCLASS_NORM          (1<<EM_fclass_bit_norm  )  /*** 0x010 ***/
#define EM_FCLASS_INF           (1<<EM_fclass_bit_inf   )  /*** 0x020 ***/
#define EM_FCLASS_SIGNAN        (1<<EM_fclass_bit_signan)  /*** 0x040 ***/
#define EM_FCLASS_QNAN          (1<<EM_fclass_bit_qnan  )  /*** 0x080 ***/
#define EM_FCLASS_NAT           (1<<EM_fclass_bit_nat   )  /*** 0x100 ***/

#define EM_MUX_BRCST              0x0
#define EM_MUX_MIX                0x8
#define EM_MUX_SHUF               0x9
#define EM_MUX_ALT                0xA
#define EM_MUX_REV                0xB

/*** sync-stype: arbitrary! need fix!!! <flags> ***/ /* !!!!!??? */
#define EM_STYPE_LOAD             0x0
#define EM_STYPE_STORE            0x1
#define EM_STYPE_EXT              0x2
#define EM_STYPE_PURGE            0x4

#define EM_MAX_MEM_OPERAND_SIZE     32

#define EM_NUM_OF_PRIVILEGE_LEVELS  4

/*** shift of branch/chk (target21 <--> target25) ***/
#define EM_IPREL_TARGET_SHIFT_AMOUNT    4

/*****************************************************/
/***** register-related constants and structures *****/
/*****************************************************/



/* branch register structure definition */

typedef U64 EM_branch_reg_t;

typedef struct EM_FPSR_s
{
    U4byte trap:6,sf0:13,sf1:13;
    U4byte sf2:13,sf3:13,reserved:6;
} EM_FPSR_t;

#define EM_FPSR_S0_ABSOLUTE_MASK           0x7f
#define EM_FPSR_SFX_TD_MASK                0x40
#define EM_FPSR_SFX_PC_MASK                0x0c
#define EM_FPSR_SFX_RESERVED_PC_VALUE      0x04

typedef struct EM_RSC_s
{
    U4byte mode:2, pl:2, be:1, reserved1:11, loadrs:14, reserved2:2;
    U4byte reserved3;
} EM_RSC_t;

typedef struct EM_BSP_s
{
    U4byte ignored:3,pointer_low:29;
    U4byte pointer_high;
} EM_BSP_t;

typedef EM_BSP_t EM_BSPSTORE_t;

typedef struct EM_EC_s
{
    U4byte count:6, ignored1:26;
    U4byte ignored2;
} EM_EC_t;

typedef struct EM_RNAT_s
{
    U4byte rse_nats_low;
    U4byte rse_nats_high:31, ignored:1;
} EM_RNAT_t;

#define EM_FRAME_RRB_MASK_LOW   0x3ffff
#define EM_FRAME_RRB_MASK_HIGH  0xffffffc0
#define EM_FRAME_FP_RRB_MASK 0xfe000000
#define EM_FRAME_FP_RRB_BIT_POS 25
#define EM_FRAME_RRB_FR_LOW_BIT_MASK 0x2000000
#define EM_FRAME_SOL_POS 7
#define EM_FRAME_SOL_MASK 0x3f80
#define EM_FRAME_SOR_POS 14
#define EM_FRAME_SOR_MASK 0x3c000
#define EM_FRAME_SOR_ZERO_BITS_NUM 3
#define EM_FRAME_SOF_MASK 0x7f

typedef struct EM_frame_marker_s
{
    U4byte sof:7, sol:7, sor:4, rrb_int:7, rrb_fp:7;
    U4byte rrb_pred:6, reserved:26;
} EM_frame_marker_t;

#define EM_PFS_CPL_BIT_POS 30
#define EM_PFS_HIGH_EC_BIT_POS 20
#define EM_PFS_CPL_MASK 0xc0000000
#define EM_PFS_EC_MASK 0x03f00000

typedef struct EM_PFS_s
{
    U4byte pfm_l;
    U4byte pfm_h:6, reserved1:14, pec:6, reserved2:4, ppl:2;
} EM_PFS_t;

#define EM_PSR_CPL_MASK      0x3
#define EM_PSR_CPL_BIT_POS   0
#define EM_PSR_H_ED_MASK     0x800
#define EM_PSR_H_ED_BIT_POS  0xb
#define EM_PSR_H_MC_MASK     0x8
#define EM_PSR_H_IT_MASK     0x10

/*** PSR ***/
typedef struct EM_PSR_s
{
    U4byte
    reserved1:1, /*  0 */
    be:1, /*  1 */
    up:1, /*  2 */
    ac:1, /*  3 */
    mfl:1,/*  4 */
    mfh:1,/*  5 */
    reserved2:7,/* 6-12 */
    ic:1, /* 13 */
    i:1,  /* 14 */
    pk:1, /* 15 */
    reserved3:1, /* 16 */ 
    dt:1, /* 17 */
    dfl:1,/* 18 */ 
    dfh:1,/* 19 */
    sp:1, /* 20 */
    pp:1, /* 21 */
    di:1, /* 22 */
    si:1, /* 23 */
    db:1, /* 24 */
    lp:1, /* 25 */
    tb:1, /* 26 */
    rt:1, /* 27 */
    reserved4:4; /* 28-31 */
    U4byte
    cpl:2, /* 32,33 */
    is:1,  /* 34 */
    mc:1,  /* 35 */
    it:1,  /* 36 */
    id:1,  /* 37 */
    da:1,  /* 38 */
    dd:1,  /* 39 */
    ss:1,  /* 40 */
    ri:2,  /* 41,42 */
    ed:1,  /* 43 */
    bn:1,  /* 44 */
  reserved5:19; /* 45-63 */
} EM_PSR_t;

/*** DCR ***/
typedef struct EM_DCR_s
{
    U4byte
    pp:1, /*  0 */
    be:1, /*  1 */
    lc:1, /*  2 */
    reserved1:5, /* 3-7 */ 
    dm:1, /*  8 */
    dp:1, /*  9 */
    dk:1, /* 10 */
    dx:1, /* 11 */
    dr:1, /* 12 */
    da:1, /* 13 */
    dd:1, /* 14 */
    reserved4:1,  /* 15 , - was 'du' before eas24 and became reserved */
    reserved2:16; /* 16-31 */
    U4byte reserved3; /* 32-63 */
} EM_DCR_t;

/**** ITM ****/
typedef U64 EM_ITM_t;

/**** IVA ****/
typedef struct EM_IVA_s
{
    U4byte
    ignored:15,
    iva_low:17;
    U4byte iva_high;
} EM_IVA_t;

/**** PTA ****/
typedef struct EM_pta_s
{
    U4byte
    ve:1,        /*  0    */
    reserved1:1, /*  1    */ 
    size:6,      /* 2-7   */ 
    vf:1,        /*  8    */
    reserved2:6, /* 9-14  */  
    base_low:17; /* 15-31 */
    U4byte
    base_high:32;/* 32-63 */
} EM_PTA_t;

/**** IPSR ****/
typedef EM_PSR_t EM_IPSR_t;

/**** ISR ****/
typedef struct EM_ISR_s
{
    U4byte
    code:16,     /*  0-15 */ 
    iA_vector:8, /* 16-23 */  
    reserved1:8; /* 24-31 */
    U4byte
    x:1,         /* 32 */
    w:1,         /* 33 */
    r:1,         /* 34 */
    na:1,        /* 35 */
    sp:1,        /* 36 */
    rs:1,        /* 37 */
    ir:1,        /* 38 */
    ni:1,        /* 39 */
    so:1,        /* 40 */
    ei:2,        /* 41-42 */
    ed:1,        /* 43 */
    reserved2:20;/* 44-63 */
} EM_ISR_t;

/* Low 12 bits of low word of IFA is defined as 
   ignored according to the TLB insertion format */

typedef struct EM_IFA_s
{
    U4byte
    ignored:12,
    vpn_low:20;
    U4byte
    vpn_high;
} EM_IFA_t;

typedef struct EM_IIP_s
{
    U4byte
    vpn_low;
    U4byte
    vpn_high;
} EM_IIP_t;

typedef EM_IIP_t EM_vaddr_t;

/* Unimplemented virtual and physical addresses */
#define   EM_MAX_IMPL_VA_MSB 60  
#define   EM_MIN_IMPL_VA_MSB 50  
#define   EM_MAX_IMPL_PA_MSB 62  
#define   EM_MIN_IMPL_PA_MSB 31

/**** ITIR ***/
typedef struct EM_itir_s
{
    U4byte
    reserved1:2, /* 0-1 */
    ps:6,        /* 2-7 */
    key:24;      /* 8-31 */ 
    U4byte
    reserved2:16, /* 32-47 */
    reserved4:15, /* 48-62 - was ppn */
    reserved3:1;  /* 63 */
} EM_ITIR_t;

#define EM_ITR_PPN_HIGH_OFFSET 24

/**** IIPA ****/
typedef U64 EM_IIPA_t;

/**** IFS ****/
typedef struct
{
    U4byte
    ifm_low;       /* 0-31 */
    U4byte
    ifm_high:6,    /* 32-37 */
    reserved:25,   /* 38-62 */
    v:1;           /* 63    */
} EM_IFS_t;

/**** IIM ****/
typedef struct
{
    U4byte
    imm21:21,    /*  0-20 */
    ignored1:11; /* 21-31 */  
    U4byte ignored2; /* 32-63 */
} EM_IIM_t;

typedef struct EM_IHA_s
{
    U4byte ignored:3,iha_low:29;
    U4byte iha_high;
} EM_IHA_t;

/**** LID ****/
typedef struct
{
    U4byte
    reserved:16,
    eid:8,
    id:8;
    U4byte ignored;
} EM_LID_t;

/**** IVR ****/
typedef struct
{
    U4byte
    vec:8,       /* not "vector" due to IAS issues */
    reserved:8,
    ignored1:16;
    U4byte ignored2;
} EM_IVR_t;

/**** TPR ****/
typedef struct
{
    U4byte
    ignored1:4,
    mic:4,
    reserved:8,
    mmi:1,
    ignored2:15;
    U4byte ignored3;
} EM_TPR_t;

/**** EOI ****/
typedef struct
{
    U4byte ignored1;
    U4byte ignored2;
} EM_EOI_t;

/**** IRR ****/
typedef U64 EM_IRR_t;

/**** ITV ****/
typedef struct
{
    U4byte
    vec:8,         /* not "vector" due to IAS issues */
    reserved1:4,
    zero:1,
    reserved2:3,
    m:1,
    ignored1:15;
    U4byte ignored2;
} EM_ITV_t;

/**** PMV ****/
typedef EM_ITV_t EM_PMV_t;

/**** LRR ****/
typedef struct
{
    U4byte
    vec:8,         /* not "vector" due to IAS issues */
    dm:3,
    reserved1:1,
    ignored3:1,
    ipp:1,
    reserved2:1,
    tm:1,
    m:1,
    ignored1:15;
    U4byte ignored2;
} EM_LRR_t;

/**** BHB ****/
typedef struct
{
    U4byte
    max:10,
    ignored:2,
    base_low:20;
    U4byte 
    base_high;
} EM_BHB_t;

/**** THA ****/
typedef struct
{
    U4byte
    ptr:10,
    bh:1,
    ignored1:1,
    vaddr:20;
    U4byte 
    ignored2;
} EM_THA_t;

/**** CMCV ****/
typedef EM_ITV_t EM_CMCV_t;


/**** RR ****/
typedef struct EM_region_register_s
{
    U4byte
    ve:1,          /*  0 */
    reserved1:1,   /*  1 */
    ps:6,          /*  2-7 */
    rid:24;        /*  8-31 */ 
    U4byte  reserved2; /* 32-63 */
} EM_RR_t;

/**** PKR ****/
typedef struct EM_key_register_s
{
    U4byte
    v:1,   /* 0 */
    wd:1,  /* 1 */
    rd:1,  /* 2 */
    xd:1,  /* 3 */
    reserved1:4,  /* 4-7  */
    key:24;       /* 8-31 */
    U4byte reserved2; /* 32-63 */
} EM_PKR_t;      

typedef U64 EM_DBR_EVEN_t;
typedef U64 EM_IBR_EVEN_t;

typedef struct EM_DBR_ODD_s
{
    U4byte mask_low;           /* 0 - 31  */
    U4byte mask_high:24,       /* 32 - 55 */
           plm:4,              /* 56 - 59 */
           ignored:2,          /* 60 - 61 */
           w:1,                /* 62      */
           r:1;                /* 63      */
} EM_DBR_ODD_t;

typedef struct EM_IBR_ODD_s
{
    U4byte mask_low;           /* 0 - 31  */
    U4byte mask_high:24,       /* 32 - 55 */
           plm:4,              /* 56 - 59 */
           ignored:3,          /* 60 - 62 */
           x:1;                /* 63      */
} EM_IBR_ODD_t;

/**** PMC Registers ****/
/**** PMC0 ****/

typedef struct EM_PMC0_register_s
{
    U4byte 
    fr:1,                      /*   0     */    
    ignored1:3,                /* 1 - 3   */
    overflow:4,                /* 4 - 7   */
    ignored2:24;               /* 8 - 31  */
    U4byte ignored3;           /* 32 - 63 */
} EM_PMC0_t;

/**** PMC1 -  PMC3 ****/

typedef U64 EM_PMC1_3_t;

/**** PMC4 - ****/

typedef struct EM_PMC_register_s
{
    U4byte
    plm:4,                     /* 0 - 3   */
    ev:1,                      /* 4       */
    oi:1,                      /* 5       */
    pm:1,                      /* 6       */
    ignored1:1,                /* 7       */
    es:8,                      /* 8 - 15  */
    umask:4,                   /* 16 - 19 */
    ignored2:12;               /* 20 - 31 */
    U4byte ignored3;           /* 32 - 63 */
} EM_PMC_t;

typedef struct EM_PMD_register_s
{
    U4byte count;             /* 0 - 31  */
    U4byte sxt;               /* 32 - 63 */
} EM_PMD_t;

/* Number of implementet count bits in PMD */
#define EM_PMD_COUNT_SIZE     32

typedef U64 EM_MSR_t;

typedef struct EM_tlb_insert_reg_s
{
    U4byte
    p:1,        /* 0     */
    mx:1,       /* 1     */
    ma:3,       /* 2-4   */
    a:1,        /* 5     */
    d:1,        /* 6     */
    pl:2,       /* 7-8   */
    ar:3,       /* 9-11  */
    ppn_low:20; /* 12-31 */
    U4byte
    ppn_high:12, /* 32-43 */
    reserved2:4, /* 44-47 - was ppn_high */        
    reserved1:4, /* 48-51 */
    ed:1,        /* 52    */
    ignored:11;  /* 63-63 */
} EM_tlb_insert_reg_t;

typedef enum
{
    EM_TLB_ar_r_r_r,         /* 000 */
    EM_TLB_ar_rx_rx_rx,      /* 001 */
    EM_TLB_ar_rw_rw_rw,      /* 010 */
    EM_TLB_ar_rwx_rwx_rwx,   /* 011 */
    EM_TLB_ar_r_rw_rw,       /* 100 */     
    EM_TLB_ar_rx_rx_rwx,     /* 101 */
    EM_TLB_ar_rwx_rw_rw,     /* 110 */
    EM_TLB_ar_x_x_rx,        /* 111 */
    EM_TLB_ar_last
} EM_page_access_right_t;

typedef enum
{                                /* ma  mx */
    EM_VA_MA_WB      = 0x0,      /* 000 0  */
    EM_VA_MA_WT      = 0x4,      /* 010 0 */
    EM_VA_MA_WP      = 0x6,      /* 011 0 */
    EM_VA_MA_UC      = 0x8,      /* 100 0 */
    EM_VA_MA_UCC     = 0x9,      /* 100 1 */
    EM_VA_MA_UCE     = 0xa,      /* 101 0 */
    EM_VA_MA_WC      = 0xc,      /* 110 0 */
    EM_VA_MA_NATPAGE = 0xe       /* 111 0 */
} EM_vaddr_mem_attribute_t;

/* encodings of guest memory attributes */
typedef enum
{
    EM_IA_GVA_MA_UC        = 0x0,      /* 000 */
    EM_IA_GVA_MA_WC        = 0x1,      /* 001 */
    EM_IA_GVA_MA_WT        = 0x4,      /* 100 */
    EM_IA_GVA_MA_WP        = 0x5,      /* 101 */
    EM_IA_GVA_MA_WB        = 0x6,      /* 110 */
    EM_IA_GVA_MA_UC_MINUS  = 0x7       /* 111 */    
} EM_IA_vaddr_gmem_attribute_t;

/* In eas24: attr = ma + mx */
#define EM_VA_IS_MA_ATTRIBUTE_RESERVED(attr)                  \
    (((attr) == 0x2) || ((attr) == 0x4) || ((attr) == 0x6) ||    \
     ((attr) & 0x1))

#define EM_GVA_IS_MA_ATTRIBUTE_RESERVED(attr)                    \
    (((attr) == 0x1) || ((attr) == 0x2) || ((attr) == 0x3) ||    \
     ((attr) == 0x5) || ((attr) == 0x7) || ((attr) == 0xb) ||    \
     ((attr) == 0xd) || ((attr) == 0xf)) 
     
typedef EM_tlb_insert_reg_t EM_vhpt_short_format_t;

typedef struct EM_vhpt_long_format_s
{
    U4byte
    p:1,         /* 0     */
    mx:1,        /* 1     */
    ma:3,        /* 2-4   */
    a:1,         /* 5     */
    d:1,         /* 6     */
    pl:2,        /* 7-8   */
    ar:3,        /* 9-11  */
    ppn_low:20;  /* 12-31 */
    
    U4byte
    ppn_mid:12,  /* 32-43 */
    reserved5:4, /* 44-47 - was ppn_mid*/
    reserved1:4, /* 48-51 */
    ed:1,        /* 52    */ 
    ignored:11;  /* 53-63 */
    
    U4byte
    reserved2:2, /* 0-1   */
    ps:6,        /* 2-7   */
    key:24;      /* 8-31  */
    
    U4byte
    reserved3:16,/* 32-47 */
    reserved6:15, /* 48-62 - was ppn_high */
    reserved4:1; /* 63    */
    
    U64 tag;
    U64 avl3;
} EM_vhpt_long_format_t;

typedef struct EM_gvhpt_short_format_s
{
    U4byte
    p:1,         /* 0     */
    w:1,         /* 1     */
    u:1,         /* 2     */
    pa10:2,      /* 3-4   */
    a:1,         /* 5     */
    d:1,         /* 6     */
    pa2:1,       /* 7     */
    g:1,         /* 8     */
    ignored:3,   /* 9-11  */
    ppn:20;      /* 12 - 31 */     
}EM_gvhpt_short_format_t;

typedef struct EM_gvhpt_long_format_s
{
    U4byte
    p:1,         /* 0     */
    w:1,         /* 1     */
    u:1,         /* 2     */
    pa10:2,      /* 3-4   */
    a:1,         /* 5     */
    d:1,         /* 6     */
    pa2:1,       /* 7     */
    g:1,         /* 8     */
    ignored:3,   /* 9-11  */
    ppn_low:20;  /* 12 - 31 */ 
    U4byte
    ppn_high:16, /* 32-47 */
    reserved:16; /* 48-63 */
}EM_gvhpt_long_format_t;

/* the minimum VHPT size is 2^14 = 16K */
#define EM_MIN_VHPT_SIZE_POWER      14

/* define the number of registers */
#define EM_NUM_OF_GREGS            128
#define EM_NUM_OF_ADD22_GREGS        4
#define EM_NUM_OF_FPREGS           128
#define EM_NUM_OF_PREGS             64
#define EM_NUM_OF_BREGS              8
#define EM_NUM_OF_AREGS            128
#define EM_NUM_OF_CREGS            128
#define EM_NUM_OF_RREGS              8
#define EM_NUM_OF_PKREGS            16
#define EM_NUM_OF_DBREGS            32 /* guess for max value*/
#define EM_NUM_OF_IBREGS            32 /* guess for max value*/
#define EM_NUM_OF_PMCREGS           32 /* guess for max value*/
#define EM_NUM_OF_PMDREGS           32 /* guess for max value*/
#define EM_NUM_OF_MSREGS          2048 /* guess for max value*/
#define EM_NUM_OF_KREGS              8 /* kernel registers are AREGS */
#define EM_NUM_OF_CPUID_REGS         5 /* implementation independent part */
#define EM_NUM_OF_IRREGS             4
#define EM_NUM_OF_BANKED_REGS       16
#define EM_FIRST_BANKED_REG         16
#define EM_FIRST_IN_FP_LOW_REG_SET   0
#define EM_FIRST_IN_FP_HIGH_REG_SET 32

#define EM_PREDICATE_WIRED_TRUE      0
#define EM_STACK_BASE_REGISTER      32
#define EM_REGISTER_STACK_SIZE      96
#define EM_GREG_ROTATING_BASE       32
#define EM_PREG_ROTATING_BASE       16
#define EM_NUM_OF_ROTATING_PREGS    (EM_NUM_OF_PREGS - EM_PREG_ROTATING_BASE)
#define EM_FPREG_ROTATING_BASE      32
#define EM_NUM_OF_ROTATING_FPREGS   (EM_NUM_OF_FPREGS - EM_FPREG_ROTATING_BASE)
#define EM_GREGS_ROTATING_GROUPS    8    

/* kernel registers macros */
#define EM_IS_AREG_A_KREG(n)     (((n) >= EM_AR_KR0) && ((n) <= EM_AR_KR7))
#define EM_AREG_NUM_TO_KREG(n)   ((n) - EM_AR_KR0)
#define EM_KREG_NUM_TO_AREG(n)   ((n) + EM_AR_KR0)
#define EM_IS_KREG_A_AREG(n)     ((n) < 7)

/* PSR user and system mask */
#define EM_PSR_UM_MASK              0x3f
#define EM_PSR_SM_MASK              0xffffff
#define EM_PSR_MFL_MASK             0x10
#define EM_PSR_MFH_MASK             0x20


/* instruction and data TLB translation registers information */
#define EM_TLB_MIN_DATA_TR_NUM         8
#define EM_TLB_MIN_INST_TR_NUM         8
#define EM_TLB_MAX_DATA_TR_NUM       256
#define EM_TLB_MAX_INST_TR_NUM       256
#define EM_TLB_MIN_TLB_TC_NUM          8
#define EM_TLB_MAX_TLB_TC_NUM        256
#define EM_TLB_DATA_TR_NUM_MASK     0xff
#define EM_TLB_INST_TR_NUM_MASK     0xff

/* define the special purpose application registers */
typedef enum
{
    EM_AR_KR0  = 0,
    EM_AR_KR1  = 1,
    EM_AR_KR2  = 2,
    EM_AR_KR3  = 3,
    EM_AR_KR4  = 4,
    EM_AR_KR5  = 5,
    EM_AR_KR6  = 6,
    EM_AR_KR7  = 7,
    /* ar8-15 reserved */
    EM_AR_RSC  = 16,
    EM_AR_BSP  = 17,
    EM_AR_BSPSTORE = 18,
    EM_AR_RNAT = 19,
    /* ar20 reserved */
    EM_AR_FCR  = 21, 
    /* ar22-23 reserved */
    EM_AR_EFLAG  = 24,
    EM_AR_CSD    = 25,
    EM_AR_SSD    = 26,
    EM_AR_CFLG   = 27,
    EM_AR_FSR    = 28,
    EM_AR_FIR    = 29,
    EM_AR_FDR    = 30,
    /* ar31 reserved */
    EM_AR_CCV    = 32,
    /* ar33-35 reserved */
    EM_AR_UNAT = 36,
    /* ar37-39 reserved */
    EM_AR_FPSR = 40,
    /* ar41-43 reserved */
    EM_AR_ITC  = 44,
    /* ar45-47 reserved */
    /* ar48-63 ignored */
    EM_AR_PFS  = 64,
    EM_AR_LC   = 65,
    EM_AR_EC   = 66,
    /* ar67-111 reserved */
    /* ar112-128 ignored */
    EM_AR_LAST = 128
} EM_areg_num_t;

/*****************************/
/*** Control     Registers ***/
/*****************************/
typedef enum
{
    EM_CR_DCR  = 0,
    EM_CR_ITM  = 1,
    EM_CR_IVA  = 2,
    /*** 3-7 reserved ***/
    EM_CR_PTA  = 8,
    EM_CR_GPTA = 9,
    /*** 10-15 reserved ***/
    EM_CR_IPSR = 16,
    EM_CR_ISR  = 17,
    /*** 18 reserved ***/
    EM_CR_IIP  = 19,
    EM_CR_IFA  = 20,
    EM_CR_ITIR = 21,
    EM_CR_IIPA = 22,
    EM_CR_IFS  = 23,
    EM_CR_IIM  = 24,
    EM_CR_IHA  = 25,
    /*** 25-63 reserved ***/
    /*** SAPIC registers ***/
    EM_CR_LID  = 64,
    EM_CR_IVR  = 65,
    EM_CR_TPR  = 66,
    EM_CR_EOI  = 67,
    EM_CR_IRR0 = 68,
    EM_CR_IRR1 = 69,
    EM_CR_IRR2 = 70,
    EM_CR_IRR3 = 71,
    EM_CR_ITV  = 72,
    EM_CR_PMV  = 73,
    EM_CR_CMCV = 74,
    /*** 75-79 reserved  ***/
    EM_CR_LRR0 = 80,
    EM_CR_LRR1 = 81,
    /*** 82-127 reserved ***/
    EM_CR_LAST = 128
} EM_creg_num_t;

typedef enum
{
    EM_CPUID_VENDOR0    = 0,
    EM_CPUID_VENDOR1    = 1,
    EM_CPUID_SERIAL_NUM = 2,
    EM_CPUID_VERSION    = 3,
    EM_CPUID_FEATURES   = 4,
    EM_CPUID_LAST
} EM_cpuid_num_t;

typedef enum
{
    EM_GR_BHB = 6,
    EM_GR_THA = 7
} EM_greg_num_t;

typedef struct EM_CPUID_version_s
{
    U4byte
        number:8,
        revision:8,
        model:8,
        family:8;
    U4byte
        archrev:8,
        reserved1:24;
} EM_CPUID_version_t;

#define EM_NUM_OF_M_ROLE_APP_REGS               64
#define EM_NUM_OF_I_ROLE_APP_REGS       (EM_NUM_OF_AREGS - \
                                         EM_NUM_OF_M_ROLE_APP_REGS)

#define EM_APP_REG_IS_I_ROLE(ar_no)     ((ar_no) >= EM_NUM_OF_M_ROLE_APP_REGS)

#define EM_APP_REG_IS_RESERVED(ar_no)   ((((ar_no) > 7)  && ((ar_no) < 16)) ||\
                                         (((ar_no) > 19) && ((ar_no) < 21)) ||\
                                         (((ar_no) > 21) && ((ar_no) < 24)) ||\
                                         (((ar_no) > 30) && ((ar_no) < 32)) ||\
                                         (((ar_no) > 32) && ((ar_no) < 36)) ||\
                                         (((ar_no) > 36) && ((ar_no) < 40)) ||\
                                         (((ar_no) > 40) && ((ar_no) < 44)) ||\
                                         (((ar_no) > 44) && ((ar_no) < 48)) ||\
                                         (((ar_no) > 66) && ((ar_no) < 112)))

#define EM_APP_REG_IS_IGNORED(ar_no)    ((((ar_no) > 47)  && ((ar_no) < 64))||\
                                         ((ar_no) > 111))

#define EM_CREG_IS_I_ROLE(cr_no)        0
#define EM_CREG_IS_RESERVED(cr_no)      ((((cr_no) > 2)  && ((cr_no) < 8))  ||\
                                         (((cr_no) > 9)  && ((cr_no) < 16)) ||\
                                         ((cr_no) == 18)                    ||\
                                         (((cr_no) > 25) && ((cr_no) < 64)) ||\
                                         (((cr_no) > 74) && ((cr_no) < 80)) ||\
                                         ((cr_no) > 81))

#define EM_PMD_IS_IMPLEMENTED(pmd_no)    ((pmd_no) > 3 && (pmd_no) < 8)
#define EM_PMC_IS_IMPLEMENTED(pmc_no)    ((pmc_no) < 8)

/* Interruption Priorities, taken from Table 10-5 in EAS2.4. */
typedef enum EM_interruption_e
{
    EM_INTR_NONE                                  = 0,
    /* Aborts: IA32, IA64 */
    EM_INTR_MACHINE_RESET                         = 1,
    EM_INTR_MACHINE_CHECK_ABORT                   = 2,
    /* Interrupts: IA32, IA64 */
    EM_INTR_PLATFORM_MANAGEMENT_INTERRUPT         = 3,
    EM_INTR_EXTERNAL_INTERRUPT                    = 4,
    /* Faults: IA64 */
    EM_INTR_IR_UNIMPLEMENTED_DATA_ADDRESS_FAULT   = 5,
    EM_INTR_IR_DATA_NESTED_TLB_FAULT              = 6,
    EM_INTR_IR_ALT_DATA_TLB_FAULT                 = 7,
    EM_INTR_IR_VHPT_DATA_FAULT                    = 8,
    EM_INTR_IR_DATA_TLB_FAULT                     = 9,
    EM_INTR_IR_DATA_PAGE_NOT_PRESENT_FAULT        = 10,
    EM_INTR_IR_DATA_NAT_PAGE_CONSUMPTION_FAULT    = 11,
    EM_INTR_IR_DATA_KEY_MISS_FAULT                = 12,
    EM_INTR_IR_DATA_KEY_PERMISSION_FAULT          = 13,
    EM_INTR_IR_DATA_ACCESS_RIGHT_FAULT            = 14,
    EM_INTR_IR_DATA_ACCESS_BIT_FAULT              = 15,
    EM_INTR_IR_DATA_DEBUG_FAULT                   = 16,
    /* Faults: IA32 */
    EM_INTR_IA_INST_BREAKPOINT_FAULT              = 17,
    EM_INTR_IA_CODE_FETCH_FAULT                   = 18,
    /* Faults: IA32, IA64 */
    EM_INTR_INST_ALT_TLB_FAULT                    = 19,
    EM_INTR_INST_VHPT_FAULT                       = 20,
    EM_INTR_INST_TLB_FAULT                        = 21,
    EM_INTR_INST_PAGE_NOT_PRESENT_FAULT           = 22,
    EM_INTR_INST_NAT_PAGE_CONSUMPTION_FAULT       = 23,
    EM_INTR_INST_KEY_MISS_FAULT                   = 24,
    EM_INTR_INST_KEY_PERMISSION_FAULT             = 25,
    EM_INTR_INST_ACCESS_RIGHT_FAULT               = 26,
    EM_INTR_INST_ACCESS_BIT_FAULT                 = 27,
    /* Faults: IA64 */
    EM_INTR_INST_DEBUG_FAULT                      = 28,
    /* Faults: IA32 */
    EM_INTR_IA_INST_LENGTH_FAULT                  = 29,
    EM_INTR_IA_INVALID_OPCODE_FAULT               = 30,
    EM_INTR_IA_INST_INTERCEPT_FAULT               = 31,
    /* Faults: IA64 */
    EM_INTR_ILLEGAL_OPERATION_FAULT               = 32,
    EM_INTR_BREAK_INSTRUCTION_FAULT               = 33,
    EM_INTR_PRIVILEGED_OPERATION_FAULT            = 34,
    /* Faults: IA32, IA64 */
    EM_INTR_DISABLED_FP_REGISTER_FAULT            = 35,
    EM_INTR_DISABLED_ISA_TRANSITION_FAULT         = 36,
    EM_INTR_REGISTER_NAT_CONSUMPTION_FAULT        = 37,
    /* Faults: IA64 */
    EM_INTR_RESERVED_REGISTER_FIELD_FAULT         = 38,
    EM_INTR_PRIVILEGED_REGISTER_FAULT             = 39,
    EM_INTR_SPECULATIVE_OPERATION_FAULT           = 40,
    /* Faults: IA32 */
    EM_INTR_IA_COPROCESSOR_NOT_AVAILABLE_FAULT    = 41,
    EM_INTR_IA_FP_ERROR_FAULT                     = 42,
    EM_INTR_IA_STACK_EXCEPTION_FAULT              = 43,
    EM_INTR_IA_GENERAL_PROTECTION_FAULT           = 44,
    /* Faults: IA32, IA64 */
    EM_INTR_DATA_NESTED_TLB_FAULT                 = 45,
    EM_INTR_DATA_ALT_TLB_FAULT                    = 46,
    EM_INTR_DATA_VHPT_FAULT                       = 47,
    EM_INTR_DATA_TLB_FAULT                        = 48,
    EM_INTR_DATA_PAGE_NOT_PRESENT_FAULT           = 49,
    EM_INTR_DATA_NAT_PAGE_CONSUMPTION_FAULT       = 50,
    EM_INTR_DATA_KEY_MISS_FAULT                   = 51,
    EM_INTR_DATA_KEY_PERMISSION_FAULT             = 52,
    EM_INTR_DATA_ACCESS_RIGHT_FAULT               = 53,
    EM_INTR_DATA_DIRTY_BIT_FAULT                  = 54,
    EM_INTR_DATA_ACCESS_BIT_FAULT                 = 55,
    /* Faults: IA64 */
    EM_INTR_DATA_DEBUG_FAULT                      = 56,
    EM_INTR_UNALIGNED_DATA_REFERENCE_FAULT        = 57,
    /* Faults: IA32 */
    EM_INTR_IA_UNALIGNED_DATA_REFERENCE_FAULT     = 58,
    EM_INTR_IA_LOCKED_DATA_REFERENCE_FAULT        = 59,
    EM_INTR_IA_SEGMENT_NOT_PRESENT_FAULT          = 60,
    EM_INTR_IA_DIVIDE_BY_ZERO_FAULT               = 61,
    EM_INTR_IA_BOUND_FAULT                        = 62,
    EM_INTR_IA_KNI_NUMERIC_ERROR_FAULT            = 63,
    /* Faults: IA64 */
    EM_INTR_LOCKED_DATA_REFERENCE_FAULT           = 64,
    EM_INTR_FP_EXCEPTION_FAULT                    = 65,
    /* Traps: IA64 */
    EM_INTR_UNIMPLEMENTED_INST_ADDRESS_TRAP       = 66,
    EM_INTR_FP_TRAP                               = 67,
    EM_INTR_LOWER_PRIVILEGE_TARNSFER_TRAP         = 68,
    EM_INTR_TAKEN_BRANCH_TRAP                     = 69,
    EM_INTR_SINGLE_STEP_TRAP                      = 70,
    /* Traps: IA32 */
    EM_INTR_IA_SYSTEM_FLAG_INTERCEPT_TRAP         = 71,
    EM_INTR_IA_GATE_INTERCEPT_TRAP                = 72,
    EM_INTR_IA_INTO_TRAP                          = 73,
    EM_INTR_IA_BREAKPOINT_TRAP                    = 74,
    EM_INTR_IA_SOFTWARE_INTERRUPT_TRAP            = 75,
    EM_INTR_IA_DATA_DEBUG_TRAP                    = 76,
    EM_INTR_IA_TAKEN_BRANCH_TRAP                  = 77,
    EM_INTR_IA_SINGLE_STEP_TRAP                   = 78,

    EM_INTR_LAST                                  = 79
} EM_interruption_t;


/* Interruption Vectors, taken from Table 10-6 in EAS2.4. */
typedef enum
{
    EM_VECTOR_VHPT_TRANSLATION         = 0x0000,
    EM_VECTOR_INST_TLB                 = 0x0400,
    EM_VECTOR_DATA_TLB                 = 0x0800,
    EM_VECTOR_INST_ALT_TLB             = 0x0c00,
    EM_VECTOR_DATA_ALT_TLB             = 0x1000,
    EM_VECTOR_DATA_NESTED_TLB          = 0x1400,
    EM_VECTOR_INST_KEY_MISS            = 0x1800,
    EM_VECTOR_DATA_KEY_MISS            = 0x1C00,
    EM_VECTOR_DIRTY_BIT                = 0x2000,
    EM_VECTOR_INST_ACCESS_BIT          = 0x2400,
    EM_VECTOR_DATA_ACCESS_BIT          = 0x2800,
    EM_VECTOR_BREAK_INSTRUCTION        = 0x2C00,
    EM_VECTOR_EXTERNAL_INTERRUPT       = 0x3000,
    /*** reserved: 0x3400 through 0x4c00 ***/
    EM_VECTOR_PAGE_NOT_PRESENT         = 0x5000,
    EM_VECTOR_KEY_PERMISSION           = 0x5100,
    EM_VECTOR_INST_ACCESS_RIGHT        = 0x5200,
    EM_VECTOR_DATA_ACCESS_RIGHT        = 0x5300,
    EM_VECTOR_GENERAL_EXCEPTION        = 0x5400,
    EM_VECTOR_DISABLED_FP_REGISTER     = 0x5500,
    EM_VECTOR_NAT_CONSUMPTION          = 0x5600,
    EM_VECTOR_SPECULATION              = 0x5700,
    /*** reserved: 0x5800 ***/
    EM_VECTOR_DEBUG                    = 0x5900,
    EM_VECTOR_UNALIGNED_REFERENCE      = 0x5A00,
    EM_VECTOR_LOCKED_DATA_REFERENCE    = 0x5B00,
    EM_VECTOR_FP_EXCEPTION             = 0x5C00,
    EM_VECTOR_FP_TRAP                  = 0x5D00,
    EM_VECTOR_LOWER_PRIVILEGE_TRANSFER = 0x5E00,
    EM_VECTOR_TAKEN_BRANCH             = 0x5F00,
    EM_VECTOR_SINGLE_STEP              = 0x6000,
    /*** reserved: 0x6100 through 0x6800 ***/
    EM_VECTOR_IA_EXCEPTIONS            = 0x6900,
    EM_VECTOR_IA_INTERCEPTIONS         = 0x6A00,
    EM_VECTOR_IA_INTERRUPTIONS         = 0x6B00
    /*** reserved: 0x6c00 through 0x7f00 ***/
} EM_vector_t;

#define EM_INTR_ISR_CODE_TPA               0
#define EM_INTR_ISR_CODE_FC                1
#define EM_INTR_ISR_CODE_PROBE             2
#define EM_INTR_ISR_CODE_TAK               3
#define EM_INTR_ISR_CODE_LFETCH            4
#define EM_INTR_ISR_CODE_PROBE_FAULT       5



#define EM_ISR_CODE_ILLEGAL_OPERATION            0x0
#define EM_ISR_CODE_PRIVILEGED_OPERATION         0x10
#define EM_ISR_CODE_PRIVILEGED_REGISTER          0x20
#define EM_ISR_CODE_RESERVED_REGISTER_FIELD      0x30
#define EM_ISR_CODE_ILLEGAL_ISA_TRANSITION       0x40

#define EM_ISR_CODE_F0_F15        0
#define EM_ISR_CODE_F16_F127      1

#define EM_ISR_CODE_NAT_REGISTER_CONSUMPTION  0x10
#define EM_ISR_CODE_NAT_PAGE_CONSUMPTION      0x20

#define EM_ISR_CODE_INST_DEBUG        0
#define EM_ISR_CODE_DATA_DEBUG        1

#define EM_ISR_CODE_FP_IEEE_V          0x0001
#define EM_ISR_CODE_FP_IA_DENORMAL     0x0002
#define EM_ISR_CODE_FP_IEEE_Z          0x0004
#define EM_ISR_CODE_FP_SOFT_ASSIST     0x0008
#define EM_ISR_CODE_FP_IEEE_O          0x0800
#define EM_ISR_CODE_FP_IEEE_U          0x1000
#define EM_ISR_CODE_FP_IEEE_I          0x2000
#define EM_ISR_CODE_FP_EXPONENT        0x4000
#define EM_ISR_CODE_FP_ROUNDING_ADD_1  0x8000

#define EM_ISR_CODE_MASK_IA_TRAP            0x02
#define EM_ISR_CODE_MASK_IA_DATA_DEBUG_TRAP 0x00
#define EM_ISR_CODE_MASK_FP_TRAP            0x01
#define EM_ISR_CODE_MASK_LOWER_PRIV         0x02
#define EM_ISR_CODE_MASK_TAKEN_BRANCH       0x04
#define EM_ISR_CODE_MASK_SINGLE_STEP        0x08
#define EM_ISR_CODE_MASK_UNIMPLEMENTED_INST 0x10

#define EM_ISR_VECTOR_MASK_IA_TRAP          0x1
#define EM_ISR_VECTOR_MASK_EM_TRAP          0x0

#define EM_ISR_CODE_CHK_A_GR          0
#define EM_ISR_CODE_CHK_S_GR          1
#define EM_ISR_CODE_CHK_A_FP          2
#define EM_ISR_CODE_CHK_S_FP          3
#define EM_ISR_CODE_CHK_FCHK          4

/*** SAPIC definitions ***/
#define EM_SAPIC_SPURIOUS_VECTOR_NUM        0x0f
#define EM_SAPIC_SIZE_OF_INTERRUPT_GROUP      16
#define EM_SAPIC_NUM_OF_INTERRUPT_GROUPS      16
#define EM_SAPIC_GROUPS_IN_IRR                 4
#define EM_SAPIC_GROUP(vec) \
        ((vec) / EM_SAPIC_NUM_OF_INTERRUPT_GROUPS)
#define EM_SAPIC_IRR(vec) \
        ((vec) / (EM_SAPIC_NUM_OF_INTERRUPT_GROUPS*EM_SAPIC_GROUPS_IN_IRR))
#define EM_SAPIC_IRR_BIT_POS(vec) \
        ((vec) % (EM_SAPIC_NUM_OF_INTERRUPT_GROUPS*EM_SAPIC_GROUPS_IN_IRR))

/***  version strings at the .comment section ***/
#define EM_IAS_OBJECT_FILE_NAME "!!!!Object file name: "
#define EM_IAS_VER_NUMBER       "!!!!Major Version "
#define EM_IAS_VERSION_COMMENT  "!!!!EM_EAS2.4"
      

/*** END OF EM_H Enhanced Mode ARCHITECTURE ***/

#endif /*** EM_H ***/ 



/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright (c) 1992,1993,1994,1995,1996,1997,1998 Intel Corporation. ***/
/***                                                                       ***/

#ifndef EMDB_TYPES_H
#define EMDB_TYPES_H

/* Flags */
#define EM_FLAG_PRED   0x1
#define EM_FLAG_PRIVILEGED   0x2
#define EM_FLAG_LMEM   0x4
#define EM_FLAG_SMEM   0x8
#define EM_FLAG_CHECK_BASE_EQ_DST  0x10
#define EM_FLAG_FIRST_IN_INSTRUCTION_GROUP 0x20
#define EM_FLAG_LAST_IN_INSTRUCTION_GROUP 0x40
#define EM_FLAG_CHECK_SAME_DSTS 0x80
#define EM_FLAG_SLOT2_ONLY 0x100
#define EM_FLAG_TWO_SLOT 0x200
#define EM_FLAG_OK_IN_MLI 0x400
#define EM_FLAG_CHECK_EVEN_ODD_FREGS 0x800
#define EM_FLAG_CTYPE_UNC 0x1000            /* designates all cmp.unc like instructions */
#define EMDB_LAST_FLAG EM_FLAG_CTYPE_UNC


#define MAX_EXTENSION 8

typedef enum {
    EM_OPROLE_NONE = 0,
    EM_OPROLE_SRC,
    EM_OPROLE_DST,
    EM_OPROLE_SRC_DST,
    EM_OPROLE_DST_SRC,
    EM_OPROLE_LAST
} Operand_role_t;

typedef enum {
    EM_OPTYPE_NONE = 0,
    EM_OPTYPE_REG_FIRST,      /* The following types are registers */
    EM_OPTYPE_IREG,           /* Integer register */
    EM_OPTYPE_IREG_R0_3,      /* r0-r3 */
    EM_OPTYPE_IREG_R0,        /* Integer register R0 */
    EM_OPTYPE_IREG_R1_127,    /* r1-r127 */
    EM_OPTYPE_FREG,           /* FP register */
	EM_OPTYPE_FREG_F2_127,    /* f2-f127 */
    EM_OPTYPE_BR,             /* branch register */
    EM_OPTYPE_IP,             /* instruction pointer, not encoded */
    EM_OPTYPE_PREG,           /* predicate */
    EM_OPTYPE_PREGS_ALL,      /* the predicate register */
    EM_OPTYPE_PREGS_ROT,      /* rotating predicates */
    EM_OPTYPE_APP_REG_GRP_LOW,         /* application registers 0-63*/
    EM_OPTYPE_APP_REG_GRP_HIGH,        /* application registers 64-127*/
    EM_OPTYPE_APP_CCV,        /* ar.ccv */
    EM_OPTYPE_APP_PFS,        /* ar.pfs */
    EM_OPTYPE_CR,             /* control registers */
    EM_OPTYPE_PSR_L,          /* psr.l */
    EM_OPTYPE_PSR_UM,         /* psr.um */
    EM_OPTYPE_FPSR,           /* decoder operand types */
    EM_OPTYPE_CFM,           
    EM_OPTYPE_PSR,
    EM_OPTYPE_IFM,
    EM_OPTYPE_REG_LAST,       /* End of register - types */
    EM_OPTYPE_REGFILE_FIRST,  /* The following types are register-files */
    EM_OPTYPE_PMC,
    EM_OPTYPE_PMD,
    EM_OPTYPE_PKR,
    EM_OPTYPE_RR,
    EM_OPTYPE_IBR,
    EM_OPTYPE_DBR,
    EM_OPTYPE_ITR,
    EM_OPTYPE_DTR,
    EM_OPTYPE_MSR,
    EM_OPTYPE_CPUID,
    EM_OPTYPE_REGFILE_LAST,   /* End of register-file types */
    EM_OPTYPE_IMM_FIRST,      /* The following types are immediates */
    EM_OPTYPE_UIMM,           /* unsigned immediate */
    EM_OPTYPE_SIMM,           /* signed immediate */
    EM_OPTYPE_IREG_NUM,       /* ireg in syntax and imm7 in encodings */
    EM_OPTYPE_FREG_NUM,       /* freg in syntax and imm7 in encodings */
    EM_OPTYPE_SSHIFT_REL,     /* pc relative signed immediate
                                 which is shifted by 4 */
    EM_OPTYPE_SSHIFT_1,       /* unsigned immediate which has to be
                                 shifted 1 bit */
    EM_OPTYPE_SSHIFT_16,      /* unsigned immediate which has to be
                                 shifted 16 bits */
    EM_OPTYPE_COUNT_123,      /* immediate which can have the values of
                                 1, 2, 3 only */
    EM_OPTYPE_COUNT_PACK,     /* immediate which can have the values of
                                        0, 7, 15, 16 only */
    EM_OPTYPE_UDEC,           /* unsigned immediate which has to be
                                 decremented by 1 by the assembler */
    EM_OPTYPE_SDEC,           /* signed immediate which has to be
                                 decremented by 1 by the assembler */
    EM_OPTYPE_CCOUNT,         /* in pshl[24] - uimm5 in syntax, but encoded
                                 as its 2's complement */
    EM_OPTYPE_CPOS,           /* in dep fixed form - uimm6 in syntax, but encoded
                                 as its 2's complement */
    EM_OPTYPE_SEMAPHORE_INC,  /* immediate which is a semaphore increment amount
                                 can have the values of -16,-8,-4,-1,
                                 1,4,8,16 */
    EM_OPTYPE_ONE,            /* the number 1 */
    EM_OPTYPE_FCLASS,         /* immediate of the fclass instruction */
    EM_OPTYPE_CMP_UIMM,       /* unsigned immediate of cmp geu and ltu */
    EM_OPTYPE_CMP_UIMM_DEC,   /* unsigned immediate of cmp gtu and leu */
    EM_OPTYPE_CMP4_UIMM,      /* unsigned immediate of cmp4 geu and ltu */
    EM_OPTYPE_CMP4_UIMM_DEC,  /* unsigned immediate of cmp4 gtu and leu */
    EM_OPTYPE_ALLOC_IOL,      /* for alloc : input, local, and output
                                 can be 0-96 */
    EM_OPTYPE_ALLOC_ROT,      /* for alloc : rotating, can be 0-96 */
    EM_OPTYPE_MUX1,           /* immediate of the mux1 instruction */
    EM_OPTYPE_EIGHT,          /* immediate for ldfps base update form can have value 8 */
    EM_OPTYPE_SIXTEEN,        /* immediate for ldfp8 and ldfpd base update form can have value 16 */
    EM_OPTYPE_IMM_LAST,       /* End of immediate types */
    EM_OPTYPE_MEM,            /* memory address */
    EM_OPTYPE_LAST
} Operand_type_t;

typedef enum {
      EM_FORMAT_NONE = 0,
      EM_FORMAT_A1,
      EM_FORMAT_A2,
      EM_FORMAT_A3,
      EM_FORMAT_A4,
      EM_FORMAT_A4_1,
      EM_FORMAT_A5,
      EM_FORMAT_A6,
      EM_FORMAT_A6_1,
      EM_FORMAT_A6_2,
      EM_FORMAT_A6_3,
      EM_FORMAT_A6_4,
      EM_FORMAT_A6_5,
      EM_FORMAT_A6_6,
      EM_FORMAT_A6_7,
      EM_FORMAT_A7,
      EM_FORMAT_A7_1,
      EM_FORMAT_A7_2,
      EM_FORMAT_A7_3,
      EM_FORMAT_A7_4,
      EM_FORMAT_A7_5,
      EM_FORMAT_A7_6,
      EM_FORMAT_A7_7,
      EM_FORMAT_A8,
      EM_FORMAT_A8_1,
      EM_FORMAT_A8_2,
      EM_FORMAT_A8_3,
      EM_FORMAT_A9,
      EM_FORMAT_A10,
      EM_FORMAT_I1,
      EM_FORMAT_I2,
      EM_FORMAT_I3,
      EM_FORMAT_I4,
      EM_FORMAT_I5,
      EM_FORMAT_I6,
      EM_FORMAT_I7,
      EM_FORMAT_I8,
      EM_FORMAT_I9,
      EM_FORMAT_I10,
      EM_FORMAT_I11,
      EM_FORMAT_I12,
      EM_FORMAT_I13,
      EM_FORMAT_I14,
      EM_FORMAT_I15,
      EM_FORMAT_I16,
      EM_FORMAT_I16_1,
      EM_FORMAT_I16_2,
      EM_FORMAT_I16_3,
      EM_FORMAT_I17,
      EM_FORMAT_I17_1,
      EM_FORMAT_I17_2,
      EM_FORMAT_I17_3,
      EM_FORMAT_I18,
      EM_FORMAT_I19,
      EM_FORMAT_I20,
      EM_FORMAT_I21,
      EM_FORMAT_I22,
      EM_FORMAT_I23,
      EM_FORMAT_I24,
      EM_FORMAT_I25,
      EM_FORMAT_I26,
      EM_FORMAT_I27,
      EM_FORMAT_I28,
      EM_FORMAT_I29,
      EM_FORMAT_M1,
      EM_FORMAT_M2,
      EM_FORMAT_M3,
      EM_FORMAT_M4,
      EM_FORMAT_M5,
      EM_FORMAT_M6,
      EM_FORMAT_M7,
      EM_FORMAT_M8,
      EM_FORMAT_M9,
      EM_FORMAT_M10,
      EM_FORMAT_M11,
      EM_FORMAT_M12,
      EM_FORMAT_M13,
      EM_FORMAT_M14,
      EM_FORMAT_M15,
      EM_FORMAT_M16,
      EM_FORMAT_M17,
      EM_FORMAT_M18,
      EM_FORMAT_M19,
      EM_FORMAT_M20,
      EM_FORMAT_M21,
      EM_FORMAT_M22,
      EM_FORMAT_M23,
      EM_FORMAT_M24,
      EM_FORMAT_M25,
      EM_FORMAT_M26,
      EM_FORMAT_M27,
      EM_FORMAT_M28,
      EM_FORMAT_M29,
      EM_FORMAT_M30,
      EM_FORMAT_M31,
      EM_FORMAT_M32,
      EM_FORMAT_M33,
      EM_FORMAT_M34,
      EM_FORMAT_M34_1,
      EM_FORMAT_M35,
      EM_FORMAT_M36,
      EM_FORMAT_M37,
      EM_FORMAT_M38,
      EM_FORMAT_M39,
      EM_FORMAT_M40,
      EM_FORMAT_M41,
      EM_FORMAT_M42,
      EM_FORMAT_M43,
      EM_FORMAT_M44,
      EM_FORMAT_M45,
      EM_FORMAT_M46,
      EM_FORMAT_B1,
      EM_FORMAT_B2,
      EM_FORMAT_B3,
      EM_FORMAT_B4,
      EM_FORMAT_B5,
      EM_FORMAT_B6,
      EM_FORMAT_B7,
      EM_FORMAT_B8,
      EM_FORMAT_B9,
      EM_FORMAT_F1,
      EM_FORMAT_F1_1,
      EM_FORMAT_F2,
      EM_FORMAT_F3,
      EM_FORMAT_F4,
      EM_FORMAT_F4_1,
      EM_FORMAT_F4_2,
      EM_FORMAT_F4_3,
      EM_FORMAT_F4_4,
      EM_FORMAT_F4_5,
      EM_FORMAT_F4_6,
      EM_FORMAT_F4_7,
      EM_FORMAT_F5,
      EM_FORMAT_F5_1,
      EM_FORMAT_F5_2,
      EM_FORMAT_F5_3,
      EM_FORMAT_F6,
      EM_FORMAT_F7,
      EM_FORMAT_F8,
      EM_FORMAT_F8_4,
      EM_FORMAT_F9,
      EM_FORMAT_F9_1,
      EM_FORMAT_F10,
      EM_FORMAT_F11,
      EM_FORMAT_F12,
      EM_FORMAT_F13,
      EM_FORMAT_F14,
      EM_FORMAT_F15,
      EM_FORMAT_X41,
      EM_FORMAT_LAST
} Format_t;

typedef enum {
    EM_TROLE_NONE = 0,
    EM_TROLE_ALU,
    EM_TROLE_BR,
    EM_TROLE_FP,
    EM_TROLE_INT,
    EM_TROLE_MEM,
    EM_TROLE_MIBF,
    EM_TROLE_LAST
} Template_role_t;

typedef char *Mnemonic_t;
typedef char Major_opcode_t;
typedef short Extension_t[MAX_EXTENSION];
typedef struct {
    Operand_role_t operand_role;
    Operand_type_t operand_type;
} Operand_t;
typedef unsigned long Flags_t;


#endif /*** EMDB_TYPES_H ***/
#ifndef EM_HINTS_H
#define EM_HINTS_H

typedef enum EM_branch_hint_e
{
    EM_branch_hint_none,
    EM_branch_hint_sptk_few_dc_dc,
    EM_branch_hint_sptk_few_dc_dc_imp,
    EM_branch_hint_sptk_many_dc_dc,
    EM_branch_hint_sptk_many_dc_dc_imp,
    EM_branch_hint_few_dc_dc,
    EM_branch_hint_few_dc_dc_imp,
    EM_branch_hint_many_dc_dc,
    EM_branch_hint_many_dc_dc_imp,
    EM_branch_hint_dptk_few_dc_dc,
    EM_branch_hint_dptk_few_dc_dc_imp,
    EM_branch_hint_dptk_many_dc_dc,
    EM_branch_hint_dptk_many_dc_dc_imp,
    EM_branch_hint_sptk_few_dc_nt,
    EM_branch_hint_sptk_few_dc_nt_imp,
    EM_branch_hint_sptk_many_dc_nt,
    EM_branch_hint_sptk_many_dc_nt_imp,
    EM_branch_hint_few_dc_nt,
    EM_branch_hint_few_dc_nt_imp,
    EM_branch_hint_many_dc_nt,
    EM_branch_hint_many_dc_nt_imp,
    EM_branch_hint_dptk_few_dc_nt,
    EM_branch_hint_dptk_few_dc_nt_imp,
    EM_branch_hint_dptk_many_dc_nt,
    EM_branch_hint_dptk_many_dc_nt_imp,
    EM_branch_hint_sptk_few_tk_dc,
    EM_branch_hint_sptk_few_tk_dc_imp,
    EM_branch_hint_sptk_many_tk_dc,
    EM_branch_hint_sptk_many_tk_dc_imp,
    EM_branch_hint_few_tk_dc,
    EM_branch_hint_few_tk_dc_imp,
    EM_branch_hint_many_tk_dc,
    EM_branch_hint_many_tk_dc_imp,
    EM_branch_hint_dptk_few_tk_dc,
    EM_branch_hint_dptk_few_tk_dc_imp,
    EM_branch_hint_dptk_many_tk_dc,
    EM_branch_hint_dptk_many_tk_dc_imp,
    EM_branch_hint_sptk_few_tk_tk,
    EM_branch_hint_sptk_few_tk_tk_imp,
    EM_branch_hint_sptk_many_tk_tk,
    EM_branch_hint_sptk_many_tk_tk_imp,
    EM_branch_hint_few_tk_tk,
    EM_branch_hint_few_tk_tk_imp,
    EM_branch_hint_many_tk_tk,
    EM_branch_hint_many_tk_tk_imp,
    EM_branch_hint_dptk_few_tk_tk,
    EM_branch_hint_dptk_few_tk_tk_imp,
    EM_branch_hint_dptk_many_tk_tk,
    EM_branch_hint_dptk_many_tk_tk_imp,
    EM_branch_hint_sptk_few_tk_nt,
    EM_branch_hint_sptk_few_tk_nt_imp,
    EM_branch_hint_sptk_many_tk_nt,
    EM_branch_hint_sptk_many_tk_nt_imp,
    EM_branch_hint_few_tk_nt,
    EM_branch_hint_few_tk_nt_imp,
    EM_branch_hint_many_tk_nt,
    EM_branch_hint_many_tk_nt_imp,
    EM_branch_hint_dptk_few_tk_nt,
    EM_branch_hint_dptk_few_tk_nt_imp,
    EM_branch_hint_dptk_many_tk_nt,
    EM_branch_hint_dptk_many_tk_nt_imp,
    EM_branch_hint_sptk_few_nt_dc,
    EM_branch_hint_sptk_few_nt_dc_imp,
    EM_branch_hint_sptk_many_nt_dc,
    EM_branch_hint_sptk_many_nt_dc_imp,
    EM_branch_hint_few_nt_dc,
    EM_branch_hint_few_nt_dc_imp,
    EM_branch_hint_many_nt_dc,
    EM_branch_hint_many_nt_dc_imp,
    EM_branch_hint_dptk_few_nt_dc,
    EM_branch_hint_dptk_few_nt_dc_imp,
    EM_branch_hint_dptk_many_nt_dc,
    EM_branch_hint_dptk_many_nt_dc_imp,
    EM_branch_hint_sptk_few_nt_tk,
    EM_branch_hint_sptk_few_nt_tk_imp,
    EM_branch_hint_sptk_many_nt_tk,
    EM_branch_hint_sptk_many_nt_tk_imp,
    EM_branch_hint_few_nt_tk,
    EM_branch_hint_few_nt_tk_imp,
    EM_branch_hint_many_nt_tk,
    EM_branch_hint_many_nt_tk_imp,
    EM_branch_hint_dptk_few_nt_tk,
    EM_branch_hint_dptk_few_nt_tk_imp,
    EM_branch_hint_dptk_many_nt_tk,
    EM_branch_hint_dptk_many_nt_tk_imp,
    EM_branch_hint_sptk_few_nt_nt,
    EM_branch_hint_sptk_few_nt_nt_imp,
    EM_branch_hint_sptk_many_nt_nt,
    EM_branch_hint_sptk_many_nt_nt_imp,
    EM_branch_hint_few_nt_nt,
    EM_branch_hint_few_nt_nt_imp,
    EM_branch_hint_many_nt_nt,
    EM_branch_hint_many_nt_nt_imp,
    EM_branch_hint_dptk_few_nt_nt,
    EM_branch_hint_dptk_few_nt_nt_imp,
    EM_branch_hint_dptk_many_nt_nt,
    EM_branch_hint_dptk_many_nt_nt_imp,
    EM_branch_hint_ret_sptk_few_dc_dc,
    EM_branch_hint_ret_sptk_few_dc_dc_imp,
    EM_branch_hint_ret_sptk_many_dc_dc,
    EM_branch_hint_ret_sptk_many_dc_dc_imp,
    EM_branch_hint_ret_few_dc_dc,
    EM_branch_hint_ret_few_dc_dc_imp,
    EM_branch_hint_ret_many_dc_dc,
    EM_branch_hint_ret_many_dc_dc_imp,
    EM_branch_hint_ret_dptk_few_dc_dc,
    EM_branch_hint_ret_dptk_few_dc_dc_imp,
    EM_branch_hint_ret_dptk_many_dc_dc,
    EM_branch_hint_ret_dptk_many_dc_dc_imp,
    EM_branch_hint_ret_sptk_few_dc_nt,
    EM_branch_hint_ret_sptk_few_dc_nt_imp,
    EM_branch_hint_ret_sptk_many_dc_nt,
    EM_branch_hint_ret_sptk_many_dc_nt_imp,
    EM_branch_hint_ret_few_dc_nt,
    EM_branch_hint_ret_few_dc_nt_imp,
    EM_branch_hint_ret_many_dc_nt,
    EM_branch_hint_ret_many_dc_nt_imp,
    EM_branch_hint_ret_dptk_few_dc_nt,
    EM_branch_hint_ret_dptk_few_dc_nt_imp,
    EM_branch_hint_ret_dptk_many_dc_nt,
    EM_branch_hint_ret_dptk_many_dc_nt_imp,
    EM_branch_hint_ret_sptk_few_tk_dc,
    EM_branch_hint_ret_sptk_few_tk_dc_imp,
    EM_branch_hint_ret_sptk_many_tk_dc,
    EM_branch_hint_ret_sptk_many_tk_dc_imp,
    EM_branch_hint_ret_few_tk_dc,
    EM_branch_hint_ret_few_tk_dc_imp,
    EM_branch_hint_ret_many_tk_dc,
    EM_branch_hint_ret_many_tk_dc_imp,
    EM_branch_hint_ret_dptk_few_tk_dc,
    EM_branch_hint_ret_dptk_few_tk_dc_imp,
    EM_branch_hint_ret_dptk_many_tk_dc,
    EM_branch_hint_ret_dptk_many_tk_dc_imp,
    EM_branch_hint_ret_sptk_few_tk_tk,
    EM_branch_hint_ret_sptk_few_tk_tk_imp,
    EM_branch_hint_ret_sptk_many_tk_tk,
    EM_branch_hint_ret_sptk_many_tk_tk_imp,
    EM_branch_hint_ret_few_tk_tk,
    EM_branch_hint_ret_few_tk_tk_imp,
    EM_branch_hint_ret_many_tk_tk,
    EM_branch_hint_ret_many_tk_tk_imp,
    EM_branch_hint_ret_dptk_few_tk_tk,
    EM_branch_hint_ret_dptk_few_tk_tk_imp,
    EM_branch_hint_ret_dptk_many_tk_tk,
    EM_branch_hint_ret_dptk_many_tk_tk_imp,
    EM_branch_hint_ret_sptk_few_tk_nt,
    EM_branch_hint_ret_sptk_few_tk_nt_imp,
    EM_branch_hint_ret_sptk_many_tk_nt,
    EM_branch_hint_ret_sptk_many_tk_nt_imp,
    EM_branch_hint_ret_few_tk_nt,
    EM_branch_hint_ret_few_tk_nt_imp,
    EM_branch_hint_ret_many_tk_nt,
    EM_branch_hint_ret_many_tk_nt_imp,
    EM_branch_hint_ret_dptk_few_tk_nt,
    EM_branch_hint_ret_dptk_few_tk_nt_imp,
    EM_branch_hint_ret_dptk_many_tk_nt,
    EM_branch_hint_ret_dptk_many_tk_nt_imp,
    EM_branch_hint_ret_sptk_few_nt_dc,
    EM_branch_hint_ret_sptk_few_nt_dc_imp,
    EM_branch_hint_ret_sptk_many_nt_dc,
    EM_branch_hint_ret_sptk_many_nt_dc_imp,
    EM_branch_hint_ret_few_nt_dc,
    EM_branch_hint_ret_few_nt_dc_imp,
    EM_branch_hint_ret_many_nt_dc,
    EM_branch_hint_ret_many_nt_dc_imp,
    EM_branch_hint_ret_dptk_few_nt_dc,
    EM_branch_hint_ret_dptk_few_nt_dc_imp,
    EM_branch_hint_ret_dptk_many_nt_dc,
    EM_branch_hint_ret_dptk_many_nt_dc_imp,
    EM_branch_hint_ret_sptk_few_nt_tk,
    EM_branch_hint_ret_sptk_few_nt_tk_imp,
    EM_branch_hint_ret_sptk_many_nt_tk,
    EM_branch_hint_ret_sptk_many_nt_tk_imp,
    EM_branch_hint_ret_few_nt_tk,
    EM_branch_hint_ret_few_nt_tk_imp,
    EM_branch_hint_ret_many_nt_tk,
    EM_branch_hint_ret_many_nt_tk_imp,
    EM_branch_hint_ret_dptk_few_nt_tk,
    EM_branch_hint_ret_dptk_few_nt_tk_imp,
    EM_branch_hint_ret_dptk_many_nt_tk,
    EM_branch_hint_ret_dptk_many_nt_tk_imp,
    EM_branch_hint_ret_sptk_few_nt_nt,
    EM_branch_hint_ret_sptk_few_nt_nt_imp,
    EM_branch_hint_ret_sptk_many_nt_nt,
    EM_branch_hint_ret_sptk_many_nt_nt_imp,
    EM_branch_hint_ret_few_nt_nt,
    EM_branch_hint_ret_few_nt_nt_imp,
    EM_branch_hint_ret_many_nt_nt,
    EM_branch_hint_ret_many_nt_nt_imp,
    EM_branch_hint_ret_dptk_few_nt_nt,
    EM_branch_hint_ret_dptk_few_nt_nt_imp,
    EM_branch_hint_ret_dptk_many_nt_nt,
    EM_branch_hint_ret_dptk_many_nt_nt_imp,
    EM_branch_hint_sptk_few,
    EM_branch_hint_sptk_many,
    EM_branch_hint_spnt_few,
    EM_branch_hint_spnt_many,
    EM_branch_hint_dptk_few,
    EM_branch_hint_dptk_many,
    EM_branch_hint_dpnt_few,
    EM_branch_hint_dpnt_many,
    EM_branch_hint_sptk_few_clr,
    EM_branch_hint_sptk_many_clr,
    EM_branch_hint_spnt_few_clr,
    EM_branch_hint_spnt_many_clr,
    EM_branch_hint_dptk_few_clr,
    EM_branch_hint_dptk_many_clr,
    EM_branch_hint_dpnt_few_clr,
    EM_branch_hint_dpnt_many_clr,
    EM_branch_hint_loop_few_dc_dc,
    EM_branch_hint_loop_few_dc_dc_imp,
    EM_branch_hint_loop_many_dc_dc,
    EM_branch_hint_loop_many_dc_dc_imp,
    EM_branch_hint_exit_few_dc_dc,
    EM_branch_hint_exit_few_dc_dc_imp,
    EM_branch_hint_exit_many_dc_dc,
    EM_branch_hint_exit_many_dc_dc_imp,
    EM_branch_hint_loop_few_dc_nt,
    EM_branch_hint_loop_few_dc_nt_imp,
    EM_branch_hint_loop_many_dc_nt,
    EM_branch_hint_loop_many_dc_nt_imp,
    EM_branch_hint_exit_few_dc_nt,
    EM_branch_hint_exit_few_dc_nt_imp,
    EM_branch_hint_exit_many_dc_nt,
    EM_branch_hint_exit_many_dc_nt_imp,
    EM_branch_hint_loop_few_tk_dc,
    EM_branch_hint_loop_few_tk_dc_imp,
    EM_branch_hint_loop_many_tk_dc,
    EM_branch_hint_loop_many_tk_dc_imp,
    EM_branch_hint_exit_few_tk_dc,
    EM_branch_hint_exit_few_tk_dc_imp,
    EM_branch_hint_exit_many_tk_dc,
    EM_branch_hint_exit_many_tk_dc_imp,
    EM_branch_hint_loop_few_tk_tk,
    EM_branch_hint_loop_few_tk_tk_imp,
    EM_branch_hint_loop_many_tk_tk,
    EM_branch_hint_loop_many_tk_tk_imp,
    EM_branch_hint_exit_few_tk_tk,
    EM_branch_hint_exit_few_tk_tk_imp,
    EM_branch_hint_exit_many_tk_tk,
    EM_branch_hint_exit_many_tk_tk_imp,
    EM_branch_hint_loop_few_tk_nt,
    EM_branch_hint_loop_few_tk_nt_imp,
    EM_branch_hint_loop_many_tk_nt,
    EM_branch_hint_loop_many_tk_nt_imp,
    EM_branch_hint_exit_few_tk_nt,
    EM_branch_hint_exit_few_tk_nt_imp,
    EM_branch_hint_exit_many_tk_nt,
    EM_branch_hint_exit_many_tk_nt_imp,
    EM_branch_hint_loop_few_nt_dc,
    EM_branch_hint_loop_few_nt_dc_imp,
    EM_branch_hint_loop_many_nt_dc,
    EM_branch_hint_loop_many_nt_dc_imp,
    EM_branch_hint_exit_few_nt_dc,
    EM_branch_hint_exit_few_nt_dc_imp,
    EM_branch_hint_exit_many_nt_dc,
    EM_branch_hint_exit_many_nt_dc_imp,
    EM_branch_hint_loop_few_nt_tk,
    EM_branch_hint_loop_few_nt_tk_imp,
    EM_branch_hint_loop_many_nt_tk,
    EM_branch_hint_loop_many_nt_tk_imp,
    EM_branch_hint_exit_few_nt_tk,
    EM_branch_hint_exit_few_nt_tk_imp,
    EM_branch_hint_exit_many_nt_tk,
    EM_branch_hint_exit_many_nt_tk_imp,
    EM_branch_hint_loop_few_nt_nt,
    EM_branch_hint_loop_few_nt_nt_imp,
    EM_branch_hint_loop_many_nt_nt,
    EM_branch_hint_loop_many_nt_nt_imp,
    EM_branch_hint_exit_few_nt_nt,
    EM_branch_hint_exit_few_nt_nt_imp,
    EM_branch_hint_exit_many_nt_nt,
    EM_branch_hint_exit_many_nt_nt_imp,
    EM_branch_hint_special,
    EM_branch_hint_last
} EM_branch_hint_t;

typedef enum EM_memory_access_hint_e
{
    EM_memory_access_hint_none,
    EM_memory_access_hint_nt1,
    EM_memory_access_hint_nta,
    EM_memory_access_hint_nt2,
    EM_memory_access_hint_last
} EM_memory_access_hint_t;


#endif  /* EM_HINTS_H */
/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright (c) 1992,1993,1994,1995,1996,1997,1998 Intel Corporation. ***/
/***                                                                       ***/

#ifndef _EM_TOOLS_H
#define _EM_TOOLS_H

typedef struct EM_version_s
{
	int      major;
	int      minor;
} EM_version_t;

typedef struct EM_library_version_s
{
	EM_version_t   xversion;
	EM_version_t   api;
	EM_version_t   emdb;
    char           date[12];
    char           time[9];
} EM_library_version_t;

#endif /* _EM_TOOLS_H */

/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright (c) 1992,1993,1994,1995,1996,1997,1998 Intel Corporation. ***/
/***                                                                       ***/

#ifndef IEL_H
#define IEL_H

#if defined(BIG_ENDIAN) && defined(BYTE_ORDER)
#if BYTE_ORDER != BIG_ENDIAN
#undef BIG_ENDIAN
#endif
#endif


typedef char  S1byte;
typedef short S2byte;
typedef int   S4byte;

typedef unsigned char  U1byte;
typedef unsigned short U2byte;
typedef unsigned int   U4byte;

typedef unsigned short U16;
typedef unsigned char  U8;


/*---------
         IEL types declarations
                              ------------*/


#ifdef  LP64

#ifdef WINNT
typedef unsigned __int64 Ulong64;
typedef __int64 Slong64;
#else
typedef unsigned long Ulong64;
typedef long Slong64;
#endif

extern  Ulong64         IEL_temp64;

typedef union
{
    U4byte dw[1];
    U4byte qw[1];
}U32, S32;

typedef union
{
    Ulong64 qw[1];
    unsigned int dw[2];
} U64,S64;

typedef union
{
    Ulong64   qw[2];
    unsigned int dw[4];
} U128, S128;

#else /* LP32 */
      
typedef struct
{
    U4byte dw[1];
}U32, S32;

typedef struct
{
    U4byte   dw[2];
} U64, S64;

typedef struct
{
    U4byte   dw[4];
} U128,S128;

#endif /* end of LP32 */

/*---------
         IEL temporary variables
                              ------------*/

#ifdef __cplusplus
extern "C" {
#endif

extern  unsigned int    IEL_t1, IEL_t2, IEL_t3, IEL_t4;
extern  U32             IEL_tempc;
extern  U64             IEL_et1, IEL_et2;
extern  U128            IEL_ext1, IEL_ext2, IEL_ext3, IEL_ext4, IEL_ext5;
extern  U128            IEL_POSINF, IEL_NEGINF, IEL_MINUS1;
extern  S128            IEL_ts1, IEL_ts2;

#ifdef __cplusplus
}
#endif

/*---------
         IEL constants
                    ------------*/
#define IEL_MAX32 ((U4byte) (0xFFFFFFFF))
#define IEL_UMAX32 ((U4byte) (0xFFFFFFFF))
#define IEL_SMAX32 ((U4byte) (0x7FFFFFFF))

#define IEL_UMAX64  0xffffffffffffffff
#define IEL_SMAX64  0x7fffffffffffffff

#define IEL128(x) (*(U128*)(&(x)))
#define IEL64(x)  (*(U64*)(&(x)))
#define IEL32(x)  (*(U32*)(&(x)))



#define IEL_FALSE 0
#define IEL_TRUE  1

#ifdef __cplusplus

#define IEL_OK 0
#define IEL_OVFL 1

typedef unsigned int IEL_Err;

#else

typedef enum
{
    IEL_OK = 0,
    IEL_OVFL
} IEL_Err;
#endif

typedef enum
{
    IEL_BIN = 2,
    IEL_OCT = 8,
    IEL_DEC = 10,
    IEL_SDEC = 100,
    IEL_HEX = 16,
    IEL_UHEX = 116
} IEL_Base;

/*----------
         IEL structure elements
                         ---------*/

#   ifdef   BIG_ENDIAN

#       define  qw0_64 qw[0]
#       define  qw0_128 qw[1]
#       define  qw1_128 qw[0]
#       define  dw0_32  dw[0]
#       define  dw0_64  dw[1]
#       define  dw1_64  dw[0]
#       define  dw0_128 dw[3]
#       define  dw1_128 dw[2]
#       define  dw2_128 dw[1]
#       define  dw3_128 dw[0]

#   else  /*** BIG_ENDIAN ***/

#       define  qw0_64 qw[0]
#       define  qw0_128 qw[0]
#       define  qw1_128 qw[1]
#       define  dw0_32  dw[0]
#       define  dw0_64  dw[0]
#       define  dw1_64  dw[1]
#       define  dw0_128 dw[0]
#       define  dw1_128 dw[1]
#       define  dw2_128 dw[2]
#       define  dw3_128 dw[3]

#   endif /*** BIG_ENDIAN ***/


#define DW0(x)          ((sizeof((x))==4)?((x).dw0_32):\
                         (sizeof((x))==8)?((x).dw0_64):\
                         (sizeof((x))==16)?((x).dw0_128):0)
#define DW1(x)          ((sizeof((x))==8)?((x).dw1_64):\
                         (sizeof((x))==16)?((x).dw1_128):0)
#define DW2(x)          ((sizeof((x))==16)?((x).dw2_128):0)
#define DW3(x)          ((sizeof((x))==16)?((x).dw3_128):0)

#define IEL_GETDW0(x) DW0(x) 
#define IEL_GETDW1(x) DW1(x)
#define IEL_GETDW2(x) DW2(x)
#define IEL_GETDW3(x) DW3(x)

#ifdef  LP64

#define IEL_CONST32(x)      {(U4byte )(x)}
#define IEL_CONST(x)        IEL_CONST32(x)
#define IEL_CONST64L(x)     {(Ulong64 )(x)}

#ifdef BIG_ENDIAN

#define IEL_CONST64(x0, x1)             {((Ulong64)x0<<32)|x1}
#define IEL_CONST128(x0, x1, x2, x3)    {{((Ulong64)x2<<32)|x3, ((Ulong64)x0<<32)|x1}}
#define IEL_CONST128L(x0, x1)           {{(Ulong64 )x1, (Ulong64 )x0}}

#else /* BIG_ENDIAN */

#define IEL_CONST64(x0, x1)             {((Ulong64)x1<<32)|x0}
#define IEL_CONST128(x0, x1, x2, x3)    {{((Ulong64)x1<<32)|x0, ((Ulong64)x3<<32)|x2}}
#define IEL_CONST128L(x0, x1)           {{(Ulong64 )x0, (Ulong64 )x1}}

#endif /* BIG_ENDIAN */

#define IEL_GETQW0(x)    ((sizeof(x)==4) ? (Ulong64)((x).dw0_32) : \
                         ((sizeof(x)==8) ?  (x).qw0_64 : \
                         ((sizeof(x)==16) ? (x).qw0_128 : 0)))

#define IEL_GETQW1(x)   ((sizeof(x)==sizeof (U128)) ? ((x).qw1_128) : (0))

#define QW0(x) IEL_GETQW0(x)
#define QW1(x) IEL_GETQW1(x)

#define SQW0(x)     ((sizeof(x)==4)? (((x).dw0_32 & 0x80000000)? \
                     (((Ulong64)(-1)<<32) | (Ulong64)(x).dw0_32) : (Ulong64)(x).dw0_32) : \
                     (sizeof(x)==8)?((x).qw0_64):\
                     (sizeof(x)==16)?((x).qw0_128):0)

#define SQW1(x)    ((sizeof(x)==4)? (((x).dw0_32 & 0x80000000)? (Ulong64)(-1):0) :\
                    (sizeof(x)==8)?((x).qw0_64 & 0x8000000000000000)?(Ulong64)(-1):0 :\
                    (sizeof(x)==16)?((x).qw1_128):0)

#define IEL_INCU(x)      ((sizeof(x) == sizeof(U32)) ? \
                            ((x).dw0_32++,((x).dw0_32==0)) :\
                         (sizeof(x) == sizeof(U64)) ? \
                            ((x).qw0_64++,((x).qw0_64==0)) : \
                         (sizeof(x) == sizeof(U128)) ? \
                            ((x).qw0_128++,(!(x).qw0_128) ? \
                             ((x).qw1_128++, ((x).qw1_128==0)) : \
                             IEL_OK) : IEL_OVFL)

#define IEL_INCS(x)     ((sizeof(x) == sizeof(U32)) ? \
                          (((x).dw0_32++),(((x).dw0_32==0x80000000)||((x).dw0_32==0))) : \
						(sizeof(x) == sizeof(U64)) ? \
                         (((x).qw0_64++),(((x).qw0_64==0x8000000000000000)||((x).qw0_64==0))) : \
                           (sizeof(x) == sizeof(U128)) ? \
                          ((x).qw0_128++, !(x).qw0_128? ((x).qw0_128=0, (x).qw1_128++, \
                             (((x).qw1_128==0)||((x).qw1_128==0x8000000000000000))) : IEL_OK): \
                           IEL_OVFL)
                             

#define IEL_DECU(x)     ((sizeof(x) == sizeof(U32)) ? \
                            (((x).dw0_32--),((x).dw0_32==IEL_MAX32)) : \
                         (sizeof(x) == sizeof(U64)) ? \
                            ((x).qw0_64--,((x).qw0_64==IEL_UMAX64)) : \
                         (sizeof(x) == sizeof(U128)) ? \
                            ((x).qw0_128--,((x).qw0_128==IEL_UMAX64) ? \
                             ((x).qw1_128--, ((x).qw1_128==IEL_UMAX64)) : \
                             IEL_OK) : IEL_OVFL)

#define IEL_DECS(x)     ((sizeof(x) == sizeof(U32)) ? \
                            (((x).dw0_32--),((x).dw0_32==IEL_UMAX32) || \
                             (x).dw0_32==IEL_SMAX32) : \
                         (sizeof(x) == sizeof(U64)) ? \
                            (((x).qw0_64--),(((x).qw0_64==IEL_UMAX64))||((x).qw0_64==IEL_SMAX64)) : \
                         (sizeof(x) == sizeof(U128)) ? \
                            ((x).qw0_128--,((x).qw0_128==IEL_UMAX64) ? \
                             ((x).qw1_128--, ((x).qw1_128==IEL_UMAX64)|| \
                             ((x).qw1_128==IEL_SMAX64)): \
                             IEL_OK) : IEL_OVFL)

#define IEL_AND(x, y, z)    ((sizeof(x) == sizeof (U32)) ? \
                                ((x).dw0_32 = DW0(y) & DW0(z), \
                                  ((DW1(y) & DW1(z)) || (QW1(y) & QW1(z)))) : \
                             (sizeof(x) == sizeof (U64)) ? \
                                (((x).qw0_64 = QW0(y) & QW0(z)), \
                                 ((QW1(y) & QW1(z)) != 0)) : \
                             (sizeof(x) == sizeof (U128)) ? \
                                ((x).qw0_128 = QW0(y) & QW0(z), \
                                 (x).qw1_128 = QW1(y) & QW1(z),IEL_OK) : \
                                 IEL_OVFL)

#define IEL_OR(x, y, z)    ((sizeof(x) == sizeof (U32)) ? \
                                ((x).dw0_32 = DW0(y) | DW0(z), \
                                 (((DW1(y) | DW1(z)) ||(QW1(y) | QW1(z))))) : \
                             (sizeof(x) == sizeof (U64)) ? \
                                (((x).qw0_64 = QW0(y) | QW0(z)), \
                                 ((QW1(y) | QW1(z)) != 0)) : \
                             (sizeof(x) == sizeof (U128)) ? \
                                ((x).qw0_128 = QW0(y) | QW0(z), \
                                 (x).qw1_128 = QW1(y) | QW1(z),IEL_OK) : \
                                 IEL_OVFL)

#define IEL_XOR(x, y, z)    ((sizeof(x) == sizeof (U32)) ? \
                                ((x).dw0_32 = DW0(y) ^ DW0(z), \
                                 (((DW1(y) ^ DW1(z))||(QW1(y) ^ QW1(z))))) : \
                             (sizeof(x) == sizeof (U64)) ? \
                                (((x).qw0_64 = QW0(y) ^ QW0(z)), \
                                 ((QW1(y) ^ QW1(z)) != 0)) : \
                             (sizeof(x) == sizeof (U128)) ? \
                                ((x).qw0_128 = QW0(y) ^ QW0(z), \
                                 (x).qw1_128 = QW1(y) ^ QW1(z),IEL_OK) : \
                                 IEL_OVFL)

#define IEL_ANDNOT(x, y, z)  ((sizeof(x) == sizeof (U32)) ? \
                                ((x).dw0_32 = DW0(y) & ~DW0(z), \
                                 (((DW1(y) & ~DW1(z))||(QW1(y) & ~QW1(z))))) : \
                             (sizeof(x) == sizeof (U64)) ? \
                                (((x).qw0_64 = QW0(y) & (~QW0(z))), \
                                 ((QW1(y) & (~QW1(z))) != 0)) : \
                             (sizeof(x) == sizeof (U128)) ? \
                                ((x).qw0_128 = QW0(y) & (~QW0(z)), \
                                 (x).qw1_128 = QW1(y) & (~QW1(z)),IEL_OK) : \
                                 IEL_OVFL)


#define IEL_ASSIGNU(x, y)   	((sizeof(x) == sizeof(U32)) ?\
                                ((x).dw0_32 = (y).dw0_32, (DW1(y) || QW1(y))):\
								(sizeof(x) == sizeof (U64)) ? \
                                ((x).qw0_64 = QW0(y), (QW1(y)!=0)) : \
                                (sizeof(x) == sizeof (U128)) ? \
                                ((x).qw0_128 = QW0(y), (x).qw1_128 = QW1(y), \
                                 IEL_OK) : IEL_OVFL)

#define IEL_ASSIGNS(x, y)   (IEL_ext1.qw0_128 = SQW0(y), \
                             IEL_ext1.qw1_128 = SQW1(y), \
                             (sizeof(x) == sizeof (U32)) ? \
                                ((x).dw0_32 = IEL_ext1.dw0_128, \
                                 ((x).dw0_32 & 0x80000000) ? \
                                 (~IEL_ext1.dw1_128||~IEL_ext1.qw1_128) : \
                                 (IEL_ext1.dw1_128 || IEL_ext1.qw1_128)) : \
                             (sizeof(x) == sizeof (U64)) ? \
                              ((x).qw0_64 = IEL_ext1.qw0_128,\
                               ((x).qw0_64 & 0x8000000000000000)?\
                               (~IEL_ext1.qw1_128 !=0): (IEL_ext1.qw1_128!=0)):\
                              (sizeof(x) == sizeof (U128)) ? \
                               ((x).qw0_128 = IEL_ext1.qw0_128,\
                                (x).qw1_128 = IEL_ext1.qw1_128, IEL_OK):\
                               IEL_OVFL)
                             
#define IEL_NOT(x, y)       ((sizeof(x )== sizeof(U32)) ? \
                                ((x).dw0_32 = (~DW0(y)), \
                                 ((~DW1(y))|| (~QW1(y)))): \
                             (sizeof(x) == sizeof (U64)) ? \
                                ((x).qw0_64 = (~QW0(y)), \
                                 ((~QW1(y)) != 0)) : \
                             (sizeof(x) == sizeof (U128)) ? \
                                ((x).qw0_128 = (~QW0(y)),\
                                 (x).qw1_128 = (~QW1(y)), \
                                 IEL_OK) : IEL_OVFL)

#define IEL_ZERO(x)         ((sizeof(x) == sizeof (U32)) ? \
                                ((x).dw0_32 = 0) : \
                             (sizeof(x) == sizeof (U64)) ? \
                                ((x).qw0_64 = 0) : \
                             (sizeof(x) == sizeof (U128)) ? \
                                ((x).qw0_128 = 0, (x).qw1_128 = 0, IEL_OK): \
                                IEL_OVFL)
                            

#define QW1_1(x)    ((sizeof(x)==16)? ((x).qw1_128):1)

#define IEL_C0(x) (QW0(x) < QW0(IEL_ext1))
/*#define IEL_C1(x) ((QW1(x)-IEL_C0(x)) < QW1(IEL_ext1))*/

#define IEL_C1(x) (IEL_C0(x) ? (QW1_1(x)<=QW1(IEL_ext1)) : (QW1(x)<QW1(IEL_ext1)))


#define IEL_ADDU(x, y, z)   ((sizeof(x)==sizeof(U32))?\
                                (IEL_t1=DW0(y),(x).dw0_32=DW0(y)+DW0(z),\
                                 ((IEL_t1>DW0(x))||(DW1(y))||(QW1(y)) ||\
                                (DW1(z))||(QW1(z)))):\
                             (sizeof(x)==sizeof(U64))?\
                                (IEL_temp64 = QW0(y),(x).qw0_64=QW0(y)+QW0(z),\
                                 ((IEL_temp64 > QW0(x))||(QW1(y))||(QW1(z)))):\
                             (sizeof(x)==sizeof(U128))?\
                                (IEL_ASSIGNU(IEL_ext1, y),\
                                 (x).qw0_128=QW0(y)+QW0(z),\
                                 (x).qw1_128=QW1(y)+QW1(z)+IEL_C0(x),\
                                 IEL_C1(x)) : IEL_OVFL)

#define IEL_EQU(x, y)       (((Ulong64)QW0(x)==(Ulong64)QW0(y)) && \
                             ((Ulong64)QW1(x)==(Ulong64)QW1(y)))

#define IEL_ISZERO(x)       ((QW0(x)==0) && (QW1(x)==0))

#define IEL_CMPGU(x, y)     ((QW1(x)>QW1(y)) || \
                             ((QW1(x)==QW1(y)) && (QW0(x)>QW0(y))))

#define IEL_CMPGEU(x, y)        ((QW1(x)>QW1(y)) || \
                             ((QW1(x)==QW1(y)) && (QW0(x)>=QW0(y))))

#define IEL_SHL(x, y, n)    ((((n) <=0) || ((n) >= (sizeof(x) << 3)))? \
                                 ((IEL_ISZERO(y)||(!(n)))? \
                                  (IEL_ASSIGNU(x,y),IEL_OK) : \
                                   (IEL_ZERO(x), IEL_OVFL)) : \
                             ((sizeof(x) == sizeof (U32)) ? \
                                (IEL_t1 = (DW1(y) || DW2(y) || DW3(y) \
                                 || (DW0(y)>= (U4byte)1<<(32-(n)))), \
                                 (x).dw0_32 = DW0(y) << (n), (IEL_t1)) : \
                              (sizeof(x) == sizeof (U64)) ? \
                              (IEL_t1 = QW1(y)||(QW0(y) >= (Ulong64)1<<(64-(n))), \
                              (x).qw0_64= QW0(y)<<(n),IEL_t1): \
                               ((sizeof(x) == sizeof(U128)) ? \
                              ((n)==64)? \
                               (IEL_t1=(QW1(y)>0),(x).qw1_128=QW0(y),\
                                (x).qw0_128=0,IEL_t1) : \
                               ((n)<64)? \
                               (IEL_t1 = (QW1(y) >= (Ulong64)1<<(64-(n))),\
                                (x).qw1_128 = (QW1(y)<<(n))|\
                                QW0(y)>>(64-(n)),\
                                (x).qw0_128 = QW0(y) << (n), IEL_t1) :\
                                /* n > 64 */\
                               (IEL_t1 = (QW1(y))||\
                                          (QW0(y) >= (Ulong64)1<<(128-(n))),\
                                 (x).qw1_128 = QW0(y) << ((n)-64),(x).qw0_128 = 0,\
                                 IEL_t1):IEL_OVFL)))

#define IEL_SHL128(x, y, n)   (((n)==64)? \
                               (IEL_t1=(QW1(y)>0),(x).qw1_128=QW0(y),\
                                (x).qw0_128=0,IEL_t1) : \
                               ((n)<64)? \
                               (IEL_t1 = (QW1(y) >= (Ulong64)1<<(64-(n))),\
                                (x).qw1_128 = (QW1(y)<<(n))|\
                                QW0(y)>>(64-(n)),\
                                (x).qw0_128 = QW0(y) << (n), IEL_t1) :\
                                /* n > 64 */\
                               (IEL_t1 = QW1(y)||\
                                          (QW0(y) >= (Ulong64)1<<(128-(n))),\
                                (x).qw1_128 = QW0(y) << ((n)-64),(x).qw0_128 = 0,\
                                 IEL_t1))



#define IEL_ISNEG(x)        ((sizeof(x) == sizeof(U32)) ? \
                                ((DW0(x) & 0x80000000)!=0) : \
                             (sizeof(x) == sizeof(U64)) ? \
                                ((QW0(x) & 0x8000000000000000)!=0) : \
                             (sizeof(x) == sizeof(U128)) ? \
                                ((QW1(x) & 0x8000000000000000)!=0) : IEL_FALSE)

#define IEL_ISNINF(x)        ((sizeof(x) == sizeof(U32)) ? \
                                (DW0(x)==0x80000000) : \
                             (sizeof(x) == sizeof(U64)) ? \
                                (QW0(x)==0x8000000000000000) : \
                             (sizeof(x) == sizeof(U128)) ? \
                                (QW1(x)==0x8000000000000000 && QW0(x)==0) : \
                                (IEL_OVFL))




#define IEL_INL(x, n)       (((((Ulong64)1<<n)-1) & x))
#define IEL_INH(x, n)       (x>>n)

#define IEL_SHR(x, y, n)  \
(((n) <=0)?  ((IEL_ISZERO(y)||(!n)) \
              ?  (IEL_ASSIGNU(x,y),IEL_OK)\
              :  (IEL_ZERO(x), IEL_OVFL))\
          : \
         ((sizeof(x) == sizeof (U32)) ? \
        ((n)==64) ?  \
                  ((x).dw0_32 = (U4byte)QW1(y), (DW3(y)!=0)) : \
        ((n)>64) ? \
             ((x).dw0_32 = (U4byte)(QW1(y)>>((n)-64)),\
             ((QW1(y)>>((n)-64))> QW0(x))) : \
        /* n < 64 */  \
             (IEL_temp64 = (QW0(y)>>(n))|(QW1(y)<<(64-(n))),\
			 (x).dw0_32 = (U4byte)IEL_temp64, \
             ((QW1(y)>>(n))> 0) || (IEL_temp64 > QW0(x))) : \
       (sizeof(x) == sizeof (U64)) \
   ? (((n)==64)?  ((x).qw0_64=QW1(y),IEL_OK)\
               :(((n) < 64)?  ((x).qw0_64 = (QW0(y)>>(n))|(QW1(y)<<(64-(n))),\
                              (QW1(y)>>(n)!=0))\
                /* n > 64 */  :  ((x).qw0_64 = QW1(y)>>((n)-64), IEL_OK)))\
   :(sizeof(x) == sizeof(U128)) \
     ?  (((n)==64)?  ((x).qw0_128=QW1(y),(x).qw1_128=0,IEL_OK) \
                  : (((n)<64)?  ((x).qw0_128 = (QW0(y)>>(n))|(QW1(y)<<(64-(n))),\
                                 (x).qw1_128 = QW1(y)>>(n),\
                                 IEL_OK)\
                   /* n>64 */:  ((x).qw0_128=QW1(y)>>((n)-64),(x).qw1_128=0,\
                                 IEL_OK)))\
     : IEL_OVFL))


#define IEL_SHR128(x, y, n) (((n) <=0) ? \
                             ((IEL_ISZERO(y)||!(n)) ? \
                              (IEL_ASSIGNU(x, y), IEL_OK) :\
                              (IEL_ZERO(x), IEL_OVFL)): \
                               (sizeof(x) == sizeof (U128)) ? \
                       (n==64) ?  \
                                ((x).qw0_128 = QW1(y), \
                                (x).qw1_128 = 0, (IEL_OK)) : \
                       (((n)<64)?  ((x).qw0_128 = (QW0(y)>>(n))|(QW1(y)<<(64-(n))),\
                                 (x).qw1_128 = QW1(y)>>(n),IEL_OK)\
                   /* n>64 */:  ((x).qw0_128=QW1(y)>>((n)-64),(x).qw1_128=0,\
                                 IEL_OK)): IEL_OVFL)

#define IEL_SEXT(x, y)      (IEL_ASSIGNU(x,y),\
                             ((!IEL_ISNEG(y)) || (sizeof(x)==sizeof(y))) ? \
                             (IEL_OK) : \
                            ((sizeof(x) == sizeof(U64)) ? \
                                ((sizeof(y) == sizeof(U32)) ? \
                                    ((x).dw1_64=IEL_MAX32,IEL_OK):(IEL_OVFL)): \
                            ((sizeof(x) == sizeof(U128)) ? \
                                ((sizeof(y) == sizeof(U32)) ? \
                                    ((x).dw1_128 = IEL_MAX32, \
                                     (x).dw2_128 = IEL_MAX32, \
                                     (x).dw3_128 = IEL_MAX32, IEL_OK) : \
                                (sizeof(y) == sizeof (U64)) ? \
                                    ((x).dw2_128 = IEL_MAX32, \
                                    (x).dw3_128 = IEL_MAX32, IEL_OK):IEL_OVFL):\
                                (IEL_OVFL))))


#define IEL_ISNEG128(x)      (((x).qw1_128 & 0x8000000000000000)!=0)


#define IEL_ADDU128(x, y, z) (IEL_ASSIGNU(IEL_ext1, y),\
                                (x).qw0_128=(y).qw0_128 + (z).qw0_128,\
                                 (x).qw1_128=(y).qw1_128+(z).qw1_128+ IEL_C0(x),\
                                 IEL_C1(x))

#define iel_check_result_sign(addend1, addend2, res)\
   ((((addend1.qw1_128 ^ addend2.qw1_128) & 0x8000000000000000) == 0) && \
    (((addend1.qw1_128 ^ res.qw1_128) & 0x8000000000000000) != 0))

#define IEL_ADDS(x, y, z)   (IEL_ASSIGNS(IEL_ext4, y), \
                             IEL_ASSIGNS(IEL_ext2, z), \
                             IEL_ADDU128(IEL_ext3, IEL_ext4, IEL_ext2), \
                             IEL_ASSIGNS(x, IEL_ext3)||\
                             iel_check_result_sign(IEL_ext4, IEL_ext2, IEL_ext3))

#define IEL_SUBU(x, y, z) (IEL_ISZERO(z) ? IEL_ASSIGNU(x, y) : \
                           (IEL_COMP(IEL_ext2 ,z), \
                           (!(IEL_ADDU(x, y,IEL_ext2)))))

#define IEL_CONVERT4L(x, y0, y1) \
                             ((sizeof(x) == sizeof(U64)) ? \
                                ((x).qw0_64 = y0, y1):\
                             (sizeof(x) == sizeof(U128)) ? \
                                ((x).qw0_128 = y0, \
                                 (x).qw1_128 = y1, \
                                 IEL_OK) : IEL_OVFL)

#define IEL_CONVERT2L(x,y0) IEL_CONVERT4L(x,y0,0)

#define IEL128(x) (*(U128*)(&(x)))

#else /* not LP64 */

#define IEL_CONST32(x)      {(U4byte )(x)}
#define IEL_CONST(x)        IEL_CONST32(x)

#ifdef BIG_ENDIAN

#define IEL_CONST64(x0, x1)         {{(U4byte )(x1),(U4byte )(x0)}}
#define IEL_CONST128(x0, x1, x2, x3)    {{x3, x2, x1, x0}}

#else /* BIG_ENDIAN */

#define IEL_CONST64(x0, x1)         {{(U4byte )(x0), (U4byte )(x1)}}
#define IEL_CONST128(x0, x1, x2, x3)    {{x0, x1, x2, x3}}

#endif /* BIG_ENDIAN */

/* DWn_1(x) macros return 1 (instead of 0) in order to  prevent warnings */
/* This does not affect the produced code since the 1 can appear only in */
/* a "dead portion of code" derived by preprocessor */

#define DW1_1(x)    ((sizeof((x))==8)?((x).dw1_64):\
                         (sizeof((x))==16)?((x).dw1_128):1)
#define DW2_1(x)    ((sizeof((x))==16)?((x).dw2_128):1)
#define DW3_1(x)    ((sizeof((x))==16)?((x).dw3_128):1)





#define SDW0(x)         ((sizeof((x))==4)?((x).dw0_32):\
                         (sizeof((x))==8)?((x).dw0_64):\
                         (sizeof((x))==16)?((x).dw0_128):0)
#define SDW1(x)         ((sizeof((x))==4)?((x).dw0_32 & 0x80000000) ? -1 : 0 : \
                         (sizeof((x))==8)?((x).dw1_64): \
                         (sizeof((x))==16)?((x).dw1_128):0)
#define SDW2(x)         ((sizeof((x))==4)?((x).dw0_32 & 0x80000000) ? -1 : 0 :\
                         (sizeof((x))==8)?((x).dw1_64 & 0x80000000) ? -1 : 0 :\
                         (sizeof((x))==16)?((x).dw2_128):0)
#define SDW3(x)         ((sizeof((x))==4)?((x).dw0_32 & 0x80000000) ? -1 : 0 :\
                         (sizeof((x))==8)?((x).dw1_64 & 0x80000000) ? -1 : 0 :\
                         (sizeof((x))==16)?((x).dw3_128):0)



#define IEL_INCU(x)     ((sizeof(x)==4) ? \
                            (((x).dw0_32++),((x).dw0_32==0)) : \
                         (sizeof(x)==8) ? \
                            (((x).dw0_64++),!((x).dw0_64) ? \
                             ((x).dw1_64++, ((x).dw1_64==0)) : IEL_OK) : \
                         (sizeof(x)==16) ? \
                            (((x).dw0_128++),!((x).dw0_128) ? \
                            (((x).dw1_128++),!((x).dw1_128) ? \
                            (((x).dw2_128++),!((x).dw2_128) ? \
                            (((x).dw3_128++),((x).dw3_128==0)) : \
                             (IEL_OK)) : (IEL_OK)) : (IEL_OK)) : IEL_OVFL)

#define IEL_INCS(x)     ((sizeof(x) == sizeof(U32)) ? \
                            (((x).dw0_32++),(((x).dw0_32==0)) || \
                             (x).dw0_32==0x80000000): \
                         (sizeof(x) == sizeof(U64)) ? \
                            (((x).dw0_64++),!((x).dw0_64) ? \
                             ((x).dw1_64++, ((x).dw1_64==0) || \
                              (x).dw1_64==0x80000000) : IEL_OK) : \
                         (sizeof(x) == sizeof(U128)) ? \
                            (((x).dw0_128++),!((x).dw0_128) ? \
                            (((x).dw1_128++),!((x).dw1_128) ? \
                            (((x).dw2_128++),!((x).dw2_128) ? \
                            (((x).dw3_128++),((x).dw3_128==0) || \
                            (x).dw3_128==0x80000000) : \
                             (IEL_OK)) : (IEL_OK)) : (IEL_OK)) : IEL_OVFL)

#define IEL_DECU(x)     ((sizeof(x) == sizeof(U32)) ? \
                            (((x).dw0_32--),((x).dw0_32==IEL_MAX32)) : \
                         (sizeof(x) == sizeof(U64)) ? \
                            (((x).dw0_64--),((x).dw0_64==IEL_MAX32) ? \
                             ((x).dw1_64--, ((x).dw1_64==IEL_MAX32)) : IEL_OK):\
                         (sizeof(x) == sizeof(U128)) ? \
                            (((x).dw0_128--),((x).dw0_128==IEL_MAX32) ? \
                            (((x).dw1_128--),((x).dw1_128==IEL_MAX32) ? \
                            (((x).dw2_128--),((x).dw2_128==IEL_MAX32) ? \
                            (((x).dw3_128--),((x).dw3_128==IEL_MAX32)) : \
                             (IEL_OK)) : (IEL_OK)) : (IEL_OK)) : IEL_OVFL)


#define IEL_DECS(x)     ((sizeof(x) == sizeof(U32)) ? \
                            (((x).dw0_32--),((x).dw0_32==IEL_MAX32) || \
                             (x).dw0_32==0x7fffffff) : \
                         (sizeof(x) == sizeof(U64)) ? \
                            (((x).dw0_64--),((x).dw0_64==IEL_MAX32) ? \
                             ((x).dw1_64--, ((x).dw1_64==IEL_MAX32) || \
                             (x).dw1_64==0x7fffffff) : IEL_OK) : \
                         (sizeof(x) == sizeof(U128)) ? \
                            (((x).dw0_128--),((x).dw0_128==IEL_MAX32) ? \
                            (((x).dw1_128--),((x).dw1_128==IEL_MAX32) ? \
                            (((x).dw2_128--),((x).dw2_128==IEL_MAX32) ? \
                            (((x).dw3_128--),((x).dw3_128==IEL_MAX32) || \
                             (x).dw3_128==0x7fffffff) : \
                             (IEL_OK)) : (IEL_OK)) : (IEL_OK)) : IEL_OVFL)

#define IEL_AND(x, y, z)    ((sizeof(x) == sizeof (U32)) ? \
                                ((x).dw0_32 = DW0(y) & DW0(z), \
                                 (((DW1(y) & DW1(z)) | (DW2(y) & DW2(z)) |\
                                   (DW3(y) & DW3(z))) != 0)) : \
                             (sizeof(x) == sizeof (U64)) ? \
                                (((x).dw0_64 = DW0(y) & DW0(z), \
                                 ((x).dw1_64 = DW1(y) & DW1(z))), \
                                 ((DW2(y) & DW2(z)) | \
                                  (DW3(y) & DW3(z))) != 0) : \
                             (sizeof(x) == sizeof (U128)) ? \
                                (((x).dw0_128 = DW0(y) & DW0(z)),  \
                                 ((x).dw1_128 = DW1(y) & DW1(z)),  \
                                 ((x).dw2_128 = DW2(y) & DW2(z)),  \
                                 ((x).dw3_128 = DW3(y) & DW3(z)), \
                                 IEL_OK) : IEL_OVFL) 

#define IEL_OR(x, y, z)    ((sizeof(x) == sizeof (U32)) ? \
                                ((x).dw0_32 = DW0(y) | DW0(z), \
                                 (((DW1(y) | DW1(z)) | (DW2(y) | DW2(z)) |\
                                   (DW3(y) | DW3(z))) != 0)) : \
                             (sizeof(x) == sizeof (U64)) ? \
                                (((x).dw0_64 = DW0(y) | DW0(z), \
                                 ((x).dw1_64 = DW1(y) | DW1(z))), \
                                 ((DW2(y) | DW2(z)) | \
                                  (DW3(y) | DW3(z))) != 0) : \
                             (sizeof(x) == sizeof (U128)) ? \
                                (((x).dw0_128 = DW0(y) | DW0(z)),  \
                                 ((x).dw1_128 = DW1(y) | DW1(z)),  \
                                 ((x).dw2_128 = DW2(y) | DW2(z)),  \
                                 ((x).dw3_128 = DW3(y) | DW3(z)),  \
                                 IEL_OK) : IEL_OVFL) 

#define IEL_XOR(x, y, z)    ((sizeof(x) == sizeof (U32)) ? \
                                ((x).dw0_32 = DW0(y) ^ DW0(z), \
                                 (((DW1(y) ^ DW1(z)) | (DW2(y) ^ DW2(z)) |\
                                   (DW3(y) ^ DW3(z))) != 0)) : \
                             (sizeof(x) == sizeof (U64)) ? \
                                (((x).dw0_64 = DW0(y) ^ DW0(z), \
                                 ((x).dw1_64 = DW1(y) ^ DW1(z))), \
                                 ((DW2(y) ^ DW2(z)) | \
                                  (DW3(y) ^ DW3(z))) != 0) : \
                             (sizeof(x) == sizeof (U128)) ? \
                                (((x).dw0_128 = DW0(y) ^ DW0(z)),  \
                                 ((x).dw1_128 = DW1(y) ^ DW1(z)),  \
                                 ((x).dw2_128 = DW2(y) ^ DW2(z)),  \
                                 ((x).dw3_128 = DW3(y) ^ DW3(z)),  \
                                 IEL_OK) : IEL_OVFL)

#define IEL_ANDNOT(x, y, z)    ((sizeof(x) == sizeof (U32)) ? \
                                ((x).dw0_32 = DW0(y) & (~DW0(z)), \
                                 (((DW1(y) & (~DW1(z))) | (DW2(y)&(~DW2(z)))\
                                   | (DW3(y) & (~DW3(z)))) != 0)) : \
                             (sizeof(x) == sizeof (U64)) ? \
                                (((x).dw0_64 = DW0(y) & (~DW0(z)), \
                                 ((x).dw1_64 = DW1(y) & (~DW1(z)))), \
                                 ((DW2(y) & (~DW2(z))) | \
                                  (DW3(y) & (~DW3(z)))) != 0) : \
                             (sizeof(x) == sizeof (U128)) ? \
                                (((x).dw0_128 = DW0(y) & (~DW0(z))),  \
                                 ((x).dw1_128 = DW1(y) & (~DW1(z))),  \
                                 ((x).dw2_128 = DW2(y) & (~DW2(z))),  \
                                 ((x).dw3_128 = DW3(y) & (~DW3(z))),  \
                                 IEL_OK) : IEL_OVFL) 
 
#define IEL_ASSIGNU(x, y)   ((sizeof(x) == sizeof (U32)) ? \
                                ((x).dw0_32 = DW0(y), \
                                 ((DW1(y) | DW2(y) | DW3(y)) != 0)) : \
                             (sizeof(x) == sizeof (U64)) ? \
                                ((x).dw0_64 = DW0(y), (x).dw1_64 = DW1(y), \
                                 ((DW2(y) | DW3(y)) != 0)) : \
                             (sizeof(x) == sizeof(U128)) ? \
                                ((x).dw0_128 = DW0(y), (x).dw1_128 = DW1(y),\
                                 (x).dw2_128 = DW2(y), (x).dw3_128 = DW3(y),\
                                 IEL_OK) : IEL_OVFL)

#define IEL_ASSIGNS(x, y)   (IEL_ext1.dw0_128 = SDW0(y), \
                             IEL_ext1.dw1_128 = SDW1(y), \
                             IEL_ext1.dw2_128 = SDW2(y), \
                             IEL_ext1.dw3_128 = SDW3(y), \
                             (sizeof(x) == sizeof (U32)) ? \
                                ((x).dw0_32 = IEL_ext1.dw0_128, \
                                 ((x).dw0_32 & 0x80000000) ? \
                                 ((~IEL_ext1.dw1_128||~IEL_ext1.dw2_128||\
                                   ~IEL_ext1.dw3_128)) : \
                                 ((IEL_ext1.dw1_128 || IEL_ext1.dw2_128||\
                                   IEL_ext1.dw3_128) != 0)) : \
                             (sizeof(x) == sizeof (U64)) ? \
                                ((x).dw0_64 = IEL_ext1.dw0_128, \
                                 (x).dw1_64 = IEL_ext1.dw1_128, \
                                 ((x).dw1_64 & 0x80000000) ? \
                                 (~IEL_ext1.dw2_128||~IEL_ext1.dw3_128):\
                                 (IEL_ext1.dw2_128|| IEL_ext1.dw3_128)):\
                             (sizeof(x) == sizeof(U128)) ? \
                                ((x).dw0_128 = IEL_ext1.dw0_128,\
                                 (x).dw1_128 = IEL_ext1.dw1_128,\
                                 (x).dw2_128 = IEL_ext1.dw2_128,\
                                 (x).dw3_128 = IEL_ext1.dw3_128,\
                                 IEL_OK) : IEL_OVFL)

/* Duplicate IEL_ASSIGNS for macro-->function transform */

#define IEL_REAL_ASSIGNS(x, y)  (IEL_ext1.dw0_128 = SDW0(y), \
                             IEL_ext1.dw1_128 = SDW1(y), \
                             IEL_ext1.dw2_128 = SDW2(y), \
                             IEL_ext1.dw3_128 = SDW3(y), \
                             (sizeof(x) == sizeof (U32)) ? \
                                ((x).dw0_32 = IEL_ext1.dw0_128, \
                                 ((x).dw0_32 & 0x80000000) ? \
                                 ((~IEL_ext1.dw1_128||~IEL_ext1.dw2_128||\
                                   ~IEL_ext1.dw3_128)) : \
                                 ((IEL_ext1.dw1_128 || IEL_ext1.dw2_128||\
                                   IEL_ext1.dw3_128) != 0)) : \
                             (sizeof(x) == sizeof (U64)) ? \
                                ((x).dw0_64 = IEL_ext1.dw0_128, \
                                 (x).dw1_64 = IEL_ext1.dw1_128, \
                                 ((x).dw1_64 & 0x80000000) ? \
                                 (~IEL_ext1.dw2_128||~IEL_ext1.dw3_128):\
                                 (IEL_ext1.dw2_128|| IEL_ext1.dw3_128)):\
                             (sizeof(x) == sizeof(U128)) ? \
                                ((x).dw0_128 = IEL_ext1.dw0_128,\
                                 (x).dw1_128 = IEL_ext1.dw1_128,\
                                 (x).dw2_128 = IEL_ext1.dw2_128,\
                                 (x).dw3_128 = IEL_ext1.dw3_128,\
                                 IEL_OK) : IEL_OVFL)



#define IEL_NOT(x, y)       ((sizeof(x)==4) ? \
                                ((x).dw0_32 = (~DW0(y)), \
                                 (((~DW1(y))|(~DW2(y)) | (~DW3(y))) != 0)): \
                             (sizeof(x)==8) ? \
                                ((x).dw0_64=(~DW0(y)), (x).dw1_64=(~DW1(y)),\
                                 (((~DW2(y)) | (~DW3(y))) != 0)) : \
                             (sizeof(x)==16) ? \
                                ((x).dw0_128=(~DW0(y)), \
                                 (x).dw1_128=(~DW1(y)), \
                                 (x).dw2_128 = (~DW2(y)), \
                                 (x).dw3_128 = (~DW3(y)), \
                                 IEL_OK) : IEL_OVFL)

#define IEL_ZERO(x)         ((sizeof(x) == sizeof(U32)) ? \
                                ((x).dw0_32 = 0) : \
                             (sizeof(x) == sizeof (U64)) ? \
                                ((x).dw0_64 = 0, (x).dw1_64 = 0) : \
                             (sizeof(x) == sizeof (U128)) ? \
                                ((x).dw0_128 = 0, (x).dw1_128 = 0, \
                                 (x).dw2_128 = 0, (x).dw3_128 = 0, IEL_OK) :\
                                IEL_OVFL)


#define IEL_C1_1(x) (IEL_C0(x) ? (DW1_1(x)<=DW1(IEL_ext1)) : \
                               (DW1(x)<DW1(IEL_ext1)))
#define IEL_C2_1(x) (IEL_C1_1(x) ? (DW2_1(x)<=DW2(IEL_ext1)) : \
                               (DW2(x)<DW2(IEL_ext1)))
#define IEL_C3_1(x) (IEL_C2_1(x) ? (DW3_1(x)<=DW3(IEL_ext1)) : \
                               (DW3(x)<DW3(IEL_ext1)))


#define IEL_C0(x) (DW0(x) < DW0(IEL_ext1))
#define IEL_C1(x) (IEL_C0(x) ? (DW1(x)<=DW1(IEL_ext1)) : \
                               (DW1(x)<DW1(IEL_ext1)))
#define IEL_C2(x) (IEL_C1(x) ? (DW2(x)<=DW2(IEL_ext1)) : \
                               (DW2(x)<DW2(IEL_ext1)))
#define IEL_C3(x) (IEL_C2(x) ? (DW3(x)<=DW3(IEL_ext1)) : \
                               (DW3(x)<DW3(IEL_ext1)))


#define IEL_R_C0(x) (DW0(x) < DW0(IEL_ext1))
#define IEL_R_C1(x) (IEL_R_C0(x) ? (DW1(x)<=DW1(IEL_ext1)) : \
                               (DW1(x)<DW1(IEL_ext1)))
#define IEL_R_C2(x) (IEL_R_C1(x) ? ((sizeof(x) == sizeof(U128)) ? \
                                   (DW2_1(x)<=DW2(IEL_ext1)) : 1) : \
                               (DW2(x)<DW2(IEL_ext1)))
#define IEL_R_C3(x) (IEL_R_C2(x) ? ((sizeof(x) == sizeof(U128)) ? \
                                   (DW3_1(x)<=DW3(IEL_ext1)) : 1) : \
                               (DW3(x)<DW3(IEL_ext1)))


#define IEL_ADDU(x, y, z)   ((sizeof(x)==4)?\
                                (IEL_t1=DW0(y),(x).dw0_32=DW0(y)+DW0(z),\
                                 ((IEL_t1>DW0(x))||(DW1(y))||(DW2(y)) ||\
                                (DW3(y))||(DW1(z))||(DW2(z))||(DW3(z)))):\
                             (sizeof(x)==8)?\
                                (IEL_ASSIGNU(IEL_ext1, y),\
                                 (x).dw0_64=DW0(y)+DW0(z),\
                                 (x).dw1_64=DW1(y)+DW1(z)+IEL_C0(x),\
                                 (IEL_C1_1(x)||(DW2(y))||(DW3(y))||\
                                  (DW2(z))||(DW3(z)))):\
                             (sizeof(x)==16)?\
                                (IEL_ASSIGNU(IEL_ext1, y),\
                                 (x).dw0_128=DW0(y)+DW0(z),\
                                 (x).dw1_128=DW1(y)+DW1(z)+IEL_C0(x),\
                                 (x).dw2_128=DW2(y)+DW2(z)+IEL_C1_1(x),\
                                 (x).dw3_128=DW3(y)+DW3(z)+IEL_C2_1(x),\
                                 (IEL_C3_1(x))): IEL_OVFL)


#define IEL_EQU(x, y)       ((DW0(x)==DW0(y)) && (DW1(x)==DW1(y)) && \
                             (DW2(x)==DW2(y)) && (DW3(x)==DW3(y)))
#define IEL_ISZERO(x)       ((DW0(x)==0) && (DW1(x)==0) && (DW2(x)==0) && \
                             (DW3(x)==0))

#define IEL_ISNEG(x)         ((sizeof(x)==4)?\
                                ((DW0(x)&0x80000000)!=0) : \
                             (sizeof(x)==8)?\
                                ((DW1(x) & 0x80000000)!=0) : \
                             (sizeof(x)==16)?\
                                ((DW3(x)&0x80000000)!=0) : IEL_FALSE)

#define IEL_ISNINF(x)        ((sizeof(x) == sizeof(U32)) ? \
                                (DW0(x)==0x80000000) : \
                             (sizeof(x) == sizeof(U64)) ? \
                                (DW1(x)==0x80000000 && DW0(x)==0) : \
                             (sizeof(x) == sizeof(U128)) ? \
                                (DW3(x)==0x80000000 && DW2(x)==0 && \
                                  DW1(x)==0 && DW0(x)==0) : IEL_FALSE)


#define IEL_SEXT(x, y)      (IEL_ASSIGNU(x,y),\
                             ((!IEL_ISNEG(y)) || (sizeof(x)==sizeof(y))) ? \
                             (IEL_OK) : \
                            ((sizeof(x) == sizeof(U64)) ? \
                                ((sizeof(y) == sizeof(U32)) ? \
                                    ((x).dw1_64=IEL_MAX32,IEL_OK):(IEL_OVFL)): \
                            ((sizeof(x) == sizeof(U128)) ? \
                                ((sizeof(y) == sizeof(U32)) ? \
                                    ((x).dw1_128 = IEL_MAX32, \
                                     (x).dw2_128 = IEL_MAX32, \
                                     (x).dw3_128 = IEL_MAX32, IEL_OK) : \
                                (sizeof(y) == sizeof (U64)) ? \
                                    ((x).dw2_128 = IEL_MAX32, \
                                    (x).dw3_128 = IEL_MAX32, IEL_OK):IEL_OVFL):\
                                (IEL_OVFL))))



#define IEL_CMPGU(x, y)     ((sizeof(x) == sizeof(U128)) ? \
                               ((DW3_1(x)>DW3(y)) || \
                               ((DW3(x)==DW3(y)) && (DW2_1(x)>DW2(y))) || \
                               ((DW3(x)==DW3(y)) && (DW2(x)==DW2(y)) && \
                               (DW1_1(x)>DW1(y))) || \
                               ((DW3(x)==DW3(y)) && (DW2(x)==DW2(y)) && \
                               (DW1(x)==DW1(y)) && (DW0(x)>DW0(y)))) : \
                             (sizeof(x) == sizeof(U64)) ? \
                               (((DW3(x)==DW3(y)) && (DW2(x)==DW2(y)) && \
                               (DW1_1(x)>DW1(y))) || \
                               ((DW3(x)==DW3(y)) && (DW2(x)==DW2(y)) && \
                               (DW1(x)==DW1(y)) && (DW0(x)>DW0(y)))) : \
                         /*  (sizeof(x) == sizeof(U32)) */ \
                               (((DW3(x)==DW3(y)) && (DW2(x)==DW2(y)) && \
                               (DW1(x)==DW1(y)) && (DW0(x)>DW0(y)))))

#define IEL_CMPGEU(x, y)  ((sizeof(x) == sizeof(U128)) ? \
                              ((DW3_1(x)>DW3(y)) || \
                             ((DW3(x)==DW3(y)) && (DW2_1(x)>DW2(y))) || \
                             ((DW3(x)==DW3(y)) && (DW2(x)==DW2(y)) && \
                              (DW1_1(x)>DW1(y))) || \
                             ((DW3(x)==DW3(y)) && (DW2(x)==DW2(y)) && \
                              (DW1(x)==DW1(y)) && (DW0(x)>=DW0(y)))) : \
                           (sizeof(x) == sizeof(U64)) ? \
                              (((DW3(x)==DW3(y)) && (DW2(x)==DW2(y)) && \
                              (DW1_1(x)>DW1(y))) || \
                             ((DW3(x)==DW3(y)) && (DW2(x)==DW2(y)) && \
                              (DW1(x)==DW1(y)) && (DW0(x)>=DW0(y)))) : \
                        /* (sizeof(x) == sizeof(U32)) */  \
                             (((DW3(x)==DW3(y)) && (DW2(x)==DW2(y)) && \
                              (DW1(x)==DW1(y)) && (DW0(x)>=DW0(y)))))


/*
#define IEL_CMPGU(x, y)     ((DW3(x)>DW3(y)) || \
                             ((DW3(x)==DW3(y)) && (DW2(x)>DW2(y))) || \
                             ((DW3(x)==DW3(y)) && (DW2(x)==DW2(y)) && \
                              (DW1(x)>DW1(y))) || \
                             ((DW3(x)==DW3(y)) && (DW2(x)==DW2(y)) && \
                              (DW1(x)==DW1(y)) && (DW0(x)>DW0(y))))

#define IEL_CMPGEU(x, y)     ((DW3(x)>DW3(y)) || \
                             ((DW3(x)==DW3(y)) && (DW2(x)>DW2(y))) || \
                             ((DW3(x)==DW3(y)) && (DW2(x)==DW2(y)) && \
                              (DW1(x)>DW1(y))) || \
                             ((DW3(x)==DW3(y)) && (DW2(x)==DW2(y)) && \
                              (DW1(x)==DW1(y)) && (DW0(x)>=DW0(y))))

*/

#define IEL_SHL(x, y, n)        (IEL_t2=n, \
                                 ((n) <=0 || (n) >= (sizeof(x)<<3)) ? \
                                 ((IEL_ISZERO(y)||!(n)) ? \
                                        (IEL_ASSIGNU(x, y), IEL_OK) :\
                                        (IEL_ZERO(x), IEL_OVFL)):\
                             ((sizeof(x) == sizeof (U32)) ? \
                                (IEL_t1 = (DW1(y) || DW2(y) || DW3(y) \
                                 || (DW0(y)>= ((U4byte)1<<(32-IEL_t2)))), \
                                 (x).dw0_32 = DW0(y) << IEL_t2, (IEL_t1)) : \
                              (sizeof(x) == sizeof (U64)) ? \
                                ((n) == 32) ? ((IEL_t1 = DW3(y) || DW2(y)\
                                                 || DW1(y)), \
                                    (x).dw1_64 = DW0(y), \
                                    (x).dw0_64 = 0, \
                                    (IEL_t1)) : \
                                ((n) < 32) ? \
                                    (IEL_t1 = (DW2(y) || DW3(y) || \
                                               (DW1(y)>=((U4byte)1<<(32-IEL_t2)))), \
                                     (x).dw1_64 = DW1(y)<<IEL_t2 | \
                                                DW0(y)>>(32-IEL_t2), \
                                     (x).dw0_64 = DW0(y)<<IEL_t2,(IEL_t1)) :\
                                    (IEL_t1 = (DW1(y) || DW2(y) || DW3(y) ||\
                                             ((DW0(y)>=((U4byte)1<<(64-IEL_t2))))),\
                                     (x).dw1_64 = DW0(y)<<(IEL_t2-32), \
                                     (x).dw0_64 = 0, (IEL_t1)) : \
                               (sizeof(x) == sizeof (U128)) ? \
                                ((n)==32) ? (IEL_t1 = (DW3(y)), \
                                          (x).dw3_128 = DW2(y), \
                                          (x).dw2_128 = DW1(y), \
                                          (x).dw1_128 = DW0(y), \
                                          (x).dw0_128 = 0, (IEL_t1!=0)) : \
                                ((n)==64) ? (IEL_t1 = (DW3(y) || DW2(y)),\
                                          (x).dw3_128 = DW1(y), \
                                          (x).dw2_128 = DW0(y), \
                                          (x).dw1_128 = 0, \
                                          (x).dw0_128 = 0, (IEL_t1)) : \
                                ((n)==96) ? (IEL_t1 = (DW3(y) || DW2(y)  \
                                                    || DW1(y)), \
                                          (x).dw3_128 = DW0(y), \
                                          (x).dw2_128 = 0, \
                                          (x).dw1_128 = 0, \
                                          (x).dw0_128 = 0, (IEL_t1)) : \
                                ((n)>96) ? \
                                    (IEL_t1 = (DW1(y) || DW2(y) || DW3(y) ||\
                                       ((DW0(y)>=((U4byte)1<<(128-IEL_t2))))), \
                                     (x).dw3_128 = DW0(y)<<(IEL_t2-96), \
                                     (x).dw2_128 = 0, \
                                     (x).dw1_128 = 0, \
                                     (x).dw0_128 = 0, \
                                     (IEL_t1)) : \
                                ((n)>64) ? \
                                    (IEL_t1 = (DW2(y) || DW3(y) || \
                                              ((DW1(y)>=(U4byte)1<<(96-IEL_t2)))),\
                                     (x).dw3_128 = DW1(y)<<(IEL_t2-64) | \
                                                    DW0(y)>>(96-IEL_t2),\
                                     (x).dw2_128 = DW0(y)<<(IEL_t2-64), \
                                     (x).dw1_128 = 0, \
                                     (x).dw0_128 = 0, \
                                     (IEL_t1)) : \
                                ((n)>32) ?  \
                                    (IEL_t1 = (DW3(y) || ((IEL_t2!=32) & \
                                                 (DW2(y)>=(U4byte)1<<(64-IEL_t2)))),\
                                     (x).dw3_128 = DW2(y)<<(IEL_t2-32) | \
                                                    DW1(y)>>(64-IEL_t2),\
                                     (x).dw2_128 = DW1(y)<<(IEL_t2-32) | \
                                                    DW0(y)>>(64-IEL_t2),\
                                     (x).dw1_128 = DW0(y)<<(IEL_t2-32), \
                                     (x).dw0_128 = 0, \
                                     (IEL_t1)) : \
                                    (IEL_t1 = (DW3(y)>=(U4byte)1<<(32-IEL_t2)), \
                                     (x).dw3_128 = DW3(y)<<(IEL_t2) | \
                                                    DW2(y)>>(32-IEL_t2),\
                                     (x).dw2_128 = DW2(y)<<(IEL_t2) | \
                                                    DW1(y)>>(32-IEL_t2),\
                                     (x).dw1_128 = DW1(y)<<(IEL_t2) | \
                                                    DW0(y)>>(32-IEL_t2),\
                                     (x).dw0_128 = DW0(y)<<IEL_t2, \
                                     (IEL_t1)) : (IEL_OVFL)))

#define IEL_SHL128(x, y, n)     (IEL_t2=(n), \
                               (sizeof(x) == sizeof (U128)) ? \
                                ((n)==32) ? (IEL_t1 = (U4byte)(DW3(y)), \
                                          (x).dw3_128 = (U4byte)DW2(y), \
                                          (x).dw2_128 = (U4byte)DW1(y), \
                                          (x).dw1_128 = (U4byte)DW0(y), \
                                          (x).dw0_128 = 0, (IEL_t1!=0)) : \
                                ((n)==64) ? (IEL_t1 = (U4byte)(DW3(y) || DW2(y)),\
                                          (x).dw3_128 = (U4byte)DW1(y), \
                                          (x).dw2_128 = (U4byte)DW0(y), \
                                          (x).dw1_128 = 0, \
                                          (x).dw0_128 = 0, (IEL_t1)) : \
                                ((n)==96) ? (IEL_t1 = (U4byte)(DW3(y) || DW2(y)  \
                                                    || DW1(y)), \
                                          (x).dw3_128 = (U4byte)DW0(y), \
                                          (x).dw2_128 = 0, \
                                          (x).dw1_128 = 0, \
                                          (x).dw0_128 = 0, (IEL_t1)) : \
                                ((n)>96) ? \
                                    (IEL_t1 = (U4byte)(DW1(y) || DW2(y) || DW3(y) ||\
                                       ((DW0(y)>=((U4byte)1<<(128-IEL_t2))))), \
                                     (x).dw3_128 = (U4byte)DW0(y)<<(IEL_t2-96), \
                                     (x).dw2_128 = 0, \
                                     (x).dw1_128 = 0, \
                                     (x).dw0_128 = 0, \
                                     (IEL_t1)) : \
                                ((n)>64) ? \
                                    (IEL_t1 = (U4byte)(DW2(y) || DW3(y) || \
                                              ((DW1(y)>=(U4byte)1<<(96-IEL_t2)))),\
                                     (x).dw3_128 = (U4byte)DW1(y)<<(IEL_t2-64) | \
                                                    DW0(y)>>(96-IEL_t2),\
                                     (x).dw2_128 = (U4byte)DW0(y)<<(IEL_t2-64), \
                                     (x).dw1_128 = 0, \
                                     (x).dw0_128 = 0, \
                                     (IEL_t1)) : \
                                ((n)>32) ?  \
                                    (IEL_t1 = (U4byte)(DW3(y) || ((IEL_t2!=32) & \
                                                 (DW2(y)>=(U4byte)1<<(64-IEL_t2)))),\
                                     (x).dw3_128 = (U4byte)DW2(y)<<(IEL_t2-32) | \
                                                    DW1(y)>>(64-IEL_t2),\
                                     (x).dw2_128 = (U4byte)DW1(y)<<(IEL_t2-32) | \
                                                    DW0(y)>>(64-IEL_t2),\
                                     (x).dw1_128 = (U4byte)DW0(y)<<(IEL_t2-32), \
                                     (x).dw0_128 = 0, \
                                     (IEL_t1)) : \
                                    (IEL_t1 = (U4byte)(DW3(y)>=(U4byte)1<<(32-IEL_t2)), \
                                     (x).dw3_128 = (U4byte)DW3(y)<<(IEL_t2) | \
                                                    DW2(y)>>(32-IEL_t2),\
                                     (x).dw2_128 = (U4byte)DW2(y)<<(IEL_t2) | \
                                                    DW1(y)>>(32-IEL_t2),\
                                     (x).dw1_128 = (U4byte)DW1(y)<<(IEL_t2) | \
                                                    DW0(y)>>(32-IEL_t2),\
                                     (x).dw0_128 = (U4byte)DW0(y)<<IEL_t2, \
                                     (IEL_t1)) : (IEL_OVFL))


#define IEL_INL(x, n)           (((((U4byte)1<<n)-1) & x))
#define IEL_INH(x, n)       (x>>n)
#define IEL_SHR(x, y, n)    (IEL_t2=n,                              \
                                ((n) <=0) ? \
                                 ((IEL_ISZERO(y)||!(n)) ? \
                                        (IEL_ASSIGNU(x, y), IEL_OK) :\
                                        (IEL_ZERO(x), IEL_OVFL)): \
                               ((sizeof(x) == sizeof (U32)) ? \
                                ((n)==32) ? \
                                          ((x).dw0_32 = DW1(y), \
                                           (DW3(y) || DW2(y))) : \
                                ((n)==64) ?  \
                                          ((x).dw0_32 = DW2(y), \
                                           (DW3(y)!=0)) : \
                                ((n)==96) ? \
                                          ((x).dw0_32 = DW3(y), \
                                           (IEL_OK)) : \
                                ((n)>96) ? \
                                     ((x).dw0_32 = DW3(y)>>(IEL_t2-96), \
                                      (IEL_OK)) : \
                                ((n)>64) ? \
                                     ((x).dw0_32 = ((DW2(y)>>(IEL_t2-64))| \
                                                  (DW3(y)<<(96-IEL_t2))),\
                                      ((DW3(y)>>(IEL_t2-64))!=0)) : \
                                ((n)>32) ?  \
                                     ((x).dw0_32 = ((DW1(y)>>(IEL_t2-32))| \
                                                   (DW2(y)<<(64-IEL_t2))), \
                                      ((DW2(y)>>(IEL_t2-32)) || DW3(y))) :  \
                                    ((x).dw0_32 = DW0(y)>>(IEL_t2) | \
                                                    DW1(y)<<(32-IEL_t2),\
                                     (DW3(y) || DW2(y) ||DW1(y)>>(IEL_t2))) : \
                               (sizeof(x) == sizeof (U64)) ? \
                                ((n)==32) ? \
                                          ((x).dw0_64 = DW1(y), \
                                           (x).dw1_64 = DW2(y), \
                                           (DW3(y)!=0)) : \
                                ((n)==64) ?  \
                                          ((x).dw0_64 = DW2(y), \
                                           (x).dw1_64 = DW3(y), \
                                           (IEL_OK)) : \
                                ((n)==96) ? \
                                          ((x).dw0_64 = DW3(y), \
                                           (x).dw1_64 = 0, \
                                           (IEL_OK)) : \
                                ((n)>96) ? \
                                     ((x).dw0_64 = DW3(y)>>(IEL_t2-96), \
                                      (x).dw1_64 = 0, \
                                      (IEL_OK)) : \
                                ((n)>64) ? \
                                     ((x).dw0_64 = ((DW2(y)>>(IEL_t2-64))| \
                                                  (DW3(y)<<(96-IEL_t2))),\
                                      (x).dw1_64 = DW3(y)>>(IEL_t2-64), \
                                      (IEL_OK)) : \
                                ((n)>32) ?  \
                                     ((x).dw0_64 = ((DW1(y)>>(IEL_t2-32))| \
                                                   (DW2(y)<<(64-IEL_t2))), \
                                      (x).dw1_64 = ((DW2(y)>>(IEL_t2-32))| \
                                                   (DW3(y)<<(64-IEL_t2))),\
                                      (DW3(y)>>(IEL_t2-32) != 0)) :  \
                                    ((x).dw0_64 = DW0(y)>>(IEL_t2) | \
                                                    DW1(y)<<(32-IEL_t2),\
                                     (x).dw1_64 = DW1(y)>>(IEL_t2) | \
                                                    DW2(y)<<(32-IEL_t2),\
                                     (DW3(y) || DW2(y)>>(IEL_t2))) : \
                               (sizeof(x) == sizeof (U128)) ? \
                                ((n)==32) ? \
                                          ((x).dw0_128 = DW1(y), \
                                           (x).dw1_128 = DW2(y), \
                                           (x).dw2_128 = DW3(y), \
                                           (x).dw3_128 = 0, (IEL_OK)) : \
                                ((n)==64) ?  \
                                          ((x).dw0_128 = DW2(y), \
                                           (x).dw1_128 = DW3(y), \
                                           (x).dw2_128 = 0, \
                                           (x).dw3_128 = 0, (IEL_OK)) : \
                                ((n)==96) ? \
                                          ((x).dw0_128 = DW3(y), \
                                           (x).dw1_128 = 0, \
                                           (x).dw2_128 = 0, \
                                           (x).dw3_128 = 0, (IEL_OK)) : \
                                ((n)>96) ? \
                                     ((x).dw0_128 = DW3(y)>>(IEL_t2-96), \
                                      (x).dw1_128 = 0, \
                                      (x).dw2_128 = 0, \
                                      (x).dw3_128 = 0, \
                                      (IEL_OK)) : \
                                ((n)>64) ? \
                                     ((x).dw0_128 = ((DW2(y)>>(IEL_t2-64))| \
                                                  (DW3(y)<<(96-IEL_t2))),\
                                      (x).dw1_128 = DW3(y)>>(IEL_t2-64), \
                                      (x).dw2_128 = 0, \
                                      (x).dw3_128 = 0, \
                                      (IEL_OK)) : \
                                ((n)>32) ?  \
                                     ((x).dw0_128 = ((DW1(y)>>(IEL_t2-32))| \
                                                   (DW2(y)<<(64-IEL_t2))), \
                                      (x).dw1_128 = ((DW2(y)>>(IEL_t2-32))| \
                                                   (DW3(y)<<(64-IEL_t2))),\
                                      (x).dw2_128 = DW3(y)>>(IEL_t2-32), \
                                      (x).dw3_128 = 0, \
                                      (IEL_OK)) : \
                                    ((x).dw0_128 = DW0(y)>>(IEL_t2) | \
                                                    DW1(y)<<(32-IEL_t2),\
                                     (x).dw1_128 = DW1(y)>>(IEL_t2) | \
                                                    DW2(y)<<(32-IEL_t2),\
                                     (x).dw2_128 = DW2(y)>>(IEL_t2) | \
                                                    DW3(y)<<(32-IEL_t2),\
                                     (x).dw3_128 = DW3(y)>>IEL_t2, \
                                     (IEL_OK)) : (IEL_OVFL)))

#define IEL_SHR128(x, y, n) (IEL_t2=n,                              \
                                ((n) <=0) ? \
                                 ((IEL_ISZERO(y)||!(n)) ? \
                                        (IEL_ASSIGNU(x, y), IEL_OK) :\
                                        (IEL_ZERO(x), IEL_OVFL)): \
                               (sizeof(x) == sizeof (U128)) ? \
                                ((n)==32) ? \
                                          ((x).dw0_128 = DW1(y), \
                                           (x).dw1_128 = DW2(y), \
                                           (x).dw2_128 = DW3(y), \
                                           (x).dw3_128 = 0, (IEL_OK)) : \
                                ((n)==64) ?  \
                                          ((x).dw0_128 = DW2(y), \
                                           (x).dw1_128 = DW3(y), \
                                           (x).dw2_128 = 0, \
                                           (x).dw3_128 = 0, (IEL_OK)) : \
                                ((n)==96) ? \
                                          ((x).dw0_128 = DW3(y), \
                                           (x).dw1_128 = 0, \
                                           (x).dw2_128 = 0, \
                                           (x).dw3_128 = 0, (IEL_OK)) : \
                                ((n)>96) ? \
                                     ((x).dw0_128 = DW3(y)>>(IEL_t2-96), \
                                      (x).dw1_128 = 0, \
                                      (x).dw2_128 = 0, \
                                      (x).dw3_128 = 0, \
                                      (IEL_OK)) : \
                                ((n)>64) ? \
                                     ((x).dw0_128 = ((DW2(y)>>(IEL_t2-64))| \
                                                  (DW3(y)<<(96-IEL_t2))),\
                                      (x).dw1_128 = DW3(y)>>(IEL_t2-64), \
                                      (x).dw2_128 = 0, \
                                      (x).dw3_128 = 0, \
                                      (IEL_OK)) : \
                                ((n)>32) ?  \
                                     ((x).dw0_128 = ((DW1(y)>>(IEL_t2-32))| \
                                                   (DW2(y)<<(64-IEL_t2))), \
                                      (x).dw1_128 = ((DW2(y)>>(IEL_t2-32))| \
                                                   (DW3(y)<<(64-IEL_t2))),\
                                      (x).dw2_128 = DW3(y)>>(IEL_t2-32), \
                                      (x).dw3_128 = 0, \
                                      (IEL_OK)) : \
                                    ((x).dw0_128 = DW0(y)>>(IEL_t2) | \
                                                    DW1(y)<<(32-IEL_t2),\
                                     (x).dw1_128 = DW1(y)>>(IEL_t2) | \
                                                    DW2(y)<<(32-IEL_t2),\
                                     (x).dw2_128 = DW2(y)>>(IEL_t2) | \
                                                    DW3(y)<<(32-IEL_t2),\
                                     (x).dw3_128 = DW3(y)>>IEL_t2, \
                                     (IEL_OK)) : (IEL_OVFL))




#ifndef IEL_USE_FUNCTIONS
#define IEL_ADDS(x, y, z)   (IEL_ASSIGNS(IEL_ext4, y), \
                             IEL_ASSIGNS(IEL_ext2, z), \
                             IEL_ADDU (IEL_ext3, IEL_ext4, IEL_ext2), \
                             ((IEL_ISNEG(IEL_ext4) && IEL_ISNEG(IEL_ext2) && \
                               (!(IEL_ISNEG(IEL_ext3)))) | \
                              ((!(IEL_ISNEG(IEL_ext4))) && \
                               (!(IEL_ISNEG(IEL_ext2))) && \
                               IEL_ISNEG(IEL_ext3))  | \
                              (IEL_ASSIGNS(x, IEL_ext3))))
#else

#define IEL_ADDU128(x, y, z) (IEL_ASSIGNU(IEL_ext1, y),\
                                (x).dw0_128=DW0(y)+DW0(z),\
                                 (x).dw1_128=DW1(y)+DW1(z)+IEL_C0(x),\
                                 (x).dw2_128=DW2(y)+DW2(z)+IEL_C1(x),\
                                 (x).dw3_128=DW3(y)+DW3(z)+IEL_C2(x),\
                                 (IEL_C3(x)))
#define IEL_ISNEG128(x)      ((DW3(x)&0x80000000)!=0)

#define IEL_ADDS(x, y, z)   (IEL_ASSIGNS(IEL_ext4, y), \
                             IEL_ASSIGNS(IEL_ext2, z), \
                             IEL_ADDU128 (IEL_ext3, IEL_ext4, IEL_ext2), \
                             ((IEL_ISNEG128(IEL_ext4) && \
                               IEL_ISNEG128(IEL_ext2) && \
                               (!(IEL_ISNEG128(IEL_ext3)))) | \
                              ((!(IEL_ISNEG128(IEL_ext4))) && \
                               (!(IEL_ISNEG128(IEL_ext2))) && \
                               IEL_ISNEG128(IEL_ext3))  | \
                              (IEL_ASSIGNS(x, IEL_ext3))))

#endif 

#endif /*** not LP64 ***/

#define IEL_SEXT64(x)   ((x).dw1_64 = ((x).dw0_64 & 0x80000000) ? -1 : 0)
/* common for lp32 and lp64 */
#define IEL_CONVERT4(x, y0, y1, y2, y3) \
                            ((sizeof(x) == sizeof(U32)) ? \
                                ((x).dw0_32 = y0, y1 || y2 || y3) : \
                             (sizeof(x) == sizeof(U64)) ? \
                                ((x).dw0_64 = y0, \
                                 (x).dw1_64 = y1, \
                                 y2 || y3) :\
                             (sizeof(x) == sizeof(U128)) ? \
                                ((x).dw0_128 = y0, \
                                 (x).dw1_128 = y1, \
                                 (x).dw2_128 = y2, \
                                 (x).dw3_128 = y3, \
                                 IEL_OK) : IEL_OVFL)

#define IEL_CONVERT2(x, y0, y1) IEL_CONVERT4(x, y0, y1, 0, 0)
#define IEL_CONVERT1(x, y0)     IEL_CONVERT4(x, y0, 0, 0, 0)
#define IEL_CONVERT IEL_CONVERT4

#define IEL_COMP(x, y) (IEL_NOT(x, y), IEL_INCU(x), IEL_OK)
#define IEL_COMPLEMENTS(x, y) (IEL_ASSIGNS(IEL_ts1, y), \
                              IEL_COMP(IEL_ts1, IEL_ts1), \
                              IEL_ASSIGNS(x, IEL_ts1))

#define IEL_CMPEU(x, y)  IEL_EQU(x, y)
#define IEL_CMPNEU(x, y) (!(IEL_EQU(x, y)))
#define IEL_CMPLU(x, y) IEL_CMPGU(y, x)
#define IEL_CMPLEU(x, y) IEL_CMPGEU(y, x)
#define IEL_CMPU(x, y)  (IEL_CMPGU(x, y)-IEL_CMPLU(x, y))
#define IEL_SUBU(x, y, z) (IEL_ISZERO(z) ? IEL_ASSIGNU(x, y) : \
                           (IEL_COMP(IEL_ext2 ,z), \
                           (!(IEL_ADDU(x, y,IEL_ext2)))))


#define IEL_SUBS(x, y, z)   (IEL_ISZERO(z) ? IEL_ASSIGNS(x, y) : \
                             (IEL_ASSIGNS(IEL_ext5, z), \
                             IEL_COMP(IEL_ext5,IEL_ext5),\
                             IEL_ADDS(x, y, IEL_ext5)||IEL_ISNINF(IEL_ext5)))

#define IEL_CMPES(x, y)     (IEL_ASSIGNS(IEL_ts1, x), \
                             IEL_ASSIGNS(IEL_ts2, y), \
                             IEL_CMPEU(IEL_ts1, IEL_ts2))

#define IEL_CMPNES(x, y)    (!(IEL_CMPES(x, y)))

#define IEL_CMPGS(x, y)     (((IEL_ISNEG(x)) && (!(IEL_ISNEG(y)))) ? (0) : \
                             ((!(IEL_ISNEG(x))) && (IEL_ISNEG(y))) ? (1) : \
                             (IEL_ASSIGNS(IEL_ext3, x), \
                             IEL_ASSIGNS(IEL_ext4, y), \
                             IEL_CMPGU(IEL_ext3, IEL_ext4)))

#define IEL_CMPGES(x, y)    (IEL_CMPGS(x, y) || IEL_CMPES(x, y))
#define IEL_CMPLES(x, y)    IEL_CMPGES(y, x)
#define IEL_CMPLS(x, y)     IEL_CMPGS(y, x)
#define IEL_CMPS(x, y)      (IEL_CMPGS(x, y)-IEL_CMPLS(x, y))
#define IEL_CHECKU(x, n)    (!IEL_SHL128(IEL_ext1, x, 128-(n)))
#define IEL_CHECKS(x, n)    ((IEL_ISNEG(x)) ? \
                                (IEL_ASSIGNS(IEL_ts1, x), \
                                IEL_COMP(IEL_ts1, IEL_ts1), \
                                !((IEL_SHL128(IEL_ts1, IEL_ts1, 128-(n))) || \
                                (IEL_ISNEG(IEL_ts1)&&(!IEL_ISNINF(IEL_ts1))))):\
                                (!(IEL_SHL128(IEL_ts1, x, 128-(n)) || \
                                IEL_ISNEG(IEL_ts1))))

#define IEL_SAR(x, y, n)  (IEL_ISNEG(y) ? \
                           (IEL_SEXT(IEL_ext4, y), (IEL_SHR(x, IEL_ext4, n),  \
                            IEL_SHL(IEL_ext5, IEL_MINUS1, 128-n),  \
                            IEL_OR(IEL_ext5, x, IEL_ext5) , \
                            ((IEL_ASSIGNS(x,IEL_ext5))||(n>=(sizeof(x)<<3))))) \
                            : IEL_SHR(x, y, n))

#define IEL_MULU(x, y, z)   (IEL_ASSIGNU (IEL_ext2, y), \
                             IEL_ASSIGNU (IEL_ext3, z), \
                             (IEL_t4=IEL_mul(&IEL_ext1,&IEL_ext2,&IEL_ext3),\
                              IEL_ASSIGNU (x, IEL_ext1) || IEL_t4))
#define IEL_MULS(x, y, z)    (IEL_ASSIGNS (IEL_ext2, y), \
                             IEL_ASSIGNS (IEL_ext3, z), \
                             IEL_t3 = IEL_ISNEG(y)^IEL_ISNEG(z), \
                             (IEL_ISNEG(IEL_ext2)) ? \
                                IEL_COMP(IEL_ext2, IEL_ext2) : (0),\
                             (IEL_ISNEG(IEL_ext3)) ? \
                                IEL_COMP(IEL_ext3, IEL_ext3) : (0),\
                             IEL_t2 = \
                                (IEL_mul(&IEL_ext1, &IEL_ext2, &IEL_ext3) ||\
                                 (IEL_ISNEG(IEL_ext1) && \
                                  (!IEL_ISNINF(IEL_ext1)))), \
                             IEL_t3 ? IEL_COMP(IEL_ext1,IEL_ext1):(0),\
                             (IEL_ASSIGNS(x,IEL_ext1) || IEL_t2))
#define IEL_DIVU(x, y, z)   (IEL_ISZERO(z) ? (IEL_ASSIGNU(x,IEL_POSINF), \
                                              IEL_OVFL):\
                            (IEL_ASSIGNU (IEL_ext2, y), \
                             IEL_ASSIGNU (IEL_ext3, z), \
                             (IEL_t4=IEL_div(&IEL_ext1,&IEL_ext2,&IEL_ext3),\
                              IEL_ASSIGNU (x, IEL_ext1) || IEL_t4)))
#define IEL_DIVS(x, y, z)   (IEL_ISZERO(z) ? ((IEL_ISNEG(y)) ? \
                            IEL_ASSIGNU(IEL_ext2, IEL_NEGINF): \
                            IEL_ASSIGNU(IEL_ext2, IEL_POSINF)\
                            , IEL_ASSIGNU(x, IEL_ext2), IEL_OVFL) :\
                             (IEL_ASSIGNS (IEL_ext2, y), \
                             IEL_ASSIGNS (IEL_ext3, z), \
                             IEL_t3 = IEL_ISNEG(y)^IEL_ISNEG(z), \
                             (IEL_ISNEG(IEL_ext2)) ? \
                                IEL_COMP(IEL_ext2, IEL_ext2) : (0),\
                             (IEL_ISNEG(IEL_ext3)) ? \
                                IEL_COMP(IEL_ext3, IEL_ext3) : (0),\
                             IEL_t2 = \
                                (IEL_div(&IEL_ext1, &IEL_ext2, &IEL_ext3) ||\
                                 (IEL_ISNEG(IEL_ext1) && (!IEL_t3))), \
                             IEL_t3 ? IEL_COMP(IEL_ext1,IEL_ext1):(0),\
                             (IEL_ASSIGNS(x,IEL_ext1) || IEL_t2)))

#define IEL_REMU(x, y, z)   (IEL_ASSIGNU (IEL_ext2, y), \
                             IEL_ASSIGNU (IEL_ext3, z), \
                             (IEL_t1 = IEL_rem(&IEL_ext1, &IEL_ext2, &IEL_ext3), \
                              IEL_t2 = IEL_ASSIGNU (x, IEL_ext1), IEL_t1||IEL_t2))
#define IEL_REMS(x, y, z)    (IEL_ASSIGNS (IEL_ext2, y), \
                             IEL_ASSIGNS (IEL_ext3, z), \
                             IEL_t3 = IEL_ISNEG(y), \
                             (IEL_ISNEG(IEL_ext2)) ? \
                                IEL_COMP(IEL_ext2, IEL_ext2) : (0),\
                             (IEL_ISNEG(IEL_ext3)) ? \
                                IEL_COMP(IEL_ext3, IEL_ext3) : (0),\
                             IEL_t2 = \
                                (IEL_rem(&IEL_ext1, &IEL_ext2, &IEL_ext3)|| \
                                 IEL_ISNEG(IEL_ext1)), \
                             IEL_t3 ? IEL_COMP(IEL_ext1,IEL_ext1):(0),\
                             (IEL_ASSIGNS(x,IEL_ext1) || IEL_t2))




/**** INT64.H MACROS ****/

#ifdef IEL_INT64

#define INCU64(x)           (IEL_INCU(IEL64(x)), (x))
#define DECU64(x)           (IEL_DECU(IEL64(x)), (x))
#define ADDU64(x, y, t)     (IEL_ADDU(IEL64(x), IEL64(x), IEL64(y)), (x))
#define SUBU64(x, y, t)     (IEL_SUBU(IEL64(x), IEL64(x), IEL64(y)), (x))
#define ANDNOT64(x, y)      (IEL_ANDNOT(IEL64(x), IEL64(x), IEL64(y)),(x))
#define AND64NOT32(x, y)    (IEL_ANDNOT(IEL64(x), IEL64(x), IEL32(y)),(x))
#define ANDU64(x, y)        (IEL_AND(IEL64(x), IEL64(x), IEL64(y)), (x))
#define ORU64(x, y)         (IEL_OR(IEL64(x), IEL64(x), IEL64(y)), (x))
#define NOTU64(x)           (IEL_NOT(IEL64(x), IEL64(x)), (x))
#define ZU64(x)             (IEL_ZERO(IEL64(x)), (x))
#define INIT64(x, y)        (IEL_CONVERT1(x, y), (x))
#define CONST64(x)          IEL_CONST64(x, 0)
#define SCONST64(x)         IEL_CONST64(x, x>>31)
#define CONST64_64(x, y)    IEL_CONST64(y, x)
#define ISZERO64(x)         (IEL_ISZERO(IEL64(x)))
#define EQU64(x, y)         (IEL_EQU(IEL64(x), IEL64(y)))
#define LEU64(x, y)         (IEL_CMPLEU(IEL64(x), IEL64(y)))
#define LU64(x, y)          (IEL_CMPLU(IEL64(x), IEL64(y)))
#define LSU64(x, y)         (IEL_CMPLS(IEL64(x), IEL64(y)))
#define GEU64(x, y)         (IEL_CMPGEU(IEL64(x), IEL64(y)))
#define GU64(x, y)          (IEL_CMPGU(IEL64(x), IEL64(y)))
#define CMP64(x, y, t)      (IEL_CMPU(IEL64(x), IEL64(y)))
#define SHL64(x, y)         (IEL_SHL(IEL64(x), IEL64(x), y), (x))
#define ISNEG(x)            (IEL_ISNEG(IEL64(x)))
#define SEXT64(x)           ((x).dw1_64 = ((x).dw0_64 & 0x80000000) ? -1 : 0)
#define CMP128(x, y, t)     (IEL_CMPU(IEL128(x), IEL128(y)))
#define EQU128(x, y)        (IEL_EQU(IEL128(x), IEL128(y)))
#define LU64TU32(a, b)      (IEL_CMPLU(IEL64(a), IEL32(b)))
#define LU64EU32(a,b)       (IEL_CMPLEU(IEL64(a), IEL32(b)))
#define GU64TU32(a,b)       (IEL_CMPGU(IEL64(a), IEL32(b)))
#define GU64EU32(a,b)       (IEL_CMPGEU(IEL64(a), IEL32(b)))
#define GU64_32(a,b)        (GU64TU32(a, b))
#define INITL64(x, y, z)    (IEL_CONVERT2(x, z, y), (IEL64(x)))
 
#ifdef LP64
#   define ADD2U64(x, y)    ADDU64(x, y, 0)
#   define SUB2U64(x, y)    SUBU64(x, y, 0)
#   define LOWER32(x)   (*(int*)(&x) & 0x00000000ffffffff)
#   define HIGHER32(x)  (*(int*)(&x)>>32)
#else /*** LP64 ***/
#   define ADD2U64(x, y) ((x).low+=(y), (x).high += ((x).low < (y)), (x))
#   define SUB2U64(x, y) ( (x).high -= ((x).low < (y)),(x).low-=(y), (x))
#   define LOWER32(x)           (IEL_GETDW0(IEL64(x)))
#   define HIGHER32(x)          (IEL_GETDW1(IEL64(x)))
#endif /*** LP64 ***/
#endif /*** IEL_INT64 ***/


#ifndef LP64
/* In order to decrease the macro expansion space */

#ifdef IEL_USE_FUNCTIONS 

#ifdef __cplusplus
extern "C" {
#endif

int IEL_au(void *x, void *y, int sx, int sy);
int IEL_c0(void *x, int sx);
int IEL_c1(void *x, int sx);
int IEL_c2(void *x, int sx);
int IEL_c3(void *x, int sx);
IEL_Err IEL_as(void *x, void *y, int sx, int sy);

#ifdef __cplusplus
}
#endif

#undef IEL_ASSIGNU
#undef IEL_ASSIGNS
#undef IEL_C0
#undef IEL_C1
#undef IEL_C2
#undef IEL_C3

#define IEL_ASSIGNU(x, y)   IEL_au((void *)&(x),(void *)&(y),sizeof(x),sizeof(y))
#define IEL_ASSIGNS(x, y)   IEL_as((void *)&(x),(void *)&(y),sizeof(x),sizeof(y))

#define IEL_C0(x) IEL_c0((void *)&(x),sizeof(x))
#define IEL_C1(x) IEL_c1((void *)&(x),sizeof(x))
#define IEL_C2(x) IEL_c2((void *)&(x),sizeof(x))
#define IEL_C3(x) IEL_c3((void *)&(x),sizeof(x))


#endif /* IEL_USE_FUNCTIONS */


#endif /* LP64 */
/* Prototypes */

#ifdef __cplusplus
extern "C" {
#endif

IEL_Err IEL_mul(U128 *xr, U128 *y, U128 *z);
IEL_Err IEL_rem(U128 *x, U128 *y, U128 *z);
IEL_Err IEL_div(U128 *x, U128 *y, U128 *z);
IEL_Err IEL_U128tostr(const U128 *x, char *strptr, int  base, const unsigned int  length);
IEL_Err IEL_U64tostr(const U64 *x, char *strptr, int  base, const unsigned int  length);
IEL_Err IEL_S128tostr(const S128 *x, char *strptr, int  base,const unsigned int  length);
IEL_Err IEL_S64tostr(const S64 *x, char *strptr, int  base,const unsigned int  length);
IEL_Err  IEL_strtoU128( char *str1, char **endptr, int  base, U128 *x);
IEL_Err  IEL_strtoU64(char *str1, char **endptr, int  base, U64 *x);
IEL_Err  IEL_strtoS128(char *str1, char **endptr, int  base, S128 *x);
IEL_Err  IEL_strtoS64(char *str1, char **endptr, int  base, S64 *x);

#ifdef __cplusplus
}
#endif


/* INT64, inside varibales redefinition */

#ifdef IEL_INT64

#undef low
#define low dw0_64
#undef high
#define high dw1_64
#undef b1st
#undef b2st
#undef b3st
#undef b4st
#define b1st dw0_128
#define b2st dw1_128
#define b3st dw3_128
#define b4st dw4_128

#endif

#endif /**** IEL_H ****/

/***                                                                       ***/
/***   INTEL CORPORATION PROPRIETARY INFORMATION                           ***/
/***                                                                       ***/
/***   This software is supplied under the terms of a license              ***/
/***   agreement or nondisclosure agreement with Intel Corporation         ***/
/***   and may not be copied or disclosed except in accordance with        ***/
/***   the terms of that agreement.                                        ***/
/***   Copyright (c) 1992,1993,1994,1995,1996,1997,1998 Intel Corporation. ***/
/***                                                                       ***/

#ifndef _INST_ID_H
#define _INST_ID_H

/*** EMDB version: 3.18 ***/
typedef unsigned Inst_id_t;

typedef enum Inst_id_e
{
EM_INST_NONE = 0,
EM_ILLOP = EM_INST_NONE,
EM_IGNOP,
EM_ADD_R1_R2_R3,
EM_ADD_R1_R2_R3_1,
EM_SUB_R1_R2_R3,
EM_SUB_R1_R2_R3_1,
EM_ADDP4_R1_R2_R3,
EM_AND_R1_R2_R3,
EM_ANDCM_R1_R2_R3,
EM_OR_R1_R2_R3,
EM_XOR_R1_R2_R3,
EM_SHLADD_R1_R2_COUNT2_R3,
EM_SHLADDP4_R1_R2_COUNT2_R3,
EM_SUB_R1_IMM8_R3,
EM_AND_R1_IMM8_R3,
EM_ANDCM_R1_IMM8_R3,
EM_OR_R1_IMM8_R3,
EM_XOR_R1_IMM8_R3,
EM_ADDS_R1_IMM14_R3,
EM_ADDP4_R1_IMM14_R3,
EM_ADDL_R1_IMM22_R3,
EM_CMP_LT_P1_P2_R2_R3,
EM_CMP_LTU_P1_P2_R2_R3,
EM_CMP_EQ_P1_P2_R2_R3,
EM_CMP_LT_UNC_P1_P2_R2_R3,
EM_CMP_LTU_UNC_P1_P2_R2_R3,
EM_CMP_EQ_UNC_P1_P2_R2_R3,
EM_CMP_EQ_AND_P1_P2_R2_R3,
EM_CMP_EQ_OR_P1_P2_R2_R3,
EM_CMP_EQ_OR_ANDCM_P1_P2_R2_R3,
EM_CMP_NE_AND_P1_P2_R2_R3,
EM_CMP_NE_OR_P1_P2_R2_R3,
EM_CMP_NE_OR_ANDCM_P1_P2_R2_R3,
EM_CMP4_LT_P1_P2_R2_R3,
EM_CMP4_LTU_P1_P2_R2_R3,
EM_CMP4_EQ_P1_P2_R2_R3,
EM_CMP4_LT_UNC_P1_P2_R2_R3,
EM_CMP4_LTU_UNC_P1_P2_R2_R3,
EM_CMP4_EQ_UNC_P1_P2_R2_R3,
EM_CMP4_EQ_AND_P1_P2_R2_R3,
EM_CMP4_EQ_OR_P1_P2_R2_R3,
EM_CMP4_EQ_OR_ANDCM_P1_P2_R2_R3,
EM_CMP4_NE_AND_P1_P2_R2_R3,
EM_CMP4_NE_OR_P1_P2_R2_R3,
EM_CMP4_NE_OR_ANDCM_P1_P2_R2_R3,
EM_CMP_GT_AND_P1_P2_R0_R3,
EM_CMP_GT_OR_P1_P2_R0_R3,
EM_CMP_GT_OR_ANDCM_P1_P2_R0_R3,
EM_CMP_LE_AND_P1_P2_R0_R3,
EM_CMP_LE_OR_P1_P2_R0_R3,
EM_CMP_LE_OR_ANDCM_P1_P2_R0_R3,
EM_CMP_GE_AND_P1_P2_R0_R3,
EM_CMP_GE_OR_P1_P2_R0_R3,
EM_CMP_GE_OR_ANDCM_P1_P2_R0_R3,
EM_CMP_LT_AND_P1_P2_R0_R3,
EM_CMP_LT_OR_P1_P2_R0_R3,
EM_CMP_LT_OR_ANDCM_P1_P2_R0_R3,
EM_CMP4_GT_AND_P1_P2_R0_R3,
EM_CMP4_GT_OR_P1_P2_R0_R3,
EM_CMP4_GT_OR_ANDCM_P1_P2_R0_R3,
EM_CMP4_LE_AND_P1_P2_R0_R3,
EM_CMP4_LE_OR_P1_P2_R0_R3,
EM_CMP4_LE_OR_ANDCM_P1_P2_R0_R3,
EM_CMP4_GE_AND_P1_P2_R0_R3,
EM_CMP4_GE_OR_P1_P2_R0_R3,
EM_CMP4_GE_OR_ANDCM_P1_P2_R0_R3,
EM_CMP4_LT_AND_P1_P2_R0_R3,
EM_CMP4_LT_OR_P1_P2_R0_R3,
EM_CMP4_LT_OR_ANDCM_P1_P2_R0_R3,
EM_CMP_LT_P1_P2_IMM8_R3,
EM_CMP_LTU_P1_P2_IMM8_R3,
EM_CMP_EQ_P1_P2_IMM8_R3,
EM_CMP_LT_UNC_P1_P2_IMM8_R3,
EM_CMP_LTU_UNC_P1_P2_IMM8_R3,
EM_CMP_EQ_UNC_P1_P2_IMM8_R3,
EM_CMP_EQ_AND_P1_P2_IMM8_R3,
EM_CMP_EQ_OR_P1_P2_IMM8_R3,
EM_CMP_EQ_OR_ANDCM_P1_P2_IMM8_R3,
EM_CMP_NE_AND_P1_P2_IMM8_R3,
EM_CMP_NE_OR_P1_P2_IMM8_R3,
EM_CMP_NE_OR_ANDCM_P1_P2_IMM8_R3,
EM_CMP4_LT_P1_P2_IMM8_R3,
EM_CMP4_LTU_P1_P2_IMM8_R3,
EM_CMP4_EQ_P1_P2_IMM8_R3,
EM_CMP4_LT_UNC_P1_P2_IMM8_R3,
EM_CMP4_LTU_UNC_P1_P2_IMM8_R3,
EM_CMP4_EQ_UNC_P1_P2_IMM8_R3,
EM_CMP4_EQ_AND_P1_P2_IMM8_R3,
EM_CMP4_EQ_OR_P1_P2_IMM8_R3,
EM_CMP4_EQ_OR_ANDCM_P1_P2_IMM8_R3,
EM_CMP4_NE_AND_P1_P2_IMM8_R3,
EM_CMP4_NE_OR_P1_P2_IMM8_R3,
EM_CMP4_NE_OR_ANDCM_P1_P2_IMM8_R3,
EM_PADD1_R1_R2_R3,
EM_PADD2_R1_R2_R3,
EM_PADD4_R1_R2_R3,
EM_PADD1_SSS_R1_R2_R3,
EM_PADD2_SSS_R1_R2_R3,
EM_PADD1_UUU_R1_R2_R3,
EM_PADD2_UUU_R1_R2_R3,
EM_PADD1_UUS_R1_R2_R3,
EM_PADD2_UUS_R1_R2_R3,
EM_PSUB1_R1_R2_R3,
EM_PSUB2_R1_R2_R3,
EM_PSUB4_R1_R2_R3,
EM_PSUB1_SSS_R1_R2_R3,
EM_PSUB2_SSS_R1_R2_R3,
EM_PSUB1_UUU_R1_R2_R3,
EM_PSUB2_UUU_R1_R2_R3,
EM_PSUB1_UUS_R1_R2_R3,
EM_PSUB2_UUS_R1_R2_R3,
EM_PAVG1_R1_R2_R3,
EM_PAVG2_R1_R2_R3,
EM_PAVG1_RAZ_R1_R2_R3,
EM_PAVG2_RAZ_R1_R2_R3,
EM_PAVGSUB1_R1_R2_R3,
EM_PAVGSUB2_R1_R2_R3,
EM_PCMP1_EQ_R1_R2_R3,
EM_PCMP2_EQ_R1_R2_R3,
EM_PCMP4_EQ_R1_R2_R3,
EM_PCMP1_GT_R1_R2_R3,
EM_PCMP2_GT_R1_R2_R3,
EM_PCMP4_GT_R1_R2_R3,
EM_PSHLADD2_R1_R2_COUNT2_R3,
EM_PSHRADD2_R1_R2_COUNT2_R3,
EM_PMPYSHR2_R1_R2_R3_COUNT2,
EM_PMPYSHR2_U_R1_R2_R3_COUNT2,
EM_PMPY2_R_R1_R2_R3,
EM_PMPY2_L_R1_R2_R3,
EM_MIX1_R_R1_R2_R3,
EM_MIX2_R_R1_R2_R3,
EM_MIX4_R_R1_R2_R3,
EM_MIX1_L_R1_R2_R3,
EM_MIX2_L_R1_R2_R3,
EM_MIX4_L_R1_R2_R3,
EM_PACK2_USS_R1_R2_R3,
EM_PACK2_SSS_R1_R2_R3,
EM_PACK4_SSS_R1_R2_R3,
EM_UNPACK1_H_R1_R2_R3,
EM_UNPACK2_H_R1_R2_R3,
EM_UNPACK4_H_R1_R2_R3,
EM_UNPACK1_L_R1_R2_R3,
EM_UNPACK2_L_R1_R2_R3,
EM_UNPACK4_L_R1_R2_R3,
EM_PMIN1_U_R1_R2_R3,
EM_PMAX1_U_R1_R2_R3,
EM_PMIN2_R1_R2_R3,
EM_PMAX2_R1_R2_R3,
EM_PSAD1_R1_R2_R3,
EM_MUX1_R1_R2_MBTYPE4,
EM_MUX2_R1_R2_MHTYPE8,
EM_PSHR2_R1_R3_R2,
EM_PSHR4_R1_R3_R2,
EM_SHR_R1_R3_R2,
EM_PSHR2_U_R1_R3_R2,
EM_PSHR4_U_R1_R3_R2,
EM_SHR_U_R1_R3_R2,
EM_PSHR2_R1_R3_COUNT5,
EM_PSHR4_R1_R3_COUNT5,
EM_PSHR2_U_R1_R3_COUNT5,
EM_PSHR4_U_R1_R3_COUNT5,
EM_PSHL2_R1_R2_R3,
EM_PSHL4_R1_R2_R3,
EM_SHL_R1_R2_R3,
EM_PSHL2_R1_R2_COUNT5,
EM_PSHL4_R1_R2_COUNT5,
EM_POPCNT_R1_R3,
EM_SHRP_R1_R2_R3_COUNT6,
EM_EXTR_U_R1_R3_POS6_LEN6,
EM_EXTR_R1_R3_POS6_LEN6,
EM_DEP_Z_R1_R2_POS6_LEN6,
EM_DEP_Z_R1_IMM8_POS6_LEN6,
EM_DEP_R1_IMM1_R3_POS6_LEN6,
EM_DEP_R1_R2_R3_POS6_LEN4,
EM_TBIT_Z_P1_P2_R3_POS6,
EM_TBIT_Z_UNC_P1_P2_R3_POS6,
EM_TBIT_Z_AND_P1_P2_R3_POS6,
EM_TBIT_NZ_AND_P1_P2_R3_POS6,
EM_TBIT_Z_OR_P1_P2_R3_POS6,
EM_TBIT_NZ_OR_P1_P2_R3_POS6,
EM_TBIT_Z_OR_ANDCM_P1_P2_R3_POS6,
EM_TBIT_NZ_OR_ANDCM_P1_P2_R3_POS6,
EM_TNAT_Z_P1_P2_R3,
EM_TNAT_Z_UNC_P1_P2_R3,
EM_TNAT_Z_AND_P1_P2_R3,
EM_TNAT_NZ_AND_P1_P2_R3,
EM_TNAT_Z_OR_P1_P2_R3,
EM_TNAT_NZ_OR_P1_P2_R3,
EM_TNAT_Z_OR_ANDCM_P1_P2_R3,
EM_TNAT_NZ_OR_ANDCM_P1_P2_R3,
EM_MOVL_R1_IMM64,
EM_BREAK_I_IMM21,
EM_NOP_I_IMM21,
EM_CHK_S_I_R2_TARGET25,
EM_MOV_SPTK_FEW_DC_DC_B1_R2_TAG13,
EM_MOV_SPTK_FEW_DC_DC_IMP_B1_R2_TAG13,
EM_MOV_SPTK_MANY_DC_DC_B1_R2_TAG13,
EM_MOV_SPTK_MANY_DC_DC_IMP_B1_R2_TAG13,
EM_MOV_FEW_DC_DC_B1_R2_TAG13,
EM_MOV_FEW_DC_DC_IMP_B1_R2_TAG13,
EM_MOV_MANY_DC_DC_B1_R2_TAG13,
EM_MOV_MANY_DC_DC_IMP_B1_R2_TAG13,
EM_MOV_DPTK_FEW_DC_DC_B1_R2_TAG13,
EM_MOV_DPTK_FEW_DC_DC_IMP_B1_R2_TAG13,
EM_MOV_DPTK_MANY_DC_DC_B1_R2_TAG13,
EM_MOV_DPTK_MANY_DC_DC_IMP_B1_R2_TAG13,
EM_MOV_SPTK_FEW_DC_NT_B1_R2_TAG13,
EM_MOV_SPTK_FEW_DC_NT_IMP_B1_R2_TAG13,
EM_MOV_SPTK_MANY_DC_NT_B1_R2_TAG13,
EM_MOV_SPTK_MANY_DC_NT_IMP_B1_R2_TAG13,
EM_MOV_FEW_DC_NT_B1_R2_TAG13,
EM_MOV_FEW_DC_NT_IMP_B1_R2_TAG13,
EM_MOV_MANY_DC_NT_B1_R2_TAG13,
EM_MOV_MANY_DC_NT_IMP_B1_R2_TAG13,
EM_MOV_DPTK_FEW_DC_NT_B1_R2_TAG13,
EM_MOV_DPTK_FEW_DC_NT_IMP_B1_R2_TAG13,
EM_MOV_DPTK_MANY_DC_NT_B1_R2_TAG13,
EM_MOV_DPTK_MANY_DC_NT_IMP_B1_R2_TAG13,
EM_MOV_SPTK_FEW_TK_DC_B1_R2_TAG13,
EM_MOV_SPTK_FEW_TK_DC_IMP_B1_R2_TAG13,
EM_MOV_SPTK_MANY_TK_DC_B1_R2_TAG13,
EM_MOV_SPTK_MANY_TK_DC_IMP_B1_R2_TAG13,
EM_MOV_FEW_TK_DC_B1_R2_TAG13,
EM_MOV_FEW_TK_DC_IMP_B1_R2_TAG13,
EM_MOV_MANY_TK_DC_B1_R2_TAG13,
EM_MOV_MANY_TK_DC_IMP_B1_R2_TAG13,
EM_MOV_DPTK_FEW_TK_DC_B1_R2_TAG13,
EM_MOV_DPTK_FEW_TK_DC_IMP_B1_R2_TAG13,
EM_MOV_DPTK_MANY_TK_DC_B1_R2_TAG13,
EM_MOV_DPTK_MANY_TK_DC_IMP_B1_R2_TAG13,
EM_MOV_SPTK_FEW_TK_TK_B1_R2_TAG13,
EM_MOV_SPTK_FEW_TK_TK_IMP_B1_R2_TAG13,
EM_MOV_SPTK_MANY_TK_TK_B1_R2_TAG13,
EM_MOV_SPTK_MANY_TK_TK_IMP_B1_R2_TAG13,
EM_MOV_FEW_TK_TK_B1_R2_TAG13,
EM_MOV_FEW_TK_TK_IMP_B1_R2_TAG13,
EM_MOV_MANY_TK_TK_B1_R2_TAG13,
EM_MOV_MANY_TK_TK_IMP_B1_R2_TAG13,
EM_MOV_DPTK_FEW_TK_TK_B1_R2_TAG13,
EM_MOV_DPTK_FEW_TK_TK_IMP_B1_R2_TAG13,
EM_MOV_DPTK_MANY_TK_TK_B1_R2_TAG13,
EM_MOV_DPTK_MANY_TK_TK_IMP_B1_R2_TAG13,
EM_MOV_SPTK_FEW_TK_NT_B1_R2_TAG13,
EM_MOV_SPTK_FEW_TK_NT_IMP_B1_R2_TAG13,
EM_MOV_SPTK_MANY_TK_NT_B1_R2_TAG13,
EM_MOV_SPTK_MANY_TK_NT_IMP_B1_R2_TAG13,
EM_MOV_FEW_TK_NT_B1_R2_TAG13,
EM_MOV_FEW_TK_NT_IMP_B1_R2_TAG13,
EM_MOV_MANY_TK_NT_B1_R2_TAG13,
EM_MOV_MANY_TK_NT_IMP_B1_R2_TAG13,
EM_MOV_DPTK_FEW_TK_NT_B1_R2_TAG13,
EM_MOV_DPTK_FEW_TK_NT_IMP_B1_R2_TAG13,
EM_MOV_DPTK_MANY_TK_NT_B1_R2_TAG13,
EM_MOV_DPTK_MANY_TK_NT_IMP_B1_R2_TAG13,
EM_MOV_SPTK_FEW_NT_DC_B1_R2_TAG13,
EM_MOV_SPTK_FEW_NT_DC_IMP_B1_R2_TAG13,
EM_MOV_SPTK_MANY_NT_DC_B1_R2_TAG13,
EM_MOV_SPTK_MANY_NT_DC_IMP_B1_R2_TAG13,
EM_MOV_FEW_NT_DC_B1_R2_TAG13,
EM_MOV_FEW_NT_DC_IMP_B1_R2_TAG13,
EM_MOV_MANY_NT_DC_B1_R2_TAG13,
EM_MOV_MANY_NT_DC_IMP_B1_R2_TAG13,
EM_MOV_DPTK_FEW_NT_DC_B1_R2_TAG13,
EM_MOV_DPTK_FEW_NT_DC_IMP_B1_R2_TAG13,
EM_MOV_DPTK_MANY_NT_DC_B1_R2_TAG13,
EM_MOV_DPTK_MANY_NT_DC_IMP_B1_R2_TAG13,
EM_MOV_SPTK_FEW_NT_TK_B1_R2_TAG13,
EM_MOV_SPTK_FEW_NT_TK_IMP_B1_R2_TAG13,
EM_MOV_SPTK_MANY_NT_TK_B1_R2_TAG13,
EM_MOV_SPTK_MANY_NT_TK_IMP_B1_R2_TAG13,
EM_MOV_FEW_NT_TK_B1_R2_TAG13,
EM_MOV_FEW_NT_TK_IMP_B1_R2_TAG13,
EM_MOV_MANY_NT_TK_B1_R2_TAG13,
EM_MOV_MANY_NT_TK_IMP_B1_R2_TAG13,
EM_MOV_DPTK_FEW_NT_TK_B1_R2_TAG13,
EM_MOV_DPTK_FEW_NT_TK_IMP_B1_R2_TAG13,
EM_MOV_DPTK_MANY_NT_TK_B1_R2_TAG13,
EM_MOV_DPTK_MANY_NT_TK_IMP_B1_R2_TAG13,
EM_MOV_SPTK_FEW_NT_NT_B1_R2_TAG13,
EM_MOV_SPTK_FEW_NT_NT_IMP_B1_R2_TAG13,
EM_MOV_SPTK_MANY_NT_NT_B1_R2_TAG13,
EM_MOV_SPTK_MANY_NT_NT_IMP_B1_R2_TAG13,
EM_MOV_FEW_NT_NT_B1_R2_TAG13,
EM_MOV_FEW_NT_NT_IMP_B1_R2_TAG13,
EM_MOV_MANY_NT_NT_B1_R2_TAG13,
EM_MOV_MANY_NT_NT_IMP_B1_R2_TAG13,
EM_MOV_DPTK_FEW_NT_NT_B1_R2_TAG13,
EM_MOV_DPTK_FEW_NT_NT_IMP_B1_R2_TAG13,
EM_MOV_DPTK_MANY_NT_NT_B1_R2_TAG13,
EM_MOV_DPTK_MANY_NT_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_SPTK_FEW_DC_DC_B1_R2_TAG13,
EM_MOV_RET_SPTK_FEW_DC_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_SPTK_MANY_DC_DC_B1_R2_TAG13,
EM_MOV_RET_SPTK_MANY_DC_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_FEW_DC_DC_B1_R2_TAG13,
EM_MOV_RET_FEW_DC_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_MANY_DC_DC_B1_R2_TAG13,
EM_MOV_RET_MANY_DC_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_DPTK_FEW_DC_DC_B1_R2_TAG13,
EM_MOV_RET_DPTK_FEW_DC_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_DPTK_MANY_DC_DC_B1_R2_TAG13,
EM_MOV_RET_DPTK_MANY_DC_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_SPTK_FEW_DC_NT_B1_R2_TAG13,
EM_MOV_RET_SPTK_FEW_DC_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_SPTK_MANY_DC_NT_B1_R2_TAG13,
EM_MOV_RET_SPTK_MANY_DC_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_FEW_DC_NT_B1_R2_TAG13,
EM_MOV_RET_FEW_DC_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_MANY_DC_NT_B1_R2_TAG13,
EM_MOV_RET_MANY_DC_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_DPTK_FEW_DC_NT_B1_R2_TAG13,
EM_MOV_RET_DPTK_FEW_DC_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_DPTK_MANY_DC_NT_B1_R2_TAG13,
EM_MOV_RET_DPTK_MANY_DC_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_SPTK_FEW_TK_DC_B1_R2_TAG13,
EM_MOV_RET_SPTK_FEW_TK_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_SPTK_MANY_TK_DC_B1_R2_TAG13,
EM_MOV_RET_SPTK_MANY_TK_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_FEW_TK_DC_B1_R2_TAG13,
EM_MOV_RET_FEW_TK_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_MANY_TK_DC_B1_R2_TAG13,
EM_MOV_RET_MANY_TK_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_DPTK_FEW_TK_DC_B1_R2_TAG13,
EM_MOV_RET_DPTK_FEW_TK_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_DPTK_MANY_TK_DC_B1_R2_TAG13,
EM_MOV_RET_DPTK_MANY_TK_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_SPTK_FEW_TK_TK_B1_R2_TAG13,
EM_MOV_RET_SPTK_FEW_TK_TK_IMP_B1_R2_TAG13,
EM_MOV_RET_SPTK_MANY_TK_TK_B1_R2_TAG13,
EM_MOV_RET_SPTK_MANY_TK_TK_IMP_B1_R2_TAG13,
EM_MOV_RET_FEW_TK_TK_B1_R2_TAG13,
EM_MOV_RET_FEW_TK_TK_IMP_B1_R2_TAG13,
EM_MOV_RET_MANY_TK_TK_B1_R2_TAG13,
EM_MOV_RET_MANY_TK_TK_IMP_B1_R2_TAG13,
EM_MOV_RET_DPTK_FEW_TK_TK_B1_R2_TAG13,
EM_MOV_RET_DPTK_FEW_TK_TK_IMP_B1_R2_TAG13,
EM_MOV_RET_DPTK_MANY_TK_TK_B1_R2_TAG13,
EM_MOV_RET_DPTK_MANY_TK_TK_IMP_B1_R2_TAG13,
EM_MOV_RET_SPTK_FEW_TK_NT_B1_R2_TAG13,
EM_MOV_RET_SPTK_FEW_TK_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_SPTK_MANY_TK_NT_B1_R2_TAG13,
EM_MOV_RET_SPTK_MANY_TK_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_FEW_TK_NT_B1_R2_TAG13,
EM_MOV_RET_FEW_TK_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_MANY_TK_NT_B1_R2_TAG13,
EM_MOV_RET_MANY_TK_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_DPTK_FEW_TK_NT_B1_R2_TAG13,
EM_MOV_RET_DPTK_FEW_TK_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_DPTK_MANY_TK_NT_B1_R2_TAG13,
EM_MOV_RET_DPTK_MANY_TK_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_SPTK_FEW_NT_DC_B1_R2_TAG13,
EM_MOV_RET_SPTK_FEW_NT_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_SPTK_MANY_NT_DC_B1_R2_TAG13,
EM_MOV_RET_SPTK_MANY_NT_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_FEW_NT_DC_B1_R2_TAG13,
EM_MOV_RET_FEW_NT_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_MANY_NT_DC_B1_R2_TAG13,
EM_MOV_RET_MANY_NT_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_DPTK_FEW_NT_DC_B1_R2_TAG13,
EM_MOV_RET_DPTK_FEW_NT_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_DPTK_MANY_NT_DC_B1_R2_TAG13,
EM_MOV_RET_DPTK_MANY_NT_DC_IMP_B1_R2_TAG13,
EM_MOV_RET_SPTK_FEW_NT_TK_B1_R2_TAG13,
EM_MOV_RET_SPTK_FEW_NT_TK_IMP_B1_R2_TAG13,
EM_MOV_RET_SPTK_MANY_NT_TK_B1_R2_TAG13,
EM_MOV_RET_SPTK_MANY_NT_TK_IMP_B1_R2_TAG13,
EM_MOV_RET_FEW_NT_TK_B1_R2_TAG13,
EM_MOV_RET_FEW_NT_TK_IMP_B1_R2_TAG13,
EM_MOV_RET_MANY_NT_TK_B1_R2_TAG13,
EM_MOV_RET_MANY_NT_TK_IMP_B1_R2_TAG13,
EM_MOV_RET_DPTK_FEW_NT_TK_B1_R2_TAG13,
EM_MOV_RET_DPTK_FEW_NT_TK_IMP_B1_R2_TAG13,
EM_MOV_RET_DPTK_MANY_NT_TK_B1_R2_TAG13,
EM_MOV_RET_DPTK_MANY_NT_TK_IMP_B1_R2_TAG13,
EM_MOV_RET_SPTK_FEW_NT_NT_B1_R2_TAG13,
EM_MOV_RET_SPTK_FEW_NT_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_SPTK_MANY_NT_NT_B1_R2_TAG13,
EM_MOV_RET_SPTK_MANY_NT_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_FEW_NT_NT_B1_R2_TAG13,
EM_MOV_RET_FEW_NT_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_MANY_NT_NT_B1_R2_TAG13,
EM_MOV_RET_MANY_NT_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_DPTK_FEW_NT_NT_B1_R2_TAG13,
EM_MOV_RET_DPTK_FEW_NT_NT_IMP_B1_R2_TAG13,
EM_MOV_RET_DPTK_MANY_NT_NT_B1_R2_TAG13,
EM_MOV_RET_DPTK_MANY_NT_NT_IMP_B1_R2_TAG13,
EM_MOV_R1_B2,
EM_MOV_PR_R2_MASK17,
EM_MOV_PR_ROT_IMM44,
EM_MOV_R1_IP,
EM_MOV_R1_PR,
EM_MOV_I_AR3_R2,
EM_MOV_I_AR3_IMM8,
EM_MOV_I_R1_AR3,
EM_ZXT1_R1_R3,
EM_ZXT2_R1_R3,
EM_ZXT4_R1_R3,
EM_SXT1_R1_R3,
EM_SXT2_R1_R3,
EM_SXT4_R1_R3,
EM_CZX1_L_R1_R3,
EM_CZX2_L_R1_R3,
EM_CZX1_R_R1_R3,
EM_CZX2_R_R1_R3,
EM_LD1_R1_R3,
EM_LD1_NT1_R1_R3,
EM_LD1_NTA_R1_R3,
EM_LD2_R1_R3,
EM_LD2_NT1_R1_R3,
EM_LD2_NTA_R1_R3,
EM_LD4_R1_R3,
EM_LD4_NT1_R1_R3,
EM_LD4_NTA_R1_R3,
EM_LD8_R1_R3,
EM_LD8_NT1_R1_R3,
EM_LD8_NTA_R1_R3,
EM_LD1_S_R1_R3,
EM_LD1_S_NT1_R1_R3,
EM_LD1_S_NTA_R1_R3,
EM_LD2_S_R1_R3,
EM_LD2_S_NT1_R1_R3,
EM_LD2_S_NTA_R1_R3,
EM_LD4_S_R1_R3,
EM_LD4_S_NT1_R1_R3,
EM_LD4_S_NTA_R1_R3,
EM_LD8_S_R1_R3,
EM_LD8_S_NT1_R1_R3,
EM_LD8_S_NTA_R1_R3,
EM_LD1_A_R1_R3,
EM_LD1_A_NT1_R1_R3,
EM_LD1_A_NTA_R1_R3,
EM_LD2_A_R1_R3,
EM_LD2_A_NT1_R1_R3,
EM_LD2_A_NTA_R1_R3,
EM_LD4_A_R1_R3,
EM_LD4_A_NT1_R1_R3,
EM_LD4_A_NTA_R1_R3,
EM_LD8_A_R1_R3,
EM_LD8_A_NT1_R1_R3,
EM_LD8_A_NTA_R1_R3,
EM_LD1_SA_R1_R3,
EM_LD1_SA_NT1_R1_R3,
EM_LD1_SA_NTA_R1_R3,
EM_LD2_SA_R1_R3,
EM_LD2_SA_NT1_R1_R3,
EM_LD2_SA_NTA_R1_R3,
EM_LD4_SA_R1_R3,
EM_LD4_SA_NT1_R1_R3,
EM_LD4_SA_NTA_R1_R3,
EM_LD8_SA_R1_R3,
EM_LD8_SA_NT1_R1_R3,
EM_LD8_SA_NTA_R1_R3,
EM_LD1_BIAS_R1_R3,
EM_LD1_BIAS_NT1_R1_R3,
EM_LD1_BIAS_NTA_R1_R3,
EM_LD2_BIAS_R1_R3,
EM_LD2_BIAS_NT1_R1_R3,
EM_LD2_BIAS_NTA_R1_R3,
EM_LD4_BIAS_R1_R3,
EM_LD4_BIAS_NT1_R1_R3,
EM_LD4_BIAS_NTA_R1_R3,
EM_LD8_BIAS_R1_R3,
EM_LD8_BIAS_NT1_R1_R3,
EM_LD8_BIAS_NTA_R1_R3,
EM_LD1_ACQ_R1_R3,
EM_LD1_ACQ_NT1_R1_R3,
EM_LD1_ACQ_NTA_R1_R3,
EM_LD2_ACQ_R1_R3,
EM_LD2_ACQ_NT1_R1_R3,
EM_LD2_ACQ_NTA_R1_R3,
EM_LD4_ACQ_R1_R3,
EM_LD4_ACQ_NT1_R1_R3,
EM_LD4_ACQ_NTA_R1_R3,
EM_LD8_ACQ_R1_R3,
EM_LD8_ACQ_NT1_R1_R3,
EM_LD8_ACQ_NTA_R1_R3,
EM_LD8_FILL_R1_R3,
EM_LD8_FILL_NT1_R1_R3,
EM_LD8_FILL_NTA_R1_R3,
EM_LD1_C_CLR_R1_R3,
EM_LD1_C_CLR_NT1_R1_R3,
EM_LD1_C_CLR_NTA_R1_R3,
EM_LD2_C_CLR_R1_R3,
EM_LD2_C_CLR_NT1_R1_R3,
EM_LD2_C_CLR_NTA_R1_R3,
EM_LD4_C_CLR_R1_R3,
EM_LD4_C_CLR_NT1_R1_R3,
EM_LD4_C_CLR_NTA_R1_R3,
EM_LD8_C_CLR_R1_R3,
EM_LD8_C_CLR_NT1_R1_R3,
EM_LD8_C_CLR_NTA_R1_R3,
EM_LD1_C_NC_R1_R3,
EM_LD1_C_NC_NT1_R1_R3,
EM_LD1_C_NC_NTA_R1_R3,
EM_LD2_C_NC_R1_R3,
EM_LD2_C_NC_NT1_R1_R3,
EM_LD2_C_NC_NTA_R1_R3,
EM_LD4_C_NC_R1_R3,
EM_LD4_C_NC_NT1_R1_R3,
EM_LD4_C_NC_NTA_R1_R3,
EM_LD8_C_NC_R1_R3,
EM_LD8_C_NC_NT1_R1_R3,
EM_LD8_C_NC_NTA_R1_R3,
EM_LD1_C_CLR_ACQ_R1_R3,
EM_LD1_C_CLR_ACQ_NT1_R1_R3,
EM_LD1_C_CLR_ACQ_NTA_R1_R3,
EM_LD2_C_CLR_ACQ_R1_R3,
EM_LD2_C_CLR_ACQ_NT1_R1_R3,
EM_LD2_C_CLR_ACQ_NTA_R1_R3,
EM_LD4_C_CLR_ACQ_R1_R3,
EM_LD4_C_CLR_ACQ_NT1_R1_R3,
EM_LD4_C_CLR_ACQ_NTA_R1_R3,
EM_LD8_C_CLR_ACQ_R1_R3,
EM_LD8_C_CLR_ACQ_NT1_R1_R3,
EM_LD8_C_CLR_ACQ_NTA_R1_R3,
EM_LD1_R1_R3_R2,
EM_LD1_NT1_R1_R3_R2,
EM_LD1_NTA_R1_R3_R2,
EM_LD2_R1_R3_R2,
EM_LD2_NT1_R1_R3_R2,
EM_LD2_NTA_R1_R3_R2,
EM_LD4_R1_R3_R2,
EM_LD4_NT1_R1_R3_R2,
EM_LD4_NTA_R1_R3_R2,
EM_LD8_R1_R3_R2,
EM_LD8_NT1_R1_R3_R2,
EM_LD8_NTA_R1_R3_R2,
EM_LD1_S_R1_R3_R2,
EM_LD1_S_NT1_R1_R3_R2,
EM_LD1_S_NTA_R1_R3_R2,
EM_LD2_S_R1_R3_R2,
EM_LD2_S_NT1_R1_R3_R2,
EM_LD2_S_NTA_R1_R3_R2,
EM_LD4_S_R1_R3_R2,
EM_LD4_S_NT1_R1_R3_R2,
EM_LD4_S_NTA_R1_R3_R2,
EM_LD8_S_R1_R3_R2,
EM_LD8_S_NT1_R1_R3_R2,
EM_LD8_S_NTA_R1_R3_R2,
EM_LD1_A_R1_R3_R2,
EM_LD1_A_NT1_R1_R3_R2,
EM_LD1_A_NTA_R1_R3_R2,
EM_LD2_A_R1_R3_R2,
EM_LD2_A_NT1_R1_R3_R2,
EM_LD2_A_NTA_R1_R3_R2,
EM_LD4_A_R1_R3_R2,
EM_LD4_A_NT1_R1_R3_R2,
EM_LD4_A_NTA_R1_R3_R2,
EM_LD8_A_R1_R3_R2,
EM_LD8_A_NT1_R1_R3_R2,
EM_LD8_A_NTA_R1_R3_R2,
EM_LD1_SA_R1_R3_R2,
EM_LD1_SA_NT1_R1_R3_R2,
EM_LD1_SA_NTA_R1_R3_R2,
EM_LD2_SA_R1_R3_R2,
EM_LD2_SA_NT1_R1_R3_R2,
EM_LD2_SA_NTA_R1_R3_R2,
EM_LD4_SA_R1_R3_R2,
EM_LD4_SA_NT1_R1_R3_R2,
EM_LD4_SA_NTA_R1_R3_R2,
EM_LD8_SA_R1_R3_R2,
EM_LD8_SA_NT1_R1_R3_R2,
EM_LD8_SA_NTA_R1_R3_R2,
EM_LD1_BIAS_R1_R3_R2,
EM_LD1_BIAS_NT1_R1_R3_R2,
EM_LD1_BIAS_NTA_R1_R3_R2,
EM_LD2_BIAS_R1_R3_R2,
EM_LD2_BIAS_NT1_R1_R3_R2,
EM_LD2_BIAS_NTA_R1_R3_R2,
EM_LD4_BIAS_R1_R3_R2,
EM_LD4_BIAS_NT1_R1_R3_R2,
EM_LD4_BIAS_NTA_R1_R3_R2,
EM_LD8_BIAS_R1_R3_R2,
EM_LD8_BIAS_NT1_R1_R3_R2,
EM_LD8_BIAS_NTA_R1_R3_R2,
EM_LD1_ACQ_R1_R3_R2,
EM_LD1_ACQ_NT1_R1_R3_R2,
EM_LD1_ACQ_NTA_R1_R3_R2,
EM_LD2_ACQ_R1_R3_R2,
EM_LD2_ACQ_NT1_R1_R3_R2,
EM_LD2_ACQ_NTA_R1_R3_R2,
EM_LD4_ACQ_R1_R3_R2,
EM_LD4_ACQ_NT1_R1_R3_R2,
EM_LD4_ACQ_NTA_R1_R3_R2,
EM_LD8_ACQ_R1_R3_R2,
EM_LD8_ACQ_NT1_R1_R3_R2,
EM_LD8_ACQ_NTA_R1_R3_R2,
EM_LD8_FILL_R1_R3_R2,
EM_LD8_FILL_NT1_R1_R3_R2,
EM_LD8_FILL_NTA_R1_R3_R2,
EM_LD1_C_CLR_R1_R3_R2,
EM_LD1_C_CLR_NT1_R1_R3_R2,
EM_LD1_C_CLR_NTA_R1_R3_R2,
EM_LD2_C_CLR_R1_R3_R2,
EM_LD2_C_CLR_NT1_R1_R3_R2,
EM_LD2_C_CLR_NTA_R1_R3_R2,
EM_LD4_C_CLR_R1_R3_R2,
EM_LD4_C_CLR_NT1_R1_R3_R2,
EM_LD4_C_CLR_NTA_R1_R3_R2,
EM_LD8_C_CLR_R1_R3_R2,
EM_LD8_C_CLR_NT1_R1_R3_R2,
EM_LD8_C_CLR_NTA_R1_R3_R2,
EM_LD1_C_NC_R1_R3_R2,
EM_LD1_C_NC_NT1_R1_R3_R2,
EM_LD1_C_NC_NTA_R1_R3_R2,
EM_LD2_C_NC_R1_R3_R2,
EM_LD2_C_NC_NT1_R1_R3_R2,
EM_LD2_C_NC_NTA_R1_R3_R2,
EM_LD4_C_NC_R1_R3_R2,
EM_LD4_C_NC_NT1_R1_R3_R2,
EM_LD4_C_NC_NTA_R1_R3_R2,
EM_LD8_C_NC_R1_R3_R2,
EM_LD8_C_NC_NT1_R1_R3_R2,
EM_LD8_C_NC_NTA_R1_R3_R2,
EM_LD1_C_CLR_ACQ_R1_R3_R2,
EM_LD1_C_CLR_ACQ_NT1_R1_R3_R2,
EM_LD1_C_CLR_ACQ_NTA_R1_R3_R2,
EM_LD2_C_CLR_ACQ_R1_R3_R2,
EM_LD2_C_CLR_ACQ_NT1_R1_R3_R2,
EM_LD2_C_CLR_ACQ_NTA_R1_R3_R2,
EM_LD4_C_CLR_ACQ_R1_R3_R2,
EM_LD4_C_CLR_ACQ_NT1_R1_R3_R2,
EM_LD4_C_CLR_ACQ_NTA_R1_R3_R2,
EM_LD8_C_CLR_ACQ_R1_R3_R2,
EM_LD8_C_CLR_ACQ_NT1_R1_R3_R2,
EM_LD8_C_CLR_ACQ_NTA_R1_R3_R2,
EM_LD1_R1_R3_IMM9,
EM_LD1_NT1_R1_R3_IMM9,
EM_LD1_NTA_R1_R3_IMM9,
EM_LD2_R1_R3_IMM9,
EM_LD2_NT1_R1_R3_IMM9,
EM_LD2_NTA_R1_R3_IMM9,
EM_LD4_R1_R3_IMM9,
EM_LD4_NT1_R1_R3_IMM9,
EM_LD4_NTA_R1_R3_IMM9,
EM_LD8_R1_R3_IMM9,
EM_LD8_NT1_R1_R3_IMM9,
EM_LD8_NTA_R1_R3_IMM9,
EM_LD1_S_R1_R3_IMM9,
EM_LD1_S_NT1_R1_R3_IMM9,
EM_LD1_S_NTA_R1_R3_IMM9,
EM_LD2_S_R1_R3_IMM9,
EM_LD2_S_NT1_R1_R3_IMM9,
EM_LD2_S_NTA_R1_R3_IMM9,
EM_LD4_S_R1_R3_IMM9,
EM_LD4_S_NT1_R1_R3_IMM9,
EM_LD4_S_NTA_R1_R3_IMM9,
EM_LD8_S_R1_R3_IMM9,
EM_LD8_S_NT1_R1_R3_IMM9,
EM_LD8_S_NTA_R1_R3_IMM9,
EM_LD1_A_R1_R3_IMM9,
EM_LD1_A_NT1_R1_R3_IMM9,
EM_LD1_A_NTA_R1_R3_IMM9,
EM_LD2_A_R1_R3_IMM9,
EM_LD2_A_NT1_R1_R3_IMM9,
EM_LD2_A_NTA_R1_R3_IMM9,
EM_LD4_A_R1_R3_IMM9,
EM_LD4_A_NT1_R1_R3_IMM9,
EM_LD4_A_NTA_R1_R3_IMM9,
EM_LD8_A_R1_R3_IMM9,
EM_LD8_A_NT1_R1_R3_IMM9,
EM_LD8_A_NTA_R1_R3_IMM9,
EM_LD1_SA_R1_R3_IMM9,
EM_LD1_SA_NT1_R1_R3_IMM9,
EM_LD1_SA_NTA_R1_R3_IMM9,
EM_LD2_SA_R1_R3_IMM9,
EM_LD2_SA_NT1_R1_R3_IMM9,
EM_LD2_SA_NTA_R1_R3_IMM9,
EM_LD4_SA_R1_R3_IMM9,
EM_LD4_SA_NT1_R1_R3_IMM9,
EM_LD4_SA_NTA_R1_R3_IMM9,
EM_LD8_SA_R1_R3_IMM9,
EM_LD8_SA_NT1_R1_R3_IMM9,
EM_LD8_SA_NTA_R1_R3_IMM9,
EM_LD1_BIAS_R1_R3_IMM9,
EM_LD1_BIAS_NT1_R1_R3_IMM9,
EM_LD1_BIAS_NTA_R1_R3_IMM9,
EM_LD2_BIAS_R1_R3_IMM9,
EM_LD2_BIAS_NT1_R1_R3_IMM9,
EM_LD2_BIAS_NTA_R1_R3_IMM9,
EM_LD4_BIAS_R1_R3_IMM9,
EM_LD4_BIAS_NT1_R1_R3_IMM9,
EM_LD4_BIAS_NTA_R1_R3_IMM9,
EM_LD8_BIAS_R1_R3_IMM9,
EM_LD8_BIAS_NT1_R1_R3_IMM9,
EM_LD8_BIAS_NTA_R1_R3_IMM9,
EM_LD1_ACQ_R1_R3_IMM9,
EM_LD1_ACQ_NT1_R1_R3_IMM9,
EM_LD1_ACQ_NTA_R1_R3_IMM9,
EM_LD2_ACQ_R1_R3_IMM9,
EM_LD2_ACQ_NT1_R1_R3_IMM9,
EM_LD2_ACQ_NTA_R1_R3_IMM9,
EM_LD4_ACQ_R1_R3_IMM9,
EM_LD4_ACQ_NT1_R1_R3_IMM9,
EM_LD4_ACQ_NTA_R1_R3_IMM9,
EM_LD8_ACQ_R1_R3_IMM9,
EM_LD8_ACQ_NT1_R1_R3_IMM9,
EM_LD8_ACQ_NTA_R1_R3_IMM9,
EM_LD8_FILL_R1_R3_IMM9,
EM_LD8_FILL_NT1_R1_R3_IMM9,
EM_LD8_FILL_NTA_R1_R3_IMM9,
EM_LD1_C_CLR_R1_R3_IMM9,
EM_LD1_C_CLR_NT1_R1_R3_IMM9,
EM_LD1_C_CLR_NTA_R1_R3_IMM9,
EM_LD2_C_CLR_R1_R3_IMM9,
EM_LD2_C_CLR_NT1_R1_R3_IMM9,
EM_LD2_C_CLR_NTA_R1_R3_IMM9,
EM_LD4_C_CLR_R1_R3_IMM9,
EM_LD4_C_CLR_NT1_R1_R3_IMM9,
EM_LD4_C_CLR_NTA_R1_R3_IMM9,
EM_LD8_C_CLR_R1_R3_IMM9,
EM_LD8_C_CLR_NT1_R1_R3_IMM9,
EM_LD8_C_CLR_NTA_R1_R3_IMM9,
EM_LD1_C_NC_R1_R3_IMM9,
EM_LD1_C_NC_NT1_R1_R3_IMM9,
EM_LD1_C_NC_NTA_R1_R3_IMM9,
EM_LD2_C_NC_R1_R3_IMM9,
EM_LD2_C_NC_NT1_R1_R3_IMM9,
EM_LD2_C_NC_NTA_R1_R3_IMM9,
EM_LD4_C_NC_R1_R3_IMM9,
EM_LD4_C_NC_NT1_R1_R3_IMM9,
EM_LD4_C_NC_NTA_R1_R3_IMM9,
EM_LD8_C_NC_R1_R3_IMM9,
EM_LD8_C_NC_NT1_R1_R3_IMM9,
EM_LD8_C_NC_NTA_R1_R3_IMM9,
EM_LD1_C_CLR_ACQ_R1_R3_IMM9,
EM_LD1_C_CLR_ACQ_NT1_R1_R3_IMM9,
EM_LD1_C_CLR_ACQ_NTA_R1_R3_IMM9,
EM_LD2_C_CLR_ACQ_R1_R3_IMM9,
EM_LD2_C_CLR_ACQ_NT1_R1_R3_IMM9,
EM_LD2_C_CLR_ACQ_NTA_R1_R3_IMM9,
EM_LD4_C_CLR_ACQ_R1_R3_IMM9,
EM_LD4_C_CLR_ACQ_NT1_R1_R3_IMM9,
EM_LD4_C_CLR_ACQ_NTA_R1_R3_IMM9,
EM_LD8_C_CLR_ACQ_R1_R3_IMM9,
EM_LD8_C_CLR_ACQ_NT1_R1_R3_IMM9,
EM_LD8_C_CLR_ACQ_NTA_R1_R3_IMM9,
EM_ST1_R3_R2,
EM_ST1_NTA_R3_R2,
EM_ST2_R3_R2,
EM_ST2_NTA_R3_R2,
EM_ST4_R3_R2,
EM_ST4_NTA_R3_R2,
EM_ST8_R3_R2,
EM_ST8_NTA_R3_R2,
EM_ST1_REL_R3_R2,
EM_ST1_REL_NTA_R3_R2,
EM_ST2_REL_R3_R2,
EM_ST2_REL_NTA_R3_R2,
EM_ST4_REL_R3_R2,
EM_ST4_REL_NTA_R3_R2,
EM_ST8_REL_R3_R2,
EM_ST8_REL_NTA_R3_R2,
EM_ST8_SPILL_R3_R2,
EM_ST8_SPILL_NTA_R3_R2,
EM_ST1_R3_R2_IMM9,
EM_ST1_NTA_R3_R2_IMM9,
EM_ST2_R3_R2_IMM9,
EM_ST2_NTA_R3_R2_IMM9,
EM_ST4_R3_R2_IMM9,
EM_ST4_NTA_R3_R2_IMM9,
EM_ST8_R3_R2_IMM9,
EM_ST8_NTA_R3_R2_IMM9,
EM_ST1_REL_R3_R2_IMM9,
EM_ST1_REL_NTA_R3_R2_IMM9,
EM_ST2_REL_R3_R2_IMM9,
EM_ST2_REL_NTA_R3_R2_IMM9,
EM_ST4_REL_R3_R2_IMM9,
EM_ST4_REL_NTA_R3_R2_IMM9,
EM_ST8_REL_R3_R2_IMM9,
EM_ST8_REL_NTA_R3_R2_IMM9,
EM_ST8_SPILL_R3_R2_IMM9,
EM_ST8_SPILL_NTA_R3_R2_IMM9,
EM_LDFS_F1_R3,
EM_LDFS_NT1_F1_R3,
EM_LDFS_NTA_F1_R3,
EM_LDFD_F1_R3,
EM_LDFD_NT1_F1_R3,
EM_LDFD_NTA_F1_R3,
EM_LDF8_F1_R3,
EM_LDF8_NT1_F1_R3,
EM_LDF8_NTA_F1_R3,
EM_LDFE_F1_R3,
EM_LDFE_NT1_F1_R3,
EM_LDFE_NTA_F1_R3,
EM_LDFS_S_F1_R3,
EM_LDFS_S_NT1_F1_R3,
EM_LDFS_S_NTA_F1_R3,
EM_LDFD_S_F1_R3,
EM_LDFD_S_NT1_F1_R3,
EM_LDFD_S_NTA_F1_R3,
EM_LDF8_S_F1_R3,
EM_LDF8_S_NT1_F1_R3,
EM_LDF8_S_NTA_F1_R3,
EM_LDFE_S_F1_R3,
EM_LDFE_S_NT1_F1_R3,
EM_LDFE_S_NTA_F1_R3,
EM_LDFS_A_F1_R3,
EM_LDFS_A_NT1_F1_R3,
EM_LDFS_A_NTA_F1_R3,
EM_LDFD_A_F1_R3,
EM_LDFD_A_NT1_F1_R3,
EM_LDFD_A_NTA_F1_R3,
EM_LDF8_A_F1_R3,
EM_LDF8_A_NT1_F1_R3,
EM_LDF8_A_NTA_F1_R3,
EM_LDFE_A_F1_R3,
EM_LDFE_A_NT1_F1_R3,
EM_LDFE_A_NTA_F1_R3,
EM_LDFS_SA_F1_R3,
EM_LDFS_SA_NT1_F1_R3,
EM_LDFS_SA_NTA_F1_R3,
EM_LDFD_SA_F1_R3,
EM_LDFD_SA_NT1_F1_R3,
EM_LDFD_SA_NTA_F1_R3,
EM_LDF8_SA_F1_R3,
EM_LDF8_SA_NT1_F1_R3,
EM_LDF8_SA_NTA_F1_R3,
EM_LDFE_SA_F1_R3,
EM_LDFE_SA_NT1_F1_R3,
EM_LDFE_SA_NTA_F1_R3,
EM_LDF_FILL_F1_R3,
EM_LDF_FILL_NT1_F1_R3,
EM_LDF_FILL_NTA_F1_R3,
EM_LDFS_C_CLR_F1_R3,
EM_LDFS_C_CLR_NT1_F1_R3,
EM_LDFS_C_CLR_NTA_F1_R3,
EM_LDFD_C_CLR_F1_R3,
EM_LDFD_C_CLR_NT1_F1_R3,
EM_LDFD_C_CLR_NTA_F1_R3,
EM_LDF8_C_CLR_F1_R3,
EM_LDF8_C_CLR_NT1_F1_R3,
EM_LDF8_C_CLR_NTA_F1_R3,
EM_LDFE_C_CLR_F1_R3,
EM_LDFE_C_CLR_NT1_F1_R3,
EM_LDFE_C_CLR_NTA_F1_R3,
EM_LDFS_C_NC_F1_R3,
EM_LDFS_C_NC_NT1_F1_R3,
EM_LDFS_C_NC_NTA_F1_R3,
EM_LDFD_C_NC_F1_R3,
EM_LDFD_C_NC_NT1_F1_R3,
EM_LDFD_C_NC_NTA_F1_R3,
EM_LDF8_C_NC_F1_R3,
EM_LDF8_C_NC_NT1_F1_R3,
EM_LDF8_C_NC_NTA_F1_R3,
EM_LDFE_C_NC_F1_R3,
EM_LDFE_C_NC_NT1_F1_R3,
EM_LDFE_C_NC_NTA_F1_R3,
EM_LDFS_F1_R3_R2,
EM_LDFS_NT1_F1_R3_R2,
EM_LDFS_NTA_F1_R3_R2,
EM_LDFD_F1_R3_R2,
EM_LDFD_NT1_F1_R3_R2,
EM_LDFD_NTA_F1_R3_R2,
EM_LDF8_F1_R3_R2,
EM_LDF8_NT1_F1_R3_R2,
EM_LDF8_NTA_F1_R3_R2,
EM_LDFE_F1_R3_R2,
EM_LDFE_NT1_F1_R3_R2,
EM_LDFE_NTA_F1_R3_R2,
EM_LDFS_S_F1_R3_R2,
EM_LDFS_S_NT1_F1_R3_R2,
EM_LDFS_S_NTA_F1_R3_R2,
EM_LDFD_S_F1_R3_R2,
EM_LDFD_S_NT1_F1_R3_R2,
EM_LDFD_S_NTA_F1_R3_R2,
EM_LDF8_S_F1_R3_R2,
EM_LDF8_S_NT1_F1_R3_R2,
EM_LDF8_S_NTA_F1_R3_R2,
EM_LDFE_S_F1_R3_R2,
EM_LDFE_S_NT1_F1_R3_R2,
EM_LDFE_S_NTA_F1_R3_R2,
EM_LDFS_A_F1_R3_R2,
EM_LDFS_A_NT1_F1_R3_R2,
EM_LDFS_A_NTA_F1_R3_R2,
EM_LDFD_A_F1_R3_R2,
EM_LDFD_A_NT1_F1_R3_R2,
EM_LDFD_A_NTA_F1_R3_R2,
EM_LDF8_A_F1_R3_R2,
EM_LDF8_A_NT1_F1_R3_R2,
EM_LDF8_A_NTA_F1_R3_R2,
EM_LDFE_A_F1_R3_R2,
EM_LDFE_A_NT1_F1_R3_R2,
EM_LDFE_A_NTA_F1_R3_R2,
EM_LDFS_SA_F1_R3_R2,
EM_LDFS_SA_NT1_F1_R3_R2,
EM_LDFS_SA_NTA_F1_R3_R2,
EM_LDFD_SA_F1_R3_R2,
EM_LDFD_SA_NT1_F1_R3_R2,
EM_LDFD_SA_NTA_F1_R3_R2,
EM_LDF8_SA_F1_R3_R2,
EM_LDF8_SA_NT1_F1_R3_R2,
EM_LDF8_SA_NTA_F1_R3_R2,
EM_LDFE_SA_F1_R3_R2,
EM_LDFE_SA_NT1_F1_R3_R2,
EM_LDFE_SA_NTA_F1_R3_R2,
EM_LDF_FILL_F1_R3_R2,
EM_LDF_FILL_NT1_F1_R3_R2,
EM_LDF_FILL_NTA_F1_R3_R2,
EM_LDFS_C_CLR_F1_R3_R2,
EM_LDFS_C_CLR_NT1_F1_R3_R2,
EM_LDFS_C_CLR_NTA_F1_R3_R2,
EM_LDFD_C_CLR_F1_R3_R2,
EM_LDFD_C_CLR_NT1_F1_R3_R2,
EM_LDFD_C_CLR_NTA_F1_R3_R2,
EM_LDF8_C_CLR_F1_R3_R2,
EM_LDF8_C_CLR_NT1_F1_R3_R2,
EM_LDF8_C_CLR_NTA_F1_R3_R2,
EM_LDFE_C_CLR_F1_R3_R2,
EM_LDFE_C_CLR_NT1_F1_R3_R2,
EM_LDFE_C_CLR_NTA_F1_R3_R2,
EM_LDFS_C_NC_F1_R3_R2,
EM_LDFS_C_NC_NT1_F1_R3_R2,
EM_LDFS_C_NC_NTA_F1_R3_R2,
EM_LDFD_C_NC_F1_R3_R2,
EM_LDFD_C_NC_NT1_F1_R3_R2,
EM_LDFD_C_NC_NTA_F1_R3_R2,
EM_LDF8_C_NC_F1_R3_R2,
EM_LDF8_C_NC_NT1_F1_R3_R2,
EM_LDF8_C_NC_NTA_F1_R3_R2,
EM_LDFE_C_NC_F1_R3_R2,
EM_LDFE_C_NC_NT1_F1_R3_R2,
EM_LDFE_C_NC_NTA_F1_R3_R2,
EM_LDFS_F1_R3_IMM9,
EM_LDFS_NT1_F1_R3_IMM9,
EM_LDFS_NTA_F1_R3_IMM9,
EM_LDFD_F1_R3_IMM9,
EM_LDFD_NT1_F1_R3_IMM9,
EM_LDFD_NTA_F1_R3_IMM9,
EM_LDF8_F1_R3_IMM9,
EM_LDF8_NT1_F1_R3_IMM9,
EM_LDF8_NTA_F1_R3_IMM9,
EM_LDFE_F1_R3_IMM9,
EM_LDFE_NT1_F1_R3_IMM9,
EM_LDFE_NTA_F1_R3_IMM9,
EM_LDFS_S_F1_R3_IMM9,
EM_LDFS_S_NT1_F1_R3_IMM9,
EM_LDFS_S_NTA_F1_R3_IMM9,
EM_LDFD_S_F1_R3_IMM9,
EM_LDFD_S_NT1_F1_R3_IMM9,
EM_LDFD_S_NTA_F1_R3_IMM9,
EM_LDF8_S_F1_R3_IMM9,
EM_LDF8_S_NT1_F1_R3_IMM9,
EM_LDF8_S_NTA_F1_R3_IMM9,
EM_LDFE_S_F1_R3_IMM9,
EM_LDFE_S_NT1_F1_R3_IMM9,
EM_LDFE_S_NTA_F1_R3_IMM9,
EM_LDFS_A_F1_R3_IMM9,
EM_LDFS_A_NT1_F1_R3_IMM9,
EM_LDFS_A_NTA_F1_R3_IMM9,
EM_LDFD_A_F1_R3_IMM9,
EM_LDFD_A_NT1_F1_R3_IMM9,
EM_LDFD_A_NTA_F1_R3_IMM9,
EM_LDF8_A_F1_R3_IMM9,
EM_LDF8_A_NT1_F1_R3_IMM9,
EM_LDF8_A_NTA_F1_R3_IMM9,
EM_LDFE_A_F1_R3_IMM9,
EM_LDFE_A_NT1_F1_R3_IMM9,
EM_LDFE_A_NTA_F1_R3_IMM9,
EM_LDFS_SA_F1_R3_IMM9,
EM_LDFS_SA_NT1_F1_R3_IMM9,
EM_LDFS_SA_NTA_F1_R3_IMM9,
EM_LDFD_SA_F1_R3_IMM9,
EM_LDFD_SA_NT1_F1_R3_IMM9,
EM_LDFD_SA_NTA_F1_R3_IMM9,
EM_LDF8_SA_F1_R3_IMM9,
EM_LDF8_SA_NT1_F1_R3_IMM9,
EM_LDF8_SA_NTA_F1_R3_IMM9,
EM_LDFE_SA_F1_R3_IMM9,
EM_LDFE_SA_NT1_F1_R3_IMM9,
EM_LDFE_SA_NTA_F1_R3_IMM9,
EM_LDF_FILL_F1_R3_IMM9,
EM_LDF_FILL_NT1_F1_R3_IMM9,
EM_LDF_FILL_NTA_F1_R3_IMM9,
EM_LDFS_C_CLR_F1_R3_IMM9,
EM_LDFS_C_CLR_NT1_F1_R3_IMM9,
EM_LDFS_C_CLR_NTA_F1_R3_IMM9,
EM_LDFD_C_CLR_F1_R3_IMM9,
EM_LDFD_C_CLR_NT1_F1_R3_IMM9,
EM_LDFD_C_CLR_NTA_F1_R3_IMM9,
EM_LDF8_C_CLR_F1_R3_IMM9,
EM_LDF8_C_CLR_NT1_F1_R3_IMM9,
EM_LDF8_C_CLR_NTA_F1_R3_IMM9,
EM_LDFE_C_CLR_F1_R3_IMM9,
EM_LDFE_C_CLR_NT1_F1_R3_IMM9,
EM_LDFE_C_CLR_NTA_F1_R3_IMM9,
EM_LDFS_C_NC_F1_R3_IMM9,
EM_INST1_LAST
} Inst_id_t1;

typedef enum Inst_id_e2
{
EM_LDFS_C_NC_NT1_F1_R3_IMM9 = EM_INST1_LAST,
EM_LDFS_C_NC_NTA_F1_R3_IMM9,
EM_LDFD_C_NC_F1_R3_IMM9,
EM_LDFD_C_NC_NT1_F1_R3_IMM9,
EM_LDFD_C_NC_NTA_F1_R3_IMM9,
EM_LDF8_C_NC_F1_R3_IMM9,
EM_LDF8_C_NC_NT1_F1_R3_IMM9,
EM_LDF8_C_NC_NTA_F1_R3_IMM9,
EM_LDFE_C_NC_F1_R3_IMM9,
EM_LDFE_C_NC_NT1_F1_R3_IMM9,
EM_LDFE_C_NC_NTA_F1_R3_IMM9,
EM_STFS_R3_F2,
EM_STFS_NTA_R3_F2,
EM_STFD_R3_F2,
EM_STFD_NTA_R3_F2,
EM_STF8_R3_F2,
EM_STF8_NTA_R3_F2,
EM_STFE_R3_F2,
EM_STFE_NTA_R3_F2,
EM_STF_SPILL_R3_F2,
EM_STF_SPILL_NTA_R3_F2,
EM_STFS_R3_F2_IMM9,
EM_STFS_NTA_R3_F2_IMM9,
EM_STFD_R3_F2_IMM9,
EM_STFD_NTA_R3_F2_IMM9,
EM_STF8_R3_F2_IMM9,
EM_STF8_NTA_R3_F2_IMM9,
EM_STFE_R3_F2_IMM9,
EM_STFE_NTA_R3_F2_IMM9,
EM_STF_SPILL_R3_F2_IMM9,
EM_STF_SPILL_NTA_R3_F2_IMM9,
EM_LDFPS_F1_F2_R3,
EM_LDFPS_NT1_F1_F2_R3,
EM_LDFPS_NTA_F1_F2_R3,
EM_LDFPD_F1_F2_R3,
EM_LDFPD_NT1_F1_F2_R3,
EM_LDFPD_NTA_F1_F2_R3,
EM_LDFP8_F1_F2_R3,
EM_LDFP8_NT1_F1_F2_R3,
EM_LDFP8_NTA_F1_F2_R3,
EM_LDFPS_S_F1_F2_R3,
EM_LDFPS_S_NT1_F1_F2_R3,
EM_LDFPS_S_NTA_F1_F2_R3,
EM_LDFPD_S_F1_F2_R3,
EM_LDFPD_S_NT1_F1_F2_R3,
EM_LDFPD_S_NTA_F1_F2_R3,
EM_LDFP8_S_F1_F2_R3,
EM_LDFP8_S_NT1_F1_F2_R3,
EM_LDFP8_S_NTA_F1_F2_R3,
EM_LDFPS_A_F1_F2_R3,
EM_LDFPS_A_NT1_F1_F2_R3,
EM_LDFPS_A_NTA_F1_F2_R3,
EM_LDFPD_A_F1_F2_R3,
EM_LDFPD_A_NT1_F1_F2_R3,
EM_LDFPD_A_NTA_F1_F2_R3,
EM_LDFP8_A_F1_F2_R3,
EM_LDFP8_A_NT1_F1_F2_R3,
EM_LDFP8_A_NTA_F1_F2_R3,
EM_LDFPS_SA_F1_F2_R3,
EM_LDFPS_SA_NT1_F1_F2_R3,
EM_LDFPS_SA_NTA_F1_F2_R3,
EM_LDFPD_SA_F1_F2_R3,
EM_LDFPD_SA_NT1_F1_F2_R3,
EM_LDFPD_SA_NTA_F1_F2_R3,
EM_LDFP8_SA_F1_F2_R3,
EM_LDFP8_SA_NT1_F1_F2_R3,
EM_LDFP8_SA_NTA_F1_F2_R3,
EM_LDFPS_C_CLR_F1_F2_R3,
EM_LDFPS_C_CLR_NT1_F1_F2_R3,
EM_LDFPS_C_CLR_NTA_F1_F2_R3,
EM_LDFPD_C_CLR_F1_F2_R3,
EM_LDFPD_C_CLR_NT1_F1_F2_R3,
EM_LDFPD_C_CLR_NTA_F1_F2_R3,
EM_LDFP8_C_CLR_F1_F2_R3,
EM_LDFP8_C_CLR_NT1_F1_F2_R3,
EM_LDFP8_C_CLR_NTA_F1_F2_R3,
EM_LDFPS_C_NC_F1_F2_R3,
EM_LDFPS_C_NC_NT1_F1_F2_R3,
EM_LDFPS_C_NC_NTA_F1_F2_R3,
EM_LDFPD_C_NC_F1_F2_R3,
EM_LDFPD_C_NC_NT1_F1_F2_R3,
EM_LDFPD_C_NC_NTA_F1_F2_R3,
EM_LDFP8_C_NC_F1_F2_R3,
EM_LDFP8_C_NC_NT1_F1_F2_R3,
EM_LDFP8_C_NC_NTA_F1_F2_R3,
EM_LDFPS_F1_F2_R3_8,
EM_LDFPS_NT1_F1_F2_R3_8,
EM_LDFPS_NTA_F1_F2_R3_8,
EM_LDFPD_F1_F2_R3_16,
EM_LDFPD_NT1_F1_F2_R3_16,
EM_LDFPD_NTA_F1_F2_R3_16,
EM_LDFP8_F1_F2_R3_16,
EM_LDFP8_NT1_F1_F2_R3_16,
EM_LDFP8_NTA_F1_F2_R3_16,
EM_LDFPS_S_F1_F2_R3_8,
EM_LDFPS_S_NT1_F1_F2_R3_8,
EM_LDFPS_S_NTA_F1_F2_R3_8,
EM_LDFPD_S_F1_F2_R3_16,
EM_LDFPD_S_NT1_F1_F2_R3_16,
EM_LDFPD_S_NTA_F1_F2_R3_16,
EM_LDFP8_S_F1_F2_R3_16,
EM_LDFP8_S_NT1_F1_F2_R3_16,
EM_LDFP8_S_NTA_F1_F2_R3_16,
EM_LDFPS_A_F1_F2_R3_8,
EM_LDFPS_A_NT1_F1_F2_R3_8,
EM_LDFPS_A_NTA_F1_F2_R3_8,
EM_LDFPD_A_F1_F2_R3_16,
EM_LDFPD_A_NT1_F1_F2_R3_16,
EM_LDFPD_A_NTA_F1_F2_R3_16,
EM_LDFP8_A_F1_F2_R3_16,
EM_LDFP8_A_NT1_F1_F2_R3_16,
EM_LDFP8_A_NTA_F1_F2_R3_16,
EM_LDFPS_SA_F1_F2_R3_8,
EM_LDFPS_SA_NT1_F1_F2_R3_8,
EM_LDFPS_SA_NTA_F1_F2_R3_8,
EM_LDFPD_SA_F1_F2_R3_16,
EM_LDFPD_SA_NT1_F1_F2_R3_16,
EM_LDFPD_SA_NTA_F1_F2_R3_16,
EM_LDFP8_SA_F1_F2_R3_16,
EM_LDFP8_SA_NT1_F1_F2_R3_16,
EM_LDFP8_SA_NTA_F1_F2_R3_16,
EM_LDFPS_C_CLR_F1_F2_R3_8,
EM_LDFPS_C_CLR_NT1_F1_F2_R3_8,
EM_LDFPS_C_CLR_NTA_F1_F2_R3_8,
EM_LDFPD_C_CLR_F1_F2_R3_16,
EM_LDFPD_C_CLR_NT1_F1_F2_R3_16,
EM_LDFPD_C_CLR_NTA_F1_F2_R3_16,
EM_LDFP8_C_CLR_F1_F2_R3_16,
EM_LDFP8_C_CLR_NT1_F1_F2_R3_16,
EM_LDFP8_C_CLR_NTA_F1_F2_R3_16,
EM_LDFPS_C_NC_F1_F2_R3_8,
EM_LDFPS_C_NC_NT1_F1_F2_R3_8,
EM_LDFPS_C_NC_NTA_F1_F2_R3_8,
EM_LDFPD_C_NC_F1_F2_R3_16,
EM_LDFPD_C_NC_NT1_F1_F2_R3_16,
EM_LDFPD_C_NC_NTA_F1_F2_R3_16,
EM_LDFP8_C_NC_F1_F2_R3_16,
EM_LDFP8_C_NC_NT1_F1_F2_R3_16,
EM_LDFP8_C_NC_NTA_F1_F2_R3_16,
EM_LFETCH_R3,
EM_LFETCH_NT1_R3,
EM_LFETCH_NT2_R3,
EM_LFETCH_NTA_R3,
EM_LFETCH_EXCL_R3,
EM_LFETCH_EXCL_NT1_R3,
EM_LFETCH_EXCL_NT2_R3,
EM_LFETCH_EXCL_NTA_R3,
EM_LFETCH_FAULT_R3,
EM_LFETCH_FAULT_NT1_R3,
EM_LFETCH_FAULT_NT2_R3,
EM_LFETCH_FAULT_NTA_R3,
EM_LFETCH_FAULT_EXCL_R3,
EM_LFETCH_FAULT_EXCL_NT1_R3,
EM_LFETCH_FAULT_EXCL_NT2_R3,
EM_LFETCH_FAULT_EXCL_NTA_R3,
EM_LFETCH_R3_R2,
EM_LFETCH_NT1_R3_R2,
EM_LFETCH_NT2_R3_R2,
EM_LFETCH_NTA_R3_R2,
EM_LFETCH_EXCL_R3_R2,
EM_LFETCH_EXCL_NT1_R3_R2,
EM_LFETCH_EXCL_NT2_R3_R2,
EM_LFETCH_EXCL_NTA_R3_R2,
EM_LFETCH_FAULT_R3_R2,
EM_LFETCH_FAULT_NT1_R3_R2,
EM_LFETCH_FAULT_NT2_R3_R2,
EM_LFETCH_FAULT_NTA_R3_R2,
EM_LFETCH_FAULT_EXCL_R3_R2,
EM_LFETCH_FAULT_EXCL_NT1_R3_R2,
EM_LFETCH_FAULT_EXCL_NT2_R3_R2,
EM_LFETCH_FAULT_EXCL_NTA_R3_R2,
EM_LFETCH_R3_IMM9,
EM_LFETCH_NT1_R3_IMM9,
EM_LFETCH_NT2_R3_IMM9,
EM_LFETCH_NTA_R3_IMM9,
EM_LFETCH_EXCL_R3_IMM9,
EM_LFETCH_EXCL_NT1_R3_IMM9,
EM_LFETCH_EXCL_NT2_R3_IMM9,
EM_LFETCH_EXCL_NTA_R3_IMM9,
EM_LFETCH_FAULT_R3_IMM9,
EM_LFETCH_FAULT_NT1_R3_IMM9,
EM_LFETCH_FAULT_NT2_R3_IMM9,
EM_LFETCH_FAULT_NTA_R3_IMM9,
EM_LFETCH_FAULT_EXCL_R3_IMM9,
EM_LFETCH_FAULT_EXCL_NT1_R3_IMM9,
EM_LFETCH_FAULT_EXCL_NT2_R3_IMM9,
EM_LFETCH_FAULT_EXCL_NTA_R3_IMM9,
EM_CMPXCHG1_ACQ_R1_R3_R2_AR_CCV,
EM_CMPXCHG1_ACQ_NT1_R1_R3_R2_AR_CCV,
EM_CMPXCHG1_ACQ_NTA_R1_R3_R2_AR_CCV,
EM_CMPXCHG2_ACQ_R1_R3_R2_AR_CCV,
EM_CMPXCHG2_ACQ_NT1_R1_R3_R2_AR_CCV,
EM_CMPXCHG2_ACQ_NTA_R1_R3_R2_AR_CCV,
EM_CMPXCHG4_ACQ_R1_R3_R2_AR_CCV,
EM_CMPXCHG4_ACQ_NT1_R1_R3_R2_AR_CCV,
EM_CMPXCHG4_ACQ_NTA_R1_R3_R2_AR_CCV,
EM_CMPXCHG8_ACQ_R1_R3_R2_AR_CCV,
EM_CMPXCHG8_ACQ_NT1_R1_R3_R2_AR_CCV,
EM_CMPXCHG8_ACQ_NTA_R1_R3_R2_AR_CCV,
EM_CMPXCHG1_REL_R1_R3_R2_AR_CCV,
EM_CMPXCHG1_REL_NT1_R1_R3_R2_AR_CCV,
EM_CMPXCHG1_REL_NTA_R1_R3_R2_AR_CCV,
EM_CMPXCHG2_REL_R1_R3_R2_AR_CCV,
EM_CMPXCHG2_REL_NT1_R1_R3_R2_AR_CCV,
EM_CMPXCHG2_REL_NTA_R1_R3_R2_AR_CCV,
EM_CMPXCHG4_REL_R1_R3_R2_AR_CCV,
EM_CMPXCHG4_REL_NT1_R1_R3_R2_AR_CCV,
EM_CMPXCHG4_REL_NTA_R1_R3_R2_AR_CCV,
EM_CMPXCHG8_REL_R1_R3_R2_AR_CCV,
EM_CMPXCHG8_REL_NT1_R1_R3_R2_AR_CCV,
EM_CMPXCHG8_REL_NTA_R1_R3_R2_AR_CCV,
EM_XCHG1_R1_R3_R2,
EM_XCHG1_NT1_R1_R3_R2,
EM_XCHG1_NTA_R1_R3_R2,
EM_XCHG2_R1_R3_R2,
EM_XCHG2_NT1_R1_R3_R2,
EM_XCHG2_NTA_R1_R3_R2,
EM_XCHG4_R1_R3_R2,
EM_XCHG4_NT1_R1_R3_R2,
EM_XCHG4_NTA_R1_R3_R2,
EM_XCHG8_R1_R3_R2,
EM_XCHG8_NT1_R1_R3_R2,
EM_XCHG8_NTA_R1_R3_R2,
EM_FETCHADD4_ACQ_R1_R3_INC3,
EM_FETCHADD4_ACQ_NT1_R1_R3_INC3,
EM_FETCHADD4_ACQ_NTA_R1_R3_INC3,
EM_FETCHADD8_ACQ_R1_R3_INC3,
EM_FETCHADD8_ACQ_NT1_R1_R3_INC3,
EM_FETCHADD8_ACQ_NTA_R1_R3_INC3,
EM_FETCHADD4_REL_R1_R3_INC3,
EM_FETCHADD4_REL_NT1_R1_R3_INC3,
EM_FETCHADD4_REL_NTA_R1_R3_INC3,
EM_FETCHADD8_REL_R1_R3_INC3,
EM_FETCHADD8_REL_NT1_R1_R3_INC3,
EM_FETCHADD8_REL_NTA_R1_R3_INC3,
EM_SETF_SIG_F1_R2,
EM_SETF_EXP_F1_R2,
EM_SETF_S_F1_R2,
EM_SETF_D_F1_R2,
EM_GETF_SIG_R1_F2,
EM_GETF_EXP_R1_F2,
EM_GETF_S_R1_F2,
EM_GETF_D_R1_F2,
EM_CHK_S_M_R2_TARGET25,
EM_CHK_S_F2_TARGET25,
EM_CHK_A_NC_R1_TARGET25,
EM_CHK_A_CLR_R1_TARGET25,
EM_CHK_A_NC_F1_TARGET25,
EM_CHK_A_CLR_F1_TARGET25,
EM_INVALA,
EM_FWB,
EM_MF,
EM_MF_A,
EM_SRLZ_D,
EM_SRLZ_I,
EM_SYNC_I,
EM_FLUSHRS,
EM_LOADRS,
EM_INVALA_E_R1,
EM_INVALA_E_F1,
EM_FC_R3,
EM_PTC_E_R3,
EM_MOV_M_AR3_R2,
EM_MOV_M_AR3_IMM8,
EM_MOV_M_R1_AR3,
EM_MOV_CR3_R2,
EM_MOV_R1_CR3,
EM_ALLOC_R1_AR_PFS_I_L_O_R,
EM_MOV_PSR_L_R2,
EM_MOV_PSR_UM_R2,
EM_MOV_R1_PSR,
EM_MOV_R1_PSR_UM,
EM_BREAK_M_IMM21,
EM_NOP_M_IMM21,
EM_PROBE_R_R1_R3_R2,
EM_PROBE_W_R1_R3_R2,
EM_PROBE_R_R1_R3_IMM2,
EM_PROBE_W_R1_R3_IMM2,
EM_PROBE_RW_FAULT_R3_IMM2,
EM_PROBE_R_FAULT_R3_IMM2,
EM_PROBE_W_FAULT_R3_IMM2,
EM_ITC_D_R2,
EM_ITC_I_R2,
EM_MOV_RR_R3_R2,
EM_MOV_DBR_R3_R2,
EM_MOV_IBR_R3_R2,
EM_MOV_PKR_R3_R2,
EM_MOV_PMC_R3_R2,
EM_MOV_PMD_R3_R2,
EM_MOV_MSR_R3_R2,
EM_ITR_D_DTR_R3_R2,
EM_ITR_I_ITR_R3_R2,
EM_MOV_R1_RR_R3,
EM_MOV_R1_DBR_R3,
EM_MOV_R1_IBR_R3,
EM_MOV_R1_PKR_R3,
EM_MOV_R1_PMC_R3,
EM_MOV_R1_MSR_R3,
EM_MOV_R1_PMD_R3,
EM_MOV_R1_CPUID_R3,
EM_SUM_IMM24,
EM_RUM_IMM24,
EM_SSM_IMM24,
EM_RSM_IMM24,
EM_PTC_L_R3_R2,
EM_PTC_G_R3_R2,
EM_PTC_GA_R3_R2,
EM_PTR_D_R3_R2,
EM_PTR_I_R3_R2,
EM_THASH_R1_R3,
EM_TTAG_R1_R3,
EM_TPA_R1_R3,
EM_TAK_R1_R3,
EM_BR_COND_SPTK_FEW_TARGET25,
EM_BR_COND_SPTK_MANY_TARGET25,
EM_BR_COND_SPNT_FEW_TARGET25,
EM_BR_COND_SPNT_MANY_TARGET25,
EM_BR_COND_DPTK_FEW_TARGET25,
EM_BR_COND_DPTK_MANY_TARGET25,
EM_BR_COND_DPNT_FEW_TARGET25,
EM_BR_COND_DPNT_MANY_TARGET25,
EM_BR_COND_SPTK_FEW_CLR_TARGET25,
EM_BR_COND_SPTK_MANY_CLR_TARGET25,
EM_BR_COND_SPNT_FEW_CLR_TARGET25,
EM_BR_COND_SPNT_MANY_CLR_TARGET25,
EM_BR_COND_DPTK_FEW_CLR_TARGET25,
EM_BR_COND_DPTK_MANY_CLR_TARGET25,
EM_BR_COND_DPNT_FEW_CLR_TARGET25,
EM_BR_COND_DPNT_MANY_CLR_TARGET25,
EM_BR_WEXIT_SPTK_FEW_TARGET25,
EM_BR_WEXIT_SPTK_MANY_TARGET25,
EM_BR_WEXIT_SPNT_FEW_TARGET25,
EM_BR_WEXIT_SPNT_MANY_TARGET25,
EM_BR_WEXIT_DPTK_FEW_TARGET25,
EM_BR_WEXIT_DPTK_MANY_TARGET25,
EM_BR_WEXIT_DPNT_FEW_TARGET25,
EM_BR_WEXIT_DPNT_MANY_TARGET25,
EM_BR_WEXIT_SPTK_FEW_CLR_TARGET25,
EM_BR_WEXIT_SPTK_MANY_CLR_TARGET25,
EM_BR_WEXIT_SPNT_FEW_CLR_TARGET25,
EM_BR_WEXIT_SPNT_MANY_CLR_TARGET25,
EM_BR_WEXIT_DPTK_FEW_CLR_TARGET25,
EM_BR_WEXIT_DPTK_MANY_CLR_TARGET25,
EM_BR_WEXIT_DPNT_FEW_CLR_TARGET25,
EM_BR_WEXIT_DPNT_MANY_CLR_TARGET25,
EM_BR_WTOP_SPTK_FEW_TARGET25,
EM_BR_WTOP_SPTK_MANY_TARGET25,
EM_BR_WTOP_SPNT_FEW_TARGET25,
EM_BR_WTOP_SPNT_MANY_TARGET25,
EM_BR_WTOP_DPTK_FEW_TARGET25,
EM_BR_WTOP_DPTK_MANY_TARGET25,
EM_BR_WTOP_DPNT_FEW_TARGET25,
EM_BR_WTOP_DPNT_MANY_TARGET25,
EM_BR_WTOP_SPTK_FEW_CLR_TARGET25,
EM_BR_WTOP_SPTK_MANY_CLR_TARGET25,
EM_BR_WTOP_SPNT_FEW_CLR_TARGET25,
EM_BR_WTOP_SPNT_MANY_CLR_TARGET25,
EM_BR_WTOP_DPTK_FEW_CLR_TARGET25,
EM_BR_WTOP_DPTK_MANY_CLR_TARGET25,
EM_BR_WTOP_DPNT_FEW_CLR_TARGET25,
EM_BR_WTOP_DPNT_MANY_CLR_TARGET25,
EM_BR_CLOOP_SPTK_FEW_TARGET25,
EM_BR_CLOOP_SPTK_MANY_TARGET25,
EM_BR_CLOOP_SPNT_FEW_TARGET25,
EM_BR_CLOOP_SPNT_MANY_TARGET25,
EM_BR_CLOOP_DPTK_FEW_TARGET25,
EM_BR_CLOOP_DPTK_MANY_TARGET25,
EM_BR_CLOOP_DPNT_FEW_TARGET25,
EM_BR_CLOOP_DPNT_MANY_TARGET25,
EM_BR_CLOOP_SPTK_FEW_CLR_TARGET25,
EM_BR_CLOOP_SPTK_MANY_CLR_TARGET25,
EM_BR_CLOOP_SPNT_FEW_CLR_TARGET25,
EM_BR_CLOOP_SPNT_MANY_CLR_TARGET25,
EM_BR_CLOOP_DPTK_FEW_CLR_TARGET25,
EM_BR_CLOOP_DPTK_MANY_CLR_TARGET25,
EM_BR_CLOOP_DPNT_FEW_CLR_TARGET25,
EM_BR_CLOOP_DPNT_MANY_CLR_TARGET25,
EM_BR_CEXIT_SPTK_FEW_TARGET25,
EM_BR_CEXIT_SPTK_MANY_TARGET25,
EM_BR_CEXIT_SPNT_FEW_TARGET25,
EM_BR_CEXIT_SPNT_MANY_TARGET25,
EM_BR_CEXIT_DPTK_FEW_TARGET25,
EM_BR_CEXIT_DPTK_MANY_TARGET25,
EM_BR_CEXIT_DPNT_FEW_TARGET25,
EM_BR_CEXIT_DPNT_MANY_TARGET25,
EM_BR_CEXIT_SPTK_FEW_CLR_TARGET25,
EM_BR_CEXIT_SPTK_MANY_CLR_TARGET25,
EM_BR_CEXIT_SPNT_FEW_CLR_TARGET25,
EM_BR_CEXIT_SPNT_MANY_CLR_TARGET25,
EM_BR_CEXIT_DPTK_FEW_CLR_TARGET25,
EM_BR_CEXIT_DPTK_MANY_CLR_TARGET25,
EM_BR_CEXIT_DPNT_FEW_CLR_TARGET25,
EM_BR_CEXIT_DPNT_MANY_CLR_TARGET25,
EM_BR_CTOP_SPTK_FEW_TARGET25,
EM_BR_CTOP_SPTK_MANY_TARGET25,
EM_BR_CTOP_SPNT_FEW_TARGET25,
EM_BR_CTOP_SPNT_MANY_TARGET25,
EM_BR_CTOP_DPTK_FEW_TARGET25,
EM_BR_CTOP_DPTK_MANY_TARGET25,
EM_BR_CTOP_DPNT_FEW_TARGET25,
EM_BR_CTOP_DPNT_MANY_TARGET25,
EM_BR_CTOP_SPTK_FEW_CLR_TARGET25,
EM_BR_CTOP_SPTK_MANY_CLR_TARGET25,
EM_BR_CTOP_SPNT_FEW_CLR_TARGET25,
EM_BR_CTOP_SPNT_MANY_CLR_TARGET25,
EM_BR_CTOP_DPTK_FEW_CLR_TARGET25,
EM_BR_CTOP_DPTK_MANY_CLR_TARGET25,
EM_BR_CTOP_DPNT_FEW_CLR_TARGET25,
EM_BR_CTOP_DPNT_MANY_CLR_TARGET25,
EM_BR_CALL_SPTK_FEW_B1_TARGET25,
EM_BR_CALL_SPTK_MANY_B1_TARGET25,
EM_BR_CALL_SPNT_FEW_B1_TARGET25,
EM_BR_CALL_SPNT_MANY_B1_TARGET25,
EM_BR_CALL_DPTK_FEW_B1_TARGET25,
EM_BR_CALL_DPTK_MANY_B1_TARGET25,
EM_BR_CALL_DPNT_FEW_B1_TARGET25,
EM_BR_CALL_DPNT_MANY_B1_TARGET25,
EM_BR_CALL_SPTK_FEW_CLR_B1_TARGET25,
EM_BR_CALL_SPTK_MANY_CLR_B1_TARGET25,
EM_BR_CALL_SPNT_FEW_CLR_B1_TARGET25,
EM_BR_CALL_SPNT_MANY_CLR_B1_TARGET25,
EM_BR_CALL_DPTK_FEW_CLR_B1_TARGET25,
EM_BR_CALL_DPTK_MANY_CLR_B1_TARGET25,
EM_BR_CALL_DPNT_FEW_CLR_B1_TARGET25,
EM_BR_CALL_DPNT_MANY_CLR_B1_TARGET25,
EM_BR_COND_SPTK_FEW_B2,
EM_BR_COND_SPTK_MANY_B2,
EM_BR_COND_SPNT_FEW_B2,
EM_BR_COND_SPNT_MANY_B2,
EM_BR_COND_DPTK_FEW_B2,
EM_BR_COND_DPTK_MANY_B2,
EM_BR_COND_DPNT_FEW_B2,
EM_BR_COND_DPNT_MANY_B2,
EM_BR_COND_SPTK_FEW_CLR_B2,
EM_BR_COND_SPTK_MANY_CLR_B2,
EM_BR_COND_SPNT_FEW_CLR_B2,
EM_BR_COND_SPNT_MANY_CLR_B2,
EM_BR_COND_DPTK_FEW_CLR_B2,
EM_BR_COND_DPTK_MANY_CLR_B2,
EM_BR_COND_DPNT_FEW_CLR_B2,
EM_BR_COND_DPNT_MANY_CLR_B2,
EM_BR_IA_SPTK_FEW_B2,
EM_BR_IA_SPTK_MANY_B2,
EM_BR_IA_SPNT_FEW_B2,
EM_BR_IA_SPNT_MANY_B2,
EM_BR_IA_DPTK_FEW_B2,
EM_BR_IA_DPTK_MANY_B2,
EM_BR_IA_DPNT_FEW_B2,
EM_BR_IA_DPNT_MANY_B2,
EM_BR_IA_SPTK_FEW_CLR_B2,
EM_BR_IA_SPTK_MANY_CLR_B2,
EM_BR_IA_SPNT_FEW_CLR_B2,
EM_BR_IA_SPNT_MANY_CLR_B2,
EM_BR_IA_DPTK_FEW_CLR_B2,
EM_BR_IA_DPTK_MANY_CLR_B2,
EM_BR_IA_DPNT_FEW_CLR_B2,
EM_BR_IA_DPNT_MANY_CLR_B2,
EM_BR_RET_SPTK_FEW_B2,
EM_BR_RET_SPTK_MANY_B2,
EM_BR_RET_SPNT_FEW_B2,
EM_BR_RET_SPNT_MANY_B2,
EM_BR_RET_DPTK_FEW_B2,
EM_BR_RET_DPTK_MANY_B2,
EM_BR_RET_DPNT_FEW_B2,
EM_BR_RET_DPNT_MANY_B2,
EM_BR_RET_SPTK_FEW_CLR_B2,
EM_BR_RET_SPTK_MANY_CLR_B2,
EM_BR_RET_SPNT_FEW_CLR_B2,
EM_BR_RET_SPNT_MANY_CLR_B2,
EM_BR_RET_DPTK_FEW_CLR_B2,
EM_BR_RET_DPTK_MANY_CLR_B2,
EM_BR_RET_DPNT_FEW_CLR_B2,
EM_BR_RET_DPNT_MANY_CLR_B2,
EM_BR_CALL_SPTK_FEW_B1_B2,
EM_BR_CALL_SPTK_MANY_B1_B2,
EM_BR_CALL_SPNT_FEW_B1_B2,
EM_BR_CALL_SPNT_MANY_B1_B2,
EM_BR_CALL_DPTK_FEW_B1_B2,
EM_BR_CALL_DPTK_MANY_B1_B2,
EM_BR_CALL_DPNT_FEW_B1_B2,
EM_BR_CALL_DPNT_MANY_B1_B2,
EM_BR_CALL_SPTK_FEW_CLR_B1_B2,
EM_BR_CALL_SPTK_MANY_CLR_B1_B2,
EM_BR_CALL_SPNT_FEW_CLR_B1_B2,
EM_BR_CALL_SPNT_MANY_CLR_B1_B2,
EM_BR_CALL_DPTK_FEW_CLR_B1_B2,
EM_BR_CALL_DPTK_MANY_CLR_B1_B2,
EM_BR_CALL_DPNT_FEW_CLR_B1_B2,
EM_BR_CALL_DPNT_MANY_CLR_B1_B2,
EM_BRP_SPTK_FEW_DC_DC_TARGET25_TAG13,
EM_BRP_SPTK_FEW_DC_DC_IMP_TARGET25_TAG13,
EM_BRP_SPTK_MANY_DC_DC_TARGET25_TAG13,
EM_BRP_SPTK_MANY_DC_DC_IMP_TARGET25_TAG13,
EM_BRP_LOOP_FEW_DC_DC_TARGET25_TAG13,
EM_BRP_LOOP_FEW_DC_DC_IMP_TARGET25_TAG13,
EM_BRP_LOOP_MANY_DC_DC_TARGET25_TAG13,
EM_BRP_LOOP_MANY_DC_DC_IMP_TARGET25_TAG13,
EM_BRP_DPTK_FEW_DC_DC_TARGET25_TAG13,
EM_BRP_DPTK_FEW_DC_DC_IMP_TARGET25_TAG13,
EM_BRP_DPTK_MANY_DC_DC_TARGET25_TAG13,
EM_BRP_DPTK_MANY_DC_DC_IMP_TARGET25_TAG13,
EM_BRP_EXIT_FEW_DC_DC_TARGET25_TAG13,
EM_BRP_EXIT_FEW_DC_DC_IMP_TARGET25_TAG13,
EM_BRP_EXIT_MANY_DC_DC_TARGET25_TAG13,
EM_BRP_EXIT_MANY_DC_DC_IMP_TARGET25_TAG13,
EM_BRP_SPTK_FEW_DC_NT_TARGET25_TAG13,
EM_BRP_SPTK_FEW_DC_NT_IMP_TARGET25_TAG13,
EM_BRP_SPTK_MANY_DC_NT_TARGET25_TAG13,
EM_BRP_SPTK_MANY_DC_NT_IMP_TARGET25_TAG13,
EM_BRP_LOOP_FEW_DC_NT_TARGET25_TAG13,
EM_BRP_LOOP_FEW_DC_NT_IMP_TARGET25_TAG13,
EM_BRP_LOOP_MANY_DC_NT_TARGET25_TAG13,
EM_BRP_LOOP_MANY_DC_NT_IMP_TARGET25_TAG13,
EM_BRP_DPTK_FEW_DC_NT_TARGET25_TAG13,
EM_BRP_DPTK_FEW_DC_NT_IMP_TARGET25_TAG13,
EM_BRP_DPTK_MANY_DC_NT_TARGET25_TAG13,
EM_BRP_DPTK_MANY_DC_NT_IMP_TARGET25_TAG13,
EM_BRP_EXIT_FEW_DC_NT_TARGET25_TAG13,
EM_BRP_EXIT_FEW_DC_NT_IMP_TARGET25_TAG13,
EM_BRP_EXIT_MANY_DC_NT_TARGET25_TAG13,
EM_BRP_EXIT_MANY_DC_NT_IMP_TARGET25_TAG13,
EM_BRP_SPTK_FEW_TK_DC_TARGET25_TAG13,
EM_BRP_SPTK_FEW_TK_DC_IMP_TARGET25_TAG13,
EM_BRP_SPTK_MANY_TK_DC_TARGET25_TAG13,
EM_BRP_SPTK_MANY_TK_DC_IMP_TARGET25_TAG13,
EM_BRP_LOOP_FEW_TK_DC_TARGET25_TAG13,
EM_BRP_LOOP_FEW_TK_DC_IMP_TARGET25_TAG13,
EM_BRP_LOOP_MANY_TK_DC_TARGET25_TAG13,
EM_BRP_LOOP_MANY_TK_DC_IMP_TARGET25_TAG13,
EM_BRP_DPTK_FEW_TK_DC_TARGET25_TAG13,
EM_BRP_DPTK_FEW_TK_DC_IMP_TARGET25_TAG13,
EM_BRP_DPTK_MANY_TK_DC_TARGET25_TAG13,
EM_BRP_DPTK_MANY_TK_DC_IMP_TARGET25_TAG13,
EM_BRP_EXIT_FEW_TK_DC_TARGET25_TAG13,
EM_BRP_EXIT_FEW_TK_DC_IMP_TARGET25_TAG13,
EM_BRP_EXIT_MANY_TK_DC_TARGET25_TAG13,
EM_BRP_EXIT_MANY_TK_DC_IMP_TARGET25_TAG13,
EM_BRP_SPTK_FEW_TK_TK_TARGET25_TAG13,
EM_BRP_SPTK_FEW_TK_TK_IMP_TARGET25_TAG13,
EM_BRP_SPTK_MANY_TK_TK_TARGET25_TAG13,
EM_BRP_SPTK_MANY_TK_TK_IMP_TARGET25_TAG13,
EM_BRP_LOOP_FEW_TK_TK_TARGET25_TAG13,
EM_BRP_LOOP_FEW_TK_TK_IMP_TARGET25_TAG13,
EM_BRP_LOOP_MANY_TK_TK_TARGET25_TAG13,
EM_BRP_LOOP_MANY_TK_TK_IMP_TARGET25_TAG13,
EM_BRP_DPTK_FEW_TK_TK_TARGET25_TAG13,
EM_BRP_DPTK_FEW_TK_TK_IMP_TARGET25_TAG13,
EM_BRP_DPTK_MANY_TK_TK_TARGET25_TAG13,
EM_BRP_DPTK_MANY_TK_TK_IMP_TARGET25_TAG13,
EM_BRP_EXIT_FEW_TK_TK_TARGET25_TAG13,
EM_BRP_EXIT_FEW_TK_TK_IMP_TARGET25_TAG13,
EM_BRP_EXIT_MANY_TK_TK_TARGET25_TAG13,
EM_BRP_EXIT_MANY_TK_TK_IMP_TARGET25_TAG13,
EM_BRP_SPTK_FEW_TK_NT_TARGET25_TAG13,
EM_BRP_SPTK_FEW_TK_NT_IMP_TARGET25_TAG13,
EM_BRP_SPTK_MANY_TK_NT_TARGET25_TAG13,
EM_BRP_SPTK_MANY_TK_NT_IMP_TARGET25_TAG13,
EM_BRP_LOOP_FEW_TK_NT_TARGET25_TAG13,
EM_BRP_LOOP_FEW_TK_NT_IMP_TARGET25_TAG13,
EM_BRP_LOOP_MANY_TK_NT_TARGET25_TAG13,
EM_BRP_LOOP_MANY_TK_NT_IMP_TARGET25_TAG13,
EM_BRP_DPTK_FEW_TK_NT_TARGET25_TAG13,
EM_BRP_DPTK_FEW_TK_NT_IMP_TARGET25_TAG13,
EM_BRP_DPTK_MANY_TK_NT_TARGET25_TAG13,
EM_BRP_DPTK_MANY_TK_NT_IMP_TARGET25_TAG13,
EM_BRP_EXIT_FEW_TK_NT_TARGET25_TAG13,
EM_BRP_EXIT_FEW_TK_NT_IMP_TARGET25_TAG13,
EM_BRP_EXIT_MANY_TK_NT_TARGET25_TAG13,
EM_BRP_EXIT_MANY_TK_NT_IMP_TARGET25_TAG13,
EM_BRP_SPTK_FEW_NT_DC_TARGET25_TAG13,
EM_BRP_SPTK_FEW_NT_DC_IMP_TARGET25_TAG13,
EM_BRP_SPTK_MANY_NT_DC_TARGET25_TAG13,
EM_BRP_SPTK_MANY_NT_DC_IMP_TARGET25_TAG13,
EM_BRP_LOOP_FEW_NT_DC_TARGET25_TAG13,
EM_BRP_LOOP_FEW_NT_DC_IMP_TARGET25_TAG13,
EM_BRP_LOOP_MANY_NT_DC_TARGET25_TAG13,
EM_BRP_LOOP_MANY_NT_DC_IMP_TARGET25_TAG13,
EM_BRP_DPTK_FEW_NT_DC_TARGET25_TAG13,
EM_BRP_DPTK_FEW_NT_DC_IMP_TARGET25_TAG13,
EM_BRP_DPTK_MANY_NT_DC_TARGET25_TAG13,
EM_BRP_DPTK_MANY_NT_DC_IMP_TARGET25_TAG13,
EM_BRP_EXIT_FEW_NT_DC_TARGET25_TAG13,
EM_BRP_EXIT_FEW_NT_DC_IMP_TARGET25_TAG13,
EM_BRP_EXIT_MANY_NT_DC_TARGET25_TAG13,
EM_BRP_EXIT_MANY_NT_DC_IMP_TARGET25_TAG13,
EM_BRP_SPTK_FEW_NT_TK_TARGET25_TAG13,
EM_BRP_SPTK_FEW_NT_TK_IMP_TARGET25_TAG13,
EM_BRP_SPTK_MANY_NT_TK_TARGET25_TAG13,
EM_BRP_SPTK_MANY_NT_TK_IMP_TARGET25_TAG13,
EM_BRP_LOOP_FEW_NT_TK_TARGET25_TAG13,
EM_BRP_LOOP_FEW_NT_TK_IMP_TARGET25_TAG13,
EM_BRP_LOOP_MANY_NT_TK_TARGET25_TAG13,
EM_BRP_LOOP_MANY_NT_TK_IMP_TARGET25_TAG13,
EM_BRP_DPTK_FEW_NT_TK_TARGET25_TAG13,
EM_BRP_DPTK_FEW_NT_TK_IMP_TARGET25_TAG13,
EM_BRP_DPTK_MANY_NT_TK_TARGET25_TAG13,
EM_BRP_DPTK_MANY_NT_TK_IMP_TARGET25_TAG13,
EM_BRP_EXIT_FEW_NT_TK_TARGET25_TAG13,
EM_BRP_EXIT_FEW_NT_TK_IMP_TARGET25_TAG13,
EM_BRP_EXIT_MANY_NT_TK_TARGET25_TAG13,
EM_BRP_EXIT_MANY_NT_TK_IMP_TARGET25_TAG13,
EM_BRP_SPTK_FEW_NT_NT_TARGET25_TAG13,
EM_BRP_SPTK_FEW_NT_NT_IMP_TARGET25_TAG13,
EM_BRP_SPTK_MANY_NT_NT_TARGET25_TAG13,
EM_BRP_SPTK_MANY_NT_NT_IMP_TARGET25_TAG13,
EM_BRP_LOOP_FEW_NT_NT_TARGET25_TAG13,
EM_BRP_LOOP_FEW_NT_NT_IMP_TARGET25_TAG13,
EM_BRP_LOOP_MANY_NT_NT_TARGET25_TAG13,
EM_BRP_LOOP_MANY_NT_NT_IMP_TARGET25_TAG13,
EM_BRP_DPTK_FEW_NT_NT_TARGET25_TAG13,
EM_BRP_DPTK_FEW_NT_NT_IMP_TARGET25_TAG13,
EM_BRP_DPTK_MANY_NT_NT_TARGET25_TAG13,
EM_BRP_DPTK_MANY_NT_NT_IMP_TARGET25_TAG13,
EM_BRP_EXIT_FEW_NT_NT_TARGET25_TAG13,
EM_BRP_EXIT_FEW_NT_NT_IMP_TARGET25_TAG13,
EM_BRP_EXIT_MANY_NT_NT_TARGET25_TAG13,
EM_BRP_EXIT_MANY_NT_NT_IMP_TARGET25_TAG13,
EM_BRP_SPTK_FEW_DC_DC_B2_TAG13,
EM_BRP_SPTK_FEW_DC_DC_IMP_B2_TAG13,
EM_BRP_SPTK_MANY_DC_DC_B2_TAG13,
EM_BRP_SPTK_MANY_DC_DC_IMP_B2_TAG13,
EM_BRP_DPTK_FEW_DC_DC_B2_TAG13,
EM_BRP_DPTK_FEW_DC_DC_IMP_B2_TAG13,
EM_BRP_DPTK_MANY_DC_DC_B2_TAG13,
EM_BRP_DPTK_MANY_DC_DC_IMP_B2_TAG13,
EM_BRP_SPTK_FEW_DC_NT_B2_TAG13,
EM_BRP_SPTK_FEW_DC_NT_IMP_B2_TAG13,
EM_BRP_SPTK_MANY_DC_NT_B2_TAG13,
EM_BRP_SPTK_MANY_DC_NT_IMP_B2_TAG13,
EM_BRP_DPTK_FEW_DC_NT_B2_TAG13,
EM_BRP_DPTK_FEW_DC_NT_IMP_B2_TAG13,
EM_BRP_DPTK_MANY_DC_NT_B2_TAG13,
EM_BRP_DPTK_MANY_DC_NT_IMP_B2_TAG13,
EM_BRP_SPTK_FEW_TK_DC_B2_TAG13,
EM_BRP_SPTK_FEW_TK_DC_IMP_B2_TAG13,
EM_BRP_SPTK_MANY_TK_DC_B2_TAG13,
EM_BRP_SPTK_MANY_TK_DC_IMP_B2_TAG13,
EM_BRP_DPTK_FEW_TK_DC_B2_TAG13,
EM_BRP_DPTK_FEW_TK_DC_IMP_B2_TAG13,
EM_BRP_DPTK_MANY_TK_DC_B2_TAG13,
EM_BRP_DPTK_MANY_TK_DC_IMP_B2_TAG13,
EM_BRP_SPTK_FEW_TK_TK_B2_TAG13,
EM_BRP_SPTK_FEW_TK_TK_IMP_B2_TAG13,
EM_BRP_SPTK_MANY_TK_TK_B2_TAG13,
EM_BRP_SPTK_MANY_TK_TK_IMP_B2_TAG13,
EM_BRP_DPTK_FEW_TK_TK_B2_TAG13,
EM_BRP_DPTK_FEW_TK_TK_IMP_B2_TAG13,
EM_BRP_DPTK_MANY_TK_TK_B2_TAG13,
EM_BRP_DPTK_MANY_TK_TK_IMP_B2_TAG13,
EM_BRP_SPTK_FEW_TK_NT_B2_TAG13,
EM_BRP_SPTK_FEW_TK_NT_IMP_B2_TAG13,
EM_BRP_SPTK_MANY_TK_NT_B2_TAG13,
EM_BRP_SPTK_MANY_TK_NT_IMP_B2_TAG13,
EM_BRP_DPTK_FEW_TK_NT_B2_TAG13,
EM_BRP_DPTK_FEW_TK_NT_IMP_B2_TAG13,
EM_BRP_DPTK_MANY_TK_NT_B2_TAG13,
EM_BRP_DPTK_MANY_TK_NT_IMP_B2_TAG13,
EM_BRP_SPTK_FEW_NT_DC_B2_TAG13,
EM_BRP_SPTK_FEW_NT_DC_IMP_B2_TAG13,
EM_BRP_SPTK_MANY_NT_DC_B2_TAG13,
EM_BRP_SPTK_MANY_NT_DC_IMP_B2_TAG13,
EM_BRP_DPTK_FEW_NT_DC_B2_TAG13,
EM_BRP_DPTK_FEW_NT_DC_IMP_B2_TAG13,
EM_BRP_DPTK_MANY_NT_DC_B2_TAG13,
EM_BRP_DPTK_MANY_NT_DC_IMP_B2_TAG13,
EM_BRP_SPTK_FEW_NT_TK_B2_TAG13,
EM_BRP_SPTK_FEW_NT_TK_IMP_B2_TAG13,
EM_BRP_SPTK_MANY_NT_TK_B2_TAG13,
EM_BRP_SPTK_MANY_NT_TK_IMP_B2_TAG13,
EM_BRP_DPTK_FEW_NT_TK_B2_TAG13,
EM_BRP_DPTK_FEW_NT_TK_IMP_B2_TAG13,
EM_BRP_DPTK_MANY_NT_TK_B2_TAG13,
EM_BRP_DPTK_MANY_NT_TK_IMP_B2_TAG13,
EM_BRP_SPTK_FEW_NT_NT_B2_TAG13,
EM_BRP_SPTK_FEW_NT_NT_IMP_B2_TAG13,
EM_BRP_SPTK_MANY_NT_NT_B2_TAG13,
EM_BRP_SPTK_MANY_NT_NT_IMP_B2_TAG13,
EM_BRP_DPTK_FEW_NT_NT_B2_TAG13,
EM_BRP_DPTK_FEW_NT_NT_IMP_B2_TAG13,
EM_BRP_DPTK_MANY_NT_NT_B2_TAG13,
EM_BRP_DPTK_MANY_NT_NT_IMP_B2_TAG13,
EM_BRP_RET_SPTK_FEW_DC_DC_B2_TAG13,
EM_BRP_RET_SPTK_FEW_DC_DC_IMP_B2_TAG13,
EM_BRP_RET_SPTK_MANY_DC_DC_B2_TAG13,
EM_BRP_RET_SPTK_MANY_DC_DC_IMP_B2_TAG13,
EM_BRP_RET_DPTK_FEW_DC_DC_B2_TAG13,
EM_BRP_RET_DPTK_FEW_DC_DC_IMP_B2_TAG13,
EM_BRP_RET_DPTK_MANY_DC_DC_B2_TAG13,
EM_BRP_RET_DPTK_MANY_DC_DC_IMP_B2_TAG13,
EM_BRP_RET_SPTK_FEW_DC_NT_B2_TAG13,
EM_BRP_RET_SPTK_FEW_DC_NT_IMP_B2_TAG13,
EM_BRP_RET_SPTK_MANY_DC_NT_B2_TAG13,
EM_BRP_RET_SPTK_MANY_DC_NT_IMP_B2_TAG13,
EM_BRP_RET_DPTK_FEW_DC_NT_B2_TAG13,
EM_BRP_RET_DPTK_FEW_DC_NT_IMP_B2_TAG13,
EM_BRP_RET_DPTK_MANY_DC_NT_B2_TAG13,
EM_BRP_RET_DPTK_MANY_DC_NT_IMP_B2_TAG13,
EM_BRP_RET_SPTK_FEW_TK_DC_B2_TAG13,
EM_BRP_RET_SPTK_FEW_TK_DC_IMP_B2_TAG13,
EM_BRP_RET_SPTK_MANY_TK_DC_B2_TAG13,
EM_BRP_RET_SPTK_MANY_TK_DC_IMP_B2_TAG13,
EM_BRP_RET_DPTK_FEW_TK_DC_B2_TAG13,
EM_BRP_RET_DPTK_FEW_TK_DC_IMP_B2_TAG13,
EM_BRP_RET_DPTK_MANY_TK_DC_B2_TAG13,
EM_BRP_RET_DPTK_MANY_TK_DC_IMP_B2_TAG13,
EM_BRP_RET_SPTK_FEW_TK_TK_B2_TAG13,
EM_BRP_RET_SPTK_FEW_TK_TK_IMP_B2_TAG13,
EM_BRP_RET_SPTK_MANY_TK_TK_B2_TAG13,
EM_BRP_RET_SPTK_MANY_TK_TK_IMP_B2_TAG13,
EM_BRP_RET_DPTK_FEW_TK_TK_B2_TAG13,
EM_BRP_RET_DPTK_FEW_TK_TK_IMP_B2_TAG13,
EM_BRP_RET_DPTK_MANY_TK_TK_B2_TAG13,
EM_BRP_RET_DPTK_MANY_TK_TK_IMP_B2_TAG13,
EM_BRP_RET_SPTK_FEW_TK_NT_B2_TAG13,
EM_BRP_RET_SPTK_FEW_TK_NT_IMP_B2_TAG13,
EM_BRP_RET_SPTK_MANY_TK_NT_B2_TAG13,
EM_BRP_RET_SPTK_MANY_TK_NT_IMP_B2_TAG13,
EM_BRP_RET_DPTK_FEW_TK_NT_B2_TAG13,
EM_BRP_RET_DPTK_FEW_TK_NT_IMP_B2_TAG13,
EM_BRP_RET_DPTK_MANY_TK_NT_B2_TAG13,
EM_BRP_RET_DPTK_MANY_TK_NT_IMP_B2_TAG13,
EM_BRP_RET_SPTK_FEW_NT_DC_B2_TAG13,
EM_BRP_RET_SPTK_FEW_NT_DC_IMP_B2_TAG13,
EM_BRP_RET_SPTK_MANY_NT_DC_B2_TAG13,
EM_BRP_RET_SPTK_MANY_NT_DC_IMP_B2_TAG13,
EM_BRP_RET_DPTK_FEW_NT_DC_B2_TAG13,
EM_BRP_RET_DPTK_FEW_NT_DC_IMP_B2_TAG13,
EM_BRP_RET_DPTK_MANY_NT_DC_B2_TAG13,
EM_BRP_RET_DPTK_MANY_NT_DC_IMP_B2_TAG13,
EM_BRP_RET_SPTK_FEW_NT_TK_B2_TAG13,
EM_BRP_RET_SPTK_FEW_NT_TK_IMP_B2_TAG13,
EM_BRP_RET_SPTK_MANY_NT_TK_B2_TAG13,
EM_BRP_RET_SPTK_MANY_NT_TK_IMP_B2_TAG13,
EM_BRP_RET_DPTK_FEW_NT_TK_B2_TAG13,
EM_BRP_RET_DPTK_FEW_NT_TK_IMP_B2_TAG13,
EM_BRP_RET_DPTK_MANY_NT_TK_B2_TAG13,
EM_BRP_RET_DPTK_MANY_NT_TK_IMP_B2_TAG13,
EM_BRP_RET_SPTK_FEW_NT_NT_B2_TAG13,
EM_BRP_RET_SPTK_FEW_NT_NT_IMP_B2_TAG13,
EM_BRP_RET_SPTK_MANY_NT_NT_B2_TAG13,
EM_BRP_RET_SPTK_MANY_NT_NT_IMP_B2_TAG13,
EM_BRP_RET_DPTK_FEW_NT_NT_B2_TAG13,
EM_BRP_RET_DPTK_FEW_NT_NT_IMP_B2_TAG13,
EM_BRP_RET_DPTK_MANY_NT_NT_B2_TAG13,
EM_BRP_RET_DPTK_MANY_NT_NT_IMP_B2_TAG13,
EM_COVER,
EM_CLRRRB,
EM_CLRRRB_PR,
EM_RFI,
EM_BSW_0,
EM_BSW_1,
EM_EPC,
EM_BREAK_B_IMM21,
EM_NOP_B_IMM21,
EM_FMA_S0_F1_F3_F4_F2,
EM_FMA_S1_F1_F3_F4_F2,
EM_FMA_S2_F1_F3_F4_F2,
EM_FMA_S3_F1_F3_F4_F2,
EM_FMA_S_S0_F1_F3_F4_F2,
EM_FMA_S_S1_F1_F3_F4_F2,
EM_FMA_S_S2_F1_F3_F4_F2,
EM_FMA_S_S3_F1_F3_F4_F2,
EM_FMA_D_S0_F1_F3_F4_F2,
EM_FMA_D_S1_F1_F3_F4_F2,
EM_FMA_D_S2_F1_F3_F4_F2,
EM_FMA_D_S3_F1_F3_F4_F2,
EM_FPMA_S0_F1_F3_F4_F2,
EM_FPMA_S1_F1_F3_F4_F2,
EM_FPMA_S2_F1_F3_F4_F2,
EM_FPMA_S3_F1_F3_F4_F2,
EM_FMS_S0_F1_F3_F4_F2,
EM_FMS_S1_F1_F3_F4_F2,
EM_FMS_S2_F1_F3_F4_F2,
EM_FMS_S3_F1_F3_F4_F2,
EM_FMS_S_S0_F1_F3_F4_F2,
EM_FMS_S_S1_F1_F3_F4_F2,
EM_FMS_S_S2_F1_F3_F4_F2,
EM_FMS_S_S3_F1_F3_F4_F2,
EM_FMS_D_S0_F1_F3_F4_F2,
EM_FMS_D_S1_F1_F3_F4_F2,
EM_FMS_D_S2_F1_F3_F4_F2,
EM_FMS_D_S3_F1_F3_F4_F2,
EM_FPMS_S0_F1_F3_F4_F2,
EM_FPMS_S1_F1_F3_F4_F2,
EM_FPMS_S2_F1_F3_F4_F2,
EM_FPMS_S3_F1_F3_F4_F2,
EM_FNMA_S0_F1_F3_F4_F2,
EM_FNMA_S1_F1_F3_F4_F2,
EM_FNMA_S2_F1_F3_F4_F2,
EM_FNMA_S3_F1_F3_F4_F2,
EM_FNMA_S_S0_F1_F3_F4_F2,
EM_FNMA_S_S1_F1_F3_F4_F2,
EM_FNMA_S_S2_F1_F3_F4_F2,
EM_FNMA_S_S3_F1_F3_F4_F2,
EM_FNMA_D_S0_F1_F3_F4_F2,
EM_FNMA_D_S1_F1_F3_F4_F2,
EM_FNMA_D_S2_F1_F3_F4_F2,
EM_FNMA_D_S3_F1_F3_F4_F2,
EM_FPNMA_S0_F1_F3_F4_F2,
EM_FPNMA_S1_F1_F3_F4_F2,
EM_FPNMA_S2_F1_F3_F4_F2,
EM_FPNMA_S3_F1_F3_F4_F2,
EM_XMA_L_F1_F3_F4_F2,
EM_XMA_H_F1_F3_F4_F2,
EM_XMA_HU_F1_F3_F4_F2,
EM_FSELECT_F1_F3_F4_F2,
EM_FCMP_EQ_S0_P1_P2_F2_F3,
EM_FCMP_EQ_S1_P1_P2_F2_F3,
EM_FCMP_EQ_S2_P1_P2_F2_F3,
EM_FCMP_EQ_S3_P1_P2_F2_F3,
EM_FCMP_LT_S0_P1_P2_F2_F3,
EM_FCMP_LT_S1_P1_P2_F2_F3,
EM_FCMP_LT_S2_P1_P2_F2_F3,
EM_FCMP_LT_S3_P1_P2_F2_F3,
EM_FCMP_LE_S0_P1_P2_F2_F3,
EM_FCMP_LE_S1_P1_P2_F2_F3,
EM_FCMP_LE_S2_P1_P2_F2_F3,
EM_FCMP_LE_S3_P1_P2_F2_F3,
EM_FCMP_UNORD_S0_P1_P2_F2_F3,
EM_FCMP_UNORD_S1_P1_P2_F2_F3,
EM_FCMP_UNORD_S2_P1_P2_F2_F3,
EM_FCMP_UNORD_S3_P1_P2_F2_F3,
EM_FCMP_EQ_UNC_S0_P1_P2_F2_F3,
EM_FCMP_EQ_UNC_S1_P1_P2_F2_F3,
EM_FCMP_EQ_UNC_S2_P1_P2_F2_F3,
EM_FCMP_EQ_UNC_S3_P1_P2_F2_F3,
EM_FCMP_LT_UNC_S0_P1_P2_F2_F3,
EM_FCMP_LT_UNC_S1_P1_P2_F2_F3,
EM_FCMP_LT_UNC_S2_P1_P2_F2_F3,
EM_FCMP_LT_UNC_S3_P1_P2_F2_F3,
EM_FCMP_LE_UNC_S0_P1_P2_F2_F3,
EM_FCMP_LE_UNC_S1_P1_P2_F2_F3,
EM_FCMP_LE_UNC_S2_P1_P2_F2_F3,
EM_FCMP_LE_UNC_S3_P1_P2_F2_F3,
EM_FCMP_UNORD_UNC_S0_P1_P2_F2_F3,
EM_FCMP_UNORD_UNC_S1_P1_P2_F2_F3,
EM_FCMP_UNORD_UNC_S2_P1_P2_F2_F3,
EM_FCMP_UNORD_UNC_S3_P1_P2_F2_F3,
EM_FCLASS_M_P1_P2_F2_FCLASS9,
EM_FCLASS_M_UNC_P1_P2_F2_FCLASS9,
EM_FRCPA_S0_F1_P2_F2_F3,
EM_FRCPA_S1_F1_P2_F2_F3,
EM_FRCPA_S2_F1_P2_F2_F3,
EM_FRCPA_S3_F1_P2_F2_F3,
EM_FPRCPA_S0_F1_P2_F2_F3,
EM_FPRCPA_S1_F1_P2_F2_F3,
EM_FPRCPA_S2_F1_P2_F2_F3,
EM_FPRCPA_S3_F1_P2_F2_F3,
EM_FRSQRTA_S0_F1_P2_F3,
EM_FRSQRTA_S1_F1_P2_F3,
EM_FRSQRTA_S2_F1_P2_F3,
EM_FRSQRTA_S3_F1_P2_F3,
EM_FPRSQRTA_S0_F1_P2_F3,
EM_FPRSQRTA_S1_F1_P2_F3,
EM_FPRSQRTA_S2_F1_P2_F3,
EM_FPRSQRTA_S3_F1_P2_F3,
EM_FMIN_S0_F1_F2_F3,
EM_FMIN_S1_F1_F2_F3,
EM_FMIN_S2_F1_F2_F3,
EM_FMIN_S3_F1_F2_F3,
EM_FMAX_S0_F1_F2_F3,
EM_FMAX_S1_F1_F2_F3,
EM_FMAX_S2_F1_F2_F3,
EM_FMAX_S3_F1_F2_F3,
EM_FAMIN_S0_F1_F2_F3,
EM_FAMIN_S1_F1_F2_F3,
EM_FAMIN_S2_F1_F2_F3,
EM_FAMIN_S3_F1_F2_F3,
EM_FAMAX_S0_F1_F2_F3,
EM_FAMAX_S1_F1_F2_F3,
EM_FAMAX_S2_F1_F2_F3,
EM_FAMAX_S3_F1_F2_F3,
EM_FPMIN_S0_F1_F2_F3,
EM_FPMIN_S1_F1_F2_F3,
EM_FPMIN_S2_F1_F2_F3,
EM_FPMIN_S3_F1_F2_F3,
EM_FPMAX_S0_F1_F2_F3,
EM_FPMAX_S1_F1_F2_F3,
EM_FPMAX_S2_F1_F2_F3,
EM_FPMAX_S3_F1_F2_F3,
EM_FPAMIN_S0_F1_F2_F3,
EM_FPAMIN_S1_F1_F2_F3,
EM_FPAMIN_S2_F1_F2_F3,
EM_FPAMIN_S3_F1_F2_F3,
EM_FPAMAX_S0_F1_F2_F3,
EM_FPAMAX_S1_F1_F2_F3,
EM_FPAMAX_S2_F1_F2_F3,
EM_FPAMAX_S3_F1_F2_F3,
EM_FPCMP_EQ_S0_F1_F2_F3,
EM_FPCMP_EQ_S1_F1_F2_F3,
EM_FPCMP_EQ_S2_F1_F2_F3,
EM_FPCMP_EQ_S3_F1_F2_F3,
EM_FPCMP_LT_S0_F1_F2_F3,
EM_FPCMP_LT_S1_F1_F2_F3,
EM_FPCMP_LT_S2_F1_F2_F3,
EM_FPCMP_LT_S3_F1_F2_F3,
EM_FPCMP_LE_S0_F1_F2_F3,
EM_FPCMP_LE_S1_F1_F2_F3,
EM_FPCMP_LE_S2_F1_F2_F3,
EM_FPCMP_LE_S3_F1_F2_F3,
EM_FPCMP_UNORD_S0_F1_F2_F3,
EM_FPCMP_UNORD_S1_F1_F2_F3,
EM_FPCMP_UNORD_S2_F1_F2_F3,
EM_FPCMP_UNORD_S3_F1_F2_F3,
EM_FPCMP_NEQ_S0_F1_F2_F3,
EM_FPCMP_NEQ_S1_F1_F2_F3,
EM_FPCMP_NEQ_S2_F1_F2_F3,
EM_FPCMP_NEQ_S3_F1_F2_F3,
EM_FPCMP_NLT_S0_F1_F2_F3,
EM_FPCMP_NLT_S1_F1_F2_F3,
EM_FPCMP_NLT_S2_F1_F2_F3,
EM_FPCMP_NLT_S3_F1_F2_F3,
EM_FPCMP_NLE_S0_F1_F2_F3,
EM_FPCMP_NLE_S1_F1_F2_F3,
EM_FPCMP_NLE_S2_F1_F2_F3,
EM_FPCMP_NLE_S3_F1_F2_F3,
EM_FPCMP_ORD_S0_F1_F2_F3,
EM_FPCMP_ORD_S1_F1_F2_F3,
EM_FPCMP_ORD_S2_F1_F2_F3,
EM_FPCMP_ORD_S3_F1_F2_F3,
EM_FMERGE_S_F1_F2_F3,
EM_FMERGE_NS_F1_F2_F3,
EM_FMERGE_SE_F1_F2_F3,
EM_FMIX_LR_F1_F2_F3,
EM_FMIX_R_F1_F2_F3,
EM_FMIX_L_F1_F2_F3,
EM_FSXT_R_F1_F2_F3,
EM_FSXT_L_F1_F2_F3,
EM_FPACK_F1_F2_F3,
EM_FSWAP_F1_F2_F3,
EM_FSWAP_NL_F1_F2_F3,
EM_FSWAP_NR_F1_F2_F3,
EM_FAND_F1_F2_F3,
EM_FANDCM_F1_F2_F3,
EM_FOR_F1_F2_F3,
EM_FXOR_F1_F2_F3,
EM_FPMERGE_S_F1_F2_F3,
EM_FPMERGE_NS_F1_F2_F3,
EM_FPMERGE_SE_F1_F2_F3,
EM_FCVT_FX_S0_F1_F2,
EM_FCVT_FX_S1_F1_F2,
EM_FCVT_FX_S2_F1_F2,
EM_FCVT_FX_S3_F1_F2,
EM_FCVT_FXU_S0_F1_F2,
EM_FCVT_FXU_S1_F1_F2,
EM_FCVT_FXU_S2_F1_F2,
EM_FCVT_FXU_S3_F1_F2,
EM_FCVT_FX_TRUNC_S0_F1_F2,
EM_FCVT_FX_TRUNC_S1_F1_F2,
EM_FCVT_FX_TRUNC_S2_F1_F2,
EM_FCVT_FX_TRUNC_S3_F1_F2,
EM_FCVT_FXU_TRUNC_S0_F1_F2,
EM_FCVT_FXU_TRUNC_S1_F1_F2,
EM_FCVT_FXU_TRUNC_S2_F1_F2,
EM_FCVT_FXU_TRUNC_S3_F1_F2,
EM_FPCVT_FX_S0_F1_F2,
EM_FPCVT_FX_S1_F1_F2,
EM_FPCVT_FX_S2_F1_F2,
EM_FPCVT_FX_S3_F1_F2,
EM_FPCVT_FXU_S0_F1_F2,
EM_FPCVT_FXU_S1_F1_F2,
EM_FPCVT_FXU_S2_F1_F2,
EM_FPCVT_FXU_S3_F1_F2,
EM_FPCVT_FX_TRUNC_S0_F1_F2,
EM_FPCVT_FX_TRUNC_S1_F1_F2,
EM_FPCVT_FX_TRUNC_S2_F1_F2,
EM_FPCVT_FX_TRUNC_S3_F1_F2,
EM_FPCVT_FXU_TRUNC_S0_F1_F2,
EM_FPCVT_FXU_TRUNC_S1_F1_F2,
EM_FPCVT_FXU_TRUNC_S2_F1_F2,
EM_FPCVT_FXU_TRUNC_S3_F1_F2,
EM_FCVT_XF_F1_F2,
EM_FSETC_S0_AMASK7_OMASK7,
EM_FSETC_S1_AMASK7_OMASK7,
EM_FSETC_S2_AMASK7_OMASK7,
EM_FSETC_S3_AMASK7_OMASK7,
EM_FCLRF_S0,
EM_FCLRF_S1,
EM_FCLRF_S2,
EM_FCLRF_S3,
EM_FCHKF_S0_TARGET25,
EM_FCHKF_S1_TARGET25,
EM_FCHKF_S2_TARGET25,
EM_FCHKF_S3_TARGET25,
EM_BREAK_F_IMM21,
EM_NOP_F_IMM21,
EM_EMDB_INST_LAST = EM_NOP_F_IMM21,
EM_HALT_R3,
EM_RFI_X,
EM_BR_IA_SPECIAL_B2,
EM_INST_LAST
} Inst_id_t2;

#endif /* _INST_ID_H */


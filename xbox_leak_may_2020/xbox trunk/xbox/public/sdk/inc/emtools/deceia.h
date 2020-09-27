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

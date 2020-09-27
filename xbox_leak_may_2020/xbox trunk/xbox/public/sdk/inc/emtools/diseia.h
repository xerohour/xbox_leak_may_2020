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





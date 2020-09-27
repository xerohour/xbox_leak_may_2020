/** M6502: portable 6502 emulator ****************************/
/**                                                         **/
/**                          Codes.h                        **/
/**                                                         **/
/** This file contains implementation for the main table of **/
/** 6502 commands. It is included from m6502.c.             **/
/**                                                         **/
/** Copyright (C) Marat Fayzullin 1996                      **/
/**               Alex Krasivsky  1996                      **/
/** Modified      BERO            1998                      **/
/**     You are not allowed to distribute this software     **/
/**     commercially. Please, notify me, if you make any    **/
/**     changes to this file.                               **/
/*************************************************************/
/*	
	Undocumented opcodes aren't implemented(correctly) due to
	conflicting information. Oh well.	
*/

case 0x2B:
case 0x0B: _ICounta+=2;MR_Im(I);M_AND(I);_P&=0xFE;_P|=_A>>7;break;
case 0x8B: _ICounta+=2;MR_Im(I);M_AND(I);M_AND(_X);break;
case 0x6B: _ICounta+=2;MR_Im(I);M_AND(I);M_ROR(_A);_P&=0xFE;_P|=(_A>>6)&1;break;

case 0x9F: _ICounta+=5;MW_Ay(_A&_X&1);break;	      /* SHA   $nnnn,Y */
case 0x93: _ICounta+=6;MR_Iy(I);Wr6502(I,_A&_X&1);break;
case 0x9B: _ICounta+=5;_S=_A&_X;MW_Ay(_A&_X&1);break; /* SHS   $nnnn,Y */
case 0x9E: _ICounta+=5;MW_Ay(_X&1);break;
case 0x9C: _ICounta+=5;MW_Ay(_Y&1);break;


case 0x10: _ICounta+=2;if(_P&N_FLAG) _PC_++; else { M_JR; } break; /* BPL * REL */
case 0x30: _ICounta+=2;if(_P&N_FLAG) { M_JR; } else _PC_++; break; /* BMI * REL */
case 0xD0: _ICounta+=2;if(_P&Z_FLAG) _PC_++; else { M_JR; } break; /* BNE * REL */
case 0xF0: _ICounta+=2;if(_P&Z_FLAG) { M_JR; } else _PC_++; break; /* BEQ * REL */
case 0x90: _ICounta+=2;if(_P&C_FLAG) _PC_++; else { M_JR; } break; /* BCC * REL */
case 0xB0: _ICounta+=2;if(_P&C_FLAG) { M_JR; } else _PC_++; break; /* BCS * REL */
case 0x50: _ICounta+=2;if(_P&V_FLAG) _PC_++; else { M_JR; } break; /* BVC * REL */
case 0x70: _ICounta+=2;if(_P&V_FLAG) { M_JR; } else _PC_++; break; /* BVS * REL */

/* RTI */
case 0x40:
  _ICounta+=6;
  M_POP(_P);_P|=R_FLAG;M_POP(_PC.B.l);M_POP(_PC.B.h);
  break;

/* RTS */
case 0x60:
  _ICounta+=6;
  M_POP(_PC.B.l);M_POP(_PC.B.h);
  _PC_++;break;

/* JSR $ssss ABS */
case 0x20:
  _ICounta+=6;
  K.B.l=Op6502(_PC_);_PC_++;
  K.B.h=Op6502(_PC_);
  M_PUSH(_PC.B.h);
  M_PUSH(_PC.B.l);
  _PC=K;break;

/* JMP $ssss ABS */
case 0x4C: 
	  K.B.l=Op6502(_PC_);_PC_++;
	  K.B.h=Op6502(_PC_);
          _ICounta+=3;
          _PC=K;
          break;

/* JMP ($ssss) ABDINDIR */
case 0x6C:
  _ICounta+=5;
  K.B.l=Op6502(_PC_);_PC_++;
  K.B.h=Op6502(_PC_);
  _PC.B.l=Rd6502(K.W);
  K.W++;
  _PC.B.h=Rd6502(K.W);
  break;


/* BRK */
case 0x00:
  _ICounta+=7;
  _PC_++;
  M_PUSH(_PC.B.h);M_PUSH(_PC.B.l);
  M_PUSH(_P|B_FLAG);
  _P=(_P|I_FLAG)&~D_FLAG;
  _PC.B.l=Op6502(0xFFFE);
  _PC.B.h=Op6502(0xFFFF);
  break;

/* CLI */
case 0x58:
  _ICounta+=2;
  _P&=~I_FLAG;
  break;

/* PLP */
case 0x28:
  _ICounta+=4;
  M_POP(I);
  _P=I|R_FLAG;
  break;

case 0x08: _ICounta+=3;M_PUSH(_P);break;               /* PHP */
case 0x18: _ICounta+=2;_P&=~C_FLAG;break;              /* CLC */
case 0xB8: _ICounta+=2;_P&=~V_FLAG;break;              /* CLV */
case 0xD8: _ICounta+=2;_P&=~D_FLAG;break;              /* CLD */
case 0x38: _ICounta+=2;_P|=C_FLAG;break;               /* SEC */
case 0xF8: _ICounta+=2;_P|=D_FLAG;break;               /* SED */
case 0x78: _ICounta+=2;_P|=I_FLAG;break;               /* SEI */
case 0x48: _ICounta+=3;M_PUSH(_A);break;               /* PHA */
case 0x68: _ICounta+=4;M_POP(_A);M_FL(_A);break;     /* PLA */
case 0x98: _ICounta+=2;_A=_Y;M_FL(_A);break;       /* TYA */
case 0xA8: _ICounta+=2;_Y=_A;M_FL(_Y);break;       /* TAY */
case 0xC8: _ICounta+=2;_Y++;M_FL(_Y);break;          /* INY */
case 0x88: _ICounta+=2;_Y--;M_FL(_Y);break;          /* DEY */
case 0x8A: _ICounta+=2;_A=_X;M_FL(_A);break;       /* TXA */
case 0xAA: _ICounta+=2;_X=_A;M_FL(_X);break;       /* TAX */
case 0xE8: _ICounta+=2;_X++;M_FL(_X);break;          /* INX */
case 0xCA: _ICounta+=2;_X--;M_FL(_X);break;          /* DEX */
case 0xEA: _ICounta+=2;break;                            /* NOP */
case 0x9A: _ICounta+=2;_S=_X;break;                  /* TXS */
case 0xBA: _ICounta+=2;_X=_S;M_FL(M.X);break;                  /* TSX */

case 0x24: _ICounta+=3;MR_Zp(I);M_BIT(I);break;       /* BIT $ss ZP */
case 0x2C: _ICounta+=4;MR_Ab(I);M_BIT(I);break;       /* BIT $ssss ABS */

case 0x05: _ICounta+=3;MR_Zp(I);M_ORA(I);break;       /* ORA $ss ZP */
case 0x06: _ICounta+=5;MM_Zp(M_ASL);break;            /* ASL $ss ZP */
case 0x25: _ICounta+=3;MR_Zp(I);M_AND(I);break;       /* AND $ss ZP */
case 0x26: _ICounta+=5;MM_Zp(M_ROL);break;            /* ROL $ss ZP */
case 0x45: _ICounta+=3;MR_Zp(I);M_EOR(I);break;       /* EOR $ss ZP */
case 0x46: _ICounta+=5;MM_Zp(M_LSR);break;            /* LSR $ss ZP */
case 0x65: _ICounta+=3;MR_Zp(I);M_ADC(I);break;       /* ADC $ss ZP */
case 0x66: _ICounta+=5;MM_Zp(M_ROR);break;            /* ROR $ss ZP */
case 0x84: _ICounta+=3;MW_Zp(_Y);break;             /* STY $ss ZP */
case 0x85: _ICounta+=3;MW_Zp(_A);break;             /* STA $ss ZP */
case 0x86: _ICounta+=3;MW_Zp(_X);break;             /* STX $ss ZP */
case 0xA4: _ICounta+=3;MR_Zp(_Y);M_FL(_Y);break;  /* LDY $ss ZP */
case 0xA5: _ICounta+=3;MR_Zp(_A);M_FL(_A);break;  /* LDA $ss ZP */
case 0xA6: _ICounta+=3;MR_Zp(_X);M_FL(_X);break;  /* LDX $ss ZP */
case 0xC4: _ICounta+=3;MR_Zp(I);M_CMP(_Y,I);break;  /* CPY $ss ZP */
case 0xC5: _ICounta+=3;MR_Zp(I);M_CMP(_A,I);break;  /* CMP $ss ZP */
case 0xC6: _ICounta+=5;MM_Zp(M_DEC);break;            /* DEC $ss ZP */
case 0xE4: _ICounta+=3;MR_Zp(I);M_CMP(_X,I);break;  /* CPX $ss ZP */

case 0xE5: _ICounta+=3;MR_Zp(I);M_SBC(I);break;       /* SBC $ss ZP */
case 0xE6: _ICounta+=5;MM_Zp(M_INC);break;            /* INC $ss ZP */

case 0x0D: _ICounta+=4;MR_Ab(I);M_ORA(I);break;       /* ORA $ssss ABS */
case 0x0E: _ICounta+=6;MM_Ab(M_ASL);break;            /* ASL $ssss ABS */
case 0x2D: _ICounta+=4;MR_Ab(I);M_AND(I);break;       /* AND $ssss ABS */
case 0x2E: _ICounta+=6;MM_Ab(M_ROL);break;            /* ROL $ssss ABS */
case 0x4D: _ICounta+=4;MR_Ab(I);M_EOR(I);break;       /* EOR $ssss ABS */
case 0x4E: _ICounta+=6;MM_Ab(M_LSR);break;            /* LSR $ssss ABS */
case 0x6D: _ICounta+=4;MR_Ab(I);M_ADC(I);break;       /* ADC $ssss ABS */
case 0x6E: _ICounta+=6;MM_Ab(M_ROR);break;            /* ROR $ssss ABS */

case 0x8C: _ICounta+=4;MW_Ab(_Y);break;             /* STY $ssss ABS */
case 0x8D: _ICounta+=4;MW_Ab(_A);break;             /* STA $ssss ABS */
case 0x8E: _ICounta+=4;MW_Ab(_X);break;             /* STX $ssss ABS */

case 0xAC: _ICounta+=4;MR_Ab(_Y);M_FL(_Y);break;  /* LDY $ssss ABS */
case 0xAD: _ICounta+=4;MR_Ab(_A);M_FL(_A);break;  /* LDA $ssss ABS */
case 0xAE: _ICounta+=4;MR_Ab(_X);M_FL(_X);break;  /* LDX $ssss ABS */


case 0xCC: _ICounta+=4;MR_Ab(I);M_CMP(_Y,I);break;  /* CPY $ssss ABS */
case 0xCD: _ICounta+=4;MR_Ab(I);M_CMP(_A,I);break;  /* CMP $ssss ABS */
case 0xCE: _ICounta+=6;MM_Ab(M_DEC);break;            /* DEC $ssss ABS */
case 0xEC: _ICounta+=4;MR_Ab(I);M_CMP(_X,I);break;  /* CPX $ssss ABS */
case 0xED: _ICounta+=4;MR_Ab(I);M_SBC(I);break;       /* SBC $ssss ABS */
case 0xEE: _ICounta+=6;MM_Ab(M_INC);break;            /* INC $ssss ABS */

case 0x09: _ICounta+=2;MR_Im(I);M_ORA(I);break;       /* ORA #$ss IMM */
case 0x29: _ICounta+=2;MR_Im(I);M_AND(I);break;       /* AND #$ss IMM */
case 0x49: _ICounta+=2;MR_Im(I);M_EOR(I);break;       /* EOR #$ss IMM */
case 0x69: _ICounta+=2;MR_Im(I);M_ADC(I);break;       /* ADC #$ss IMM */
case 0xA0: _ICounta+=2;MR_Im(_Y);M_FL(_Y);break;  /* LDY #$ss IMM */
case 0xA2: _ICounta+=2;MR_Im(_X);M_FL(_X);break;  /* LDX #$ss IMM */
case 0xA9: _ICounta+=2;MR_Im(_A);M_FL(_A);break;  /* LDA #$ss IMM */
case 0xC0: _ICounta+=2;MR_Im(I);M_CMP(_Y,I);break;  /* CPY #$ss IMM */
case 0xC9: _ICounta+=2;MR_Im(I);M_CMP(_A,I);break;  /* CMP #$ss IMM */
case 0xE0: _ICounta+=2;MR_Im(I);M_CMP(_X,I);break;  /* CPX #$ss IMM */
case 0xE9: _ICounta+=2;MR_Im(I);M_SBC(I);break;       /* SBC #$ss IMM */

case 0x15: _ICounta+=4;MR_Zx(I);M_ORA(I);break;       /* ORA $ss,x ZP,x */
case 0x16: _ICounta+=6;MM_Zx(M_ASL);break;            /* ASL $ss,x ZP,x */
case 0x35: _ICounta+=4;MR_Zx(I);M_AND(I);break;       /* AND $ss,x ZP,x */
case 0x36: _ICounta+=6;MM_Zx(M_ROL);break;            /* ROL $ss,x ZP,x */
case 0x55: _ICounta+=4;MR_Zx(I);M_EOR(I);break;       /* EOR $ss,x ZP,x */
case 0x56: _ICounta+=6;MM_Zx(M_LSR);break;            /* LSR $ss,x ZP,x */
case 0x75: _ICounta+=4;MR_Zx(I);M_ADC(I);break;       /* ADC $ss,x ZP,x */
case 0x76: _ICounta+=6;MM_Zx(M_ROR);break;            /* ROR $ss,x ZP,x */
case 0x94: _ICounta+=4;MW_Zx(_Y);break;             /* STY $ss,x ZP,x */
case 0x95: _ICounta+=4;MW_Zx(_A);break;             /* STA $ss,x ZP,x */
case 0x96: _ICounta+=4;MW_Zy(_X);break;             /* STX $ss,y ZP,y */
case 0xB4: _ICounta+=4;MR_Zx(_Y);M_FL(_Y);break;  /* LDY $ss,x ZP,x */
case 0xB5: _ICounta+=4;MR_Zx(_A);M_FL(_A);break;  /* LDA $ss,x ZP,x */
case 0xB6: _ICounta+=4;MR_Zy(_X);M_FL(_X);break;  /* LDX $ss,y ZP,y */
case 0xD5: _ICounta+=4;MR_Zx(I);M_CMP(_A,I);break;  /* CMP $ss,x ZP,x */
case 0xD6: _ICounta+=6;MM_Zx(M_DEC);break;            /* DEC $ss,x ZP,x */
case 0xF5: _ICounta+=4;MR_Zx(I);M_SBC(I);break;       /* SBC $ss,x ZP,x */
case 0xF6: _ICounta+=6;MM_Zx(M_INC);break;            /* INC $ss,x ZP,x */

case 0x19: _ICounta+=4;MR_Ay(I);M_ORA(I);break;       /* ORA $ssss,y ABS,y */
case 0x1D: _ICounta+=4;MR_Ax(I);M_ORA(I);break;       /* ORA $ssss,x ABS,x */

case 0x1E: _ICounta+=7;MM_Ax(M_ASL);break;            /* ASL $ssss,x ABS,x */
case 0x39: _ICounta+=4;MR_Ay(I);M_AND(I);break;       /* AND $ssss,y ABS,y */
case 0x3D: _ICounta+=4;MR_Ax(I);M_AND(I);break;       /* AND $ssss,x ABS,x */
case 0x3E: _ICounta+=7;MM_Ax(M_ROL);break;            /* ROL $ssss,x ABS,x */
case 0x59: _ICounta+=4;MR_Ay(I);M_EOR(I);break;       /* EOR $ssss,y ABS,y */
case 0x5D: _ICounta+=4;MR_Ax(I);M_EOR(I);break;       /* EOR $ssss,x ABS,x */
case 0x5E: _ICounta+=7;MM_Ax(M_LSR);break;            /* LSR $ssss,x ABS,x */
case 0x79: _ICounta+=4;MR_Ay(I);M_ADC(I);break;       /* ADC $ssss,y ABS,y */
case 0x7D: _ICounta+=4;MR_Ax(I);M_ADC(I);break;       /* ADC $ssss,x ABS,x */
case 0x7E: _ICounta+=7;MM_Ax(M_ROR);break;            /* ROR $ssss,x ABS,x */
case 0x99: _ICounta+=5;MW_Ay(_A);break;             /* STA $ssss,y ABS,y */
case 0x9D: _ICounta+=5;MW_Ax(_A);break;             /* STA $ssss,x ABS,x */

case 0xB9: _ICounta+=4;MR_Ay(_A);M_FL(_A);break;  /* LDA $ssss,y ABS,y */
case 0xBC: _ICounta+=4;MR_Ax(_Y);M_FL(_Y);break;  /* LDY $ssss,x ABS,x */
case 0xBD: _ICounta+=4;MR_Ax(_A);M_FL(_A);break;  /* LDA $ssss,x ABS,x */
case 0xBE: _ICounta+=4;MR_Ay(_X);M_FL(_X);break;  /* LDX $ssss,y ABS,y */
case 0xD9: _ICounta+=4;MR_Ay(I);M_CMP(_A,I);break;  /* CMP $ssss,y ABS,y */
case 0xDD: _ICounta+=4;MR_Ax(I);M_CMP(_A,I);break;  /* CMP $ssss,x ABS,x */
case 0xDE: _ICounta+=7;MM_Ax(M_DEC);break;            /* DEC $ssss,x ABS,x */
case 0xF9: _ICounta+=4;MR_Ay(I);M_SBC(I);break;       /* SBC $ssss,y ABS,y */
case 0xFD: _ICounta+=4;MR_Ax(I);M_SBC(I);break;       /* SBC $ssss,x ABS,x */
case 0xFE: _ICounta+=7;MM_Ax(M_INC);break;            /* INC $ssss,x ABS,x */

case 0x01: _ICounta+=6;MR_Ix(I);M_ORA(I);break;       /* ORA ($ss,x) INDEXINDIR */
case 0x11: _ICounta+=5;MR_Iy(I);M_ORA(I);break;       /* ORA ($ss),y INDIRINDEX */
case 0x21: _ICounta+=6;MR_Ix(I);M_AND(I);break;       /* AND ($ss,x) INDEXINDIR */
case 0x31: _ICounta+=5;MR_Iy(I);M_AND(I);break;       /* AND ($ss),y INDIRINDEX */
case 0x41: _ICounta+=6;MR_Ix(I);M_EOR(I);break;       /* EOR ($ss,x) INDEXINDIR */
case 0x51: _ICounta+=5;MR_Iy(I);M_EOR(I);break;       /* EOR ($ss),y INDIRINDEX */
case 0x61: _ICounta+=6;MR_Ix(I);M_ADC(I);break;       /* ADC ($ss,x) INDEXINDIR */
case 0x71: _ICounta+=5;MR_Iy(I);M_ADC(I);break;       /* ADC ($ss),y INDIRINDEX */
case 0x81: _ICounta+=6;MW_Ix(_A);break;             /* STA ($ss,x) INDEXINDIR */
case 0x91: _ICounta+=6;MW_Iy(_A);break;             /* STA ($ss),y INDIRINDEX */
case 0xA1: _ICounta+=6;MR_Ix(_A);M_FL(_A);break;  /* LDA ($ss,x) INDEXINDIR */
case 0xB1: _ICounta+=5;MR_Iy(_A);M_FL(_A);break;  /* LDA ($ss),y INDIRINDEX */
case 0xC1: _ICounta+=6;MR_Ix(I);M_CMP(_A,I);break;  /* CMP ($ss,x) INDEXINDIR */
case 0xD1: _ICounta+=5;MR_Iy(I);M_CMP(_A,I);break;  /* CMP ($ss),y INDIRINDEX */
case 0xE1: _ICounta+=6;MR_Ix(I);M_SBC(I);break;       /* SBC ($ss,x) INDEXINDIR */
case 0xF1: _ICounta+=5;MR_Iy(I);M_SBC(I);break;       /* SBC ($ss),y INDIRINDEX */

case 0x0A: _ICounta+=2;M_ASL(_A);break;             /* ASL a ACC */
case 0x2A: _ICounta+=2;M_ROL(_A);break;             /* ROL a ACC */
case 0x4A: _ICounta+=2;M_LSR(_A);break;             /* LSR a ACC */
case 0x6A: _ICounta+=2;M_ROR(_A);break;             /* ROR a ACC */

default:
_ICounta+=2;
_PC_-=1;
if(!(BadOPList[_PC_>>3]&(1<<(_PC_&0x7)))) {
 sprintf(errmsg,"Ignored opcode: $%02x at $%04x",Page[_PC_>>13][_PC_],_PC_); 
 howlong=180;
// printf("Ignored opcode: $%02x at $%04x\n",Page[_PC_>>13][_PC_],_PC_);
 }
BadOPList[_PC_>>3]|=1<<(_PC_&0x7);
_PC_+=1;
break;

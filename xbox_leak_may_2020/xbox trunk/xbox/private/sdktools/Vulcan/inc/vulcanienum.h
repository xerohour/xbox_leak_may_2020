/***********************************************************************
* Microsoft  Vulcan
*
* Microsoft Confidential.  Copyright 1997-1999 Microsoft Corporation.
*
* File: VulcanIEnum.h
*
* File Comments:
*
*
***********************************************************************/

#ifndef __VULCANIENUM__
#define __VULCANIENUM__

enum OpenFlags
{
   // Print internal statistics.  This is intended primarily for debugging
   // purposes, or for getting some interesting information about what
   // just happened.  !!!DO NOT DEPEND ON THE OUTPUT FORMAT!!! as it may
   // change from drop to drop.
   //
   Open_PrintStats      = 0x00000001,

   // Only disassemble blocks into their component instructions as
   // necessary.  Accessing any instruction, or any information from the
   // instruction list (including VBlock::CountInsts) will likely cause a
   // disassembly.
   //
   // Several internal data structures are kept throughout the lifetime of
   // the VComp in order to handle the delayed disassembly.  These
   // structures will be significantly smaller than the size of all the
   // VInsts, but if you are going to cause all blocks to be disassembled
   // anyhow this option could actually result in decreased performance.
   //
   // When you are finished manipulating a block, call
   // VBlock::ReleaseSymbolic on it.  This may cause the storage allocated
   // to the block's VInsts to be released, depending on what you have
   // done with the block.
   //
   // WARNING: Errata42 checking cannot currently be performed at
   // VComp::Write time when this is enabled, and will be silently
   // skipped.  We will eventually fix that limitation, but for now you
   // should not enable compact blocks if you are producing final
   // customer-ready bits.
   //
   Open_CompactBlocks   = 0x00000002,

   // Ignore missing CV info.  Please note that this is handy for
   // experimental purposes but is not safe in a production environment.
   // Please use this with care.
   //
   Open_IgnoreCvError   = 0x00000004,

   // Open all dlls that we can find from this program.  Used by
   // VProg::Open; ignored by VComp::Open.
   //
   Open_DoDllClosure    = 0x00000008,

   // For use only by BBT.  Don't try this at home.
   //
   Open_BBTMode         = 0x00000010,

   // Opens the image with full PDB info. This is the default level.
   // This level allows full binary modification, which can be emitted.
   //
   Open_FullLevel       = 0x00000000,

   // Opens the image, ignoring any PDB info. Use this if you only want to
   // redirect an import or export.
   // This level only allows redirection modifications to be emitted.
   //
   Open_Level1          = 0x00000020,

   // Opens the image, using minimal PDB info. Usage unknown???
   // This level only allows certain modifications to be emitted.
   //
   Open_Level2          = 0x00000040,

   // Opens the image, using specified level, defaulting to Open_FullLevel.  If
   // not enough info, then the level is degraged.  The resulting level can
   // be retrieved by calling VComp::GetOpenLevel.
   //
   // WARNING: Missing information may limit what operations can be done.
   //
   Open_Degradable      = 0x00000080,
};

enum WriteFlags
{
   // Write_NoFixups allows the new binary to be written without propagating the
   // fixups information.  This means the binary has no chance to be reprocessed
   // unless VCOmp::Write() is invoked with fRereadable flag ON.  This does not
   // mean that fRereadable will override Write_NoFixups.  fRereadable writes
   // out different information so the binary can be reprocessed.

   Write_NoFixups       = 0x00000001,

   // Write_NoOMAP allows the new binary to be written with no OFFSET_MAP info.
   // This will prevent the new binary from being debuggable.  There is no
   // overriding flag for this option if specified.
   //
   // WARNING: no OMAP means no debugging period since stack trace
   // WARNING: will be completely wrong.

   Write_NoOMAP         = 0x00000002,

   // Write_Rereadable allows the emitted binary to be reprocessed by any
   // Vulcan tools.  And as long as this flag is specified when VComp::Write()
   // is called, the emitted binary continues to be reprocessable.

   Write_Rereadable     = 0x00000004,
};

enum PlatformType
{
    platformtUnknown,
    platformtX86,
    platformtIA64,
    platformtLVM, /* Soon to be obsolete */
    platformtMSIL,
};

enum SectionFlags
{
    Sec_DISCARDABLE = 0x00000001,
    Sec_NOTCACHED   = 0x00000002,
    Sec_NOTPAGED    = 0x00000004,
    Sec_SHARED      = 0x00000008,
    Sec_EXECUTABLE  = 0x00000010,
    Sec_READONLY    = 0x00000020,
    Sec_READWRITE   = 0x00000040,
    Sec_NORELOC     = 0x00000080,
};

enum Value
{
    BranchCondValue,
    EffectiveAddressValue,
    Arg0,
    Arg1,
    Arg2,
    Arg3,
    Arg4
};

enum BlockTermType
{
    BlockTermUnknown,
    BlockTermFallThrough,
    BlockTermTrap,
    BlockTermTrapCond,
    BlockTermBranch,
    BlockTermBranchCond,
    BlockTermBranchCondIndirect,
    BlockTermBranchIndirect,
    BlockTermCall,
    BlockTermCallIndirect,
    BlockTermCallCond,
    BlockTermCallCondIndirect,
    BlockTermNop,
    BlockTermRet,
};

struct Inst
{
    enum EOpParam {
        Dest,
        Src1,
        Src2,

        //For non-x86 archs (NYI)
        Dest2,
        Src3,
        Src4,
        Src5,
        QP
    };

    enum Ex86Prefix // very x86 specific -- very temporary
    {
        x86PrefixNotUsed = 0,
        x86PrefixIRep = 1,
        x86PrefixIRepNE = 2,
        x86PrefixLock = 3
    };
};

enum EOpndTypes
{
    OpndNotUsed = 0,        // not used
    OpndImmediate,          // constant
    OpndImmediate64,        // 64 bit constant
    OpndReg,                // register
    OpndStack,              // Stack
    OpndAddress,            // addressing mode
    OpndBlock,              // pointer to basic block
    OpndData,               // pointer to bytes; size in the second operand
    OpndGenAddressBlock,    // general form addr + disp
    OpndPcrel,              // argument imm should be added to PC (used for a
                            // short time while building the representation
    OpndRegPair,
    OpndSymbol,             // Anal symbolic representation (Operand points to a CSymbolRef)
    OpndOverflow,           // Operand is a pointer to an overflow area -- IA64 motivated
    OpndHiWordAdjust,       // for AXP LDAH inst: high 16 bits of address (adjusted)
    OpndLoWord              // for AXP LDA,LDL,STL insts: low 16 bits of address
};

enum EAddressBaseType
{
    AddressBaseNotUsed=0,
    AddressBaseRegister
};

enum EAddrDispType
{
    AddrDispNotUsed = 0,
    AddrDispImmediate,
    AddrDispBBlock,
    AddrDispProc,       //Not valid - will point to entry block instead
    AddrGenAddressBlock,
};

enum IA64Completers
{
    // Branch branch prediction hardware control
    BrNone = 0, BrClr,

    // Branch prediction vector hint
    BrpDcDc = 0, BrpNone = BrpDcDc, BrpDcNt, BrpTkDc, BrpTkTk, BrpTkNt,
    BrpNtDc, BrpNtTk, BrpNtNt,

    // Branch prediction importance hint (must share flag space with the above)
    BrpNotImp = 16, BrpImp,

    // Speculation Check ALAT control
    ChkClr = 0, ChkNc,

    // Compare Type
    CmpNone = 0, CmpUnc, CmpOr, CmpAnd, CmpOrAndCm,

    // Compute Zero Index, Mix, FSxt Direction
    LeftForm = 0, RightForm, LeftRightForm,

    // Compare and Exchange, Fetch & Add semaphore completer
    SemAcq = 0, SemRel,

    // Floating point status field (cannot share with compare type of left/right form)
    SF0 = 16, SF1, SF2, SF3,

    // FSwap Forms
    FSwapForm = 0, FSwapNlForm, FSwapNrForm,

    // Invala Forms
    InvalaComplete = 0, InvalaEntry,

    // Load types
    LdNone = 0, LdCNc, LdCClr, LdCClrAcq, LdAcq, LdBias, LdAdvanced, LdSpeculative, LdSpeculativeAdvanced,

    // Temporal Locality Hints (must share flag space with the above)
    LocNone = 16, LocNt1, LocNt2, LocNta,

    // Load Fault types (must share flag space with the above)
    LdFaultNone = 0, LdFaultRaise,

    // Memory Fence Modifiers
    FenceOrdering = 0, FenceAccept,

    // Move Modifiers
    MovNone = 0, MoveRet,

    // Parallel Op Modifiers
    ParNone = 0, ParSSS, ParUSS, ParUUS, ParUUU, ParRaz, ParUnsigned,

    // Unpack Modifiers
    UnpackHigh = 0, UnpackLow

};

enum IA64Syllable
{
    IllegalSlotType = 0,
    MSlot,
    ISlot,
    FSlot,
    BSlot,
    XSlot,    // For movl, long nops and branches
    B2Slot,   // For brp's mainly, issue in last B slot of bundle
    I0Slot    // Issue in 1st I slot of independence group
};

class Eff {
public:
    enum EEff {
        ANY = -1,
        Dest,
        Src1,
        Src2,
    };
};

class Register
{
public:
    enum Regs{
//********X86********
        ZERO        =     0x0,
        AL          =     0x1,
        AH          =     0x2,
        AX          =     0x3,
        EAH         =     0x4,
        EAX         =     0x7,

        BL          =     0x9,
        BH          =     0xa,
        BX          =     0xb,
        EBH         =     0xc,
        EBX         =     0xf,

        CL          =     0x11,
        CH          =     0x12,
        CX          =     0x13,
        ECH         =     0x14,
        ECX         =     0x17,

        DL          =     0x19,
        DH          =     0x1a,
        DX          =     0x1b,
        EDH         =     0x1c,
        EDX         =     0x1f,

        SI          =     0x23,
        SIH         =     0x26,
        ESI         =     0x27,

        DI          =     0x2b,
        DIH         =     0x2e,
        EDI         =     0x2f,

        SP          =     0x33,
        SPH         =     0x36,
        ESP         =     0x37,

        BP          =     0x3b,
        BPH         =     0x3e,
        EBP         =     0x3f,

        FLAGS       =     0x43,  // FL string
        EFLAGS      =     0x47,  // EF string

        CF          =     0x43,
        PF          =     0x44,
        AF          =     0x45,
        ZF          =     0x46,
        SF          =     0x47,
        TF          =     0x48,
        IF          =     0x49,
        DF          =     0x4a,
        OF          =     0x4b,
        IOPL        =     0x4c,
        NT          =     0x4d,
        RF          =     0x4e,
        VM          =     0x4f,
        AC          =     0x50,
        VIF         =     0x51,
        VIP         =     0x52,
        ID          =     0x53,

        IP          =     0x5b,
        EIP         =     0x5f,  // EP string
        ENDGENERAL  =     EIP,

        CS          =     0x63,
        DS          =     0x6b,
        SS          =     0x73,
        ES          =     0x7b,
        FS          =     0x83,
        GS          =     0x8b,

        TR0         =     0x90,
        TR1         =     0x91,
        TR2         =     0x92,
        TR3         =     0x93,
        TR4         =     0x94,
        TR5         =     0x95,
        TR6         =     0x96,
        TR7         =     0x97,

        DR0         =     0x98,
        DR1         =     0x99,
        DR2         =     0x9a,
        DR3         =     0x9b,
        DR4         =     0x9c,
        DR5         =     0x9d,
        DR6         =     0x9e,
        DR7         =     0x9f,

        CR0         =     0xa0,
        CR1         =     0xa1,
        CR2         =     0xa2,
        CR3         =     0xa3,
        CR4         =     0xa4,
        CR5         =     0xa5,
        CR6         =     0xa6,
        CR7         =     0xa7,

        MM0         =     0xa8,
        MM1         =     0xa9,
        MM2         =     0xaa,
        MM3         =     0xab,
        MM4         =     0xac,
        MM5         =     0xad,
        MM6         =     0xae,
        MM7         =     0xaf,

        CC          =     0xb0,
        PERF1       =     0xb1,
        PERF2       =     0xb2,

        XMM0        =     0xb3,
        XMM1        =     0xb4,
        XMM2        =     0xb5,
        XMM3        =     0xb6,
        XMM4        =     0xb7,
        XMM5        =     0xb8,
        XMM6        =     0xb9,
        XMM7        =     0xba,

        X86REGMAX,

//*******IA64*******

        // General Purpose Registers

        GR0 = 0,
              GR1 , GR2 , GR3 , GR4 , GR5 , GR6 , GR7 ,
        GR8 , GR9 , GR10, GR11, GR12, GR13, GR14, GR15,
        GR16, GR17, GR18, GR19, GR20, GR21, GR22, GR23,
        GR24, GR25, GR26, GR27, GR28, GR29, GR30, GR31,
        GR32, GR33, GR34, GR35, GR36, GR37, GR38, GR39,
        GR40, GR41, GR42, GR43, GR44, GR45, GR46, GR47,
        GR48, GR49, GR50, GR51, GR52, GR53, GR54, GR55,
        GR56, GR57, GR58, GR59, GR60, GR61, GR62, GR63,
        GR64, GR65, GR66, GR67, GR68, GR69, GR70, GR71,
        GR72, GR73, GR74, GR75, GR76, GR77, GR78, GR79,
        GR80, GR81, GR82, GR83, GR84, GR85, GR86, GR87,
        GR88, GR89, GR90, GR91, GR92, GR93, GR94, GR95,
        GR96, GR97, GR98, GR99, GR100, GR101, GR102, GR103,
        GR104, GR105, GR106, GR107, GR108, GR109, GR110, GR111,
        GR112, GR113, GR114, GR115, GR116, GR117, GR118, GR119,
        GR120, GR121, GR122, GR123, GR124, GR125, GR126, GR127,

        // Floating Point Registers

        FR0 , FR1 , FR2 , FR3 , FR4 , FR5 , FR6 , FR7 ,
        FR8 , FR9 , FR10, FR11, FR12, FR13, FR14, FR15,
        FR16, FR17, FR18, FR19, FR20, FR21, FR22, FR23,
        FR24, FR25, FR26, FR27, FR28, FR29, FR30, FR31,
        FR32, FR33, FR34, FR35, FR36, FR37, FR38, FR39,
        FR40, FR41, FR42, FR43, FR44, FR45, FR46, FR47,
        FR48, FR49, FR50, FR51, FR52, FR53, FR54, FR55,
        FR56, FR57, FR58, FR59, FR60, FR61, FR62, FR63,
        FR64, FR65, FR66, FR67, FR68, FR69, FR70, FR71,
        FR72, FR73, FR74, FR75, FR76, FR77, FR78, FR79,
        FR80, FR81, FR82, FR83, FR84, FR85, FR86, FR87,
        FR88, FR89, FR90, FR91, FR92, FR93, FR94, FR95,
        FR96, FR97, FR98, FR99, FR100, FR101, FR102, FR103,
        FR104, FR105, FR106, FR107, FR108, FR109, FR110, FR111,
        FR112, FR113, FR114, FR115, FR116, FR117, FR118, FR119,
        FR120, FR121, FR122, FR123, FR124, FR125, FR126, FR127,

        // Predicate Registers

        PR0 , PR1 , PR2 , PR3 , PR4 , PR5 , PR6 , PR7 ,
        PR8 , PR9 , PR10, PR11, PR12, PR13, PR14, PR15,
        PR16, PR17, PR18, PR19, PR20, PR21, PR22, PR23,
        PR24, PR25, PR26, PR27, PR28, PR29, PR30, PR31,
        PR32, PR33, PR34, PR35, PR36, PR37, PR38, PR39,
        PR40, PR41, PR42, PR43, PR44, PR45, PR46, PR47,
        PR48, PR49, PR50, PR51, PR52, PR53, PR54, PR55,
        PR56, PR57, PR58, PR59, PR60, PR61, PR62, PR63,

        // Predicate Aggregates: PR and PR.rot

        PR, PRROT,

        // Branch Registers

        BR0 , RP = BR0, BR1 , BR2 , BR3 , BR4 , BR5 , BR6 , BR7 ,

        // Application Registers

        // Kernel Registers are aliased with the first 8 AR's

        AR0 , KR0 = AR0, AR1 , KR1 = AR1, AR2 , KR2 = AR2, AR3 , KR3 =AR3,
        AR4 , KR4 = AR4, AR5 , KR5 = AR5, AR6 , KR6 = AR6, AR7 , KR7 = AR7,
        AR8 , AR9 , AR10, AR11, AR12, AR13, AR14, AR15,
        AR16, RSC = AR16, AR17, BSP = AR17, AR18, BSPSTORE = AR18, AR19, RNAT = AR19,
        AR20, AR21, AR22, AR23,
        AR24, AR25, AR26, AR27, AR28, AR29, AR30, AR31,
        AR32, CCV = AR32, AR33, AR34, AR35, AR36, UNAT = AR36, AR37, AR38, AR39,
        AR40, FPSR = AR40, AR41, AR42, AR43, AR44, ITC = AR44, AR45, AR46, AR47,
        AR48, AR49, AR50, AR51, AR52, AR53, AR54, AR55,
        AR56, AR57, AR58, AR59, AR60, AR61, AR62, AR63,
        AR64, PFS = AR64, AR65, LC = AR65, AR66, EC = AR66, AR67, AR68, AR69, AR70, AR71,
        AR72, AR73, AR74, AR75, AR76, AR77, AR78, AR79,
        AR80, AR81, AR82, AR83, AR84, AR85, AR86, AR87,
        AR88, AR89, AR90, AR91, AR92, AR93, AR94, AR95,
        AR96, AR97, AR98, AR99, AR100, AR101, AR102, AR103,
        AR104, AR105, AR106, AR107, AR108, AR109, AR110, AR111,
        AR112, AR113, AR114, AR115, AR116, AR117, AR118, AR119,
        AR120, AR121, AR122, AR123, AR124, AR125, AR126, AR127,

		// Control registers
        ia64CR0, DCR = ia64CR0, ia64CR1, ITM = ia64CR1, ia64CR2, IVA = ia64CR2, ia64CR3, ia64CR4, ia64CR5, ia64CR6, ia64CR7,
        CR8, PTA = CR8, CR9,  CR10, CR11, CR12, CR13, CR14, CR15,
        CR16, IPSR = CR16, CR17, ISR = CR17, CR18, CR19, IIP = CR19, CR20, IFA = CR20, CR21, ITIR = CR21, CR22,
                                                                     IIPA = CR22, CR23, IFS = CR23,
        CR24, IIM = CR24, CR25, IHA = CR25, CR26, CR27, CR28, CR29, CR30, CR31,
        CR32, CR33, CR34, CR35, CR36, CR37, CR38, CR39,
        CR40, CR41, CR42, CR43, CR44, CR45, CR46, CR47,
        CR48, CR49, CR50, CR51, CR52, CR53, CR54, CR55,
        CR56, CR57, CR58, CR59, CR60, CR61, CR62, CR63,
        CR64, LID = CR64, CR65, IVR = CR65, CR66, TPR = CR66, CR67, EOI = CR67, CR68, IRR0 = CR68, CR69, IRR1 = CR69,
                                                                    CR70, IRR2 = CR70, CR71, IRR3 = CR71,
        CR72, ITV = CR72, CR73, PMV = CR73, CR74, CMCV = CR74, CR75, CR76, CR77, CR78, CR79,
        CR80, LRR0 = CR80, CR81, LRR1 = CR81, CR82, CR83, CR84, CR85, CR86, CR87,
        CR88, CR89, CR90, CR91, CR92, CR93, CR94, CR95,
        CR96, CR97, CR98, CR99, CR100, CR101, CR102, CR103,
        CR104, CR105, CR106, CR107, CR108, CR109, CR110, CR111,
        CR112, CR113, CR114, CR115, CR116, CR117, CR118, CR119,
        CR120, CR121, CR122, CR123, CR124, CR125, CR126, CR127,

		// Program status register
        PSR, PSRL, PSRUM,

        // IP for ia64
        ia64IP,

        BadIA64Register,

        IA64RegisterCount,

//********AXP********

        // General Purpose Registers
        R0 = 0,
             R1,  R2,  R3,  R4,  R5,  R6,  R7,
        R8,  R9,  R10, R11, R12, R13, R14, R15,
        R16, R17, R18, R19, R20, R21, R22, R23,
        R24, R25, R26, R27, R28, R29, R30, R31,

        // Friendly names for General Purpose Registers
        V0 = 0,
             T0,  T1,  T2,  T3,  T4,  T5,  T6,  T7,
        S0,  S1,  S2,  S3,  S4,  S5,  Fp,
        A0,  A1,  A2,  A3,  A4,  A5,  T8,  T9,  T10, T11,
        Ra,  T12, At,  Gp,  Sp,  Zero,

        // Floating Point Registers
        F0,  F1,  F2,  F3,  F4,  F5,  F6,  F7,
        F8,  F9,  F10, F11, F12, F13, F14, F15,
        F16, F17, F18, F19, F20, F21, F22, F23,
        F24, F25, F26, F27, F28, F29, F30, F31,

        AXPRegisterCount
    };

    static VULCANDLL bool VULCANCALL IsStackReg(int reg) {return reg == ESP || reg == SP;}
};

typedef Register::Regs X86Reg;
typedef Register       X86Register;
typedef Register::Regs IA64Reg;
typedef Register       IA64Register;
typedef Register::Regs AXPReg;
typedef Register       AXPRegister;
typedef Register::Regs ERegister;

class COp{
public:
    enum EOpGroup
    {
        GOpERROR = 0x0,
        GADD,       GOR,         GAND,
        GXOR,       GNOT,        GNEG,
        GMUL,       GDIV,        GIADC,
        GISBB,      GSUB,
        GCMP,       GCMPL,
        GPUSH,      GPOP,
        GB,         GJMP,        GISETCC,
        GMOV,       GMOVZX,      GMOVSX,
        GBSWAP,     GLEA,
        GCVT,
        GCALL,      GRET,
        GINT,       GNOP,        GHLT,
        GROL,       GROR,
        GSAL,       GSHR,        GSAR,
        GRCL,       GRCR,

        GDATA,      GADDR,

        GIDAA,      GIAAS,       GIAAA,
        GIAAM,      GIAAD,       GIDAS,
        GIINC,      GIDEC,
        GIXADD,
        GIPUSHA,    GIPOPA,
        GIPUSHF,    GIPOPF,
        GIBOUND,    GIARPL,
        GIINS,      GIOUTS,
        GIIN,       GIOUT,
        GIXCH,      GIWAIT,      GIXLAT,
        GISAHF,     GILAHF,
        GIMOVS,     GICMPS,
        GISTOS,     GILODS,      GISCAS,
        GILES,      GILDS,       GILAR,
        GILSL,
        GIENTER,    GILEAVE,     GIINTO,
        GILOOP,
        GILOCK,     GIREP,
        GICMC,      GICLC,       GICLI,
        GICLD,      GICLTS,      GISTC,
        GISTI,      GISTD,
        GIINVD,     GIWBINVD,    GICPUID,
        GIRSM,
        GISHLD,     GISHRD,
        GIBT,       GIBTS,       GIBTR,
        GIBTC,
        GIBSF,      GIBSR,
        GILSS,      GILGS,
        GILFS,      GISLDT,      GISTR,
        GILLDT,     GILTR,
        GIVERR,     GIVERW,
        GISGDT,     GISIDT,      GILGDT,
        GILIDT,     GISMSW,      GILMSW,
        GIINVLPG,   GICMXCHG8B,
        GICMPXCH,

        GLD,        GST,

        GFADD,      GFCMP,       GFUCMPX,
        GFMUL,      GFSUB,       GFSUBR,
        GFDIV,      GFDIVR,
        GFLD,       GFST,
        GFXCHX,     GFNOP,       GFCHS,
        GFABS,      GFPTANX,     GFPATANX,
        GFSQRTX,    GFSCALEX,
        GFSINX,     GFCOSX,

        GIFCMPP,    GIFCMPPPX,
        GIFUCMPPPX, GIFUCMPPX,   GIFADDPX,  GIFMULPX,
        GIFSUBRPX,  GIFSUBPX,    GIFDIVRPX, GIFDIVPX,
        GIFSTP,
        GIFLDENV,   GIFLDCW,     GIFSTENV,  GIFSTCW,  GIFSTPQ,
        GIFRSTOR,   GIFSAVE,     GIFNSTSW,  GIFBLD,   GIFBSTP,
        GIFTST,     GIFXAM,      GIFLDL2T,
        GIFLD1,     GIFLDL2E,    GIFLDPI,   GIFLDLG2,
        GIFLDLN2,   GIFLDZ,      GIF2XM1,   GIFYL2X,  GIFXTRACT,
        GIFPREM1,   GIFDECSTP,   GIFINCSTP, GIFPREM,  GIFYL2XP1,
        GIFSINCOSX, GIFRNDINT,   GIFFREE,   GIFCLEX,  GIFINIT,
        GIRDMSR,    GIRDTSC,     GIWRMSR,   GIRDPMC,  GIXBTS,
        GIFCOMI,    GIFUCOMI,    GFCMOV,  GIFENI,     GIFDISI,    GIFSETPM,

        GMMXEMMS,
        GMMXMOV,
        GMMXPACK,
        GMMXPADD,
        GMMXPAND,
        GMMXPEQ,
        GMMXPGT,
        GMMXPMADD,
        GMMXPMUL,
        GMMXPOR,
        GMMXPSLL,   GMMXPSRA,    GMMXPSRL,
        GMMXPSUB,
        GMMXPUNPCK,
        GMMXPXOR,
        GMMXSYSTENTER,
        GMMXSYSEXIT,
        GMMXFXSAVE, GMMXFXRSTOR,
        GLDMXCSR, GSTMXCSR,

        GMMXPI2FD, GMMXPF2ID,    GMMXPFCMPGE, GMMXPFMIN,    GMMXPFRCP,    GMMXPFRSQRT, GMMXPFSUB,
        GMMXPFADD, GMMXPFCMPGT,  GMMXPFMAX,   GMMXPFRCPIT1, GMMXPFRSQIT1, GMMXPFSUBR,
        GMMXPFACC, GMMXPFCMPEQ,  GMMXPFMUL,   GMMXPFRCPIT2, GMMXPMULHRW,  GMMXPAVGUSB,
        GFEMMS,    GMMXPREFETCH, GMMXSFENCE,  GMMXXORPS,

        GMOVSS,  GMOVUPS, GCVTPI2PS, GCVTSI2SS,
        GPINSRW, GPEXTRW, GSHUFPS,   GRCP, GSQRT, GMAX, GMIN, GUNPCK,

        // UNDONE: To be moved near all the MOV instructions

        GCMOV,

        GMMXMASKMOV, GMMXPSADB, GPAUSE,

        EndGrp
    };

    enum EOp
    {
        OpERROR = 0x0,
        ADDB,         ADDW,         ADDD,
        ORB,          ORW,          ORD,
        ANDB,         ANDW,         ANDD,
        XORB,         XORW,         XORD,
        NOTB,         NOTW,         NOTD,
        NEGB,         NEGW,         NEGD,
        UMULB,        UMULW,        UMULD,
        SMULB,        SMULW,        SMULD,
        IMULW,        IMULD,
        UDIVB,        UDIVW,        UDIVD,
        SDIVB,        SDIVW,        SDIVD,
        IADCB,        IADCW,        IADCD,
        ISBBB,        ISBBW,        ISBBD,
        SUBB,         SUBW,         SUBD,
        CMPB,         CMPW,         CMPD,
        CMPLB,        CMPLW,        CMPLD,

        // IA64 Compares
        CMP_EQ,   CMP_LT,   CMP_LTU,   CMP_NE,   CMP_LE,   CMP_GT,   CMP_GE,
        CMP4_EQ,  CMP4_LT,  CMP4_LTU,  CMP4_NE,  CMP4_LE,  CMP4_GT,  CMP4_GE,
        TBIT_Z,   TBIT_NZ,  TNAT_Z,    TNAT_NZ,  PCMP1_EQ, PCMP2_EQ, PCMP4_EQ,
        PCMP1_GT, PCMP2_GT, PCMP4_GT,

        PUSHB,        PUSHW,        PUSHD,
        POPB,         POPW,         POPD,

        // Start of Conditional Branches
        // Start of Reversable Conditional Branches
        BULT,         BULE,         BUGT,         BUGE,
        BLT,          BLE,          BGT,          BGE,
        BO,           BNO,          BP,           BNP,
        BS,           BNS,          BZ,           BNZ,
        // AXP Floating Point Conditional Branches
        FBEQ, FBGE, FBGT, FBLE, FBLT, FBNE,
        // End of Reversable Conditional Branches
        BEQ,          BNE,

        // IA64 Branches
        BR_COND_SPNT_FEW, BR_COND_SPNT_MANY, BR_COND_SPTK_FEW, BR_COND_SPTK_MANY,
        BR_COND_DPNT_FEW, BR_COND_DPNT_MANY, BR_COND_DPTK_FEW, BR_COND_DPTK_MANY,

        BRL_COND_SPNT_FEW, BRL_COND_SPNT_MANY, BRL_COND_SPTK_FEW, BRL_COND_SPTK_MANY,
        BRL_COND_DPNT_FEW, BRL_COND_DPNT_MANY, BRL_COND_DPTK_FEW, BRL_COND_DPTK_MANY,

		BR_WEXIT_SPNT_FEW, BR_WEXIT_SPNT_MANY, BR_WEXIT_SPTK_FEW, BR_WEXIT_SPTK_MANY,
        BR_WEXIT_DPNT_FEW, BR_WEXIT_DPNT_MANY, BR_WEXIT_DPTK_FEW, BR_WEXIT_DPTK_MANY,

        BR_WTOP_SPNT_FEW, BR_WTOP_SPNT_MANY, BR_WTOP_SPTK_FEW, BR_WTOP_SPTK_MANY,
        BR_WTOP_DPNT_FEW, BR_WTOP_DPNT_MANY, BR_WTOP_DPTK_FEW, BR_WTOP_DPTK_MANY,

        BR_CLOOP_SPNT_FEW, BR_CLOOP_SPNT_MANY, BR_CLOOP_SPTK_FEW, BR_CLOOP_SPTK_MANY,
        BR_CLOOP_DPNT_FEW, BR_CLOOP_DPNT_MANY, BR_CLOOP_DPTK_FEW, BR_CLOOP_DPTK_MANY,

        BR_CEXIT_SPNT_FEW, BR_CEXIT_SPNT_MANY, BR_CEXIT_SPTK_FEW, BR_CEXIT_SPTK_MANY,
        BR_CEXIT_DPNT_FEW, BR_CEXIT_DPNT_MANY, BR_CEXIT_DPTK_FEW, BR_CEXIT_DPTK_MANY,

        BR_CTOP_SPNT_FEW, BR_CTOP_SPNT_MANY, BR_CTOP_SPTK_FEW, BR_CTOP_SPTK_MANY,
        BR_CTOP_DPNT_FEW, BR_CTOP_DPNT_MANY, BR_CTOP_DPTK_FEW, BR_CTOP_DPTK_MANY,

        BR_IA_SPNT_FEW, BR_IA_SPNT_MANY, BR_IA_SPTK_FEW, BR_IA_SPTK_MANY,
        BR_IA_DPNT_FEW, BR_IA_DPNT_MANY, BR_IA_DPTK_FEW, BR_IA_DPTK_MANY,

        // AXP
        BLBC, BLBS,
        // End of Conditional Branches
        IBCXZ,

        // Start of Unconditional Branches
        JMP,          IJMP16,       IJMP32,       IJMP48,

        // AXP
        BR_JMP,       BSR_JMP,      JMP_JMPI,     JSR_JMPI,
        // End of Unconditional Branches
        JSR_COROUTINE_JMPI,

        // Start of Calls
        CALL,         ICALL16,

        // IA64
        BR_CALL_SPNT_FEW, BR_CALL_SPNT_MANY, BR_CALL_SPTK_FEW,BR_CALL_SPTK_MANY,
        BR_CALL_DPNT_FEW, BR_CALL_DPNT_MANY, BR_CALL_DPTK_FEW,BR_CALL_DPTK_MANY,

        BRL_CALL_SPNT_FEW, BRL_CALL_SPNT_MANY, BRL_CALL_SPTK_FEW,BRL_CALL_SPTK_MANY,
        BRL_CALL_DPNT_FEW, BRL_CALL_DPNT_MANY, BRL_CALL_DPTK_FEW,BRL_CALL_DPTK_MANY,

        // AXP
        BR_CALL,      BSR_CALL,     JMP_CALLI,    JSR_CALLI,    RET_CALLI,
        // End of Calls
        JSR_COROUTINE_CALLI,

        // Start of Returns
        RET,          RETF,         IRET,

        // IA64
        BR_RET_SPNT_FEW, BR_RET_SPNT_MANY, BR_RET_SPTK_FEW, BR_RET_SPTK_MANY,
        BR_RET_DPNT_FEW, BR_RET_DPNT_MANY, BR_RET_DPTK_FEW, BR_RET_DPTK_MANY,
        RFI,

        // End of Returns
        RET_JMPI,

        ISETULT,      ISETULE,      ISETUGT,      ISETUGE,
        ISETLT,       ISETLE,       ISETGT,       ISETGE,
        ISETO,        ISETNO,       ISETP,        ISETNP,
        ISETS,        ISETNS,       ISETZ,        ISETNZ,

        MOVB,         MOVW,         MOVD,
        MOVZXDB,      MOVZXWB,      MOVZXDW,
        MOVSXDB,      MOVSXWB,      MOVSXDW,

        // IA64 MOV instructions
        // Execution unit specific MOV instructions
        MOV_I,        MOV_M,

        // Register set secific moves
        MOV_CPUID,
        MOV_TO_DBR, MOV_FROM_DBR,
        MOV_TO_IBR, MOV_FROM_IBR,
        MOV_TO_MSR, MOV_FROM_MSR,
        MOV_TO_PKR, MOV_FROM_PKR,
        MOV_TO_PMC, MOV_FROM_PMC,
        MOV_TO_PMD, MOV_FROM_PMD,
        MOV_TO_RR,  MOV_FROM_RR,

        // MOV's involving branch prediction and prefetch.
        // For now, cancellation vector and importance hint are omitted.
        MOV_FEW,      MOV_MANY,     MOV_SPTK_FEW,   MOV_SPTK_MANY,
        MOV_DP_FEW,   MOV_DP_MANY,

        // MOVE LONG
        MOVL,

        BSWAP,        LEAD,         LEAW,
        CWD,          CDQ,          CBW,
        INT,          NOP,          HLT,
        ROLB,         ROLW,         ROLD,
        RORB,         RORW,         RORD,
        SALB,         SALW,         SALD,
        SHRB,         SHRW,         SHRD,
        SARB,         SARW,         SARD,
        RCLB,         RCLW,         RCLD,
        RCRB,         RCRW,         RCRD,
        DATA,         ADDR,

        IDAA,         IAAS,         IAAA,
        IAAM,         IAAD,         IDAS,
        IINCB,        IINCW,        IINCD,
        IDECB,        IDECW,        IDECD,
        IXADDB,       IXADDW,       IXADDD,
        IPUSHA,       IPOPA,        IPUSHF,       IPOPF,
        IBOUNDW,      IBOUNDD,      IARPL,
        IINSB,        IINSW,        IINSD,
        IOUTSB,       IOUTSW,       IOUTSD,
        IINB,         IINW,         IIND,
        IOUTB,        IOUTW,        IOUTD,
        IXCHB,        IXCHW,        IXCHD,
        IWAIT,        IXLAT,
        ISAHF,        ILAHF,
        IMOVSB,       IMOVSW,       IMOVSD,
        ICMPSB,       ICMPSW,       ICMPSD,
        ISTOSB,       ISTOSW,       ISTOSD,
        ILODSB,       ILODSW,       ILODSD,
        ISCASB,       ISCASW,       ISCASD,
        ILESF,        ILESS,
        ILDSF,        ILDSS,
        ILARD,        ILARW,
        ILSLD,        ILSLW,
        IENTER,       ILEAVE,       IINTO,
        ILOOPN,       ILOOP,        ILOOPE,
        ILOCK,        IREP,         IREPNE,
        ICMC,         ICLC,         ICLI,         ICLD,
        ICLTS,        ISTC,         ISTI,         ISTD,
        IINVD,        IWBINVD,      ICPUID,       IRSM,
        ISHLDD,       ISHLDW,       ISHRDD,       ISHRDW,
        IBTD,         IBTW,         IBTSD,        IBTSW,
        IBTRD,        IBTRW,        IBTCD,        IBTCW,
        IBSFD,        IBSFW,        IBSRD,        IBSRW,
        ILSSF,        ILSSS,        ILGSF,        ILGSS,
        ILFSF,        ILFSS,        ISLDT,        ISTR,
        ILLDT,        ILTR,         IVERR,        IVERW,
        ISGDT,        ISIDT,        ILGDT,
        ILIDT,        ISMSW,        ILMSW,        IINVLPG,
        ICMXCHG8B,
        ICMPXCHB,     ICMPXCHW,     ICMPXCHD,

        LDD,          LDW,          LDH,          LDB,

        // IA64 FILL instructions
        LDD_FILL,     LDF_FILL,

        STD,          STW,          STH,          STB,

        // IA64 SPILL instructions
        STD_SPILL,    STF_SPILL,

        ASZ,          OSZ,          SEG,

        RESERVED,     GRP,          ESC,          ESCESC,
        ESCESC2,      KATMAI,      KATMAI2,

        FLOAT,
        FADDF,        FADDW,        FADDI,        FADDD,     FADDX,
        FCMPF,        FCMPW,        FCMPI,        FCMPD,     FCMPX,FUCMPX,

        // IA64 Floating comparisons
        FCMP_EQ,      FCMP_LT,      FCMP_LE,      FCMP_UNORD,
        FPCMP_EQ,     FPCMP_LT,     FPCMP_LE,     FPCMP_UNORD,
        FPCMP_NEQ,    FPCMP_NLT,    FPCMP_NLE,    FPCMP_ORD,


        FMULF,        FMULW,        FMULI,        FMULD,     FMULX,
        FSUBF,        FSUBW,        FSUBI,        FSUBD,     FSUBX,
        FSUBRF,       FSUBRW,       FSUBRI,       FSUBRD,    FSUBRX,
        FDIVF,        FDIVW,        FDIVI,        FDIVD,     FDIVX,
        FDIVRF,       FDIVRD,       FDIVRX,       FDIVRI,
        FLDF,         FLDW,         FLDI,         FLDD,      FLDQ,   FLDX,
        FSTF,         FSTW,         FSTI,         FSTD,      FSTX,
        FXCHX,
        FNOP,         FCHS,         FABS,         FPTANX,    FPATANX,
        FSQRTX,       FSCALEX,      FSINX,        FCOSX,     // fsqrt = 370

        IFCMPPF,      IFCMPPW,      IFCMPPI,      IFCMPPD,   IFCMPPX,
        IFCMPPPX,     IFUCMPPPX,    IFUCMPPX,     IFADDPX,   IFMULPX,
        IFSUBRPX,     IFSUBPX,      IFDIVRPX,     IFDIVPX,
        IFSTPF,       IFSTPW,       IFSTPI,       IFSTPD,    IFSTPX,
        IFLDENV,      IFLDCW,       IFSTENV,      IFSTCW,    IFSTPQ,
        IFRSTOR,      IFSAVE,       IFNSTSW,      IFBLD,     IFBSTP,
        IFTST,        IFXAM,        IFLDL2T,
        IFLD1,        IFLDL2E,      IFLDPI,       IFLDLG2,
        IFLDLN2,      IFLDZ,        IF2XM1,       IFYL2X,    IFXTRACT,
        IFPREM1,      IFDECSTP,     IFINCSTP,     IFPREM,    IFYL2XP1,
        IFSINCOSX,    IFRNDINT,     IFFREE,       IFCLEX,    IFINIT,
        IRDMSR,       IRDTSC,       IWRMSR,       IRDPMC,    IXBTS,
        IFCOMIP,      IFUCOMIP,     FCMOVNB,      FCMOVNE,   FCMOVNBE,
        FCMOVNU,      FCOMI,        FUCOMI,       FCMOVB,    FCMOVE,
        FCMOVBE,      FCMOVU,       IFENI,        IFDISI,    IFSETPM,

        MMXPADDB,     MMXPADDD,     MMXPADDSB,    MMXPADDSW,
        MMXPADDUSB,   MMXPADDUSW,   MMXPADDW,
        MMXPAND,      MMXPANDN,
        MMXEMMS,
        MMXPMADDWD,
        MMXMOVDTO,    MMXMOVDFROM,  MMXMOVQTO,    MMXMOVQFROM,
        MMXPACKSSDW,  MMXPACKSSWB,  MMXPACKUSWB,
        MMXPCMPEQB,   MMXPCMPEQD,   MMXPCMPEQW,
        MMXPCMPGTB,   MMXPCMPGTD,   MMXPCMPGTW,
        MMXPMULHW,    MMXPMULLW,
        MMXPOR,
        MMXPSLLD,     MMXPSLLQ,     MMXPSLLW,
        MMXPSRAD,     MMXPSRAW,
        MMXPSRLD,     MMXPSRLQ,     MMXPSRLW,
        MMXPSUBB,     MMXPSUBD,     MMXPSUBSB,    MMXPSUBSW,
        MMXPSUBUSB,   MMXPSUBUSW,   MMXPSUBW,
        MMXPUNPCKHBW, MMXPUNPCKHDQ, MMXPUNPCKHWD,
        MMXPUNPCKLBW, MMXPUNPCKLDQ, MMXPUNPCKLWD,
        MMXPXOR,
        MMXSYSTENTER,
        MMXSYSEXIT,
        MMXFXSAVE, MMXFXRSTOR,
        LDMXCSR, STMXCSR,

        MMXPI2FD, MMXPF2ID,   MMXPFCMPGE, MMXPFMIN,    MMXPFRCP,    MMXPFRSQRT, MMXPFSUB,
        MMXPFADD, MMXPFCMPGT, MMXPFMAX,   MMXPFRCPIT1, MMXPFRSQIT1, MMXPFSUBR,
        MMXPFACC, MMXPFCMPEQ, MMXPFMUL,   MMXPFRCPIT2, MMXPMULHRW,  MMXPAVGUSB,
        FEMMS,    MMXPREFETCH,MMXPREFETCHW,

        MMXPREFETCHNTA, MMXPREFETCH0, MMXPREFETCH1, MMXPREFETCH2, MMXSFENCE,

        XORPS, MOVNTPS, MOVAPS, COMISS, UCOMISS, MOVLPS, MOVHPS, UNPCKLPS, UNPCKHPS,
        MOVMSKPS, ANDPS, ANDNPS, ORPS, PMOVMSKB,


        // MMX

        // KATMAI
        MOVSS, MOVUPS, CVTPI2PS, CVTSI2SS,
        PINSRW, PEXTRW, SHUFPS,
        ADDPS, ADDSS, SUBPS, SUBSS, MULPS, MULSS, DIVPS, DIVSS, MINPS, MINSS, MAXPS, MAXSS,
        SQRTPS, SQRTSS, RSQRTPS, RSQRTSS, RCPPS, RCPSS,
        CMPEQPS, CMPEQSS, CMPLTPS, CMPLTSS, CMPLEPS, CMPLESS,
        CMPUNORDPS, CMPUNORDSS, CMPNEQPS, CMPNEQSS,
        CMPNLTPS, CMPNLTSS, CMPNLEPS, CMPNLESS,
        CMPORDPS, CMPORDSS,
        CVTTPS2PI, CVTTSS2SI, CVTPS2PI, CVTSS2SI,

        // IA64 Specific Instructions -- only those opcodes whose functionality
        // is not subsumed by the instructions above will be listed here.

        // Add Pointer
        ADDP4,

        // Register stack alloc
        ALLOC,

        // And Complement
        ANDCM,

        // Break
        BREAK_B, BREAK_F, BREAK_I, BREAK_M, BREAK_X,

		// Branch Prediction
        BRP_SPTK_FEW, BRP_SPTK_MANY, BRP_LOOP_FEW, BRP_LOOP_MANY,
        BRP_EXIT_FEW, BRP_EXIT_MANY, BRP_DPTK_FEW, BRP_DPTK_MANY,

		BRP_RET_SPTK_FEW, BRP_RET_SPTK_MANY, BRP_RET_DPTK_FEW, BRP_RET_DPTK_MANY,

		// Bank Switch
		BSW_0, BSW_1,

        // Speculation check instructions
        CHK_A, CHK_S,

        // Clear Rotating Register Base
        CLRRRB, CLRRRB_PR,

        // Compare and Exchange
        CMPXCHG1, CMPXCHG2, CMPXCHG4, CMPXCHG8,

        // Cover stack frame
        COVER,

        // Compute zero index
        CZX1, CZX2,

        // Deposit
        DEPOSIT, DEPOSIT_ZERO,

        // Enter privileged code
        EPC,

        // Extract
        EXTRACT, EXTRACT_UNSIGNED,

        // Floating abs. max
        FAMAX,

        // Floating abs. min
        FAMIN,

        // Floating and
        FAND,

        // Floating and complement
        FANDCM,

        // Flush cache
        FC,

        // Floating check flags
        FCHKF,

        // Floating class
        FCLASS,

        // Floating clear flags
        FCLRF,

        // Floating convert to integer
        FCVT_FX, FCVT_FX_TRUNC, FCVT_FXU, FCVT_FXU_TRUNC,

        // Convert signed to floating point
        FCVT_XF,

        // Fetch and Add
        FETCH_ADD4, FETCH_ADD8,

        // Flush Register Stack
        FLUSHRS,

        // Floating Multiply Add -- Also used to encode FADD and FMULT
        FMA_SINGLE,
        FMA_DOUBLE,
        FMA_DYNAMIC,

        // Floating max
        FMAX,

        // Floating Point Merge
        FMERGE_NEG_SIGN, FMERGE_SIGN, FMERGE_SIGN_EXP,

        // Floating min
        FMIN,

        // Floating mix
        FMIX,

        // Floating Multiply Subtract -- used to encode FSUB
        // We're ignoring status fields here for the time being.
        FMS_SINGLE, FMS_DOUBLE, FMS_DYNAMIC,

        // Floating Negative Multiply Add
        FNMA_SINGLE,
        FNMA_DOUBLE,
        FNMA_DYNAMIC,

        // Floating or
        FOR,

        // Floating pack
        FPACK,

        // SIMD Floating Opcodes
        FPAMAX, FPAMIN, FPCVT_FX, FPCVT_FX_TRUNC, FPCVT_FXU, FPCVT_FXU_TRUNC,
        FPMA, FPMAX, FPMERGE_NEG_SIGN, FPMERGE_SIGN, FPMERGE_SIGN_EXP, FPMIN,
        FPMS, FPNMA, FPRCPA, FPRSQRTA,

        // Floating point reciprocal approximation
        FRCPA,

        // Floating point reciprocal square root approximation
        FRSQRTA,

        // Floating select
        FSELECT,

        // Floating set controls
        FSETC,

        // Floating swap
        FSWAP,

        // Floating sign extend
        FSXT,

        // Flush Write Buffers
        FWB,

        // Floating xor
        FXOR,

        // Get Floating Point Value
        GETF_SINGLE, GETF_DOUBLE, GETF_EXP, GETF_SIG,

        // Halt
        HALT,

        // Invalidate ALAT
        INVALA,

        // Insert translation cache
        ITC_I, ITC_D,

        // Insert Translation Register
        ITR_I, ITR_D,

        // Line prefetch
        LFETCH, LFETCH_EXCL,

        // Load Register Stack
        LOADRS,

        // Memory Fence
        MEM_FENCE,

        // Mix
        MIX1, MIX2, MIX4,

        // Mux
        MUX1, MUX2,

        // NOP'S
        NOP_B, NOP_F, NOP_I, NOP_M, NOP_X,

        // Pack
        PACK2, PACK4,

        // Parallel add half word (other lengths already in x86: e.g. mmxpaddb)
        PADD2,

        // Parallel avg half word (other lengths already in x86: e.g. mmxpavgb)
        PAVG2,

        // Parallel avg subtract
        PAVGSUB1, PAVGSUB2,

        // Parallel max
        PMAX1, PMAX2,

        // Parallel min
        PMIN1, PMIN2,

        // Parallel multiply
        PMPY2,

        // Parallel multiply and shift right
        PMPYSHR2,

        // Population count
        POPCOUNT,

        // Probe access
        PROBE_R, PROBE_W, PROBE_RW,

        // Parallel sum absolute difference
        PSAD1,

        // Parallel shift left
        PSHL2, PSHL4,

         // Parallel shift left and add
        PSHLADD2,

        // Parallel shift right
        PSHR2, PSHR4,

         // Parallel shift right and add
        PSHRADD2,

        // Parallel subtract half (other lengths in x86)
        PSUB2,

        // Purge Translation Cache
        PTC_E, PTC_G, PTC_GA, PTC_L,

        // Purge Translation Register
        PTR_I, PTR_D,

        // Reset System/User Mask
        RSM, RUM,

        // Set Floating Point Value, Exponents or Significand
        SETF_SINGLE, SETF_DOUBLE, SETF_EXP, SETF_SIG,

        // Shift left and add
        SHLADD, SHLADDP4,

        // Shift right pair
        SHRP,

        // Serialize
        SRLZ_I,  SRLZ_D,

        // Set System Mask
        SSM,

        // Set user mask
        SUM,

        // Sign Extension
        SXT4, SXT2, SXT1,

        // Memory synchronization
        SYNC,

        // Translation access key
        TAK,

        // Translation hashed entry address
        THASH,

        // Translate to physical address
        TPA,

        // Translation hashed entry tag
        TTAG,

        // Unpack
        UNPACK1, UNPACK2, UNPACK4,

        // Exchange
        XCHG1, XCHG2, XCHG4, XCHG8,

        // xma -- fixed point multiply and accumulate
        XMA_LOW, XMA_HIGH, XMA_HIGH_UNSIGNED,

        // Zero Extend
        ZXT4, ZXT2, ZXT1,

        // END: IA64 specifics

        // UNDONE: To be moved near all the MOV instructions
        ICMOVO,       ICMOVNO,      ICMOVB,       ICMOVAE,
        ICMOVE,       ICMOVNE,      ICMOVBE,      ICMOVA,

        ICMOVS,       ICMOVNS,      ICMOVPE,      ICMOVNP,
        ICMOVL,       ICMOVGE,      ICMOVLE,      ICMOVG,

        ICMOVOW,      ICMOVNOW,     ICMOVBW,      ICMOVAEW,
        ICMOVEW,      ICMOVNEW,     ICMOVBEW,     ICMOVAW,

        ICMOVSW,      ICMOVNSW,     ICMOVPEW,     ICMOVNPW,
        ICMOVLW,      ICMOVGEW,     ICMOVLEW,     ICMOVGW,

        ISYSCALL,     ISYSENTER,

        MMXPMINUB,    MMXPAVGB,     MMXPAVGW,     MMXPMAXSW,
        MMXMASKMOVQ,  MMXPMULHUW,   MMXPMAXUB,    MMXPSADBW,
        MMXPMINSW,    MMXPSHUFW,    MOVNTQ,       CMPSS,
        CMPPS,

        MOVWO,
        IPAUSE,
        KATMAI3,      // For overloaded non-escaped opcodes


        // AXP Instructions

        // UNDONE: all instructions are here; need to figure out which
        // ones should be mapped to 'generic' IR opcodes.

        // Ones commented out are duplicates of previous opcodes

        //ADDD,
        ADDF, ADDG, ADDL,
        ADDL_V, ADDQ, ADDQ_V, ADDS, ADDT,
        AMASK, AND,
        BIC, BIS,
        CALL_PAL,
        CMOVEQ, CMOVGE, CMOVGT, CMOVLBC, CMOVLBS, CMOVLE, CMOVLT, CMOVNE,
        CMPBGE, CMPDEQ, CMPDLE, CMPDLT, CMPEQ, CMPGEQ, CMPGLE, CMPGLT, CMPLE,
        CMPLT, CMPTEQ, CMPTLE, CMPTLT, CMPTUN, CMPULE, CMPULT,
        CPYS, CPYSE, CPYSEE, CPYSN,
        CTLZ, CTPOP, CTTZ,
        CVTDF, CVTDG, CVTDQ, CVTFG, CVTGD, CVTGF, CVTGQ, CVTLQ,
        CVTQD, CVTQF, CVTQG, CVTQL, CVTQS, CVTQT, CVTTQ, CVTTS,
        DIVD, DIVF, DIVG, DIVS, DIVT,
        ECB, EQV,
        EXCB, EXTBL, EXTLH, EXTLL, EXTQH, EXTQL, EXTWH, EXTWL,
        FCMOVEQ, FCMOVGE, FCMOVGT, FCMOVLE, FCMOVLT,
        //FCMOVNE,
        FETCH, FETCH_M,
        FTOIS, FTOIT,
        HW_LD, HW_MFPR, HW_MTPR, HW_REI, HW_ST,
        IMPLVER,
        INSBL, INSLH, INSLL, INSQH, INSQL, INSWH, INSWL,
        ITOFF, ITOFS, ITOFT,
        LDA, LDAH, LDBU, LDF, LDG, LDL, LDL_L,
        LDQ, LDQ_L, LDQ_U, LDS, LDT, LDWU,
        MAXSB8, MAXSW4, MAXUB8, MAXUW4,
        MB, MB2, MB3,
        MF_FPCR,
        MINSB8, MINSW4, MINUB8, MINUW4,
        MSKBL, MSKLH, MSKLL, MSKQH, MSKQL, MSKWH, MSKWL,
        MT_FPCR,
        MULD, MULF, MULG, MULL, MULL_V, MULQ, MULQ_V, MULS, MULT,
        ORNOT, PERR, PKLB, PKWB,
        RC,
        RPCC, RS,
        S4ADDL, S4ADDQ, S4SUBL, S4SUBQ,
        S8ADDL, S8ADDQ, S8SUBL, S8SUBQ,
        SEXTB, SEXTW,
        SLL,
        SQRTF, SQRTG, SQRTS, SQRTT,
        SRA, SRL,
        //STB, STW,
        STF, STG, STL, STL_C, STQ, STQ_C, STQ_U, STS, STT,
        //SUBD,
        SUBF, SUBG, SUBL, SUBL_V, SUBQ, SUBQ_V, SUBS, SUBT,
        TRAPB,
        UMULH,
        UNPKBL, UNPKBW,
        WH64, WMB,
        XOR,
        ZAP, ZAPNOT,
        PADD, PARCP, PARCPH, PARCPL,
        PARSQRT, PARSQRTH, PARSQRTL,
        PCADD, PCMPEQ, PCMPLE, PCMPLT, PCMPNEQ, PCMPUN,
        PCPYS, PCPYSE, PCPYSN, PCSUB, PCVTFI, PCVTSP,
        PEXTH, PEXTL, PFMAX, PFMIN,
        PHADD, PHRSUB, PHSUB, PMOVHH,
        PMOVHL, PMOVLH, PMOVLL,
        PMUL, PMULH, PMULHN, PMULL, PMULLN, PSUB,

        // AXP Pseudo Instructions

        //BR,
        CLR, CVTST, CVTST_S,
        //FABS,
        FCLR, FMOV, FNEG,
        //FNOP,
        //MF_FPCR,
        MOV,
        //MT_FPCR,
        NEGF, NEGG, NEGL, NEGL_V, NEGQ, NEGQ_V, NEGS, NEGT,
        //NOP,
        NOT, SEXTL, UNOP,

        // End AXP Instructions

        // MSIL Instructions -- from tools\dis\inc\opcode.def
        // All real MSIL instructions must be enclosed between the
        // two pseudo instructions MSIL_BEGIN_MARKER and MSIL_END_MARKER.
        //
        MSIL_BEGIN_MARKER,
        CEE_NOP,
        CEE_BREAK,
        CEE_LDARG_0,
        CEE_LDARG_1,
        CEE_LDARG_2,
        CEE_LDARG_3,
        CEE_LDLOC_0,
        CEE_LDLOC_1,
        CEE_LDLOC_2,
        CEE_LDLOC_3,
        CEE_STLOC_0,
        CEE_STLOC_1,
        CEE_STLOC_2,
        CEE_STLOC_3,
        CEE_LDARG_S,
        CEE_LDARGA_S,
        CEE_STARG_S,
        CEE_LDLOC_S,
        CEE_LDLOCA_S,
        CEE_STLOC_S,
        CEE_LDNULL,
        CEE_LDC_I4_M1,
        CEE_LDC_I4_0,
        CEE_LDC_I4_1,
        CEE_LDC_I4_2,
        CEE_LDC_I4_3,
        CEE_LDC_I4_4,
        CEE_LDC_I4_5,
        CEE_LDC_I4_6,
        CEE_LDC_I4_7,
        CEE_LDC_I4_8,
        CEE_LDC_I4_S,
        CEE_LDC_I4,
        CEE_LDC_I8,
        CEE_LDC_R4,
        CEE_LDC_R8,
        CEE_LDPTR,
        CEE_DUP,
        CEE_POP,
        CEE_JMP,
        CEE_CALL,
        CEE_CALLI,
        CEE_RET,
        CEE_BR_S,
        CEE_BRFALSE_S,
        CEE_BRTRUE_S,
        CEE_BEQ_S,
        CEE_BGE_S,
        CEE_BGT_S,
        CEE_BLE_S,
        CEE_BLT_S,
        CEE_BNE_UN_S,
        CEE_BGE_UN_S,
        CEE_BGT_UN_S,
        CEE_BLE_UN_S,
        CEE_BLT_UN_S,
        CEE_BR,
        CEE_BRFALSE,
        CEE_BRTRUE,
        CEE_BEQ,
        CEE_BGE,
        CEE_BGT,
        CEE_BLE,
        CEE_BLT,
        CEE_BNE_UN,
        CEE_BGE_UN,
        CEE_BGT_UN,
        CEE_BLE_UN,
        CEE_BLT_UN,
        CEE_SWITCH,
        CEE_LDIND_I1,
        CEE_LDIND_U1,
        CEE_LDIND_I2,
        CEE_LDIND_U2,
        CEE_LDIND_I4,
        CEE_LDIND_U4,
        CEE_LDIND_I8,
        CEE_LDIND_I,
        CEE_LDIND_R4,
        CEE_LDIND_R8,
        CEE_LDIND_REF,
        CEE_STIND_REF,
        CEE_STIND_I1,
        CEE_STIND_I2,
        CEE_STIND_I4,
        CEE_STIND_I8,
        CEE_STIND_R4,
        CEE_STIND_R8,
        CEE_ADD,
        CEE_SUB,
        CEE_MUL,
        CEE_DIV,
        CEE_DIV_UN,
        CEE_REM,
        CEE_REM_UN,
        CEE_AND,
        CEE_OR,
        CEE_XOR,
        CEE_SHL,
        CEE_SHR,
        CEE_SHR_UN,
        CEE_NEG,
        CEE_NOT,
        CEE_CONV_I1,
        CEE_CONV_I2,
        CEE_CONV_I4,
        CEE_CONV_I8,
        CEE_CONV_R4,
        CEE_CONV_R8,
        CEE_CONV_U4,
        CEE_CONV_U8,
        CEE_CALLVIRT,
        CEE_CPOBJ,
        CEE_LDOBJ,
        CEE_LDSTR,
        CEE_NEWOBJ,
        CEE_CASTCLASS,
        CEE_ISINST,
        CEE_CONV_R_UN,
        CEE_ANN_DATA_S,
        CEE_BOX,
        CEE_UNBOX,
        CEE_THROW,
        CEE_LDFLD,
        CEE_LDFLDA,
        CEE_STFLD,
        CEE_LDSFLD,
        CEE_LDSFLDA,
        CEE_STSFLD,
        CEE_STOBJ,
        CEE_CONV_OVF_I1_UN,
        CEE_CONV_OVF_I2_UN,
        CEE_CONV_OVF_I4_UN,
        CEE_CONV_OVF_I8_UN,
        CEE_CONV_OVF_U1_UN,
        CEE_CONV_OVF_U2_UN,
        CEE_CONV_OVF_U4_UN,
        CEE_CONV_OVF_U8_UN,
        CEE_CONV_OVF_I_UN,
        CEE_CONV_OVF_U_UN,
        CEE_UNUSED49,
        CEE_NEWARR,
        CEE_LDLEN,
        CEE_LDELEMA,
        CEE_LDELEM_I1,
        CEE_LDELEM_U1,
        CEE_LDELEM_I2,
        CEE_LDELEM_U2,
        CEE_LDELEM_I4,
        CEE_LDELEM_U4,
        CEE_LDELEM_I8,
        CEE_LDELEM_I,
        CEE_LDELEM_R4,
        CEE_LDELEM_R8,
        CEE_LDELEM_REF,
        CEE_STELEM_I,
        CEE_STELEM_I1,
        CEE_STELEM_I2,
        CEE_STELEM_I4,
        CEE_STELEM_I8,
        CEE_STELEM_R4,
        CEE_STELEM_R8,
        CEE_STELEM_REF,
        CEE_UNUSED2,
        CEE_UNUSED3,
        CEE_UNUSED4,
        CEE_UNUSED5,
        CEE_UNUSED6,
        CEE_UNUSED7,
        CEE_UNUSED8,
        CEE_UNUSED9,
        CEE_UNUSED10,
        CEE_UNUSED11,
        CEE_UNUSED12,
        CEE_UNUSED13,
        CEE_UNUSED14,
        CEE_UNUSED15,
        CEE_UNUSED16,
        CEE_UNUSED17,
        CEE_CONV_OVF_I1,
        CEE_CONV_OVF_U1,
        CEE_CONV_OVF_I2,
        CEE_CONV_OVF_U2,
        CEE_CONV_OVF_I4,
        CEE_CONV_OVF_U4,
        CEE_CONV_OVF_I8,
        CEE_CONV_OVF_U8,
        CEE_UNUSED50,
        CEE_UNUSED18,
        CEE_UNUSED19,
        CEE_UNUSED20,
        CEE_UNUSED21,
        CEE_UNUSED22,
        CEE_UNUSED23,
        CEE_REFANYVAL,
        CEE_CKFINITE,
        CEE_UNUSED24,
        CEE_UNUSED25,
        CEE_MKREFANY,
        CEE_ANN_CALL,
        CEE_ANN_CATCH,
        CEE_ANN_DEAD,
        CEE_ANN_HOISTED,
        CEE_ANN_HOISTED_CALL,
        CEE_ANN_LAB,
        CEE_ANN_DEF,
        CEE_ANN_REF_S,
        CEE_ANN_PHI,
        CEE_LDTOKEN,
        CEE_CONV_U2,
        CEE_CONV_U1,
        CEE_CONV_I,
        CEE_CONV_OVF_I,
        CEE_CONV_OVF_U,
        CEE_ADD_OVF,
        CEE_ADD_OVF_UN,
        CEE_MUL_OVF,
        CEE_MUL_OVF_UN,
        CEE_SUB_OVF,
        CEE_SUB_OVF_UN,
        CEE_ENDFINALLY,
        CEE_LEAVE,
        CEE_LEAVE_S,
        CEE_STIND_I,
        CEE_CONV_U,
        CEE_UNUSED26,
        CEE_UNUSED27,
        CEE_UNUSED28,
        CEE_UNUSED29,
        CEE_UNUSED30,
        CEE_UNUSED31,
        CEE_UNUSED32,
        CEE_UNUSED33,
        CEE_UNUSED34,
        CEE_UNUSED35,
        CEE_UNUSED36,
        CEE_UNUSED37,
        CEE_UNUSED38,
        CEE_UNUSED39,
        CEE_UNUSED40,
        CEE_UNUSED41,
        CEE_UNUSED42,
        CEE_UNUSED43,
        CEE_UNUSED44,
        CEE_UNUSED45,
        CEE_UNUSED46,
        CEE_UNUSED47,
        CEE_UNUSED48,
        CEE_PREFIX7,
        CEE_PREFIX6,
        CEE_PREFIX5,
        CEE_PREFIX4,
        CEE_PREFIX3,
        CEE_PREFIX2,
        CEE_PREFIX1,
        CEE_PREFIXREF,
        CEE_ARGLIST,
        CEE_CEQ,
        CEE_CGT,
        CEE_CGT_UN,
        CEE_CLT,
        CEE_CLT_UN,
        CEE_LDFTN,
        CEE_LDVIRTFTN,
        CEE_JMPI,
        CEE_LDARG,
        CEE_LDARGA,
        CEE_STARG,
        CEE_LDLOC,
        CEE_LDLOCA,
        CEE_STLOC,
        CEE_LOCALLOC,
        CEE_UNUSED57,
        CEE_ENDFILTER,
        CEE_UNALIGNED,
        CEE_VOLATILE,
        CEE_TAILCALL,
        CEE_INITOBJ,
        CEE_ANN_LIVE,
        CEE_CPBLK,
        CEE_INITBLK,
        CEE_ANN_REF,
        CEE_RETHROW,
        CEE_UNUSED51,
        CEE_SIZEOF,
        CEE_REFANYTYPE,
        CEE_UNUSED52,
        CEE_UNUSED53,
        CEE_UNUSED54,
        CEE_UNUSED55,
        CEE_ANN_DATA,
        CEE_ILLEGAL,
        CEE_MACRO_END,
        MSIL_END_MARKER,
        // End MSIL Instructions.

        EOpMax,

        // Special opcodes for X86 escaped opcode groups
        G0=0,   G1=1,   G2=2,   G3=3,   G4=4,   G5=5,   G6=6,   G7=7,
        G8=8,   G9=9,   G10=10, G11=11, G12=12, G13=13, G14=14, G15=15,
        G16=16, G17=17, G18=18, G19=19, G20=20, G21=21
    };

    enum LVMOp
    {
        AddF4, ConvertF8F4, DivF4, LoadF4, MulF4, NegF4, PushConstF4, PushRefF4,
        PushRegF4, SubF4, AddF8, ConvertF4F8, ConvertI8F8, DivF8, LoadF8, MulF8,
        NegF8, PushConstF8, PushRefF8, PushRegF8, SubF8, EqF4, EqF8, EqI1,
        EqI2, EqI4, EqI8, EqU1, EqU2, EqU4, EqU8, GeF4,
        GeF8, GeI1, GeI2, GeI4, GeI8, GeU1, GeU2, GeU4,
        GeU8, GtF4, GtF8, GtI1, GtI2, GtI4, GtI8, GtU1,
        GtU2, GtU4, GtU8, LeF4, LeF8, LeI1, LeI2, LeI4,
        LeI8, LeU1, LeU2, LeU4, LeU8, LtF4, LtF8, LtI1,
        LtI2, LtI4, LtI8, LtU1, LtU2, LtU4, LtU8, NeF4,
        NeF8, NeI1, NeI2, NeI4, NeI8, NeU1, NeU2, NeU4,
        NeU8, AddI1, AddU1, AndU1, DivI1, DivU1, LShiftI1, LShiftU1,
        LoadU1, ModI1, ModU1, MulI1, MulU1, NarrowI2I1, NarrowU2U1, NegI1,
        NotU1, OrU1, PushConstU1, PushRefU1, PushRegU1, RShiftI1,RShiftU1,SubI1,
        SubU1, XorU1, AddI2, AddU2, AndU2, DivI2, DivU2, ExtendI1I2,
        ExtendU1U2, LShiftI2, LShiftU2, LoadU2, ModI2, ModU2, MulI2, MulU2,
        NarrowI4I2, NarrowU4U2,NegI2,NotU2,OrU2,PushConstU2,PushRefU2,PushRegU2,
        RShiftI2, RShiftU2, SubI2, SubU2, XorU2, AddI4, AddU4, AndU4,
        DivI4, DivU4, ExtendI2I4, ExtendU2U4, LShiftI4, LShiftU4, LoadU4, ModI4,
        ModU4, MulI4, MulU4, NegI4, NotU4, OrU4, PushConstU4, PushRefU4,
        PushRegU4, RShiftI4, RShiftU4, SubI4, SubU4, XorU4, AddI8, AddU8,
        AndU8, ConvertF8I8, DivI8,DivU8,ExtendI4I8,ExtendU4U8,LShiftI8,LShiftU8,
        LoadU8, ModI8, ModU8, MulI8, MulU8, NegI8, NotU8, OrU8,
        PushConstU8, PushRefU8, PushRegU8, RShiftI8, RShiftU8,SubI8,SubU8,XorU8,
        ConvertU4Ptr, BrFalse, BrTrue, Def, EndOfInput, Epilogue, Jsr, Jump,
        PopF4, PopF8, PopRegF4, PopRegF8, PopRegU1, PopRegU2, PopRegU4,PopRegU8,
        PopU1, PopU2, PopU4, PopU8, Prologue, StoreF4, StoreF8, StoreU1,
        StoreU2, StoreU4, StoreU8, End
    };

    static VULCANDLL bool VULCANCALL IsMSIL(EOp eop)
    {
       return (eop > MSIL_BEGIN_MARKER && eop < MSIL_END_MARKER);
    }

    static VULCANDLL EOp VULCANCALL GetReverseCondBranchOpcode(EOp eop)
    {
        switch (eop)
        {
            case BULT: return BUGE;
            case BULE: return BUGT;
            case BUGT: return BULE;
            case BUGE: return BULT;

            case BLT:  return BGE;
            case BLE:  return BGT;
            case BGT:  return BLE;
            case BGE:  return BLT;

            case BO:   return BNO;
            case BNO:  return BO;
            case BP:   return BNP;
            case BNP:  return BP;

            case BS:   return BNS;
            case BNS:  return BS;
            case BZ:   return BNZ;
            case BNZ:  return BZ;

            case BEQ:  return BNE;
            case BNE:  return BEQ;

            case CEE_BEQ:        return CEE_BNE_UN;
            case CEE_BEQ_S:      return CEE_BNE_UN_S;
            case CEE_BGE:        return CEE_BLT;
            case CEE_BGE_S:      return CEE_BLT_S;
            case CEE_BGE_UN:     return CEE_BLT_UN;
            case CEE_BGE_UN_S:   return CEE_BLT_UN_S;
            case CEE_BGT:        return CEE_BLE;
            case CEE_BGT_S:      return CEE_BLE_S;
            case CEE_BGT_UN:     return CEE_BLE_UN;
            case CEE_BGT_UN_S:   return CEE_BLE_UN_S;
            case CEE_BLE:        return CEE_BGT;
            case CEE_BLE_S:      return CEE_BGT_S;
            case CEE_BLE_UN:     return CEE_BGT_UN;
            case CEE_BLE_UN_S:   return CEE_BGT_UN_S;
            case CEE_BLT:        return CEE_BGE;
            case CEE_BLT_S:      return CEE_BGE_S;
            case CEE_BLT_UN:     return CEE_BGE_UN;
            case CEE_BLT_UN_S:   return CEE_BGE_UN_S;
            case CEE_BNE_UN:     return CEE_BEQ;
            case CEE_BNE_UN_S:   return CEE_BEQ_S;

            case CEE_BRTRUE:     return CEE_BRFALSE;
            case CEE_BRFALSE:    return CEE_BRTRUE;
            case CEE_BRTRUE_S:   return CEE_BRFALSE_S;
            case CEE_BRFALSE_S:  return CEE_BRTRUE_S;

            default:   break;
        }

        return(eop);   // return the old opcode with no change
    }

    static VULCANDLL bool VULCANCALL IsCondBranchReversable(EOp eop)
    {
       if ((eop >= BULT && eop <= BNE))
       {
          return (true);
       }

       // MSIL
       if ((eop >= CEE_BEQ && eop <= CEE_BLT_UN) || (eop >= CEE_BEQ_S && eop <= CEE_BLT_UN_S))
       {
          return (true);
       }

       if ((eop == CEE_BRFALSE ||
            eop == CEE_BRTRUE ||
            eop == CEE_BRFALSE_S ||
            eop == CEE_BRTRUE_S))
       {
          return (true);
       }

       return (false);
    }

    static VULCANDLL bool VULCANCALL IsCondBranch(EOp eop)
    {
       if (eop >= BULT && eop <= IBCXZ)
       {
          return (true);
       }

       // MSIL
       if ((eop >= CEE_BEQ && eop <= CEE_BLT_UN) || (eop >= CEE_BEQ_S && eop <= CEE_BLT_UN_S))
       {
          return (true);
       }

       if ((eop == CEE_BRFALSE ||
            eop == CEE_BRTRUE ||
            eop == CEE_BRFALSE_S ||
            eop == CEE_BRTRUE_S))
       {
          return (true);
       }

       return (false);
    }

    static VULCANDLL bool VULCANCALL IsUnCondBranch(EOp eop)
    {
       return ((eop >= JMP  && eop <= JSR_COROUTINE_JMPI) ||
               eop == CEE_BR || eop == CEE_BR_S ||
               eop == CEE_LEAVE || eop == CEE_LEAVE_S);
    }

    static VULCANDLL bool VULCANCALL IsRet(EOp eop)
    {
       return ((eop >= RET  && eop <= RET_JMPI) || eop == CEE_RET );
    }

    static VULCANDLL bool VULCANCALL IsCall(EOp eop)
    {
       return ((eop >= CALL && eop <= JSR_COROUTINE_CALLI) ||
               eop == CEE_CALL || eop == CEE_CALLI ||
               eop == CEE_NEWOBJ || eop == CEE_JMP || eop == CEE_JMPI );
    }

    static VULCANDLL bool VULCANCALL IsIA64Compare(EOp op)
    {
       switch (op)
       {
          case COp::FCMP_EQ:
          case COp::FCMP_LT:
          case COp::FCMP_LE:
          case COp::FCMP_UNORD:

          case COp::CMP_EQ:
          case COp::CMP_LT:
          case COp::CMP_LTU:
          case COp::CMP_NE:
          case COp::CMP_LE:
          case COp::CMP_GT:
          case COp::CMP_GE:

          case COp::CMP4_EQ:
          case COp::CMP4_LT:
          case COp::CMP4_LTU:
          case COp::CMP4_NE:
          case COp::CMP4_LE:
          case COp::CMP4_GT:
          case COp::CMP4_GE:

          case COp::TBIT_Z:
          case COp::TBIT_NZ:

          case COp::TNAT_Z:
          case COp::TNAT_NZ:

          case COp::PCMP1_EQ:
          case COp::PCMP1_GT:
          case COp::PCMP2_EQ:
          case COp::PCMP2_GT:
          case COp::PCMP4_EQ:
          case COp::PCMP4_GT:

             return true;

          default:

             return false;
       }
    }

    static VULCANDLL bool VULCANCALL Is2SlotInstruction(EOp eop)
    {
       return ((eop >= BRL_CALL_SPNT_FEW && eop <= BRL_CALL_DPTK_MANY) ||
               (eop >= BRL_COND_SPNT_FEW && eop <= BRL_COND_DPTK_MANY) ||
                eop == MOVL || eop == NOP_X || eop == BREAK_X);
    }

    static VULCANDLL bool VULCANCALL IsIA64CheckInstruction(EOp eop)
    {
        return eop == CHK_S || eop == CHK_A;
    }

    static VULCANDLL bool VULCANCALL IsData(EOp eop)
        {return eop == DATA;}

    static VULCANDLL bool VULCANCALL IsAddr(EOp eop)
        {return eop == ADDR;}

    static VULCANDLL bool VULCANCALL IsLoop(EOp eop)
        {return (eop >= ILOOPN && eop <=ILOOPE);}

    static VULCANDLL const char * VULCANCALL OpcodeStr(EOp eop);
    static VULCANDLL EOpGroup VULCANCALL OpcodeGrp(EOp eop);
    static VULCANDLL const char * VULCANCALL OpcodeGrpStr(EOp eop);
};

typedef COp::EOp EOpcodes;
typedef COp::EOpGroup EOpGrp;
typedef enum IA64Completers ECompleters;

#endif //__VULCANIENUM__

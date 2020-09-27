//
// MODEHW.H - Header file for OS specific portions of mode sets in 'C'.
//
// Copyright (c) 1997 - NVidia Corp.
// All Rights Reserved.
//
// Defines for accessing NV registers in the 'C' based modeset code.
//
//
VOID WriteFb32(U032 datum, U032 addr);
U032 ReadFb32(U032 addr);
VOID WritePriv32(U032 datum, U032 addr);
U032 ReadPriv32(U032 addr);
VOID WriteIndexed(U032 addr, U016 datum);
VOID WritePriv08(U032 addr, U008 datum);
U016 ReadIndexed(U032 addr, U008 reg);

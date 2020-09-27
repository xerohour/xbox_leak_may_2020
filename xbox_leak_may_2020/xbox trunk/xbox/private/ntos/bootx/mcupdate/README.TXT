The Production versions of the Value Microcode Updates for the Intel(r)
Celeron(tm) processors are included in this ZIP file, Revision 03.

The Microcode Updates are covered by the following royalty free software
license agreement:

-----------------------------------------------------------------------------
(c) Intel Corporation, 1995, 1996, 1997, 1998, 1999, 2000, 2001.
These microcode updates are distributed for the sole purpose of 
installation in the BIOS or Operating System of computer systems
which include a Genuine Intel microprocessor sold or distributed
to or by you. You are not authorized to use this material for
any other purpose.  
-----------------------------------------------------------------------------

Microcode Updates are platform specific, DO NOT USE THESE MICROCODE UPDATES
with Pentium(r) II processors, Pentium(r) III processors, Pentium(r) II
Xeon(tm) processors or Pentium(r) III Xeon processors!

Intel recommends that the appropriate Microcode Update be loaded into
all Pentium(r) Pro processor, Pentium(r) II processor, Pentium(r) II Xeon(tm)
processor, Pentium(r) III processor, Pentium(r) III Xeon(tm) processor,
Pentium(r) II OverDrive(r) processor and Intel(r) Celeron(tm) processors.
Intel P6 family processors are only validated with the appropriate Microcode
Update
applied.

Included are the Microcode Updates for the Intel(r) Celeron(tm) processor,
CPUID 065x, A0, and A1 stepping.

Included are the Microcode Updates for the Intel(r) Celeron(tm) processor,
CPUID 066x, A0 and B0 stepping.

Included are the Microcode Updates for the Intel(r) Celeron(tm) processor,
CPUID 068X, B0, C0, D0 stepping.

Please see the latest NDA revision of the Intel(r) Celeron(tm) Processor
Specification Update for the list of errata and errata fixed by these
microcode updates for the Intel(r) Celeron(tm) processors.

Two formats are provided:
1) .TXT which is an assembly language version
2) .PDB a database file used by the CHECKUP6.EXE program

File
Name                Description                           Stepping(s)
-------------       ------------------------------        --------------------
MU165040.TXT        Assembly format, Revision 40          CPUID 650, A0     
MU165140.TXT        Assembly format, Revision 40          CPUID 651, A1     
MU16600a.TXT        Assembly format, Revision 0a          CPUID 660, A0    
MU166503.TXT        Assembly format, Revision 03          CPUID 665, B0
MU168310.TXT        Assembly format, Revision 10          CPUID 683, B0
MU168608.TXT        Assembly format, Revision 08          CPUID 686, C0
MU168A01.TXT        Assembly format, Revision 01          CPUID 68A, D0
VAL_P_03.PDB        Database format, Revision 03

Please see the Revision 3.7 of the Extensions to the Pentium(r) Pro Processor
BIOS Writer's Guide, Chapter 9 for further information on Microcode Updates 
and the INT15 Microcode Update API.
    
The assembly version of the Microcode Updates use the following naming 
conventions:

"MUabcdef.txt", where:
MU    = Microcode Update.
"a"   = Designates 1 for SC242 and PGA370, 2 for SC330
"b"   = Reflects the family as reported by CPUID instruction.
"c"   = Reflects the model as reported by CPUID instruction.
"d"   = Reflects the stepping as reported by CPUID instruction.
"ef"  = Reflects the revision number of the Microcode Update for the 
        particular stepping.

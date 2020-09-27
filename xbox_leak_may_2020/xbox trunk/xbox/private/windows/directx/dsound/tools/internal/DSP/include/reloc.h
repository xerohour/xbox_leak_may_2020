struct reloc {
	long r_vaddr;	/* (virtual) address of reference */
	long	r_symndx;	/* index into symbol table */
	unsigned long	r_type;	/* relocation type */
	};

/*
 *   relocation types for all products and generics
 */

/*
 * All generics
 *	reloc. already performed to symbol in the same section
 */
#define  R_ABS		0L
#define RELOC  struct reloc
#define RELSZ sizeof(RELOC)


        asm volatile(
        "movb (%%esi),%%cl\n\t"
        "movb 8(%%esi),%%dl\n\t"
        "movl %%ebx,%%esi\n\t"
        "addl %%eax,%%esi\n\t"
        "movb %%cl,%%bl\n\t"
        "movb %%dl,%%al\n\t"
        "shrb $1,%%bl\n\t"
        "andb $0xaa,%%al\n\t"
        "andb $0x55,%%bl\n\t"

        "andb $0x55,%%cl\n\t"
        "shlb $1,%%dl\n\t"
        "andb $0xaa,%%dl\n\t"
        "orb  %%al, %%bl\n\t"            // Stick c1 into bl
        "orb  %%cl, %%dl\n\t"           // Stick c2 into dl
        "xorl %%eax, %%eax\n\t"
        "xorl %%ecx, %%ecx\n\t"
        /*      At this point, bl contains c1, and dl contains c2 */
        /*      and edi contains P, esi contains VRAM[] */
        /*      al will be used for zz, cl will be used for zz2  */
        "movb %%bl,%%al\n\t"
        "movb %%dl,%%cl\n\t"
        "andb $3,%%al\n\t"
        "andb $3,%%cl\n\t"
        "movb (%%esi,%%eax),%%bh\n\t"
        "movb (%%esi,%%ecx),%%dh\n\t"
        "movb %%bh,6(%%edi)\n\t"
        "movb %%dh,7(%%edi)\n\t"

        "movb %%bl,%%al\n\t"
        "movb %%dl,%%cl\n\t"
        "shrb $2,%%al\n\t"
        "shrb $2,%%cl\n\t"
        "andb $3,%%al\n\t"
        "andb $3,%%cl\n\t"
        "movb (%%esi,%%eax),%%bh\n\t"
        "movb (%%esi,%%ecx),%%dh\n\t"
        "movb %%bh,4(%%edi)\n\t"
        "movb %%dh,5(%%edi)\n\t"

        "movb %%bl,%%al\n\t"
        "movb %%dl,%%cl\n\t"
        "shrb $4,%%al\n\t"
        "shrb $4,%%cl\n\t"
        "andb $3,%%al\n\t"
        "andb $3,%%cl\n\t"
        "movb (%%esi,%%eax),%%bh\n\t"
        "movb (%%esi,%%ecx),%%dh\n\t"
        "movb %%bh,2(%%edi)\n\t"
        "movb %%dh,3(%%edi)\n\t"

        "movb %%bl,%%al\n\t"
        "movb %%dl,%%cl\n\t"
        "shrb $6,%%al\n\t"
        "shrb $6,%%cl\n\t"
        "movb (%%esi,%%eax),%%bh\n\t"
        "movb (%%esi,%%ecx),%%dh\n\t"
        "movb %%bh,0(%%edi)\n\t"
        "movb %%dh,1(%%edi)\n\t"
        :
        : "D" (P), "S" (C), "a" (cc), "b" (VRAM+0x3f00)
        : "%ecx", "%edx"
        );


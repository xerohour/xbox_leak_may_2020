

; initialize DRAM using SDRAM Control Register  
;

        ; setup PCI cycle to 76h
        mov     eax, 080000074h
        mov     dx, 0CFEh

        ; NOP Command Enable
        mov     al, 20h 
        out     dx, al

        mov     cx, -1
@@:     loop    @B

         ; All Banks Precharge Enable
        mov     al, 40h  
        out     dx, al

    	mov	    ebx, ds:[0]

        mov     cx, 1000h
@@:     loop    @B
        	

        ; CBR Enable Command
        mov     al, 80h   
        out     dx, al

        mov     cx, 8
CBRLoop:
    	mov	    ebx, ds:[0]
        loop    CBRLoop


        ; Mode Register Set Enable Command 
        mov     al, 60h   
        out     dx, al

    	;mov     di, 01F0h    ;MKF_CL2_ADDRESS_FOR_MAA;CL2 address for Rows 3:0
	    ;mov     di, 0FEA8h   ; MKF_CL2_ADDRESS_FOR_MAB; No, get correct address
    	;mov     di, 01D0h    ; CL3

        mov     di, 0DE28h
	    mov     ebx, ds:[di]


        ; Normal SDRAM Operation
        mov     al, 0h   
        out     dx, al

        mov     ebx, ds:[0] 

; 
; initialize chipset registers
;
        xor     si, si
        xor     bx, bx

RAMInitLoop:
        mov     bl, cs:BYTE PTR RAMInitData[si]
        cmp     bl, 0
        je      RAMInitDone
            
        mov     eax, 080000000h
        mov     al, bl
        mov     dx, 0CF8h
        inc     si
        out     dx, eax
        
        mov     dx, 0CFCh
        mov     ax, bx
        mov     eax, cs:DWORD PTR RAMInitData[si]
        out     dx, eax
        add     si, 4
        jmp     RAMInitLoop
                
RAMInitDone:





;
; ram initialization data pairs (register offset byte, value dword)
;
RAMInitData:

        DB  050h
        DD  0FF00800Ch
        DB  054h
        DD  009000000h
        DB  058h
        DD  000111003h
        DB  05Ch
        DD  011110000h
        DB  060h
        DD  004040404h
        DB  064h
        DD  004040404h
        DB  068h
        DD  000000000h
        DB  06Ch
        DD  000000000h
        DB  074h
        DD  000000000h
        DB  078h
        DD  0381C0100h

        ; end of RAM init data
        DB  00h
    




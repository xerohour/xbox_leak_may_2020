//assemble this with /p.

//Expected results: "narf" should expand to "nop".
//Actual results:   "narf" expands to "blah".

xvs.1.1
macro blah
    nop
endm

macro narf
    blah
endm

narf
mov oPos, v0

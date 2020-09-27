;**************************************************************************
;
;       minirvb.h    
;
;**************************************************************************        

        IF @DEF('SIM')
    	org	x:$80
        ELSE
        org     x:
        ENDIF

NUM_REFLECTIONS  equ 8


StateBlock
FIXED_STATE_INFO ds	DATASIZE_DSP_FX_STATE_1IN_4OUT 

    ; pointers to the reflection buffers
    ; they must be right after the FX_STATE block so the overlap with the output IO pointers
    ; The tool will place GPTEMPBIN pointers at compile time

pTemp               dc 0
pReflection1        dc 0
pReflection2        dc 0
pReflection3        dc 0
pReflection4        dc 0
pReflection5        dc 0
pReflection6        dc 0
pReflection7        dc 0

Reflection0Length   dc 0
Reflection1Length   dc 0
Reflection2Length   dc 0
Reflection3Length   dc 0
Reflection4Length   dc 0
Reflection5Length   dc 0
Reflection6Length   dc 0
Reflection7Length   dc 0

Reflection0Tap      dc 0
Reflection1Tap      dc 0
Reflection2Tap      dc 0
Reflection3Tap      dc 0
Reflection4Tap      dc 0
Reflection5Tap      dc 0
Reflection6Tap      dc 0
Reflection7Tap      dc 0

Reflection0Gain     dc 0
Reflection1Gain     dc 0
Reflection2Gain     dc 0
Reflection3Gain     dc 0
Reflection4Gain     dc 0
Reflection5Gain     dc 0
Reflection6Gain     dc 0
Reflection7Gain     dc 0


NewIIRBlock macro

    dc  0
    dc  0
    dc  0

    endm

IIR0
    NewIIRBlock
IIR1
    NewIIRBlock


IIR_CoefficientA1   equ     0
IIR_CoefficientB0   equ     1
IIR_DelayElement    equ     2



; after this point all variables are internal to the dsp and not-downloadable


DMAReadBlock0
    NewDMABlock

DMAReadBlock1
    NewDMABlock

DMAReadBlock2
    NewDMABlock

DMAReadBlock3
    NewDMABlock

DMAReadBlock4
    NewDMABlock

DMAReadBlock5
    NewDMABlock

DMAReadBlock6
    NewDMABlock

DMAReadBlock7
    NewDMABlock


DMAWriteBlock0
    NewDMABlock

DMAWriteBlock1
    NewDMABlock

DMAWriteBlock2
    NewDMABlock

DMAWriteBlock3
    NewDMABlock

DMAWriteBlock4
    NewDMABlock

DMAWriteBlock5
    NewDMABlock

DMAWriteBlock6
    NewDMABlock

DMAWriteBlock7
    NewDMABlock






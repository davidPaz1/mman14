; file to put code failures in 

MAIN: mov M1[ r2][r7 ], M2[ r2][r7 ]
inc M1   [   r1   ]   [   r3   ]
STR: .string "abcdefg"
    add r2,STR

M1: .mat [2][6] -1,2,3,4, 5,24,34,24
.data 1, 2

ignoreLabel: .entry LOOP
.extern OutLabel
;  mov M1[ arr][r7 ], M2[ r2][r7 ]
; M: .entry invalidLabel


LOOP: jmp END
    prn #-5
    
    mov #5, r4
    cmp #7 , #5
;    mov 4invalidLabel  , 
;    cmp #71.5 , #5! , as
;    inc #a , 
;    lea r1 , #2
    mcro a_mc
    mov M1[r3][r3] , r3
    bne LOOP
    mcroend
    
    sub r1, r4
    inc K

    a_mc


END: stop
    mcro tesT
    sub r1, r4
    inc K
    sub r1, r4
    inc K
    mcroend

LENGTH: .data 6,-9,15
K: .data 22
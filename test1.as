MAIN: mov M1[ r2][r7 ], M2[ r2][r7 ]
inc M1   [   r1   ]   [   r3   ]
STR: .string "abcdefg"
M: .entry tesTa
    add r2,STR

M1: .mat [2][6] -1,2,3,4, 5,24,34,24
TEST: .data 1, 2

.extern OutLabel

LOOP: jmp END
    prn #-5
    
    mov  r1 , #5
    cmp #7 , #5
;    mov  #-1999  , 
;    cmp #71.5 , #5! , as
;    inc #a , 

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
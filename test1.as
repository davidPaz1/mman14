STR: .string "abcdefg"
M: .entry tesTa
MAIN: mov M1[r2][r7],LENGTH
    add r2,STR

M1: .mat [2][6] -1,2,3,4, 5,24,34,24
TEST: .data 1, 2

.extern OutLabel

LOOP: jmp END
    prn #-5
    
    mov  r1 , #5
    cmp #7 , #5

    mcro a_mc
    mov M1[r3][r3] , r3
    bne LOOP
    mcroend
    
    sub r1, r4
    inc K

    a_mc


END: stop

TRY: a_mc
    mcro tesT
    sub r1, r4
    inc K
    sub r1, r4
    inc K
    mcroend

LENGTH: .data 6,-9,15
K: .data 22
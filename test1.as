MAIN: mov M1[r2][r7],LENGTH
    add r2,STR

M1: .mat [2][9] -1,2,3,4, 5,24,3424,2432
TEST: .data 1 2
STR: .string "abcdef"

LOOP: jmp END
    prn #-5
    
;    mcro mov
;    mcro aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
;    mcroend

    mcro a_mc extraTxt
    mcro a_mc
    mov M1[r3][r3],r3
    bne LOOP
    mcroend extraTxt
    mcroend
    
    sub r1, r4
    inc K

    a_mc extraTxt


END: stop

TRY: a_mc
    mcro tes_T
    sub r1, r4
    inc K
    sub r1, r4
    inc K
    mcroend

LENGTH: .data 6,-9,15
K: .data 22
START: add r1, r2
        mcro a_mc
        mov M1[r3][r3],r3
        bne LOOP
        mcroend

LOOP: a_mc
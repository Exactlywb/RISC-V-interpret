
    addi x5, x0, 1
    addi x6, x0, 10
    addi x7, x0, 1

factorial:
    add x8, x0, x5
    addi x4, x0, 0
multiply:
    add x5, x5, x8
    addi x4, x4, 1
    bne x7, x4, multiply

    addi x7, x7, 1
    bne x6, x7, factorial
    
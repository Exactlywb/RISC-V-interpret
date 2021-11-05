addi x3, x0, 10
addi x2, x0, 0
addi x4, x0, 0

summ:

add x4, x4, x2
addi x2, x2, 1

bge x3, x2, summ

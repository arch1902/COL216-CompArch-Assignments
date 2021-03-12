addi $t1,$t0,5
addi $t2,$t0,1

sum:
    add $t1,$t1,$t2
    addi $t0,$t0,1
    beq $t0,5,exit
    j sum
exit:
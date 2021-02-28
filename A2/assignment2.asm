#  MIPS Assembly Program for evaluating an expression in postfix format

# ----------------------------------------
#  Data declarations go in this section.

.data
# program specific data declarations

two:        .double 2.00
zero:       .double 0.00
sum:        .word 0
iMsg:       .asciiz "Enter the postfix expression : "
EOL:        .asciiz "\n"
index:      .word 0;
input:      .space 100

#  ----------------------------------------
#  Program code goes in this section.

.text
.globl main 
.ent main 
main:

# -----
# your program code goes here.

la $a0, iMsg    
li $v0, 4
syscall

li $v0 , 8
la $a0 , input
li $a1 , 100
syscall

li $t1,0
la $t0,input

len:
    lb   $a0,0($t0)
    beqz $a0,done
    addi $t0,$t0,1
    addi $t1,$t1,1
    j    len

done:

la $t0,input

push:
    beq $t1 , 1 , exit
    lb  $t2 , ($t0)
    sub $t1 , $t1 ,1
    
    blt $t2 , 48 , pop
    sub $t2 , $t2 , 48
    subu $sp , $sp , 1
    sb $t2 , ($sp)

    add $t0 , $t0 , 1
    
    j push



pop:
    beq $t3 , 1 , exit
    lb $t4 , ($sp)
    addu $sp , $sp , 1
    lb $t5 , ($sp)

    addi $t0 , $t0 , 1 

    beq $t2 , 42 , multiply
    beq $t2 , 43 , add_
    beq $t2 , 45 , subtract



add_:
    add $t4 , $t4 , $t5
    sb $t4 , ($sp)
    sub $t3 , $t3 ,1
    j push



multiply:
    mul $t4 , $t4 , $t5
    sb $t4 , ($sp)
    sub $t3 , $t3 ,1
    j push



subtract:
    sub $t4 , $t5 , $t4
    sb $t4 , ($sp)
    sub $t3 , $t3 ,1
    j push


exit:
    li $v0, 1
    lb $a0, ($sp)
    syscall

# -----
# Done, terminate program.

    li $v0, 10
    syscall  
                            # all done! 
.end main


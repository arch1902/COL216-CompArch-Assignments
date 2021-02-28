# ----------------------------------------
#  Data declarations go in this section.

.data
# program specific data declarations
sum:        .word 0
iMsg:       .asciiz "Enter the postfix expression : "
invalidpostfix: .asciiz "The input is not a valid postfix notation \n"
invalidchar: .ascii "The input contains an invalid character.\n"
             .asciiz "The code accepts only chars from 0-9 and *, +, - \n"
output: .asciiz "Postfix Evaluation : "
EOL:        .asciiz "\n"
index:      .word 0;
input:      .space 1024 

#  ----------------------------------------
#  Program code goes in this section.

.text
.globl main 
.ent main 
main:

# -----
# your program code goes here.

    # Getting the postfix Notation 
    la $a0, iMsg    
    li $v0, 4
    syscall
    li $v0 , 8
    la $a0 , input
    li $a1 , 1024
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
    
    #t1=> Length of the input string
    # And will also serve as the iterator
    #t0 holds the address to the start of the string
    li $t6,0 # Length of the stack

    push:
        beq $t1 , 1 , print
        lb  $t2 , ($t0)    #loads byte from memory location t0 to the register t2
        sub $t1 , $t1 ,1   #shifting the counter
        blt $t2,  42, error_invalidchar
        bgt $t2, 57, error_invalidchar 
        blt $t2 , 48 , pop #if ASCII code is less then 48 it must be a operator hence go to pop
        sub $t2 , $t2 , 48 #Convert the char form 0 to 9 to corresponing Integers  
        subu $sp , $sp , 1 #pushing the integer into the stack
        sb $t2 , ($sp)     #storing the char into the location
        add $t6 , $t6, 1
        add $t0 , $t0 , 1  #adding one byte to start address
        j push


    pop:
        blt $t6,2, error_invalidpostfix 
        lb $t4 , ($sp)
        addu $sp , $sp , 1
        lb $t5 , ($sp)
        addi $t0 , $t0 , 1
        sub $t6,$t6,1
        beq $t2 , 42 , multiply
        beq $t2 , 43 , addition
        beq $t2 , 45 , subtract
        j error_invalidchar

    addition:
        add $t4 , $t4 , $t5
        sb $t4 , ($sp)
        j push

    multiply:
        mul $t4 , $t4 , $t5
        sb $t4 , ($sp)
        j push

    subtract:
        sub $t4 , $t5 , $t4
        sb $t4 , ($sp)
        j push

    print:
        bgt $t6,1,error_invalidpostfix 

        la $a0, output  
        li $v0, 4
        syscall
        li $v0, 1
        lb $a0, ($sp)
        syscall
        la $a0, EOL   
        li $v0, 4
        syscall
        j exit

    error_invalidpostfix:
        la $a0, invalidpostfix    
        li $v0, 4
        syscall
        j exit

    error_invalidchar:
        la $a0, invalidchar 
        li $v0, 4
        syscall
        j exit       

    exit:
        li $v0, 10
        syscall  
.end main


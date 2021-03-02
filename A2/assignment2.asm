# ----------------------------------------
#  Data declarations go in this section.

.data
# program specific data declarations
iMsg:       .asciiz "Enter the postfix expression : "
invalidpostfix: .asciiz "Error !! The input is not a valid postfix notation \n"
invalidchar: .ascii "Error !! The input contains an invalid character.\n"
             .asciiz "The code accepts only chars from 0-9 and *, +, - \n"
output: .asciiz "Postfix Evaluation : "
EOL:        .asciiz "\n"
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
    ## We have choosen the buffer length for the string to be 1024 chars.

    ## The following code calculates the length of the input string 
    li $t1,0 
    la $t0,input

    len:
        lb   $a0,0($t0)
        beq  $a0,0,done
        addi $t0,$t0,1
        addi $t1,$t1,1
        j    len

    done:
        la $t0,input     #t0 holds the address to the start of the string
    
    #t1=> Length of the input string and will also serve as the iterator

    li $t6,0 # Length of the stack

    #if the input string is empty then it returns an error 
    blt $t1,2,error_invalidpostfix

    push:
        beq $t1 , 1 , print ## if we have processed all the chars then it prints the result
        lb  $t2 , ($t0)    #loads byte from memory location t0 to the register t2
        sub $t1 , $t1 ,1   #shifting the counter
        # Detecting invalid characters 
        blt $t2,  42, error_invalidchar  
        bgt $t2, 57, error_invalidchar 
    
        blt $t2 , 48 , pop #If the char is an operator go to pop loop 
        sub $t2 , $t2 , 48 #Convert the char from 0 to 9 to corresponding Integer  
        subu $sp , $sp , 4 #pushing the integer into the stack
        sw $t2 , ($sp)     #storing the integer as a word in the stack head
        add $t6 , $t6, 1   # Increasing the length of the stack
        add $t0 , $t0 , 1  # shifting t0 to the next byte
        j push


    pop:
        blt $t6,2, error_invalidpostfix #if the length of stack is less thn two
        lw $t4 , ($sp)  #loading first operand
        addu $sp , $sp , 4 #poping it from the stack
        lw $t5 , ($sp)  #loading second operand
        addi $t0 , $t0 , 1  # shifting t0 to the next byte
        sub $t6,$t6,1   #Reducing the length of the stack
        beq $t2 , 42 , multiply 
        beq $t2 , 43 , addition
        beq $t2 , 45 , subtract
        j error_invalidchar # Invalid char

    addition:
        add $t4 , $t4 , $t5
        sw $t4 , ($sp) # storing the output in the stack head
        j push

    multiply:
        mul $t4 , $t4 , $t5
        sw $t4 , ($sp) # storing the output in the stack head
        j push

    subtract:
        sub $t4 , $t5 , $t4
        sw $t4 , ($sp) # storing the output in the stack head
        j push

    print:
        bgt $t6,1,error_invalidpostfix # If stack size is more than one then it was an Invalid postfix expression.

        #Printing Output
        la $a0, output  
        li $v0, 4
        syscall
        li $v0, 1
        lw $a0, ($sp)
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
        #exiting 
        la $a0, EOL   
        li $v0, 4
        syscall
        li $v0, 10
        syscall  
.end main
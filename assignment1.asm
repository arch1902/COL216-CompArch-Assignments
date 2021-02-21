#  Name and general description of program

# ----------------------------------------
#  Data declarations go in this section.

.data
# program specific data declarations

num:        .word 4
arr1:       .space 4000
arr2:       .space 4000
two:        .float 2.0
ans:        .float 0.0
sum:        .word 0
a1Msg:      .asciiz "Area under the curve = "
errMsg:     .asciiz "Please enter atleast 1 point\n"
iMsg:       .asciiz "Please enter the number of points : "
xMsg:       .asciiz "Please enter x co-ordinate of point "
zMsg:       .asciiz ":\t"
yMsg:       .asciiz "Please enter y co-ordinate of point "
EOL:        .asciiz "\n"
index:      .word 0;

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
li $v0 , 5
syscall
sw $v0, num



li $t1 , 0
li $t7 , 0
lw $t8 , num
add $t1 , $t1 ,1

beq $t8 , 0 , error

lw $t8 , num

la $a0, xMsg
li $v0, 4
syscall

li $v0 , 1
move $a0 , $t1
syscall

la $a0, zMsg
li $v0, 4
syscall

li $v0 , 5
syscall
move $t3, $v0

la $a0, yMsg
li $v0, 4
syscall

li $v0 , 1
move $a0 , $t1
syscall

la $a0, zMsg
li $v0, 4
syscall

li $v0 , 5
syscall
move $t4, $v0

add $t1 , $t1 ,1
sub $t8 , $t8 ,1
li $t7 , 0

beq $t8 , 0, exit


loop:
    # x-coordinate input
    la $a0, xMsg
    li $v0, 4
    syscall

    li $v0 , 1
    move $a0 , $t1
    syscall

    la $a0, zMsg
    li $v0, 4
    syscall

    li $v0 , 5
    syscall
    move $t5, $v0

    # y-coordinate input
    la $a0, yMsg
    li $v0, 4
    syscall

    li $v0 , 1
    move $a0 , $t1
    syscall

    la $a0, zMsg
    li $v0, 4
    syscall
    
    li $v0 , 5
    syscall
    move $t6, $v0

    # calculations
    add $t4 , $t4 , $t6
    sub $t3 , $t5 , $t3
    mul $t3 , $t3 , $t4
    add $t7 , $t3 , $t7
    move $t3 , $t5 
    move $t4 , $t6
    add $t1 , $t1 ,1 
    sub $t8 , $t8 ,1
    bnez $t8 , loop

exit:

    sw $t7 , sum

    mtc1 $t7 , $f6
    cvt.s.w $f6 , $f6

    lw $t1 , num
    mtc1 $t1 , $f8
    cvt.s.w $f8 , $f8

    l.s $f12, two

    div.s $f10, $f6, $f12
    s.s $f10 , ans

    la $a0, a1Msg
    li $v0, 4
    syscall
    
    li $v0 ,2
    mov.s $f12 , $f10
    syscall

    la $a0, EOL
    li $v0, 4
    syscall
    la $a0, EOL
    li $v0, 4
    syscall
# -----
# Done, terminate program.

    li $v0, 10
    syscall  
    

error:
    la $a0, errMsg
    li $v0, 4
    syscall
    la $a0, EOL
    li $v0, 4
    syscall
    li $v0, 10
    syscall
                            # all done! 
.end main


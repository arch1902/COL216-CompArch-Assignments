#  To calculate area under curve

# ----------------------------------------
#  Data declarations go in this section.

.data
# program specific data declarations

two:        .double 2.00
zero:       .double 0.00
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
la $a0, iMsg    #Ask for number of points
li $v0, 4
syscall
li $v0 , 5      # n input
syscall
move $t8, $v0



li $t1 , 0
li $t7 , 0

add $t1 , $t1 ,1

beq $t8 , 0 , error   # Print error message if n=0


la $a0, xMsg        # Ask for x co-ordinate of 1st point
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

la $a0, yMsg        # Ask for y co-ordinate of 1st point
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
l.d $f6 , zero

beq $t8 , 0, exit       # Exit with 0 area if n=1


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
    mul $t0 , $t4 , $t6
    bge $t0 , 0 , positive
    blt $t0 , 0 , negative
    positive:                       # Successive y co-ordinates on same side of x-axis
        add $t4 , $t4 , $t6
        sub $t3 , $t5 , $t3

        mul $t3 , $t3 , $t4

        abs $t3 , $t3
        add $t7 , $t3 , $t7

        move $t3 , $t5 
        move $t4 , $t6
        add $t1 , $t1 ,1 
        sub $t8 , $t8 ,1
        beq $t8 , 0 , exit
        bnez $t8 , loop

    negative:                       # Successive y co-ordinates on opposite sides of x-axis
        mul $s4 , $t4 , $t4
        mul $s6 , $t6 , $t6
        
        sub $t3 , $t5 , $t3
        add $t9 , $s4 , $s6

        mul $t3 , $t3 , $t9

        abs $t4 , $t4
        abs $t9 , $t6
        add $t4 , $t4 , $t9

        mtc1 $t3, $f0
        cvt.d.w $f0 , $f0           #Convertion to double for division
        mtc1 $t4 , $f2
        cvt.d.w $f2 , $f2
        div.d $f2 , $f0 , $f2
        add.d $f6 , $f6 , $f2

        move $t3 , $t5 
        move $t4 , $t6
        add $t1 , $t1 ,1 
        sub $t8 , $t8 ,1
        beq $t8 , 0 , exit
        bnez $t8 , loop

exit:

    mtc1 $t7 , $f4
    cvt.d.w $f4 , $f4
    add.d $f4 , $f4 , $f6

    l.d $f12, two
    div.d $f10, $f4, $f12

    la $a0, a1Msg
    li $v0, 4
    syscall
    
    li $v0 ,3               #Print the area under curve
    mov.d $f12 , $f10
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


#program to calculate area under the graph formed by n points sorted according to x co-ordinate

.data

askexp: .asciiz "\nEnter the postfix expression string:\n"
input: .space 128
e1: .asciiz "Invalid character input\n"
e2: .asciiz "Invalid postfix expression\n"
finalans: .asciiz "The calculated value of postfix expression is : "


.text
.globl main
.ent main

error1:
	li $v0, 4
	la $a0, e1
	syscall
	j end

error2:
	li $v0, 4
	la $a0, e2
	syscall
	j end

main:

	la $a0, askexp		#code to print string askexp
	li $v0 4
	syscall			#syscall to print the string

	li $v0, 8 		# call code, read string
	la $a0, input 		# addr, where to put chars
	li $a1, 128 		# max chars for string
	syscall

	li $t0, 0 		#index of character of string, loop index
	li $t1, 0		#Size of stack currently, will be used to check for validity of push

	la $s0, input 		#load string to s0
	li $t2, '\n'

#Thoda sa starting ka likha..ab bas har charcter dekhna hai..agar 0-9 hai to push, agar +,-,* hai to kuch kuch krna hai..warna error
readLoop:
	lb $s2, 0($s0)		#loading one character at a time as a byte

	beq $s2,10,endloop	#checking if we reached end of line 
	beq $s2,$t2,endloop

	bge $s2, 48, variable
	beq $s2, 42, multiplication
	beq $s2, 43, addition
	beq $s2, 45, substraction
	j error1
	
variable:
	bgt $s2, 57, error1	#not a valid character
	addi $s2,$s2,-48

	subu $sp, $sp, 4 # push array[i]
	sw $s2, ($sp)
	addi $t1,$t1,1

	j increment		#skip to increment index and proceed
	
multiplication:

	slti $t5,$t2,2
	bne $t5,$zero,error2
	
	lw $s4, ($sp)
	lw $s3, 4($sp)
	addu $sp, $sp, 8

	mul $s3,$s3,$s4

	subu $sp, $sp, 4 # push array[i]
	sw $s3, ($sp)
	addi $t1,$t1,-1

	j increment		#skip to increment index and proceed
	
addition:
	slti $t5,$t2,2
	bne $t5,$zero,error2
	
	lw $s4, ($sp)
	lw $s3, 4($sp)
	addu $sp, $sp, 8

	add $s3,$s3,$s4

	subu $sp, $sp, 4 # push array[i]
	sw $s3, ($sp)
	addi $t1,$t1,-1

	j increment		#skip to increment index and proceed

substraction:
	slti $t5,$t2,2
	bne $t5,$zero,error2
	
	lw $s4, ($sp)
	lw $s3, 4($sp)
	addu $sp, $sp, 8

	sub $s3,$s3,$s4

	subu $sp, $sp, 4 # push array[i]
	sw $s3, ($sp)
	addi $t1,$t1,-1

increment:	
	addi $s0,$s0,1	
	j readLoop

endloop:
	li	$s7, 1		# $s7 =1 
	bne	$t1, $s7, error2	# if size not 1

	lw $s4, ($sp)
	addu $sp, $sp, 4
	
	la $a0, finalans		#code to print string finalans
	li $v0 4
	syscall			#syscall to print the string

	li $v0,1
	move $a0, $s4
	syscall
	
end:
	li $v0, 10
	syscall
.end main

	
	


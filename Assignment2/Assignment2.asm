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
	li $a1, 128		# max chars for string
	syscall

	li $t0, 0 		#index of character of string, loop index
	li $t1, 0		#Size of stack currently, will be used to check for validity of push

	la $s0, input 		#load string to s0
	#li $t2, '\n'

readLoop:
	lb $s2, 0($s0)		#loading one character at a time as a byte

	beq $s2,10,endloop	#checking for newline
	beq $s2,0,endloop 	#checking for null 

	bge $s2, 48, variable	#if the character is having ascii value greater than 0
	beq $s2, 42, multiplication	#if char is *
	beq $s2, 43, addition	#if char is +
	beq $s2, 45, substraction	#if char is -
	j error1		# if the character is none of the above then error invalid character
	
variable:
	bgt $s2, 57, error1	#not a valid character not in 0-9
	addi $s2,$s2,-48	#calculating exact value of the number from character

	subu $sp, $sp, 4 	# push array[i]
	sw $s2, ($sp)
	addi $t1,$t1,1

	j increment		#skip to increment index and proceed
	
multiplication:

	blt $t1, 2, error2
	
	lw $s4, ($sp)
	addu $sp, $sp, 4
	lw $s3, ($sp)


	mul $s3,$s3,$s4

	#subu $sp, $sp, 4 	# push array[i]
	sw $s3, ($sp)
	addi $t1,$t1,-1

	j increment		#skip to increment index and proceed
	
addition:
	blt $t1,2,error2	#if we cannot pop 2 numbers from the stack
	
	lw $s4, ($sp)		#loading two numbers in s4 and s3
	addu $sp, $sp, 4
	lw $s3, ($sp)

	add $s3,$s3,$s4		#performing addition of these two numbers

	#subu $sp, $sp, 4 	# push array[i]
	sw $s3, ($sp)		#set the new value after multiplication
	addi $t1,$t1,-1		#reduce the count of numbers in stack by 1

	j increment		#skip to increment index and proceed

substraction:
	blt $t1,2,error2
	
	lw $s4, ($sp)
	addu $sp, $sp, 4
	lw $s3, ($sp)

	sub $s3,$s3,$s4

	#subu $sp, $sp, 4 	# push array[i]
	sw $s3, ($sp)
	addi $t1,$t1,-1

increment:	
	addi $s0,$s0,1		#increment the index of char in string
	j readLoop

endloop:
	bne $t1, 1, error2	# if size not 1

	lw $s4, ($sp)
	addu $sp, $sp, 4
	
	la $a0, finalans	#code to print string finalans
	li $v0, 4
	syscall			#syscall to print the string

	li $v0,1
	move $a0, $s4
	syscall
	
end:
	li $v0, 10
	syscall
.end main

	
	


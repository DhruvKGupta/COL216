#program to calculate area under the graph formed by n points sorted according to x co-ordinate

.data

askexp: .asciiz "\nEnter the postfix expression string:\n"
input: .space 128
e1: .asciiz "Invalid character input\n"
of1: .asciiz  "overflow at multiplication of (x2-x1) and (y1+y2) \nAnswer calculated yet is: "
of2: .asciiz "overflow while adding the latest sum"
output: .asciiz "The area under the curve is : "
numpoints: .word 0
area: .float 0.0
zero: .float 0.0
two: .float 2.0

.text
.globl main
.ent main

error1:
	li $v0, 4
	la $a0, e1
	syscall

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

	bge $s2, 48, variable
	beq $s2, 42, multiplication
	beq $s2, 43, addition
	beq $s2, 45, substraction
	beq $s2, 47, division
	j error1
	
variable:
	bgt $s2, 57, error1	#not a valid character
		
	move $a0, $s2 		#code to print string output
	li $v0 11
	syscall			#syscall to print the string

	j increment		#skip to increment index and proceed
	
multiplication:
	move $a0, $s2 		#code to print string output
	li $v0 11
	syscall			#syscall to print the string

	j increment		#skip to increment index and proceed
	
addition:
	move $a0, $s2 		#code to print string output
	li $v0 11
	syscall			#syscall to print the string

	j increment		#skip to increment index and proceed

substraction:
	move $a0, $s2 		#code to print string output
	li $v0 11
	syscall			#syscall to print the string

	j increment		#skip to increment index and proceed
division:
	move $a0, $s2 		#code to print string output
	li $v0 11
	syscall			#syscall to print the string

	
increment:	
	addi $s0,$s0,1	
	j readLoop

endloop:

	li $v0, 10
	syscall
.end main

	
	


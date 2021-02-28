#program to calculate area under the graph formed by n points sorted according to x co-ordinate

.data

askexp: .asciiz "Enter the postfix expression string:\n"
input: .space 128
errorline2: .asciiz "Number provided is not greater than 1 please retry\n"
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


main:

	la $a0, askexp	#code to print string askexp
	li $v0 4
	syscall			#syscall to print the string

	li $v0, 8 # call code, read string
	la $a0, input # addr, where to put chars
	li $a1, 128 # max chars for string
	syscall

	li $t0, 0 	#index of character of string, loop index
	li $t1, 0	#Size of stack currently, will be used to check for validity of push

	la $s0, input #load string to s0
	li $t2, '\n'

#Thoda sa starting ka likha..ab bas har charcter dekhna hai..agar 0-9 hai to push, agar +,-,* hai to kuch kuch krna hai..warna error
readLoop:
	add $s1, $t0, $s0
	lb	$s2, 0($s1)		#

	beq $s2,$t2,endloop

	move $a0, $s2 		#code to print string output
	li $v0 11
	syscall			#syscall to print the string

	li $a0, '$' 		#code to print string output
	li $v0 11
	syscall			#syscall to print the string
	addi $t0,$t0,1
	j readLoop

endloop:

	li $v0, 10
	syscall
.end main

	
	


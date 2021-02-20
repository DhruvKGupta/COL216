#program to calculate area under the graph formed by n points sorted according to x co-ordinate
# ye dekhna ek line pe hi x aur y ko kaise le input 

.data
asknum: .asciiz "Enter the number of input points n (n>1):"
askpoints: .asciiz "Enter the points in the form 
x-coordinate 
y-coordinate seperately on each line\n"
numpoints: .word 0

.text
.globl main
.ent main
main:
	la $a0, asknum		#code to print string asknum
	li $v0 4
	syscall			#syscall to print the string

	la $v0 5		#read intiger and store in v0
	syscall			#syscall to read

	move $t0, $v0		#copy the read intiger to t0
	sw $t0, numpoints	#save to variable

	la $a0, askpoints	#code to print string askpoints
	li $v0 4
	syscall			#syscall to print the string

	la $v0 5		#read intiger x1 and store in v0
	syscall			#syscall to read

	move $t1, $v0		#copy the read intiger x1 to t1

	la $v0 5		#read intiger y1 and store in v0
	syscall			#syscall to read

	move $t2, $v0		#copy the read intiger y1 to t2
	li $t5, 2		#t5 has the counter for while loop

readLoop:
	move $t3, $t1		#store prev x co-ordinate in t3
	move $t4, $t2		#store prev y co-ordinate in t4

	la $v0 5		#read intiger xi and store in v0
	syscall			#syscall to read

	move $t1, $v0		#copy the read intiger xi to t1	

	la $v0 5		#read intiger yi and store in v0
	syscall			#syscall to read

	move $t2, $v0		#copy the read intiger yi to t1		

	add $t5, $t5, 1
	ble $t5, $t0, readLoop
	li $v0, 10
	syscall
.end main

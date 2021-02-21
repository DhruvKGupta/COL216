#program to calculate area under the graph formed by n points sorted according to x co-ordinate
# ye dekhna ek line pe hi x aur y ko kaise le input 

.data
asknum: .asciiz "Enter the number of input points n (n>1):"
askpoints: .asciiz "Enter the points in the form 
x-coordinate 
y-coordinate seperately on each line\n"
errorline: .asciiz "The x coordinate decreased. Start Again\n"
numpoints: .word 0
area: .float 0.0

.text
.globl main
.ent main


error1:
	la $a0, errorline
	li $v0 4
	syscall
	j main

error2:



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

	li $t6, 0		#current answer

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

	move $t2, $v0		#copy the read intiger yi to t2

	add $t7,$t2,$t4
	sub $t8, $t1, $t3		# $t8 = $t1 - $t3
	blt	$t8, 0, error1	# if $t8 <0 main
	
	mul $t9,$t8,$t7
	add $t6, $t6, $t9


	add $t5, $t5, 1
	ble $t5, $t0, readLoop
	div $t6,$t6,2
	li	$v0, 1		
	move $a0,$t6 
	syscall
.end main

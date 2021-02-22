#program to calculate area under the graph formed by n points sorted according to x co-ordinate
# ye dekhna ek line pe hi x aur y ko kaise le input 

.data
askconvention: .asciiz "Enter 0 for absolute value of area enclosed or any other integer otherwise\n";
asknum: .asciiz "Enter the number of input points n (n>1):"
askpoints: .asciiz "Enter the points in the form 
x-coordinate 
y-coordinate seperately on each line\n"
errorline: .asciiz "The x coordinate decreased. Start Again\n"
numpoints: .word 0
area: .float 0.0
zero: .float 0.0

.text
.globl main
.ent main


error1:
	la $a0, errorline
	li $v0 4
	syscall
	j main

main:
	la $a0, askconvention		#code to print string askconvention
	li $v0 4
	syscall			#syscall to print the string

	la $v0 5		#read intiger and store in v0
	syscall			#syscall to read

	move $s0, $v0		#copy the read intiger to s0


	la $a0, asknum		#code to print string asknum
	li $v0 4
	syscall			#syscall to print the string

	la $v0 5		#read intiger and store in v0
	syscall			#syscall to read

	move $s1, $v0		#copy the read intiger to s1
	sw $s1, numpoints	#save to variable

	la $a0, askpoints	#code to print string askpoints
	li $v0 4
	syscall			#syscall to print the string

	la $v0 5		#read intiger x1 and store in v0
	syscall			#syscall to read

	move $t1, $v0		#copy the read intiger x1 to t1

	la $v0 5		#read intiger y1 and store in v0
	syscall			#syscall to read

	li $s5, 0		#current integer sum
	l.s $f0, zero	#current float sum
	

	move $t2, $v0		#copy the read intiger y1 to t2
	li $s2, 2		#s2 has the counter for while loop

readLoop:
	move $s3, $t1		#store prev x co-ordinate in s2
	move $s4, $t2		#store prev y co-ordinate in s3

	la $v0 5		#read intiger xi and store in v0
	syscall			#syscall to read

	move $t1, $v0		#copy the read intiger xi to t1	

	la $v0 5		#read intiger yi and store in v0
	syscall			#syscall to read

	move $t2, $v0		#copy the read intiger yi to t2

	sub $t8, $t1, $s3		# $t8 = $t1 - $s3
	blt	$t8, 0, error1	# if $t8 <0 error1
	add $t7, $s4, $t2	# sum of y coordinates without absolute

	bnez $s0,commonnow	# if (s0 not 0) then go to commonnow to calculate without absolute

	move $t3, $s4		#copy y coordinates t3 and t4 for absolute values
	move $t4, $t2

	blt 0,$t3, absolute1
	sub $t3,0,$t3
absolute1:
	blt 0,$t4, absolute2
	sub $t4,0,$t4
absolute2:				#absolute values done, stored in t3 and t4

	mul $t5,$t2,$s4
	blt $t5,0, cross	#check their product for relative sign, if opposite go to cross calculation of floats

	add $t7,$t3,$t4		# add absolute value of y coordinates

commonnow:	
	mul $t9,$t8,$t7		# product of (x2-x1) and sum of y coordinates (may be absolute sum or not)
	add $s5, $s5, $t9	# add to integer sum
endofcalc:


	add $s2, $s2, 1
	ble $s2, $s1, readLoop

	mtc1 $f1,$t6
	div.s $f1, $f1,2		#convert int sum to float and divide by 2....Note : Float sum is already divided by 2
	add.s $f0,$f0,$f1		# final area

	li	$v0, 1		
	move $a0,$t6 			# Print krne ka tareeka dekh liyo float ka..f0 ko krna hai
	syscall
	
	li $v0, 10
	syscall
.end main


cross:
	sub $t5,0,$t5	#absolute value of product
	mtc1 $f1,$t5	# transfer to float
	add $t7,$t3,$t4	# add absolute values
	mtc1 $f2,$t7	# conversions of float
	mtc1 $f3,$t8
	mul.s $f4,$f1,$f3 # (y1y2)*(x2-x1)
	div.s $f5,$f4,$f2 # (y1y2)*(x2-x1)/(y1+y2)		#actual area with division by 2
	add.s $f0,$f0,$f5
	j endofcalc			# go to end of calc



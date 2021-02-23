#program to calculate area under the graph formed by n points sorted according to x co-ordinate

.data
askconvention: .asciiz "\nEnter 0 for absolute value of area enclosed or any other integer otherwise: ";
asknum: .asciiz "Enter the number of input points n (n>1): "
askpoints: .asciiz "Enter the points in the form 
x-coordinate 
y-coordinate seperately on each line\n"
errorline: .asciiz "The x coordinate decreased. Start Again\n"
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


error1:
	la $a0, errorline
	li $v0 4
	syscall
	j main
	
error2:
	la $a0, errorline2
	li $v0 4
	syscall
	j askn
	
overflow1:
	la $a0, of1
	li $v0 4
	syscall
	j wrappingUp
	
overflow2:
	la $a0, of2
	li $v0 4
	syscall
	j wrappingUp

main:

	li $s7,0
	l.s $f10,two
	la $a0, askconvention	#code to print string askconvention
	li $v0 4
	syscall			#syscall to print the string

	la $v0 5		#read intiger and store in v0
	syscall			#syscall to read

	move $s0, $v0		#copy the read intiger to s0

askn:
	la $a0, asknum		#code to print string asknum
	li $v0 4
	syscall			#syscall to print the string

	la $v0 5		#read intiger and store in v0
	syscall			#syscall to read
	ble $v0,1,error2	#check if the input value of number of points is valid


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
	l.s $f12, zero		#current float sum
	

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

	sub $t8, $t1, $s3	# $t8 = $t1 - $s3
	blt $t8, 0, error1	# if $t8 <0 error1
	add $t7, $s4, $t2	# sum of y coordinates without absolute

	bnez $s0,commonnow	# if (s0 not 0) then go to commonnow to calculate without absolute

	move $t3, $s4		#copy y coordinates t3 and t4 for absolute values
	move $t4, $t2

	bgt $t3,0, absolute1
	sub $t3,$s7,$t3
absolute1:
	blt $s7,$t4, absolute2
	sub $t4,$s7,$t4
absolute2:			#absolute values done, stored in t3 and t4

	mul $t5,$t2,$s4
	blt $t5,0, cross	#check their product for relative sign, if opposite go to cross calculation of floats
	add $t7,$t3,$t4		# add absolute value of y coordinates

commonnow:	
	mul $t9,$t8,$t7		# product of (x2-x1) and sum of y coordinates (may be absolute sum or not)
	blt $s7,$t7, posover1	#check for overflow
negover1:			#they check what is the sign of the numbers to be multiplied(t8 is positive always)
	blt $s7,$t9,overflow1	#and then if the sign of the sum is different from t9 then overflow must have happende
	j skipto
posover1:
	blt $t9,$s7,overflow1
skipto:
	blt $s7,$t9, posCheck	#overflow checks for if the sum now has overflown (however QTspim does it and this check is not actually used)
	blt $t9,$s7, negCheck
okay:
	add $s5, $s5, $t9	# add to integer sum
	

endofcalc:


	add $s2, $s2, 1
	ble $s2, $s1, readLoop
wrappingUp:
	mtc1 $s5, $f14
	cvt.s.w $f14,$f14
	div.s $f14, $f14,$f10	#convert int sum to float and divide by 2....Note : Float sum is already divided by 2
	sub.s $f12,$f14,$f12	# final area

	la $a0, output		#code to print string output
	li $v0 4
	syscall			#syscall to print the string
	li	$v0, 2			
	syscall
	
	li $v0, 10
	syscall
.end main


cross:
	mul $t5,$t3,$t4		#absolute value of product
	mtc1 $t5, $f16		# transfer to float
	cvt.s.w $f16,$f16
	add $t7,$t3,$t4		# add absolute values
	mtc1 $t7, $f18		# conversions of float
	cvt.s.w $f18,$f18
	mtc1 $t8, $f20
	cvt.s.w $f20,$f20
	mul.s $f22,$f16,$f20	# (y1y2)*(x2-x1)
	div.s $f24,$f22,$f18 	# (y1y2)*(x2-x1)/(y1+y2)		#actual area with division by 2
	add.s $f12,$f12,$f24
	j commonnow		# go to end of calc

posCheck:
	blt $s5,$s7,okay
	add $s5, $s5, $t9
	blt $s5, $s7,overflow2
	j endofcalc
	
negCheck:
	blt $s7,$s5,okay
	add $s5, $s5, $t9
	blt $s7, $s5,overflow2
	j endofcalc
	
	


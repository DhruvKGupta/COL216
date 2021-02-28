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

#Thoda sa starting ka likha..ab bas har charcter dekhna hai..agar 0-9 hai to push, agar +,-,* hai to kuch kuch krna hai..warna error
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
	
	


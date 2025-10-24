# timetemplate.S
# Written 2015 by F Lundevall
# Ported 2024/06 by W Szczerek (from MIPS to RISC-V)
# Copyright abandonded - this file is in the public domain.

#############################################################
# Choose the macro syntax for RARS or DTEK-V board.         #
# The syntax for RARS is probably due to its MIPS heritage. #
#############################################################
###################
# Macros for RARS #
###################
#.macro	PUSH (%reg)
#	addi	sp,sp,-4
#	sw	%reg,0(sp) 
#.end_macro

#.macro	POP (%reg)
#	lw	%reg,0(sp)
#	addi	sp,sp,4
#.end_macro
###################
# Macros for DTEK #
###################
#.macro	PUSH reg
#	addi sp,sp,-4
#	sw \reg,0(sp) 
#.endm

#.macro	POP reg
#	lw	\reg,0(sp)
#	addi	sp,sp,4
#.endm
#############################################################

	.data
	.align 2
mytime:	.word 	0x5957
timstr:	.asciz 	"text more text lots of text\0"
	.text
	.globl timetemplate, tick, time2string, delay, display_string, hexasc

# Function for displaying a string with a newline at the end	
display_string:	
	li a7,4
	ecall
	li a0, 10
	li a7,11
	ecall
	jr ra
	
timetemplate:
	la	a0, timstr
	jal     display_string
	
	# wait a little
	li	a0, 2		# ms
	jal	delay
	
	# call tick
	la 	a0, mytime
	jal	tick
	
	# call your function time2string
	la	a0, timstr
	la	t0, mytime
	lw	a1, 0(t0)
	jal	time2string
	
	# go back and do it all again
	j	timetemplate

	
# tick: update time pointed to by $a0
tick:	lw	t0, 0(a0)	# get time
	addi	t0, t0, 1	# increase
	andi	t1, t0, 0xf	# check lowest digit
	sltiu	t2, t1, 0xa	# if digit < a, okay
	bnez	t2, tiend
	addi	t0, t0, 0x6	# adjust lowest digit
	
	andi	t1, t0, 0xf0	# check next digit
	sltiu	t2, t1, 0x60	# if digit < 6, okay
	bnez	t2, tiend
	addi	t0, t0, 0xa0	# adjust digit
	
	li	t3, 0xF
	slli	t3, t3, 0x8
	and	t1, t0, t3	# check minute digit
	addi	t3, x0, 0xA
	slli	t3, t3, 0x8
	slt	t2, t1, t3	# if digit < a, okay
	bnez	t2, tiend
	addi	t0, t0, 0x600	# adjust digit - this one's okay, it's lower than 0x7FF 
	
	li	t3, 0xF
	slli	t3, t3, 0xC
	and	t1, t0, t3	# check last digit
	addi	t3, x0, 0x6
	slli	t3, t3, 0xC
	slt	t2, t1, t3	# if digit < 6, okay
	bnez	t2, tiend
	
	li	t3, 0xA
	slli	t3, t3, 0xC
	add	t0, t0, t3	# adjust last digit
tiend:	sw	t0,0(a0)	# save updated result
	jr	ra		# return

#########################################################
# Place for your functions: time2string, hex2asc, delay.#
#########################################################

# You can write your own code for time2string , hex2asc and delay here
 

time2string:
	# Reg a0 is area of the memory = basicly a0 is the address 
	# Reg a1 have the time info = basicly a1 is the value of the time ex 59:57  59 minutes and 57 secounds.
	
	#store values in those address 
	addi sp ,sp, -16
	sw ra, 12(sp)  
	sw a0, 8(sp)
	sw a1 , 4(sp) 
	
	# first digit 1  
	lw t0, 4(sp)   #  copia value of a1 then mask after that 
	andi t0, t0, 0xF
	
	mv a0, t0 
	jal hexasc

	lw t1, 8(sp)
	sb a0, 4(t1)
	
	# secound digit 2 
	lw t0, 4(sp)
	srli t0, t0, 0x4  # shift to right in four bits  after that it goes bits (3 => 0) so we can mask with 0xF.
	andi t0, t0, 0xF  
	 
	mv a0, t0
	jal hexasc
	
	lw t1 , 8(sp)
	sb a0, 3(t1)
	
	# the character between minutes and seconds, which is ':'
	lw t1, 8(sp)    # load the saved buffer address (originally in a0)
	li t0, 0x3A     # load ASCII ':' (0x3A) into t0
	sb t0, 2(t1)    # store ':' at index 2 of the buffer
	
	#third digit 3 ( minues ) 
	lw t0, 4(sp)
	srli t0, t0, 0x8
	andi t0, t0 , 0xF
	
	mv a0, t0
	jal hexasc
	
	lw t1, 8(sp)
	sb a0, 1(t1)
	
	#fourth digit 4 ( minues )
	lw t0, 4(sp)
	srli t0, t0, 0xC
	andi t0, t0, 0xF
	
	mv a0, t0
	jal hexasc 
	
	lw t1, 8(sp)
	sb a0, 0(t1) 
	
	# for Null byte so we close the string 
	lw t1 ,8(sp)
	li t0, 0x0
	sb t0, 5(t1)  # skriv 0( null) i index 5 
	
	#restore 
	lw a1, 4(sp)
	lw a0, 8(sp)
	lw ra, 12(sp) 
	addi sp , sp, 16
	
	jr ra
	

 # You can write your own code for hexasc here
  #
  hexasc:
  	slti t0, a0, 10     #checking if a0 < 10 => then if it's true then t0 = 1 otherwase t0 = 0
  			    # exp if a0 = 6 then slti t0, a0, 10  => 6 < 10 = true then t0 = 1 
  			    # exp2 if a0 >= 10 then 10 < 10 = false t0 = 0 
  	
  	bne t0 , zero , lessThen10 
  	
  	addi t2, t2, 0xA  # we sets t2 = 10 so we can substract a0  
     
     	sub a0, a0, t2    #a0 = a0 - t2  if a0 = 10 when we doing addi a0, a0, 0x41 => a0 = a0 + 0x41 = 10 + 0x41 = 0x51 
     	# which is 'K'in ascii.  we want to start at  'A'  then we have substaract 10 if a0 is bigger then 10
     			
    	
    	addi a0, a0, 0x41  # here we start A where A in ascii  0x41
    	# exp if a0  11 then a0 will be a0  11 - 10  1 > addi will be a0  1 + 0x31  0x32 which is 'B' in ascii.
     
     	 jr ra 		# return ra
  
 lessThen10:
    	addi a0, a0, 0x30 # start from 0 ascii = 0x30
   
        jr ra  # return ra  


    
  delay:
  	addi sp, sp, -16 # here is the stack 
  	sw ra , 8(sp) 	# store ra in that address 
  	sw a0, 4(sp) # store the origanal value of a0 in that address 
 	mv t0, a0  # a0  ms now t0  copia of a0 > t0 a0

  	
whileloop:
	 bge zero, t0 , restore  # if ( 0 > t0 > if 0 bigger then or equal t0 then we don't loop)
  	 			 # jump to restore where we restore back the values 
  	 
	addi t0, t0, -1  # this is t0  t0 - 1 > ms  ms - 1; 
	li t1, 0       # create variablen i and sets 0 now t1  i which is t1  0
	li t2 , 50000   # the iteration for forloop t3  where the forloop stops like t2  length

forloop:
	bge t1, t2 , whileloop # if t1 > t2 is like if i > length then jump back while loopen because this forloop in while loop is finsish
	addi t1, t1, 1  # increase i by one  i++ or i  i+ 1; > t1  t1 +1 
	j forloop # if i < length or t1 < t2 then iretate again forloop until t1 t2.

restore:
 	#restore 
	lw ra, 8(sp)
	lw a0, 4(sp)
	addi sp , sp , 16
	jr ra

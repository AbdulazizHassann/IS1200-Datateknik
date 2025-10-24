  # hexmain.S
  # Written 2015-09-04 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

	.text
	.globl hex2asc

hex2asc:
	li	a0, 17		# test number (from 0 to 15)
	
	addi    sp,sp,-4
	sw      ra,0(sp)
	
	jal	hexasc		# call hexasc
	
	li	a7, 11	# write a0 to stdout
	ecall

	lw      ra,0(sp)
	addi    sp,sp,4
	jr      ra	

  # You can write your own code for hexasc here

hexasc:
	li t0, 0x09 # 9 
	andi a0, a0, 0x0F # Filter out the last 4 bits
	addi a0, a0, -10 # Subtract by 10, if value less than 0 we know its a digit
	
	blt a0, zero, digit #if the input value in a0 is less or equal to 9 we want to write a digit and breanch to the digit subroutine
	addi a0, a0, 0x41 # add 7 to the value of a0 to skip over the ASCII characters between the digits and letters + the value for ASCII 0 to the value if trhe digit + 10
	ret

digit:
	addi a0, a0, 0x3A #add the value for ASCII 0 to the value if trhe digit + 10
	ret
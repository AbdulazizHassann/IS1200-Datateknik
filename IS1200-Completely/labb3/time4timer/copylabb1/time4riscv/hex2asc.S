  # hexmain.S
  # Written 2015-09-04 by F Lundevall
  # Copyright abandonded - this file is in the public domain.

	.text
	.globl hex2asc

hex2asc:
	li	a0, 2		# test number (from 0 to 15)
	
	addi    sp,sp,-4
	# this comment is for you not from the labb you wrote it.
	# test  to know  what is in ra and if what i give stores in to sp 
	# yes what we give ra as value  stores the into sp address (now you can remove the addi instraction in ra ) : removed 

	sw      ra,0(sp)
	
	jal	hexasc		# call hexasc
	
	li	a7, 11	# write a0 to stdout
	ecall

	lw      ra,0(sp)
	addi    sp,sp,4
	
	
        
	jr      ra	   # a0 ) 2   )| 2 < 10  = true = 1  a0 = 11 11 < 10 = false = 0 
	

  # You can write your own code for hexasc here
  #om a0 = 14 = E    = a0 = 14 + 0x41   = 14 - 10 = 4 
  # a0 = 4 + 0x41 = 0x45
  hexasc:
  	slti t0, a0, 10     #checking if a0 < 10 => then if it's true then t0 = 1 otherwase t0 = 0
  			    # exp if a0 = 6 then slti t0, a0, 10  => 6 < 10 = true then t0 = 1 
  			    # exp2 if a0 >= 10 then 10 < 10 = false t0 = 0 
  	
  	bne t0 , zero , lessThen10 # if ( t0!= 0) then jumpt to lessthen10. where we print out numbers 0-> 9 
			         
  	addi t2, t2, 0xA  # we sets t2 = 10 so we can substract a0  
     
     	sub a0, a0, t2    #a0 = a0 - t2  if a0 = 10 when we doing addi a0, a0, 0x41 => a0 = a0 + 0x41 = 10 + 0x41 = 0x51 
     	# which is 'K'in ascii.  we want to start at  'A'  then we have substaract 10 if a0 is bigger then 10
     			
    	
    	addi a0, a0, 0x41  # here we start A where A in ascii = 0x41
    	# exp if a0 = 11 then a0 will be a0 = 11 - 10 = 1 => addi will be a0 = 1 + 0x31 = 0x32 which is 'B' in ascii.
     
     	 jr ra 		# return ra
  
 lessThen10:
    	addi a0, a0, 0x30 # start from 0 ascii = 0x30
   
        jr ra  # return ra  


.data
   age: .word 28
   meddelande: .asciiz "\nsalamualykum all tacksam tillhör till allah\n"
   siffror : .double 15.50
   tecken: .byte  '&'
   
.text
   li $v0 , 1
   lw $a0 ,age
   syscall
   
   li $v0 , 4 
   la $a0 , meddelande
   syscall
   
   li $v0 , 3
   l.d $f12 , siffror
   syscall
   
   li $v0 , 4 
   la $a0 , tecken
   syscall
   
   
   
   
   
   
   .data 
     suaal: .asciiz "\nmaaxad iga rabta ? \n"
     
   .text 
   
   li $v0 , 4
   la $a0 , suaal
   syscall
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
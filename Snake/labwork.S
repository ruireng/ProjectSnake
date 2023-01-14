  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.global enable_interrupt
.global PUSH
.global POP
.global delay
.global time2string

.macro	PUSH reg
	addi	$sp,$sp,-4
	sw	\reg,0($sp)
.endm

.macro	POP reg
	lw	\reg,0($sp)
	addi	$sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #
	.data
	.align 2
mytime:	.word 0x5957
timstr:	.ascii "text more text lots of text\0"
	.text
	
main:
	# print timstr
	la	$a0,timstr
	li	$v0,4
	syscall
	nop
	# wait a little
	li	$a0,1000		# argument för delay
	PUSH $t0
	PUSH $t1
	PUSH $t2 
	PUSH $t3
	jal 	delay
	nop
	POP $t3
	POP $t2
	POP $t1
	POP $t0
	# call tick
	la	$a0,mytime
	jal	tick
	nop
	# call your function time2string
	la	$a0,timstr
	la	$t0,mytime		# addressen för mytime
	lw	$a1,0($t0)           	# låt $a1 vara addressen för $t0
	
	### PUSHA HÄR...
	jal	time2string
	nop
	# print a newline
	li	$a0,10
	li	$v0,11
	syscall
	nop
	# go back and do it all again
	j	main
	nop


# tick: update time pointed to by $a0
tick:	lw	$t0,0($a0)	# get time     sätter temporär register till a0    tiden: t2, t1, t0   (finns ingen t3), 
	addiu	$t0,$t0,1	# increase       ökar t0(sekunder)
	andi	$t1,$t0,0xf	# check lowest digit         1111 AND 0101 1001 0101 0111 dvs t1 blir sista siffran(7)
	sltiu	$t2,$t1,0xa	

	bnez	$t2,tiend             #om t2 är =1 gå till tiend, annars inte 
	nop        
	addiu	$t0,$t0,0x6	
	andi	$t1,$t0,0xf0	
	sltiu	$t2,$t1,0x60	
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0xa0	
	andi	$t1,$t0,0xf00	
	sltiu	$t2,$t1,0xa00	
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0x600	
	andi	$t1,$t0,0xf000	
	sltiu	$t2,$t1,0x6000	  
	bnez	$t2,tiend
	nop
	addiu	$t0,$t0,0xa000	# adjust last digit
	
	
tiend:	sw	$t0,0($a0)	# save updated result
	jr	$ra		# return
	nop

  # you can write your code for subroutine "hexasc" below this line

hexasc:
      	andi  	$a0,$a0,0xf
      	addi  	$v0,$zero,0x30
      	add   	$v0,$a0,$v0
      	jr    	$ra
	nop

delay:				#Delay-funktionen
	addi $t3, $0, 3500
	
loop:	addi $t2, $0, 0
	slt $t1, $0, $a0
	beq $t1, $0, noloop
	nop
	addi $a0, $a0, -1
forloop:	slt $t4, $t2, $t3
	beq $t4, $0, noforloop
	nop
	addi $t2, $t2, 1
	j forloop
	nop
noforloop:	j loop
	nop
noloop: 	jr $ra
	nop
	
time2string:
    	andi    $t0, $a1, 0xF000   
    	srl 	$t0, $t0, 12      
    	andi    $t1, $a1, 0xF00   
    	srl 	$t1, $t1, 8    
    	andi    $t2, $a1, 0xF0         ## Hanterar 5an.
    	srl     $t2, $t2, 4     
    	andi    $t3, $a1, 0xF   
    
   	# move    $s0, $a0       ### S0, koden där vi sparar det som sen ska printas 

    	## kanske borda pusha a0 innan, så den sparas 
    	PUSH $ra
    	PUSH $v0
    	PUSH $a0
    	move	$a0,$t0
    	jal 	hexasc
    	nop
    	POP $a0
    	sb  $v0, 0($a0)         ### spara s0 i v0(som funktionen returnar)
 
    	PUSH $a0 
    	move    $a0, $t1
    	jal 	hexasc      
    	nop
    	POP $a0
	sb  	$v0, 1($a0)         ### spara s0 i v0(som funktionen returnar)
    
    	li  	$t8, 0x3A
    	sb  	$t8, 2($a0)     ### Lägg till kolon
    
    	PUSH $a0
    	move    $a0, $t2   ### 5an... 
    	jal 	hexasc      
    	nop
    	POP $a0
    	sb  	$v0, 3($a0)         ### spara s0 i v0(som funktionen returnar)
    
    	PUSH $a0
    	move    $a0, $t3
    	jal 	hexasc      
    	nop
    	POP $a0
    	sb  	$v0, 4($a0)         ### spara s0 i v0(som funktionen returnar)
    
	andi	$t3,$t3,1	# Maska allt förutom lsb
	beq	$t3,0,even	# Branch till even
	nop
	beq	$t3,1,odd	# Branch till odd
	nop
    
    	li  	$t9, 0x00
    	sb  	$t9, 5($a0)        ## lägg till null
    
	POP $v0
	POP $ra
    	jr  	$ra
    	nop

	even:
	li	$t0,0x45		# Lägg till E
	sb	$t0,5($a0)
	li	$t0,0x00		# Lägg till NULL
	sb	$t0,6($a0)
	addi	$t7,$t7,1			# Sätt $t7 = 2
	POP $v0
	POP $ra
	jr	$ra
	nop
	
	odd:
	li	$t0,0x44		# Lägg till D
	sb	$t0,5($a0)
	li	$t0,0x00		# Lägg till NULL
	sb	$t0,6($a0)
	li	$t7,1			# Sätt $t7 = 1
	POP $v0
	POP $ra
	jr	$ra
	nop

enable_interrupt:
	ei
	jr	$ra
	nop

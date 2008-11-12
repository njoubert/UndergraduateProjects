lui 0
mxd $r0
ori $r0 0
mxd $r0
lw $r0 0
mxd $r0   #Now we have the first byte from memory 0 in $r0
lui 0
mxd $r1
ori $r1 5
mxd $r1   #Now we have 5 in $r1
msx $r0  #we put the value of the first byte into our source reg.
sll $r1  #and we shift it left logically by $r1 which is 5
msd      #we put the shifted value as the new source
srl $r1  #we shift it back
mxd $r1  #we move the shifted value to $r1
disp $r1 0  #We display the lower 3 bits.
lui 0 
mxd $r1
ori $r1 2
mxd $r1 #now we have 2 in $r1
msx $r0
sll $r1
mxd $r0 #now we have the lower 6 bits of the value in $r0
lui 0
mxd $r1
ori $r1 5 
mxd $r1 #now we have the value of 5 in $r1
msx $r0 #put the shifted address in $rs
srl $r1 #shift it
mxd $r0 #shifted value in $r0
disp $r0 1
lui 0
mxd $r0
ori $r0 0
mxd $r0
lw $r0 0
mxd $r0   #Now we have the first byte from memory 0 in $r0
lui 0
mxd $r1
ori $r1 6
mxd $r1
msx $r0
srl $r1
mxd $r0
disp $r0 2
lui 15
mxd $r0
ori $r0 15
mxd $r0
disp $r0 3  #Turn off the last display
mxd $r1
beq -1  #Halt!
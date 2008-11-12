lui  0     #reset registers to 0
mxd  $r0
not  $r0
mxd  $r0 
disp $r0 2 #turn it off
disp $r0 3 #turn it off
not  $r0
mxd  $r0
disp $r0 0 #reset display
disp $r0 1
mxd  $r1
ori  $r1 4 #for shifting by 5
mxd  $r1
msx  $r0    #save last copy of counter; loop starts here, address is 13 (0x0d)
sw   0($r1) #Mem[4] = counter; 
sll  $r1    
msd
srl  $r1
mxd   $r0
disp $r0 0  #print right 4 bits
lw   0($r1)
msd 
srl  $r1
mxd  $r0    #load the left 4 bits
disp $r0 1
lw   0($r1)#load original counter
mxd  $r0   #next 5 instructions increment $r0 by 1
not  $r0
mxd  $r0
neg  $r0
mxd  $r0
j    13
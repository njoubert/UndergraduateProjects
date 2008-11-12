lui 0
mxd $r0
ori $r0 0
mxd $r0  #Now we have 0 in $r0, the first mem address and the counter.
lui 0
mxd $r1
ori $r1 10 #Now we have 10 in $rs, our limit.
msd
sw 0($r0) #We save this value to the first memory address.


mxd $r0
mxd $r1
beq -1
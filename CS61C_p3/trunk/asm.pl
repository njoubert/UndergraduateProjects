#!/usr/bin/perl

print "v2.0 raw\n";
while (<>){
	printInstruction($_);
}


sub printInstruction(){
	my ($inst) = @_;
	$num = 0;
	if ($inst =~m/^\s*add\s+\$r([01])/i){
		#$num = ((opcode | $1) << 4) | ($3 << 3) | funct; 
		$num = ($1 << 4) | 0; 
	}
	elsif ($inst =~m/^\s*sub\s+\$r([01])/i){
		$num = ($1 << 4) | 1; 
	}
	elsif ($inst =~m/^\s*or\s+\$r([01])/i){
		$num = ($1 << 4) | 2; 
	}
	elsif ($inst =~m/^\s*and\s+\$r([01])/i){
		$num = ($1 << 4) | 3; 
	}
	elsif ($inst =~m/^\s*sll\s+\$r([01])/i){
		$num = ($1 << 4) | 4; 
	}
	elsif ($inst =~m/^\s*slt\s+\$r([01])/i){
		$num = ($1 << 4) | 5; 
	}
	elsif ($inst =~m/^\s*srl\s+\$r([01])/i){
		$num = ($1 << 4) | 6; 
	}
	elsif ($inst =~m/^\s*sra\s+\$r([01])/i){
		$num = ($1 << 4) | 7; 
	}
	elsif ($inst =~m/^\s*mxd\s+\$r([01])/i){
		$num = ($1 << 4) | 8; 
	}
	elsif ($inst =~m/^\s*msx\s+\$r([01])/i){
		$num = ($1 << 4) | 9; 
	}
	elsif ($inst =~m/^\s*msd\s+(\$r([01]))?/i){
		$num = 10; 
	}
	elsif ($inst =~m/^\s*not\s+\$r([01])/i){
		$num = ($1 << 4) | 11; 
	}
	elsif ($inst =~m/^\s*neg\s+\$r([01])/i){
		$num = ($1 << 4) | 12; 
	}
	elsif ($inst =~m/^\s*jr\s+\$r([01])/i){
		$num = ($1 << 4) | 13; 
	}
	elsif ($inst =~m/^\s*disp\s+\$r([01])(,)?\s+(\d+)/i){
		$num = (1 << 5) | ($1 << 4) | ($3 & 15); 
	}
	elsif ($inst =~m/^\s*lui\s+\$r([01])(,)?\s+(\d+)/i){
		$num = (2 << 5) | ($3 & 15); 
	}
	elsif ($inst =~m/^\s*lui\s+(\d+)/i){
		$num = (2 << 5) | ($1 & 15); 
	}
	elsif ($inst =~m/^\s*ori\s+\$r([01])(,)?\s+(\d+)/i){
		$num = (3 << 5) | ($1 << 4) | ($3 & 15); 
	}
	elsif ($inst =~m/^\s*lw\s+\$r([01])(,)?\s+(\d+)/i){
		$num = (4 << 5) | ($1 << 4) | ($3 & 15); 
	}
	elsif ($inst =~m/^\s*sw\s+\$r([01])(,)?\s+(\d+)/i){
		$num = (5 << 5) | ($1 << 4) | ($3 & 15); 
	}

	elsif ($inst =~m/^\s*lw\s+(\d+)\(\$r([01])\)/i){
		$num = (4 << 5) | ($2 << 4) | ($1 & 15); 
	}
	elsif ($inst =~m/^\s*sw\s+(\d+)\(\$r([01])\)/i){
		$num = (5 << 5) | ($2 << 4) | ($1 & 15); 
	}

	elsif ($inst =~m/^\s*j(ump)?\s+(\d+)/i){
		$num = (6 << 5) | ($2 & 31); 
	}
	elsif ($inst =~m/^\s*beq\s+(-?\d+)/i){
		$num = (7 << 5) | ($1 & 31); 
	}
	else {
		$num = 47;
	}
	printf("%x\n", $num);
}
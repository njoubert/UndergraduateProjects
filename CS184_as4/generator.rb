#!/usr/bin/ruby

stepsize = 5

File.open("out.scene", "w") do |f|

	f.puts "eye 0.0 0.0 5.0" 
	f.puts "viewport -5.0 -5.0 -1.0 5.0 -5.0 -1.0 5.0 5.0 -1.0 -5.0 5.0 -1.0"
	
	f.puts "pointlight 0 0 20 0.8 0.8 0.8"
	
	x = -10
	while x <= 10
		y = -10
		while y <= 10
			z = -20
			while z <= -10
			
				#puts "#{x} #{y} #{z}"
				f.puts "sphere \t#{x} #{y} #{z} 1.25 0.1 0.1 0.1 20.0 0.3 0.3 0.3 0.3 0.3 0.3 1.0 1.0 1.0"
				
				z += stepsize
			end
			y += stepsize	
		end
		x += stepsize
	end
	

end
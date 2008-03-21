#!/usr/bin/ruby

stepsize = 8
r = 2
limithigh = 20
limitlow = -20

File.open("out.scene", "w") do |f|

	f.puts "eye 0.0 0.0 #{limithigh*4}" 
	f.puts "viewport -8.0 -8.0 #{limithigh*(3)} 8.0 -8.0 #{limithigh*(3)} 8.0 8.0 #{limithigh*(3)} -8.0 8.0 #{limithigh*(3)}"
	
	f.puts "pointlight 50 50 50 0.7 0.6 0.5"
	f.puts "pointlight -50 -50 50 0.5 0.6 0.7"
	
	x = limitlow
	while x <= limithigh
		y = limitlow
		while y <= limithigh
			z = limitlow
			while z <= limithigh
			
				#puts "#{x} #{y} #{z}"
				f.puts "sphere \t#{x} #{y} #{z} #{r} 1.0 1.0 1.0 60.0 0.15 0.15 0.15 #{x.abs.to_f/20} #{y.abs.to_f/20} #{z.abs.to_f/20} 1.0 1.0 1.0"
				
				z += stepsize
			end
			y += stepsize	
		end
		x += stepsize
	end
	
	#f.puts "triangle -20 -20 20 20 -20 20 0 0 -20 1.0 1.0 1.0 60.0 0.15 0.15 0.15 0.4 0.4 0.4 1.0 1.0 1.0"
	

end
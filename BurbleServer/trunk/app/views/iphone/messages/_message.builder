#Pass it the current builder as a :locals => {:rootxml => xml, :person => currentperson} 

readStatus = Proc.new {|opts| opts[:builder].read(message.read?(person))}

message.to_xml(:builder => rootxml, :skip_instruct => true, :procs => [readStatus], :include => :group, :include => :waypoint)
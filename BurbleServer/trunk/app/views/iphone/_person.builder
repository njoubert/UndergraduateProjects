#Pass it the current builder as a :locals => {:rootxml => xml} 
person.to_xml(:builder => rootxml, :skip_instruct => true, :include => [:position, :group], :except => [:guid])
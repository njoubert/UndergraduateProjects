#Pass it the current builder as a :locals => {:rootxml => xml, :person => currentperson} 

readStatus = Proc.new {|opts| opts[:builder].read(message.read?(person))}

#TODO: Must wrtite a separete thing to display gorup when group_id is nil
#message.to_xml(:builder => rootxml, :skip_instruct => true, :procs => [readStatus], :include => :group)

formatter = Hash::XML_FORMATTING['datetime']

rootxml.tag!(message.class.name) do
  rootxml.tag!("id", message.id, :type => :integer)
  rootxml.tag!("sender-id", message.sender_id, :type => :integer)
  rootxml.tag!("group-id", message.group_id, :type => :integer)
  rootxml.tag!("waypoint-id", message.waypoint_id, :type => :integer)
  if message.sent_time
    rootxml.tag!("sent-time", formatter.call(message.sent_time), :type => :datetime)  
  else
    rootxml.tag!("sent-time", nil, :type => :datetime, :nil => :true)
  end
  rootxml.text(message.text)
  rootxml.read(message.read?(person))
  if message.group
    message.group.to_xml(:builder => rootxml, :skip_instruct => true)
  end
  
end

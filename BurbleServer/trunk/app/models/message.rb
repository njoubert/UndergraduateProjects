
#Represents a one-to-many broadcast of information.
#Can be displayed in the client's Feed

#To find a message, always use the Message class (not one of the subclasses). It will return the correct type.
#To set the status of a message, you can use the Message.mark() method.
class Message < ActiveRecord::Base
  belongs_to :sender, :class_name => "Person", :foreign_key => "sender_id"
  has_many :message_receivers
  has_many :people, :through => :message_receivers
  
  #This is how you create messages
  def self.factory!(type, params = nil)
    if not type
      throw "Nil type passed to message factory"
    else
      class_name = type
      if defined? class_name.constantize
        class_name.constantize.new(params)
      else
        throw "No such type of message"
      end
    end
  end
  
  #This allows you to set the message as read or not.
  def mark(person, read)
    mr = MessageReceiver.find(:first, :conditions => ["person_id = ? AND message_id = ?", person.id, self.id])
    if mr == nil
      raise "Attempting to set read status on a message and person that doesn't exist"
    end
    mr.read = read
    mr.save!
  end
  
  #This returns whether a message has been read
  def read?(person)
    return 0 < MessageReceiver.count(:all, :conditions => ["person_id = ? AND message_id = ? AND read = ?", person.id, self.id, true])
  end
  
end

class TextMessage < Message
end

class AudioMessage < Message
end

class RoutingRequestMessage < Message
  belongs_to :waypoint
end

class GroupInviteMessage < Message
  belongs_to :group
end

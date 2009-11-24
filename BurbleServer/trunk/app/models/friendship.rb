
#This stores the join table for the many-to-many person to person ("friend") self-referential relationship.
class Friendship < ActiveRecord::Base
  belongs_to :person
  belongs_to :friend, :foreign_key => "friend_id", :class_name => "Person"

  validate :already_friends?
  
  def already_friends?
    currently = Friendship.count(:all, :conditions => ["person_id = ? and friend_id = ?", person.id, friend_id])
    if (currently > 0)
      errors.add("Two people can only be in one friendship with each other.")
    end
  end

  def self.makeFriends(p1, p2)
    begin
      Friendship.transaction do
        p1.friends << p2
        p2.friends << p1
        p1.save
        p2.save
      end
    rescue Exception => ex
      logger.error ex
    end
  end
  
end

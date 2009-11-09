
#This defines the join table for the many-to-many relationship of people to groups.
#It also stores the "Active" field to reduce it to a many-to-one relationship of people to active group.
class GroupMembership < ActiveRecord::Base
  belongs_to :person
  belongs_to :group
  
  validate :unique_active?
  
  def unique_active?
    logger.warn "Checking for unique active!"
    currentActive = GroupMembership.count(:all, :conditions => ["person_id = ? and active = ?", person.id, true])
    if (currentActive != 0)
      errors.add("Person must have only one active group.")
    end
  end
  
end

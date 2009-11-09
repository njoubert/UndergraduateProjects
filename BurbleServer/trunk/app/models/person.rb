

#PERSON is the focal point of the app.

# Group Membership for a Person is handled partially by the GroupMembership class, partially here.
# A person can only have one active group membership, and this is enforced by GroupMembership.
# This means that attempting to do Group.people << person if that person is already in a group will
# cause an error.
# Since we want to be able to update a person's group, we do enable Person.group = nil and 
# Person.group = otherGroup to UPDATE the person's group membership, which is handled in this class.


class Person < ActiveRecord::Base
  has_many :friendships
  has_many :friends, :through => :friendships
  
  has_many :waypoints
  has_many :positions
  has_one :position, :order => "id DESC" #DOES THIS WORK RIGHT? IS THIS THE LATEST POSITION?
  
  has_many :group_memberships
  has_many :groups, :through => :group_memberships, :select => "distinct groups.*"
  
  has_one :group, :through => :group_memberships, :conditions => ["group_memberships.active = ?", true] #enforce with validation in GroupMembership
  validate :unique_active_group?
  
  def unique_active_group?
    if group == nil
      activeGroups = GroupMembership.find(:all, :conditions => ["person_id = ? and active = ?", id, true])
      activeGroups.each do |aG|
        logger.info "Setting a person's group memberships to inactive."
        
        aG.active = false
        aG.save!
      end
    end
  end
  
  def removeGroupMembership
    #find all the active groups for this user. Set them all to false.
    cgm = GroupMembership.find(:all, :conditions => ["person_id = ? and active = ?", self.id, true])
    for gm in cgm
      gm.active = false
      gm.save!
    end
  end
  
  def group=(g)
    if group != nil
      removeGroupMembership()
    end
    if g == nil
      return
    end
    logger.info "no group, making association"
    gm = GroupMembership.new
    gm.person = self
    gm.group = g
    gm.active = true
    gm.save!
  end
  
end

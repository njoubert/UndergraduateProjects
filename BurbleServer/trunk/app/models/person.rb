class Person < ActiveRecord::Base
  has_many :friendships
  has_many :friends, :through => :friendships
  
  has_many :waypoints
  has_many :positions
  has_one :position, :order => "id DESC" #DOES THIS WORK RIGHT? IS THIS THE LATEST POSITION?
  
  has_many :group_memberships
  has_many :groups, :through => :group_memberships, :select => "distinct groups.*"
  
  #Rather do this in code I write:
  has_one :group, :through => :group_memberships, :conditions => ["group_memberships.active = ?", true] #HOW TO ENFORCE?
  
  def removeGroupMembership
    logger.info "Setting a person's group memberships to inactive."
    #find all the active groups for this user. Set them all to false.
    cgm = GroupMembership.find(:all, :conditions => ["person_id = ? and active = ?", self.id, true])
    for gm in cgm
      gm.active = false
      gm.save!
    end
  end
  
  #This is how we override the default behavior of ActiveRecord. But we can't do this for <<
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

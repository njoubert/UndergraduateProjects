#
#PERSON is the focal point of the app.
#
# Group Membership for a Person is handled partially by the GroupMembership class, partially here.
# A person can only have one active group membership, and this is enforced by GroupMembership.
# This means that attempting to do Group.people << person if that person is already in a group will
# cause an error.
# Since we want to be able to update a person's group, we do enable Person.group = nil and 
# Person.group = otherGroup to UPDATE the person's group membership, which is handled in this class.
#
# == FRIENDSHIPS and FRIENDS
# A friend is another user of the app that this person can see. 
# This is a self-referential relationship - a person is friends with a person.
#
# == SENT_MESSAGES, MESSAGES and UNREAD_MESSAGES:
# a message has one sender (a person), and a person has many sent messages.
# a message has many people (receivers), and a person has many messages that he has received.
# we use the MessageReceiver model as a join table, and to store whether the message has been read.
# unread_messages return only messages a person has received that has false as their read column in MessageReceiver
# 
# == WAYPOINTS:
# A person has many waypoints that he has added.
#
# == POSITIONS:
# A person has many positions, which is a coordinate we have received from them.
#
# == GROUP and GROUPS:
# Groups are complicated! A person has many groups, and a group has many people, 
# connected through the GroupMembership join table.
# A person has either none or one active group. A group has active and inactive members.
# ** Adding people to groups: **
#   When we set a person's group, they leave (become inactive in) whatever group they're currently active in.
#   When we add a person that is active in group x to group y, an error is generated.
#   Thus, to add a person to a group, check that the person is not currently in a group, and if he is, make him leave first.
#   Leaving a group is as simple as setting the person's group to nil, which will call removeGroupMembership.
class Person < ActiveRecord::Base
  #friends
  has_many :friendships
  has_many :friends, :through => :friendships
  
  #messages
  has_many :sent_messages, :class_name => "Message", :foreign_key => "sender_id"
  has_many :message_receivers
  has_many :messages, :through => :message_receivers
  has_many :unread_messages, :through => :message_receivers, :source => :message, :conditions => ["message_receivers.read = ?", false]
  
  #waypoints
  has_many :waypoints
  has_many :positions
  has_one :position, :order => "id DESC" #DOES THIS WORK RIGHT? IS THIS THE LATEST POSITION?
  
  #groups
  has_many :group_memberships, :order => "created_at DESC" #HERE you can get when they've joined and when they've left, etc.
  has_many :groups, :through => :group_memberships, :select => "distinct groups.*" #this ONLY gives you all the groups he's been in. useless!
  has_one :group, :through => :group_memberships, :conditions => ["group_memberships.active = ?", true] #enforce with validation in GroupMembership
  
  #commented out since GroupMembership should take care of this!
  # validate :unique_active_group? 
  # 
  # def unique_active_group?
  #   #enforce no active group is group is nil
  #   if group == nil
  #     activeGroups = GroupMembership.find(:all, :conditions => ["person_id = ? and active = ?", id, true])
  #     activeGroups.each do |aG|
  #       logger.info "Setting a person's group memberships to inactive."
  #       aG.active = false
  #       aG.save!
  #     end
  #   end
  #   #enforce only one active group per person. Although I think we're duplicating work done in GroupMembership
  #   activeGroupsForPersonCount = GroupMembership.count(:all, :conditions => ["person_id = ? and active = ?", id, true])
  #   if activeGroupsForPersonCount > 1
  #     errors.add("More than one active group for this person")
  #   end
  # end
  
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
      self.reload
      return
    end
    gm = GroupMembership.new
    gm.person = self
    gm.group = g
    gm.active = true
    gm.save!
    self.reload
  end
               
end

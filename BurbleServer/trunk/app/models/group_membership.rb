
#This defines the join table for the many-to-many relationship of people to groups.
#It also stores the "Active" field to reduce it to a many-to-one relationship of people to active group.
class GroupMembership < ActiveRecord::Base
  belongs_to :person
  belongs_to :group
  
end

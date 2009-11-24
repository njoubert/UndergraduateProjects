
#Read PERSON
class Group < ActiveRecord::Base
  has_many :group_memberships
  has_many :people, 
              :through => :group_memberships, 
              :conditions =>  ["group_memberships.active = ?", true], 
              :select => "distinct people.*"
  has_many :allpeople, 
              :through => :group_memberships, 
              :source => :person,
              :select => "distinct people.*"
  has_many :waypoints
end

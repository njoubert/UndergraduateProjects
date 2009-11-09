
#This stores the join table for the many-to-many person to person ("friend") self-referential relationship.
class Friendship < ActiveRecord::Base
  belongs_to :person
  belongs_to :friend, :foreign_key => "friend_id", :class_name => "Person"
end

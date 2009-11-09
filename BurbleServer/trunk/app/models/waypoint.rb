class Waypoint < ActiveRecord::Base
  belongs_to :person #creator
  belongs_to :group #group created in
end

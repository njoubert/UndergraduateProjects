class BetaTester < ActiveRecord::Base
  validates_presence_of :email
  validates_uniqueness_of :email
  validates_inclusion_of :device, :in => ["iPod Touch", "iPhone"], :message => "should be iPod Touch or iPhone"
  validates_format_of :email, :with => /^([^@\s]+)@((?:[-a-z0-9]+\.)+[a-z]{2,})$/i
end

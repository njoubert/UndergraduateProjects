class Project < ActiveRecord::Base
  validates_presence_of :name, :description  
  validates_length_of :name, :minimum => 1 
  validates_length_of :description, :minimum => 5
end

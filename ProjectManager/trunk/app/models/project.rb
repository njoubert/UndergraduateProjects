class Project < ActiveRecord::Base
  # RELATIONSHIPS
  belongs_to :color
  has_and_belongs_to_many :categories
  
  # VALIDATION
  validates_presence_of :name, :description, :visible, :completed, :priority 
  validates_length_of :name, :minimum => 2
  
end

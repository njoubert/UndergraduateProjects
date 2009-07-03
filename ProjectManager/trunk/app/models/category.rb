class Category < ActiveRecord::Base
  has_and_belongs_to_many :projects
  belongs_to :color
  
  validates_presence_of :title
  validates_length_of :title, :minimum => 2
end

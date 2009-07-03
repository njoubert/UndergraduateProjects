class Color < ActiveRecord::Base
  has_many :projects
  has_many :categories
  
  validates_presence_of :title, :red, :green, :blue
  validates_length_of :title, :minimum => 2
  validates_numericality_of :red, :only_integer => true, :greater_than_or_equal_to  => 0, :less_than_or_equal_to => 255
  validates_numericality_of :green, :only_integer => true, :greater_than_or_equal_to  => 0, :less_than_or_equal_to => 255
  validates_numericality_of :blue, :only_integer => true, :greater_than_or_equal_to  => 0, :less_than_or_equal_to => 255
  
  def HTMLString
    colorString = "\##{correctLength(red.to_s(base=16))}#{correctLength(green.to_s(base=16))}#{correctLength(blue.to_s(base=16))}"
    return colorString
  end  
  
private
  def correctLength(colorStr)
    if colorStr.length == 0
      return "00"
    elsif colorStr.length == 1
      return "0" + colorStr
    elsif colorStr.length == 2
      return colorStr
    else
      return "FF"
    end
  end
end

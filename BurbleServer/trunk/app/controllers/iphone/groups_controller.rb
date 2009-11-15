class Iphone::GroupsController < Iphone::AbstractIphoneController
  
  before_filter :ensure_registered_if_iphone, :except => []
  
  def index
  end
  
end
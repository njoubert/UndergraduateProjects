module IphoneHelper
  
  def error
   render :status => 500, :template => 'iphone/error.rxml'
  end
  
end

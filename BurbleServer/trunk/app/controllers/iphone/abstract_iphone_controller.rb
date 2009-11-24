
# Abstract Controller as Base Class for iphone controllers.
class Iphone::AbstractIphoneController < ApplicationController
  before_filter :ensure_only_iphone
  
  def render_error(text, ex)
    logger.warn text if text
    logger.warn ex if ex
    @iphoneError[:error] = text
    @iphoneError[:exception] = ex.to_s if ex
    render :status => 500, :template => 'iphone/error.rxml'
  end
end
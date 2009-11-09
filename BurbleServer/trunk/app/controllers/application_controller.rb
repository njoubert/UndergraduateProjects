# Filters added to this controller apply to all controllers in the application.
# Likewise, all the methods added will be available for all controllers.

class ApplicationController < ActionController::Base
  helper :all # include all helpers, all the time
  #protect_from_forgery # See ActionController::RequestForgeryProtection for details
  
  before_filter :auth_for_iphone

  # Scrub sensitive parameters from your log
  # filter_parameter_logging :password
private
  #This method only does anything if its the iphone connecting, 
  #and if so it ensures that the person is sending their GUID to us, and finds that person.
  def auth_for_iphone
    if request.headers['HTTP_USER_AGENT'] =~ /iPhone/
      logger.info "An iPhone just logged on!"
    end
  end
end

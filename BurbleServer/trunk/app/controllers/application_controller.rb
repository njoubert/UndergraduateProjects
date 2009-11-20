# Filters added to this controller apply to all controllers in the application.
# Likewise, all the methods added will be available for all controllers.

class ApplicationController < ActionController::Base
  helper :all # include all helpers, all the time
  #protect_from_forgery # See ActionController::RequestForgeryProtection for details
  
  before_filter :detect_iphone
  
  #This will create the @person variable if the iphone user sent us his GUID.
  def ensure_registered_if_iphone
    if @iphoneUser
      begin
        @user = Person.find_by_guid(params[:guid])
        raise "no person with supplied guid found" if @user == nil
      rescue Exception => ex
        logger.error "An iPhone without a recognized UUID attempted to access something!"
        @iphoneError[:error] = "User not logged in!"
        @iphoneError[:exception] = ex.to_s
        render :status => 500, :template => 'iphone/error.rxml'
      end
    end
  end
  
  def ensure_only_iphone
    unless @iphoneUser
      logger.error "An iPhone-only page was accessed by a non-iPhone"
      flash[:error] = "Must be an iPhone to access that page."
      redirect_to(:controller => "/main")
    end
    @iphoneError = Hash.new
  end

  # Scrub sensitive parameters from your log
  # filter_parameter_logging :password
private
  #This method only does anything if its the iphone connecting, 
  #and if so it ensures that the person is sending their GUID to us, and finds that person.
  def detect_iphone
    #@iphoneUser = (request.headers['HTTP_USER_AGENT'] =~ /iPhone/) or (request.user_agent =~ /(Mobile\/.+Safari)/)
    @iphoneUser = true
  end
end

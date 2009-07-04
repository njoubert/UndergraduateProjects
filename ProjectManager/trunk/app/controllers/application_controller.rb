# Filters added to this controller apply to all controllers in the application.
# Likewise, all the methods added will be available for all controllers.

class ApplicationController < ActionController::Base
  helper :all # include all helpers, all the time
  protect_from_forgery # See ActionController::RequestForgeryProtection for details

  # Scrub sensitive parameters from your log
  # filter_parameter_logging :password
  
  before_filter :maintain_session_and_user
  
  def ensure_login
    unless @user
      flash[:notice] = "Sorry, but you need to login to continue!"
      redirect_to(new_session_path)
    end
  end
  
  def ensure_admin
    unless @user && @user.admin
      flash[:notice] = "Sorry chap, only admins can do that!"
      redirect_to(root_url)
    end
  end
  
  def ensure_logout
    if @user
      flash[:notice] = "Ah, the functionality you requested required you to be logged out!"
      redirect_to(root_url)
    end
  end
  
private

  # Checks that the session id holds up with our database, and updates the relevant information.
  # Shouldn't this be in SessionsController#update ??
  def maintain_session_and_user
    if session[:id]
      if @application_session = Session.find_by_id(session[:id])
        @application_session.update_attributes(:ip_address => request.remote_addr, :path => request.path_info)
        @user = @application_session.person
      else
        flash[:notice] = "Excuse me, but your session data appears to be outdated. Do you mind logging in again?"
        session[:id] = nil
        redirect_to(root_url)   #If he has an incorrect session id, send to root
      end
    end
  end
end

# This controller handles logins and authentication.
# Thus, to log in, you create a new session, and to log out, you destroy your session.
class SessionsController < ApplicationController

  before_filter :ensure_login, :only => :destroy
  before_filter :ensure_logout, :only => [:new, :create]
  
  # GET /sessions
  # We do not allow the viewing of all user sessions in this manner. 
  def index
    redirect_to(new_session_path)
  end

  # POST /sessions
  def create
    @session = Session.new(params[:session])
    if @session.save
      session[:id] = @session.id
      flash[:notice] = "Hello #{@session.person.name}! Good to have you back old chap!"
      redirect_to(root_url)
    else
      #A flash[:errors] will be set by the validation code in Session.
      flash[:notice] = "Sorry Sir, authentication failed!"
      render(:action => 'new')
    end
  end

  # GET /sessions/new
  # This is for logging in 
  # Requesting a new session is the same as preparing to log in
  def new
    @session = Session.new
  end

  # DELETE /sessions
  # This is for logging out.
  # Since a session represents a logged-in user, destroying it logs the current user out.
  def destroy
    Session.destroy(@application_session)
    session[:id] = @user = nil
    flash[:notice] = "Ta ta Sir! Your logout was successful."
    redirect_to(root_url)
  end
  
  
end

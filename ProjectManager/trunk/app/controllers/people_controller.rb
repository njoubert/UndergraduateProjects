
# Manages people. Not responsible for logins or auth
class PeopleController < ApplicationController
  before_filter :ensure_login
  before_filter :ensure_admin, :only => [:new, :create, :update, :edit]

  # GET /people
  def index
    @people = Person.find(:all)
  end

  # GET /people/1
  def show
    @person = Person.find(params[:id])
  end

  # GET /people/new
  def new
    @person = Person.new
  end

  # POST /people
  def create
    @person = Person.new(params[:person])
    if @person.save
      flash[:notice] = "I've added #{@person.name} to our userlist, as you requested Sir!"
      redirect_to(people_url)
    else
      #A flash[:errors] will be set by the validation code in Session.
      flash[:notice] = "Sorry Sir, but that user did not add correctly!"
      render(:action => 'new')
    end
  end
  
  # GET /people/1/edit
  def edit
    @person = Person.find(params[:id])
  end

  # PUT /people/1
  def update
    @person = Person.find(params[:id])
    
    if @person.update_attributes(params[:person])
      flash[:notice] = "I've updated the account of #{@person.name} as you requested, Sir!"
      redirect_to(people_url)
    else
      render(:action => 'edit')
    end
  end

  # DELETE /people/1
  def destroy
    @person = Person.find(params[:id])
    if @person != @user
      flash[:notice] = "I've deleted the user #{@person.name} at your request, Sir."
      Person.destroy(params[:id])
      redirect_to(people_url)
    else
      flash[:notice] = "Sorry Sir, but you cannot delete yourself!"
      redirect_to(people_url)
    end
  end

end
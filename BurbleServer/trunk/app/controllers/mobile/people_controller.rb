class Mobile::PeopleController < ApplicationController
  before_filter :ensure_only_iphone
  before_filter :ensure_registered_if_iphone, :except => [:new, :create]

  # GET /people
  def index
    @people = Person.all
    render :xml => @people
  end

  # GET /people/1
  def show
    begin
      @person = Person.find(params[:id], :include => [:group, :position])
    rescue Exception => ex
      logger.error ex
      @iphoneError[:error] = "No person found!!"
      render(:status => 500, :template => 'mobile/iphone/error.rxml')      
    end
  end
  
  # GET /people/guid/1
  def guid
    begin
      @person = Person.find_by_guid!(params[:id])
      render :action => :show
    rescue Exception => ex
      logger.error ex
      @iphoneError[:exception] = ex.to_s
      @iphoneError[:error] = "No person found!"
      render(:status => 500, :template => 'mobile/iphone/error.rxml')      
    end
  end
  
  # GET /people/name/Niels
  def name
    begin
      @people = Person.find(:all, :conditions => ["name like ?", params[:id]+"%"])
      render :xml => @people
    rescue Exception => ex
      logger.error ex
      @iphoneError[:exception] = ex.to_s
      @iphoneError[:error] = "No people found!"
      render(:status => 500, :template => 'mobile/iphone/error.rxml')      
    end
  end

  # GET /people/new
  def new
    @person = Person.new
    render :xml => @person
  end

  # POST /people
  def create
    @person = Person.new(params[:person])
    if @person.save
      render :xml => @person, :status => :created
    else
      @iphoneError[:error] = flash[:error]
      @iphoneError[:exception] = ex.to_s
      render(:status => 500, :template => 'mobile/iphone/error.rxml')      
    end
  end
  
  # GET /people/1/edit
  def edit
    begin
      @person = Person.find(params[:id])
      render :xml => @person
    rescue Exception => ex
      logger.error ex
      @iphoneError[:error] = "No person found!"
      @iphoneError[:exception] = ex.to_s
      render(:status => 500, :template => 'mobile/iphone/error.rxml')      
    end
  end

  # GET /people/1/friends
  def friends
    begin
      @person = Person.find(params[:id], :include => [:friends])
    rescue Exception => ex
      logger.error ex
      @iphoneError[:error] = "No person found!"
      @iphoneError[:exception] = ex.to_s
      render(:status => 500, :template => 'mobile/iphone/error.rxml')      
    end
  end
  
  # POST /people/1/addfriend with params[:friend_id]
  # Returns the new complete person with array of friends
  def addfriend
    begin
      @person = Person.find(params[:id])
      @friend = Person.find(params[:friend_id])
      Friendship.makeFriends(@person, @friend)
      render :action => :friends
    rescue Exception => ex
      logger.error ex
      @iphoneError[:error] = "Could not add friend!"
      @iphoneError[:exception] = ex.to_s
      render(:status => 500, :template => 'mobile/iphone/error.rxml')      
    end
  end

  # PUT /people/1
  def update
    begin
      @person = Person.find(params[:id])
      if @person.update_attributes(params[:person])
        render :action => :show
      else
        @iphoneError[:error] = "Update failed!"
        render(:status => 500, :template => 'mobile/iphone/error.rxml')
      end
    rescue Exception => ex
      logger.error ex
      @iphoneError[:error] = "No person found!"
      @iphoneError[:exception] = ex.to_s
      render(:status => 500, :template => 'mobile/iphone/error.rxml')      
    end    
  end

  # DELETE /people/1
  def destroy
    #Currently we do not support deleting users, so we just respond with an "ok!"
    head :ok
  end

end
class Mobile::IphoneController < ApplicationController
  
  before_filter :ensure_only_iphone
  before_filter :ensure_registered_if_iphone, :except => [:error, :new, :create]
  
  def error
   render :status => 500, :template => 'mobile/iphone/error.rxml'
  end
  
  def index
  end
  
  ####################################
  ######## PEOPLE MANAGEMENT
  ####################################  
  
  def new
    @person = Person.new
    render :xml => @person
  end
  
  # POST
  def create
    if request.post?
      @person = Person.new(params[:person])
      begin
        @person.save!
        render :xml => @person, :status => :created
      rescue Exception => ex
        @iphoneError[:error] = "Could not create new user!"
        @iphoneError[:exception] = ex.to_s
        render :status => 500, :template => 'mobile/iphone/error.rxml'
      end
    else
      @iphoneError[:error] = "Request not supported"
      render :status => 500, :template => 'mobile/iphone/error.rxml'
    end
  end
  
  ####################################
  ######## FRIENDS
  ####################################  
  
  def friends
    @friends = @user.friends
  end
  
  # POST
  # def addfriend
  #   if request.post?
  #     begin
  #       @friend = Person.find(params[:friend_id])
  #       Friendship.makeFriends(@user, @friend)
  #       head :ok
  #     rescue Exception => ex
  #       logger.error ex
  #       @iphoneError[:error] = "Could not add friend."
  #       @iphoneError[:exception] = ex.to_s
  #       render(:status => 500, :template => 'mobile/iphone/error.rxml')      
  #     end
  #   else
  #     @iphoneError[:error] = "Request not supported"
  #     render(:status => 500, :template => 'mobile/iphone/error.rxml')
  #   end
  # end

  ####################################  
  ######## MESSAGES 
  ####################################

  def messages
    @messages = @user.messages
  end
  
  def unread_messages
    @messages = @user.unread_messages
    render :action => :messages
  end
  
  # # PUT
  # def read_message
  #   begin
  #     @message = Message.find(params[:message_id])
  #     @message.mark(@user, true)
  #     head :ok
  #   rescue
  #     @iphoneError[:error] = "Could not mark message as read"
  #     render(:status => 500, :template => 'mobile/iphone/error.rxml')
  #   end
  # end

  ####################################
  ######## POSITION
  ####################################
  
  def position
    if request.post?
      @position = Position.new(params[:position])
      begin
        @position.save!
        head :ok
      rescue Exception => ex
        @iphoneError[:error] = "Could not save position!"
        @iphoneError[:exception] = ex.to_s
        render :status => 500, :template => 'mobile/iphone/error.rxml'
      end
    else
      @position = @user.position
      render :xml => @position
    end
  end

end

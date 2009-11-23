class Iphone::MessagesController < Iphone::AbstractIphoneController
  
  before_filter :ensure_registered_if_iphone, :except => []
  
  # GET the most recent messages
  def index
    @messages = @user.latest_messages
  end

  # GET all unread messages
  def unread
    @messages = @user.unread_messages
    render :action => :index
  end

  # POST a mesage as marked
  def mark
    begin
      @message = Message.find(params[:message_id])
      @message.mark(@user, true)
      head :ok
    rescue
      @iphoneError[:error] = "Could not mark message as read"
      render(:status => 500, :template => 'iphone/error.rxml')
    end
  end
  
  # POST a new message to a list of users.
  def send_msg
    if request.post?
      #TODO: check that sender_id is the same as the current user_id
      @message = Message.factory!(params[:type], params[:message])
      if (@message.sender != @user)
        render_error("You can only send messages on behalf of yourself.")
      else
        @receiver_ids = ActiveSupport::JSON.decode(params[:receivers])
        @receiver_ids.each do |r|
          p = Person.find(r)
          if p
            @message.people << p
          end
        end
        @message.save
      render :text => "yays!", :status => :created
      end
    else
      render_error("Request type not supported. Expected POST.", nil) 
    end
  end
      
end


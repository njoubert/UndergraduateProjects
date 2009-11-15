class Iphone::MessagesController < Iphone::AbstractIphoneController
  
  before_filter :ensure_registered_if_iphone, :except => []
  
  # GET the most recent messages
  def index
    @messages = @user.latest_messages
  end

  # GET all unread messages
  def unread
    @messages = @user.unread_messages
    render :action => :messages
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
  def send
    if request.post?
      
    else
      
    end
  end
      
end


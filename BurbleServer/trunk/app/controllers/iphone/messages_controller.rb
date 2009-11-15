class Iphone::MessagesController < Iphone::AbstractIphoneController
  
  before_filter :ensure_registered_if_iphone, :except => []
  
  def index
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
  #     render(:status => 500, :template => 'iphone/error.rxml')
  #   end
  # end  
end


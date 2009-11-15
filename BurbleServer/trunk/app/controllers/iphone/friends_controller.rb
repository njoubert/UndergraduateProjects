class Iphone::FriendsController < Iphone::AbstractIphoneController
  
  before_filter :ensure_registered_if_iphone, :except => []
  
  
  def index
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
  #       render(:status => 500, :template => 'iphone/error.rxml')      
  #     end
  #   else
  #     @iphoneError[:error] = "Request not supported"
  #     render(:status => 500, :template => 'iphone/error.rxml')
  #   end
  # end
  
end
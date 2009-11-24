class Iphone::FriendsController < Iphone::AbstractIphoneController
  
  before_filter :ensure_registered_if_iphone, :except => []
  
  # GET all friends
  def index
      @friends = @user.friends
  end
  
  # POST a new friend
  def add_friend
    if request.post?
      begin
        @friend = Person.find(params[:friend_id])
        Friendship.makeFriends(@user, @friend)
        head :ok
      rescue Exception => ex
        render_error("Could not add friend.", ex)
      end
    else
      render_error("Request not supported", nil)
    end
  end
  
end
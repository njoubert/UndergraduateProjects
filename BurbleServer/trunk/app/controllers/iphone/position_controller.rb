class Iphone::PositionController < Iphone::AbstractIphoneController
  
  before_filter :ensure_registered_if_iphone, :except => []
  
  def index
    if request.post?
      @position = Position.new(params[:position])
      begin
        @position.save!
        head :ok
      rescue Exception => ex
        @iphoneError[:error] = "Could not save position!"
        @iphoneError[:exception] = ex.to_s
        render :status => 500, :template => 'iphone/error.rxml'
      end
    else
      @position = @user.position
      render :xml => @position
    end
  end
  
end
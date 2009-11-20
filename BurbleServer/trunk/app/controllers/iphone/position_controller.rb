class Iphone::PositionController < Iphone::AbstractIphoneController
  
  before_filter :ensure_registered_if_iphone, :except => []
  
  # GET latest position for ME
  # POST new position for me
  def index
    if request.post
      @position = Position.new(params[:position])
      begin
        @position.save!
        head :ok
      rescue Exception => ex
        render_error("Could not save position!", ex)
      end
    else
      @position = @user.position
      render :xml => @position
    end
  end
  
end
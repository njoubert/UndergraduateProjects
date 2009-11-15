class Iphone::PersonController < Iphone::AbstractIphoneController
  
  before_filter :ensure_registered_if_iphone, :except => [:new, :create]
  
  def index
  end
  
  def new
    @person = Person.new
    render :xml => @persons 
  end
  
  # POST
  def create
    if request.post?
      @person = Person.new(params[:person])
      begin
        @person.save!
        render :xml => @person, :status => :created
      rescue Exception => ex
        render_error("Could not create new user!", ex)
      end
    else
      render_error("Request type not supported. Expected POST.", nil)
    end
  end

end

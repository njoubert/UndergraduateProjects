class Iphone::PersonController < Iphone::AbstractIphoneController
  
  before_filter :ensure_registered_if_iphone, :except => [:error, :new, :create]
  
  def index
  end
  
  ####################################
  ######## PEOPLE MANAGEMENT
  ####################################  
  
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
        @iphoneError[:error] = "Could not create new user!"
        @iphoneError[:exception] = ex.to_s
        render :status => 500, :template => 'iphone/error.rxml'
      end
    else
      @iphoneError[:error] = "Request not supported"
      render :status => 500, :template => 'iphone/error.rxml'
    end
  end

end

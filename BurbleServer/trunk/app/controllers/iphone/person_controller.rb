class Iphone::PersonController < Iphone::AbstractIphoneController
  
  before_filter :ensure_registered_if_iphone, :except => [:new, :create]
  
  def index
    @person = Person.find_by_guid(params[:guid])
  end
  
  def new
    @person = Person.new
    render :xml => @persons 
  end
  
  # POST
  def create
    if request.post?
      @person = Person.find_by_guid(params[:guid])        #returns 200 if we found you
      if @person
        render :xml => @person
      else
        @person = Person.new(params[:person])
        begin
          @person.save!
          render :xml => @person, :status => :created     #returns 201 if it was created
        rescue Exception => ex
          render_error("Could not create new user!", ex)
        end
      end
    else
      render_error("Request type not supported. Expected POST.", nil)
    end
  end

end

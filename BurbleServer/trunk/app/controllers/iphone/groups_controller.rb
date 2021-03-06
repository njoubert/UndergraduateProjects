class Iphone::GroupsController < Iphone::AbstractIphoneController
  
  before_filter :ensure_registered_if_iphone, :except => []
  
  # GET your current group
  def index
    render :xml => @user.group
  end
  
  # GET a blank group
  def new
    @group = Group.new
    render :xml => @group
  end
  
  # POST a new group
  def create
    # Leave the group if he is currently in a group!
    # When he creates, he also joins!
    if request.post?
      @group = Group.new(params[:group])
      begin
        @group.save!
        @user.group = @group
        render :xml => @group, :status => :created     #returns 201 if it was created
      rescue Exception => ex
        render_error("Could not create new group!", ex)
      end
    else
      render_error("Request type not supported. Expected POST.", nil)
    end
  end
  
  # GET a list of people in this group
  def members
    @friends = @user.group.people
  end
  
  # POST a new person to add to this group
  def add_member
    render_error("Request type not supported at this time.", nil)
#    if request.post?
      
#    else
      
#    end
  end
  
  # POST a group to join
  def join
    if request.post?
      @group = Group.find(params[:id])
      if @group
        begin
          @user.group = @group
          @user.save!
          render :xml => @group
        rescue Exception => ex
          render_error("Could not join you to group!", ex)
        end
      else
        render_error("Could not find the group you requested", nil)
      end
    else
      render_error("Request type not supported. Expected POST.", nil)
    end
  end
  # POST that you want to leave your current group
  def leave
    if request.post?
      @user.group = nil
      head :ok
    else
      render_error("Request type not supported. Expected POST.", nil)
    end
  end
  
  def add_waypoint_for_group
    if request.post?
      @group = Group.find(params[:id])
      if not @group
        render_error("Could not find group!", nil)
      else
        begin 
          @waypoint = Waypoint.new(params[:waypoint])
          @waypoint.person = @user
          @waypoint.group = @group
          @waypoint.save!
          render :text => @waypoint.id, :status => :created
        rescue Exception => ex
          render_error("Could not add waypoint!", ex)
        end
      end
    else
      render_error("Request type not supported. Expected POST.", nil)      
    end
  end
  
  def waypoints
    @waypoints = @user.group.waypoints
    render :xml => @waypoints
  end
  
end
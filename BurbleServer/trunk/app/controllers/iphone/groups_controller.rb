class Iphone::GroupsController < Iphone::AbstractIphoneController
  
  before_filter :ensure_registered_if_iphone, :except => []
  
  # GET your current group
  def index
  
  end
  
  # GET a blank group
  def new
    @group = Group.new
    render :xml => @group
  end
  
  # POST a new group
  def create
    if request.post?
      
    else
      
    end
  end
  
  # GET a list of people in this group
  def members
    
  end
  
  # POST a new person to add to this group
  def add_member
    if request.post?
      
    else
      
    end
  end
  
  # POST a group to join
  def join
    if request.post?
      
    else
      
    end
  end
  # POST that you want to leave your current group
  def leave
    if request.post?
      
    else
      
    end
  end
  
end
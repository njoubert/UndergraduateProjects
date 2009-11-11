class MainController < ApplicationController
  def index
    @personCount = Person.count
    @groupCount = Group.count
    @waypointCount = Waypoint.count
    @positionCount = Position.count

    if request.post?
      begin
        @beta_tester = BetaTester.new(params[:beta_tester])
        @beta_tester.save!
        flash[:notice] = "Success! You signed up with " + @beta_tester.email + "."
      rescue Exception => ex
        logger.warn ex
        if nil != BetaTester.find_by_email(params[:beta_tester][:email])
          flash[:notice] = "We already have you on file!"
        else
          flash[:notice] = "Snap, we could not save that email!"
        end
      end
    else
      @beta_tester = BetaTester.new
    end
    
  
  end
  
  

end

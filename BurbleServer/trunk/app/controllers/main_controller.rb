class MainController < ApplicationController
  def index
    @personCount = Person.count
    @groupCount = Group.count
    @waypointCount = Waypoint.count
    @positionCount = Position.count
  end

end

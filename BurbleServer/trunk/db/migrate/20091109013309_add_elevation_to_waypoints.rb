class AddElevationToWaypoints < ActiveRecord::Migration
  def self.up
    add_column :waypoints, :elevation, :float
  end

  def self.down
    remove_column :waypoints, :elevation
  end
end

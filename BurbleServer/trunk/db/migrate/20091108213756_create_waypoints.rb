class CreateWaypoints < ActiveRecord::Migration
  def self.up
    create_table :waypoints do |t|
      t.integer :person_id, :null => false
      t.integer :group_id
      t.string :name
      t.string :description
      t.float :latitude, :null => false
      t.float :longitude, :null => false
      t.timestamps
    end
  end

  def self.down
    drop_table :waypoints
  end
end

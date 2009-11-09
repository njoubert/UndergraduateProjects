class CreatePositions < ActiveRecord::Migration
  def self.up
    create_table :positions do |t|
      t.integer :person_id, :null => false
      t.float :latitude, :null => false
      t.float :longitude, :null => false
      t.float :vaccuracy
      t.float :haccuracy
      t.float :speed
      t.float :elevation
      t.float :heading
      t.string :type
      t.timestamps
    end
  end

  def self.down
    drop_table :positions
  end
end

class AddTimestampToPosition < ActiveRecord::Migration
  def self.up
    add_column :positions, :timestamp, :datetime
  end

  def self.down
    remove_column :positions, :timestamp
  end
end

class RemoveTimestampsFromPosition < ActiveRecord::Migration
  def self.up
    remove_column :positions, :created_at
    remove_column :positions, :updated_at
  end

  def self.down
    add_column :positions, :created_at, :datetime
    add_column :positions, :updated_at, :datetime
  end
end

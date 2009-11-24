class AddFbuidToPeople < ActiveRecord::Migration
  def self.up
    add_column :people, :fbuid, :integer
  end

  def self.down
    remove_column :people, :fbuid
  end
end

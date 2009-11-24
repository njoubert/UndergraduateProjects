class ChangeFbuidToString < ActiveRecord::Migration
  def self.up
    remove_column :people, :fbuid
    add_column :people, :fbuid, :string
  end

  def self.down
    remove_column :people, :fbuid
    add_column :people, :fbuid, :integer    
  end
end

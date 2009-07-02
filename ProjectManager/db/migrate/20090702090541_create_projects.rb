class CreateProjects < ActiveRecord::Migration
  def self.up
    create_table :projects do |t|
      t.string :name, :null => false
      t.string :description, :null => false
      t.boolean :visible, :default => true
      t.boolean :completed, :default => false
      t.float :priority, :default => 0.5

      t.timestamps
    end
  end

  def self.down
    drop_table :projects
  end
end

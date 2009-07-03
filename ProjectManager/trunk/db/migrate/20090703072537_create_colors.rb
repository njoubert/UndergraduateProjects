class CreateColors < ActiveRecord::Migration
  def self.up
    create_table :colors do |t|
      t.string :title
      t.integer :red, :default => 255
      t.integer :green, :default => 255
      t.integer :blue, :default => 255
      t.float :alpha, :default => 1.0

      t.timestamps
    end
    
    defaultcolor = Color.create(:title => 'default', :red => 255, :green => 255, :blue => 255, :alpha => 1.0)
    defaultcolor.save 
    
    color = Color.create(:title => 'Bright Red',:red => 255,:green => 0,:blue => 0,:alpha => 1.0)
    color.save
    color = Color.create(:title => 'Bright Green',:red => 0,:green => 255,:blue => 0,:alpha => 1.0)
    color.save
    color = Color.create(:title => 'Bright Blue',:red => 0,:green => 0,:blue => 255,:alpha => 1.0)
    color.save   
    
    # For a project, color_id of nil means "use the category color".
    add_column :projects, :color_id, :integer, :default => nil
    add_index :projects, :color_id
    add_column :categories, :color_id, :integer, :default => defaultcolor.id
    add_index :categories, :color_id
    
  end

  def self.down
    drop_table :colors
    remove_index :projects, :color_id
    remove_column :projects, :color_id, :integer
    remove_index :categories, :color_id
    remove_column :categories, :color_id, :integer

  end
end

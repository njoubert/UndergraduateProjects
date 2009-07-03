class CreateCategories < ActiveRecord::Migration
  def self.up
    create_table :categories do |t|
      t.string :title

      t.timestamps
    end
    
    create_table :categories_projects, :id => false do |t| 
      t.references :project 
      t.references :category 
    end 
    
    # Indexes are important for performance if join tables grow big 
    add_index :categories_projects, [:project_id, :category_id], :unique => true 
    add_index :categories_projects, :category_id, :unique => false 
    
  end

  def self.down
    drop_table :categories
    drop_table :categories_projects
  end
end

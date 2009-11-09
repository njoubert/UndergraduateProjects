class CreatePeople < ActiveRecord::Migration
  def self.up
    
    create_table :people do |t|
      t.string :guid, :null => false
      t.string :name, :null => false
      t.string :number
      t.string :email
      t.timestamps
    end
    
    add_index :people, :guid
  
  end

  def self.down
    drop_table :people
  end
end

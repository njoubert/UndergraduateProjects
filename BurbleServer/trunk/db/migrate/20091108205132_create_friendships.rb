class CreateFriendships < ActiveRecord::Migration
  def self.up
    create_table :friendships, :id => :false  do |t|
      t.integer :person_id, :null => false
      t.integer :friend_id, :null => false
    end
    
    add_index :friendships, :person_id
    add_index :friendships, :friend_id
  end

  def self.down
    drop_table :friendships
  end
end

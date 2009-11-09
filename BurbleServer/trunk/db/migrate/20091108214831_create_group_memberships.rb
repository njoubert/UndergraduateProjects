class CreateGroupMemberships < ActiveRecord::Migration
  def self.up
    create_table :group_memberships, :id => :false do |t|
      t.integer :person_id, :null => false
      t.integer :group_id, :null => false
      t.boolean :active, :default => true
      t.timestamps
    end
    
    add_index :group_memberships, :person_id
    add_index :group_memberships, :group_id
  end

  def self.down
    drop_table :group_memberships
  end
end

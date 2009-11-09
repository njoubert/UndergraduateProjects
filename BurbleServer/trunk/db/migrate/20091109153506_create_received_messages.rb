class CreateReceivedMessages < ActiveRecord::Migration
  def self.up
    create_table :received_messages, :id => :false do |t|
      t.integer :recipient_person_id, :null => false
      t.integer :message_id, :null => false
      t.boolean :read, :default => false
    end
    
    add_index :received_messages, :recipient_person_id
    add_index :received_messages, :message_id
  end

  def self.down
    drop_table :received_messages
  end
end

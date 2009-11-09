class RenameRecipeintInMessageReceivers < ActiveRecord::Migration
  def self.up
    rename_column :message_receivers, :recipient_person_id, :person_id
  end

  def self.down
    rename_column :message_receivers, :person_id, :recipient_person_id    
  end
end

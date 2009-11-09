class RenameReceivedMessages < ActiveRecord::Migration
  def self.up
    rename_table :received_messages, :message_receivers
  end

  def self.down
    rename_table :message_receivers, :received_messages
  end
end

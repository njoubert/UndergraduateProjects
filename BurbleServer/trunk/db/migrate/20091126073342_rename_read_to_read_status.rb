class RenameReadToReadStatus < ActiveRecord::Migration
  def self.up
    rename_column :message_receivers, :read, :has_read_bool
  end

  def self.down
    rename_column :message_receivers, :has_read_bool, :read
  end
end

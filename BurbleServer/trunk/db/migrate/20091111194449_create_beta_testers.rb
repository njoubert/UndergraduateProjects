class CreateBetaTesters < ActiveRecord::Migration
  def self.up
    create_table :beta_testers do |t|
      t.string :email
      t.string :device

      t.timestamps
    end
  end

  def self.down
    drop_table :beta_testers
  end
end

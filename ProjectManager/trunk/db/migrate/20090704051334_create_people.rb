class CreatePeople < ActiveRecord::Migration
  def self.up
    create_table :people do |t|
      t.string :name
      t.string :salt
      t.string :encrypted_password
      t.timestamps
    end
    
    #defUser = Person.new(
    #  :name => "root",
    #  :salt => "123456789",
    #  :encrypted_password => "082526cd770fd2c27786670026daf1a04152680c28bef33ca54119567356132c"
    #)
    #defUser.save
    
  end

  def self.down
    drop_table :people
  end
end

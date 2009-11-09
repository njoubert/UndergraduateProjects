require 'active_record/fixtures'

class AddPeopleData < ActiveRecord::Migration
  def self.up
    #down
    
    #dir = File.join(File.dirname(__FILE__), 'dev_data')
    #Fixtures.create_fixtures(dir, "people")
  end

  def self.down
    #Person.delete_all
  end
end
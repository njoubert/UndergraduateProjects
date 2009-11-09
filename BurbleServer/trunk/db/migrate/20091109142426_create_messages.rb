class CreateMessages < ActiveRecord::Migration
  def self.up
    create_table :messages, :force => true do |t|
      t.string :type, :null => false                            #for single-table inheritance, AWDwR p.378
      
      #common attributes
      t.integer :sender_id                        #belongs_to person that is the sender
      
      
      #text attributes
      t.text :text                                 #the text of the message
      
      #voice attributes
      t.string :audioLocation                      #the audio file of the message
      
      
      #routing request attributes
      t.integer :waypoint_id                       #the waypoint to route to
      
      
      #group invite attributes
      t.integer :group_id                          #the group people should join
      

      t.timestamps
    end
  end

  def self.down
    drop_table :messages
  end
end

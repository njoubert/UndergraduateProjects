# This file is auto-generated from the current state of the database. Instead of editing this file, 
# please use the migrations feature of Active Record to incrementally modify your database, and
# then regenerate this schema definition.
#
# Note that this schema.rb definition is the authoritative source for your database schema. If you need
# to create the application database on another system, you should be using db:schema:load, not running
# all the migrations from scratch. The latter is a flawed and unsustainable approach (the more migrations
# you'll amass, the slower it'll run and the greater likelihood for issues).
#
# It's strongly recommended to check this file into your version control system.

ActiveRecord::Schema.define(:version => 20091109154942) do

  create_table "friendships", :force => true do |t|
    t.integer "person_id", :null => false
    t.integer "friend_id", :null => false
  end

  add_index "friendships", ["friend_id"], :name => "index_friendships_on_friend_id"
  add_index "friendships", ["person_id"], :name => "index_friendships_on_person_id"

  create_table "group_memberships", :force => true do |t|
    t.integer  "person_id",                    :null => false
    t.integer  "group_id",                     :null => false
    t.boolean  "active",     :default => true
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  add_index "group_memberships", ["group_id"], :name => "index_group_memberships_on_group_id"
  add_index "group_memberships", ["person_id"], :name => "index_group_memberships_on_person_id"

  create_table "groups", :force => true do |t|
    t.string   "name"
    t.string   "description"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "message_receivers", :force => true do |t|
    t.integer "person_id",                     :null => false
    t.integer "message_id",                    :null => false
    t.boolean "read",       :default => false
  end

  add_index "message_receivers", ["message_id"], :name => "index_received_messages_on_message_id"
  add_index "message_receivers", ["person_id"], :name => "index_received_messages_on_recipient_person_id"

  create_table "messages", :force => true do |t|
    t.string   "type",          :null => false
    t.integer  "sender_id"
    t.text     "text"
    t.string   "audioLocation"
    t.integer  "waypoint_id"
    t.integer  "group_id"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "people", :force => true do |t|
    t.string   "guid",       :null => false
    t.string   "name",       :null => false
    t.string   "number"
    t.string   "email"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  add_index "people", ["guid"], :name => "index_people_on_guid"

  create_table "positions", :force => true do |t|
    t.integer  "person_id",  :null => false
    t.float    "latitude",   :null => false
    t.float    "longitude",  :null => false
    t.float    "vaccuracy"
    t.float    "haccuracy"
    t.float    "speed"
    t.float    "elevation"
    t.float    "heading"
    t.string   "type"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "waypoints", :force => true do |t|
    t.integer  "person_id",   :null => false
    t.integer  "group_id"
    t.string   "name"
    t.string   "description"
    t.float    "latitude",    :null => false
    t.float    "longitude",   :null => false
    t.datetime "created_at"
    t.datetime "updated_at"
    t.float    "elevation"
  end

end

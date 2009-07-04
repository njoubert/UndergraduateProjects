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

ActiveRecord::Schema.define(:version => 20090704081326) do

  create_table "categories", :force => true do |t|
    t.string   "title"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.integer  "color_id",   :default => 1
  end

  add_index "categories", ["color_id"], :name => "index_categories_on_color_id"

  create_table "categories_projects", :id => false, :force => true do |t|
    t.integer "project_id"
    t.integer "category_id"
  end

  add_index "categories_projects", ["category_id"], :name => "index_categories_projects_on_category_id"
  add_index "categories_projects", ["project_id", "category_id"], :name => "index_categories_projects_on_project_id_and_category_id", :unique => true

  create_table "colors", :force => true do |t|
    t.string   "title"
    t.integer  "red",        :default => 255
    t.integer  "green",      :default => 255
    t.integer  "blue",       :default => 255
    t.float    "alpha",      :default => 1.0
    t.datetime "created_at"
    t.datetime "updated_at"
  end

  create_table "people", :force => true do |t|
    t.string   "name"
    t.string   "salt"
    t.string   "encrypted_password"
    t.datetime "created_at"
    t.datetime "updated_at"
    t.boolean  "admin",              :default => false
    t.string   "email"
  end

  create_table "projects", :force => true do |t|
    t.string   "name",                           :null => false
    t.string   "description",                    :null => false
    t.boolean  "visible",     :default => true
    t.boolean  "completed",   :default => false
    t.float    "priority",    :default => 0.5
    t.datetime "created_at"
    t.datetime "updated_at"
    t.integer  "color_id"
  end

  add_index "projects", ["color_id"], :name => "index_projects_on_color_id"

  create_table "sessions", :force => true do |t|
    t.integer  "person_id"
    t.string   "ip_address"
    t.string   "path"
    t.datetime "created_at"
    t.datetime "updated_at"
  end

end

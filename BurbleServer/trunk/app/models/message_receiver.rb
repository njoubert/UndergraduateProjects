class MessageReceiver < ActiveRecord::Base
  belongs_to :message
  belongs_to :person
end

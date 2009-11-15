ActionController::Routing::Routes.draw do |map|
  
  map.namespace :iphone do |iphone|
    iphone.root :controller => "person"
    iphone.connect    ':guid',                         :controller => "person"
    iphone.friends    ':guid/person/:action/:id',     :controller => "person"
    iphone.friends    ':guid/friends/:action/:id',     :controller => "friends"
    iphone.groups     ':guid/groups/:action/:id',       :controller => "groups"
    iphone.messages   ':guid/messages/:action/:id',   :controller => "messages"
    iphone.position   ':guid/position/:action/:id',   :controller => "position"
  end
  
  map.connect 'main/:action', :controller => "main"
  map.root :controller => "main"
  
end

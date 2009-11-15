ActionController::Routing::Routes.draw do |map|
  # The priority is based upon order of creation: first created -> highest priority.

  # Sample of regular route:
  # map.connect 'products/:id', :controller => 'catalog', :action => 'view'
  # Keep in mind you can assign values other than :controller and :action

  # Sample of named route:
  #   map.purchase 'products/:id/purchase', :controller => 'catalog', :action => 'purchase'
  # This route can be invoked with purchase_url(:id => product.id)

  #map.resources :people
  
  # Sample resource route with options:
  #   map.resources :products, :member => { :short => :get, :toggle => :post }, :collection => { :sold => :get }

  # Sample resource route with sub-resources:
  #   map.resources :products, :has_many => [ :comments, :sales ], :has_one => :seller
  
  # Sample resource route with more complex sub-resources
  #   map.resources :products do |products|
  #     products.resources :comments
  #     products.resources :sales, :collection => { :recent => :get }
  #   end

  map.namespace :iphone do |iphone|
    iphone.root :controller => "person"
    iphone.connect ':guid',                         :controller => "person"
    iphone.friends ':guid/friends/:action/:id',     :controller => "friends"    
    iphone.groups ':guid/groups/:action/:id',       :controller => "groups"    
    iphone.messages ':guid/messages/:action/:id',   :controller => "messages"    
    iphone.position ':guid/position/:action/:id',   :controller => "position"    
  end
 
  # See how all your routes lay out with "rake routes"
  
  # Install the default routes as the lowest priority.
  # Note: These default routes make all actions in every controller accessible via GET requests. You should
  # consider removing the them or commenting them out if you're using named routes and resources.
  
  map.connect 'main/:action', :controller => "main"
#  map.connect ':controller/:action/:id.:format'  
#  map.connect ':controller/:action/:id'

  # You can have the root of your site routed with map.root -- just remember to delete public/index.html.
  map.root :controller => "main"
  
end

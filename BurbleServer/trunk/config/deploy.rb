set :user, 'njoubert' 
set :domain, 'njoubert.com' 
set :application, 'BurbleServer'

set :repository, "svn+ssh://#{user}@#{domain}/var/svn/#{application}/trunk" 
set :deploy_to, "/var/vhosts/#{domain}/#{application}"

role :app, domain
role :web, domain
role :db,  domain, :primary => true


namespace :deploy do 
  task :restart do 
    run "touch #{current_path}/tmp/restart.txt" 
  end 
end 

# optional task to reconfigure databases 
after "deploy:update_code", :configure_database 
desc "copy database.yml into the current release path" 
task :configure_database, :roles => :app do 
  #db_config = "/var/db/#{domain}/#{application}/config/database.yml"
  db_config = "#{deploy_to}/config/database.yml" 
  run "cp #{db_config} #{release_path}/config/database.yml" 
end 

# be sure to change these 
set :user, 'njoubert' 
set :domain, 'njoubert.com' 
set :application, 'ProjectManager' 

# file paths 
set :repository, "svn+ssh://#{user}@#{domain}/var/svn/#{application}/trunk" 
set :deploy_to, "/var/vhosts/projects.#{domain}" 

# distribute your applications across servers (the instructions below put them 
# all on the same server, defined above as 'domain', adjust as necessary) 
role :app, domain 
role :web, domain 
role :db, domain, :primary => true 

# As Capistrano executes in a non-interactive mode and therefore doesn't cause 
# any of your shell profile scripts to be run, the following might be needed 
# if (for example) you have locally installed gems or applications. Note: 
# this needs to contain the full values for the variables set, not simply 
# the deltas. 
# default_environment['PATH']='<your paths>:/usr/local/bin:/usr/bin:/bin' 
# default_environment['GEM_PATH']='<your paths>:/usr/lib/ruby/gems/1.8' 

# task which causes Passenger to initiate a restart 
namespace :deploy do 
  task :restart do 
    run "touch #{current_path}/tmp/restart.txt" 
  end 
end 

# optional task to reconfigure databases 
after "deploy:update_code", :configure_database 
desc "copy database.yml into the current release path" 
task :configure_database, :roles => :app do 
  db_config = "/var/db/projects.njoubert.com/config/database.yml" 
  run "cp #{db_config} #{release_path}/config/database.yml" 
end 

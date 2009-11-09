desc "Configure Subversion for Rails"
task :setup_svn do
  puts "Removing /log"
  system "svn remove log/*"
  system "svn commit -m 'removing all log files from subversion'"
  system 'svn propset svn:ignore "*.log" log/'
  system "svn update log/"
  system "svn commit -m 'Ignoring all files in /log/ ending in .log'"
  
  puts "Ignoring /db"
  system 'svn propset svn:ignore "*.db" db/'
  system "svn update db/"
  system "commit -m 'Ignoring all files in /db/ ending in .db'"
  
  puts "Ignoring /tmp"
  system 'svn propset svn:ignore "*" tmp/'
  system "svn update tmp/"
  system "svn commit -m 'Ignoring all files in /tmp/'"

  puts "Ignoring /doc"
  system 'svn propset svn:ignore "*" doc/'
  system "svn update doc/"
  system "svn commit -m 'Ignoring all files in /doc/'"
end
  
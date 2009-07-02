# Be sure to restart your server when you modify this file.

# Your secret key for verifying cookie session data integrity.
# If you change this key, all old sessions will become invalid!
# Make sure the secret is at least 30 characters and all random, 
# no regular words or you'll be exposed to dictionary attacks.
ActionController::Base.session = {
  :key         => '_ProjectManager_session',
  :secret      => '29d25447cc0d526e8f553c8141e91de38752d7fbc2ebf1b54a67747e9bee0b0c9fa6beead6432bb109cde718b9807c9ecca888d8ba52d70feb0e6d629aaecf34'
}

# Use the database for sessions instead of the cookie-based default,
# which shouldn't be used to store highly confidential information
# (create the session table with "rake db:sessions:create")
# ActionController::Base.session_store = :active_record_store

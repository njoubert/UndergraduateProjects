# Be sure to restart your server when you modify this file.

# Your secret key for verifying cookie session data integrity.
# If you change this key, all old sessions will become invalid!
# Make sure the secret is at least 30 characters and all random, 
# no regular words or you'll be exposed to dictionary attacks.
ActionController::Base.session = {
  :key         => '_ProjectManager_session',
  :secret      => '2c5cd5adc839a0f70891662264e2eb60854ccc16f0b46d02361763bcccd95fa7277dc6262d90e28bc6daeda69e7f8ff00d58212724f10c02495e417ea31e88c9'
}

# Use the database for sessions instead of the cookie-based default,
# which shouldn't be used to store highly confidential information
# (create the session table with "rake db:sessions:create")
# ActionController::Base.session_store = :active_record_store

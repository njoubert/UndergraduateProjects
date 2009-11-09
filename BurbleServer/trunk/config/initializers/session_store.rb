# Be sure to restart your server when you modify this file.

# Your secret key for verifying cookie session data integrity.
# If you change this key, all old sessions will become invalid!
# Make sure the secret is at least 30 characters and all random, 
# no regular words or you'll be exposed to dictionary attacks.
ActionController::Base.session = {
  :key         => '_BurbleServer_session',
  :secret      => '7e825a906794e3a5f4539d30bc21cc4841fd53073c7cd7d323752a526b0eb0b733c871eb363dabe15aa44522d523b332c203cbbd65143a69209b511d67f52a3c'
}

# Use the database for sessions instead of the cookie-based default,
# which shouldn't be used to store highly confidential information
# (create the session table with "rake db:sessions:create")
# ActionController::Base.session_store = :active_record_store

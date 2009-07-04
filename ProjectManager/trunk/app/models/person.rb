require 'digest/sha2'

# Represents a single person in our database. This is used for logins.
class Person < ActiveRecord::Base
  attr_reader :password
  
  has_many :sessions, :dependent => :destroy

  validates_uniqueness_of :name, 
    :message => 'is already in use by another person.'
  
  validates_format_of :name, :with => /^([a-z0-9_]{2,16})$/i,
    :message => 'must be 4 to 16 characters',
    :unless => :password_is_not_being_updated?
    
  validates_confirmation_of :password # This uses that magical :password_confirmation in People#_form.html.erb

  validates_presence_of :encrypted_password

  before_save :scrub_name
  after_save :flush_passwords

  ENCRYPT = Digest::SHA256

  def self.find_by_name_and_password(name, password)
    person = self.find_by_name(name)
    if person and person.encrypted_password == ENCRYPT.hexdigest(password + person.salt)
      return person
    end
  end

  def password=(password)
    @password = password
    unless password_is_not_being_updated?
      self.salt = Array.new(9){rand(256).chr}.join
      self.encrypted_password = ENCRYPT.hexdigest(password + self.salt)
    end
  end

private
  def self.encrypt(pass, salt)
    return 
  end

  def scrub_name
    #We store all usernames as lowercase only
    self.name.downcase!
  end
  
  # Clear our local copies of the plaintext password. 
  def flush_passwords
    @password = @password_confirmation = nil
  end
  
  
  # True if the object already exists and the plaintext (user-entered) password is not set
  def password_is_not_being_updated?
    self.id and self.password.blank?
  end

end

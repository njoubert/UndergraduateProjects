
# Abstract Controller as Base Class for iphone controllers.
class Iphone::AbstractIphoneController < ApplicationController
  before_filter :ensure_only_iphone
end
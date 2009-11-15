xml.instruct!

xml.result do
  
  render :partial => "iphone/person", :object => @user, :locals => {:rootxml => xml}
  
  xml.friendslist do
    if @friends
      @friends.each do |fr|
        render :partial => "iphone/person", :object => fr, :locals => {:rootxml => xml}
      end
    end
  end

end
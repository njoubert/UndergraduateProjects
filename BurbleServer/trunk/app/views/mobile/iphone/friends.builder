xml.instruct!

xml.result do
  
  render :partial => "person", :object => @user, :locals => {:rootxml => xml}
  
  xml.friendslist do
    if @friends
      @friends.each do |fr|
        render :partial => "person", :object => fr, :locals => {:rootxml => xml}
      end
    end
  end

end
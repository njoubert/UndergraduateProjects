xml.instruct!

xml.result do
  
  render :partial => "person", :object => @user, :locals => {:rootxml => xml}
  
  xml.messages do
    if @messages
      @messages.each do |ms|
        render :partial => "message", :object => ms, :locals => {:rootxml => xml, :person => @user}
      end
    end
  end

end
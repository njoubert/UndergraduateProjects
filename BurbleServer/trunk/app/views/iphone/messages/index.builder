xml.instruct!

xml.result do  
  xml.messages do
    if @messages
      @messages.each do |ms|
        render :partial => "message", :object => ms, :locals => {:rootxml => xml, :person => @user}
      end
    end
  end

end
xml.instruct!

xml.result do
  
  render :partial => "person", :object => @person, :locals => {:rootxml => xml}
  xml.friends do
    @person.friends.each do |fr|
      render :partial => "person", :object => fr, :locals => {:rootxml => xml}
    end
  end

end
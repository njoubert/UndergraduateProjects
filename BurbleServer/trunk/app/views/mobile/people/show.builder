xml.instruct!
xml.result do
  render :partial => "person", :object => @person, :locals => {:rootxml => xml}
end
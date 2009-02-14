class TextMessageClient():
    MAX_TEXT_LENGTH = 160
    
    def __init__(self):
        print 'client created'
    
    def createTextMessageError(self, text):
        string = "Your text message has failed to process correctly <below>:"
        string += text
        return string
    
    def createTextMessageResult(self, product_name, rating, max_rating, average_price, local_price, local_source, online_price, online_source):
#        Format
#        25: PRODUCT_NAME. (X.X/5) Average Price: $ ____.
#        Local: $_____ at _____.  Online: $____ at _____.
        roundingPoint = "%.1f"
        normalizedRating = (float(rating)/float(max_rating)) * 5
        finalRating = roundingPoint % normalizedRating 
        
        string = ""
        string += " (" + finalRating + "/5) "
        string += "Avg: $" + str(self.massagePrice(average_price)) + ". "
        string += "Local: $" + str(self.massagePrice(local_price)) + " at " + local_source[0:20] + ". "
        string += "Online: $" + str(self.massagePrice(online_price)) + " at " + online_source[0:20] + " "
        
        endlength = len(string)
        beginlength = min(len(product_name), self.MAX_TEXT_LENGTH - endlength - 3)
        name = product_name[0:beginlength]
        if (self.MAX_TEXT_LENGTH - endlength < len(product_name)):
            name += "..."
        name += string
        return name[0:self.MAX_TEXT_LENGTH]

    def massagePrice(self, price):
        roundingPoint = "%.2f"
        if (price > 20):
            return int(round(float(price)))
        else:
            return roundingPoint % price
            
    def receiveTextMessage(self, message):
        tm = message.split(" ")
#        [UPC]...[UPC]



#        [City] [State] [UPC]...[UPC]




tm = TextMessageClient()
text = tm.createTextMessageResult("CANON 24to105mm lens 4 USMawefawefeaergaergeagreargeargeargeargeargaorhgaeroigheargoiheargoiahergoeagh", 8.23523432, 10.0, "324.23", "302.12", "Best Buy", "288.83", "canon.com")
print "Length: " + str(len(text))
print text  
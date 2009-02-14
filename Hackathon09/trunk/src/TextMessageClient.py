import re

class TextMessageClient():
    MAX_TEXT_LENGTH = 160
    count = 0
    
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
        if (local_source != ""):
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
    
    def parseUPC(self, number):
        return (re.match("[0-9]", number) != None and len(number) >= 8 and len(number) <= 13)
    
    def receiveTextMessage(self, message):
        successes = []
        city = ""
        zip = -1
        errormessage = ""
        tm = message.split(" ")
        for i in range(len(tm)):
            text = tm[i]
            isUPC = self.parseUPC(text)
            if not isUPC and i == 0:
                if re.match("[a-zA-Z]", text) != None:
                    city = text
                elif len(text) == 5:
                    zip = text
                else:
                    errormessage += "ERROR: cannot parse " + text + "."
            elif isUPC:
                successes.append((text, city))
            else:
                errormessage += "ERRROR: cannot parse " + text + "."
        return (successes[:], errormessage)

tm = TextMessageClient()
text = tm.createTextMessageResult("CANON 24to105mm lens 4 USMawefawefeaergaergeagreargeargeargeargeargaorhgaeroigheargoiheargoiahergoeagh", 8.23523432, 10.0, "324.23", "302.12", "Best Buy", "288.83", "canon.com")
print "Length: " + str(len(text))
print text  
result = tm.receiveTextMessage("987342234567 wefwef")
print result

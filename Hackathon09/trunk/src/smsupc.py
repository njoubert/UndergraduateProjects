import sys
import re
import productSearch
import ShopLocalParser
import myclickatell


def massagePrice(price):
    roundingPoint = "%.2f"
    if (price > 20):
        return int(round(float(price)))
    
def massageName(name):
    newName = str(name)[0:25]
    if len(name) > 25:
        newName += "..."
    return newName

# TODO: argument checking
#phoneNumber = sys.argv[1]
#sms = sys.argv[2]
phoneNumber = "fake"
#sms = "2               410000039144 013803050844"
sms = "berkeley 2               410000039144 013803050844"
smsList = [x for x in sms.split(" ") if len(x) > 0]

location = None
if re.match("[a-zA-Z]+", smsList[0]):
    # handle city name
    location = smsList[0]
    smsList.pop(0)
elif len(smsList[0]) == 5:
    # TODO: handle zip code
    location = None
    smsList.pop(0)

c = myclickatell.Clickatell("njoubert", "rsd887", "3076162")
# not actually number of results returned, but number considered
numGoogleResults = 30
failList = []
for upc in smsList:
    
    validLocal = False
    validOnline = False
    response = upc + ":\n"
    avgPrice = 0
    
    # local
    if location != None:
        baseName = productSearch.getProductName(upc)
        if baseName != None:
            localData = productSearch.localDataParser().getLocalProductData(baseName, location)
            avgPrice = localData[0]
            if len(localData) > 0:
                validLocal = True
                response += "Local: " + massageName(localData[1]) + " - $" + str(massagePrice(localData[3])) + " at " + massageName(localData[2]) + "\n"
            else:
                response += "Local: No Results\n"
        else:
            response += "Local: No Results\n"
    
    # online    
    googleData = productSearch.getGoogleProductData(upc, numGoogleResults)
    if googleData != None:
        validOnline = True
        item = googleData[0]
        avgPrice += googleData[1]
        if validLocal:
            avgPrice /= 2
        response += "Online: " + massageName(item[0]) + " - $" + str(massagePrice(item[2])) + " at " + massageName(item[1]) + "\n"
    else:
        response += "Online: No Results\n"
    
    # send sms
    if validLocal or validOnline:
        print"---------------------"
        response += "Avg: $" + str(massagePrice(avgPrice))
        print response
        print len(response)
#        c.sendMsg(phoneNumber, response)
    else:
        failList.append(upc)

if len(failList) > 0:
    response = ""
    for upc in failList:
        response += upc + ": No Results\n"
    print "---------------------"
    print response
#    c.sendMsg(phoneNumber, response)
    
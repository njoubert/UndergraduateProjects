import sys
import re
import productSearch
import myclickatell


def massagePrice(price):
    roundingPoint = "%.2f"
    if (price > 20):
        return int(round(float(price)))
    
def massageName(name, limit):
    newName = str(name)[0:limit]
    if len(name) > limit:
        newName += "..."
    return newName

# TODO: argument checking
phoneNumber = sys.argv[1]
sms = sys.argv[2]

# Test cases
#phoneNumber = "fake"
#sms = "2               410000039144 013803050844"
#sms = "san francisco 2              410000039144 013803050844"
#sms = "013803050844"

smsList = [x for x in sms.split(" ") if len(x) > 0]
locationList = []
for i in range(len(smsList)):
    if re.match("[a-zA-Z]+", smsList[i]):
        # handle city name
        locationList.append(smsList[i])
    else:
        break
    
if len(locationList) == 0 and len(smsList[0]) == 5:
    # TODO: handle zip code
    print "HANDLE ZIP CODE!!!!"
    pass

smsList = smsList[len(locationList):]
location = " ".join(locationList)
print location
print smsList

c = myclickatell.Clickatell("njoubert", "rsd887", "3076162")
# not actually number of results returned, but number considered
numGoogleResults = 30
failList = []
for upc in smsList:
    
    validLocal = False
    validOnline = False
    response = ""
    avgPrice = 0
    
    baseName = productSearch.getProductName(upc)
    if baseName != None:
        # local
        if len(location) > 0:
            localData = productSearch.localDataParser().getLocalProductData(baseName, location)
            avgPrice = localData[0]
            if len(localData) > 0:
                validLocal = True
                response += "Local: " + massageName(localData[1], 25) + " $" + str(massagePrice(localData[3])) + " at " + massageName(localData[2], 15) + "\n"
            else:
                response += "Local: No Results\n"
        else:
            response += "Local: No Results\n"
    
        # online    
        googleData = productSearch.getGoogleProductData(baseName, upc, numGoogleResults)
        if googleData != None:
            validOnline = True
            item = googleData[0]
            avgPrice += googleData[1]
            if validLocal:
                avgPrice /= 2
            response += "Web: " + massageName(item[0], 25) + " $" + str(massagePrice(item[2])) + " at " + massageName(item[1], 15) + "\n"
        else:
            response += "Web: No Results\n"
    
    # send sms
    if validLocal or validOnline:
        response += "Avg: $" + str(massagePrice(avgPrice)) + "\n"
        response += upc
        print "---------------------------"
        print response
        c.sendMsg(phoneNumber, response)
    else:
        failList.append(upc)

if len(failList) > 0:
    response = ""
    for upc in failList:
        response += upc + ": No Results\n"
    print "---------------------"
    print response
    c.sendMsg(phoneNumber, response)
    
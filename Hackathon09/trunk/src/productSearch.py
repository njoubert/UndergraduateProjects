import re
from urllib2 import urlopen
import gdata.base.service

def parsePriceStr(priceStr):
    match = re.match("^[0-9\.]+", str(priceStr))
    return float(match.group(0))

# returns: name of product, or None if none exist
def getProductName(upc):
    BASE_URL = "http://www.upcdatabase.com/item/"
    REGEX_DESCRIPTION = "(<tr><td>Description</td><td></td><td>)"
    REGEX_DESCRIPTION2 = "(</td>){1}"
    name = None
        
    req = urlopen(BASE_URL+upc)
    html = req.read()
    m = re.search(REGEX_DESCRIPTION, html)
    if m != None:
        startPos = m.start() + len(REGEX_DESCRIPTION) - 2
        html = html[startPos:]
        n = re.search(REGEX_DESCRIPTION2, html)
        if n != None:
            endPos = n.start()
            name = html[:endPos].replace('&quot;','')
    return name

# returns: google product item, or None if none exist
def getGoogleProductData(upc, numResults=30):
    gb_client = gdata.base.service.GBaseService() 
    gb_client.ClientLogin('hackathon09@gmail.com', 'hackathonyozio');
    q = gdata.base.service.BaseQuery() 
    q.feed = '/base/feeds/snippets'
    q['start-index'] = '1'
    q['max-results'] = str(numResults)
    q.bq = str(upc)
    try:
        feed = gb_client.QuerySnippetsFeed(q.ToUri())
    except:
        return None

    itemList = []
    for entry in feed.entry:
        i = 0
        for attribute in entry.item_attributes:
            if attribute.name == "upc" and attribute.text == upc:
                i+=1
            if attribute.name == "price":
                price = attribute.text
                i+=1
        if i == 2:
            itemList.append([entry.title.text, entry.author[0].name.text, price])

    # item := [product name, merchant, price]
    if len(itemList) > 0:
        minPriceItem = itemList[0]
        minPrice = parsePriceStr(minPriceItem[2])
        sum = 0.0
        for item in itemList:
            itemPrice = parsePriceStr(item[2])
            item[2] = itemPrice
            sum += itemPrice
            if itemPrice < minPrice:
                minPriceItem = item
                minPrice = itemPrice
        return [minPriceItem, float(sum)/len(itemList)]
    return None


# Testing
testCases = ["", "2", "410000039144", "013803050844"]
for upc in testCases:
    name = getProductName(upc)
    print "Product name from upc: " + str(name)
    print "Cheapest item: " + str(getGoogleProductData(upc, 30))
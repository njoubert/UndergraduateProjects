import re
from urllib2 import urlopen
import gdata.base.service

def getProductName(upc):
    BASE_URL = "http://www.upcdatabase.com/item/"
    REGEX_DESCRIPTION = "(<tr><td>Description</td><td></td><td>)"
    REGEX_DESCRIPTION2 = "(</td>){1}"
    name = ""
    
    req = urlopen(BASE_URL+upc)
    html = req.read()
    m = re.search(REGEX_DESCRIPTION, html)
    if m != None:
        startPos = m.start() + len(REGEX_DESCRIPTION) - 2
        html = html[startPos:]
    n = re.search(REGEX_DESCRIPTION2, html)
    if n != None:
        endPos = n.start()
        name = html[:endPos]
    return name


def getGoogleProduct(upc, results=30):
    gb_client = gdata.base.service.GBaseService() 
    gb_client.ClientLogin('hackathon09@gmail.com', 'hackathonyozio');
    q = gdata.base.service.BaseQuery() 
    q.feed = '/base/feeds/snippets'
    q['start-index'] = '1'
    q['max-results'] = str(results)
    q.bq = str(upc)
    feed = gb_client.QuerySnippetsFeed(q.ToUri())

    itemList = []
    for entry in feed.entry:
        i = 0
        for attribute in entry.item_attributes:
            if attribute.name == "upc":
                i+=1
            if attribute.name == "price":
                price = attribute.text
                i+=1
        if i == 2:
            itemList.append([entry.title.text, entry.author[0].name.text, price])

    # item := [product name, merchant, price]
    minPriceItem = itemList[0]
    for item in itemList:
        itemPrice = item[2]
        if itemPrice < minPriceItem[2]:
            minPriceItem = item

    return minPriceItem


# Testing
print "-----"
upc = '410000039144'
name = getProductName(upc).replace('&quot;','')
print name
print "\ncheapest item: " + str(getGoogleProduct(upc, 30))

print "----"
upc = "013803050844"
name = getProductName(upc).replace('&quot;','')
print name
print "\ncheapest item: " + str(getGoogleProduct(upc, 30))
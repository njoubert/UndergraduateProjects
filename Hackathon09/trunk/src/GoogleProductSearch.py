import gdata.service
try:
    from xml.etree import ElementTree
except ImportError:
    from elementtree import ElementTree
import atom
import gdata.base
from urllib2 import urlopen

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

# testing
upc = '410000039144'
print "\ncheapest item: " + str(getGoogleProduct(upc, 30))
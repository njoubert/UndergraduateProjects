try:
    from xml.etree import ElementTree
except ImportError:
    from elementtree import ElementTree
import atom
import gdata.base
from urllib2 import urlopen

class GoogleProductSearch():
    feed = None
    itemList = []
    
    def __init__(self, upc, results):
        gb_client = gdata.base.service.GBaseService() 
        gb_client.ClientLogin('hackathon09@gmail.com', 'hackathonyozio');
        q = gdata.base.service.BaseQuery() 
        q.feed = '/base/feeds/snippets'
        q['start-index'] = '1'
        q['max-results'] = str(results)
        q.bq = str(upc)
        self.feed = gb_client.QuerySnippetsFeed(q.ToUri())
    
    def cheapestItem(self, items):
        minPriceItem = items[0]
        for item in items:
            itemPrice = item[2]
            if itemPrice < minPriceItem[2]:
                minPriceItem = item
#            print '\ntitle: ' + str(item[0])
#            print 'merchant: ' + str(item[1])
#            print 'price: ' + str(item[2])
        return "\ncheapest item: " + str(minPriceItem)
        
    def getItem(self):
        for entry in self.feed.entry:
            i = 0
            for attribute in entry.item_attributes:
                if attribute.name == "upc":
                    i+=1
                if attribute.name == "price":
                    price = attribute.text
                    i+=1
            if i == 2:
                self.itemList.append([entry.title.text, entry.author[0].name.text, price])
        return self.cheapestItem(self.itemList)

upc = '410000039144'
googleResult = GoogleProductSearch(upc,30)
print googleResult.getItem()
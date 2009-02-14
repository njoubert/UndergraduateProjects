import gdata.base.service

def getGoogleProduct(upc, results=30):
    if len(upc) == 12 or len(upc) == 13:
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
            for item in itemList:
                itemPrice = item[2]
                if itemPrice < minPriceItem[2]:
                    minPriceItem = item
            return minPriceItem
    return "none"

# testing
upc = ''
print "\ncheapest item: " + str(getGoogleProduct(upc, 30))
upc = '2'
print "\ncheapest item: " + str(getGoogleProduct(upc, 30))
upc = '410000039144'
print "\ncheapest item: " + str(getGoogleProduct(upc, 30))

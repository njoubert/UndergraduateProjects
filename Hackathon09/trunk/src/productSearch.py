import re
import sys
import getopt
from HTMLParser import HTMLParser
from urllib2 import urlopen
import gdata.base.service

def parsePriceStr(priceStr):
    match = re.search("[0-9]+[,]?[0-9\.]*", "".join(str(priceStr).split(",")))
    return float(match.group(0))

def filterResults(listOfNames, name, threshold):
    if len(listOfNames) != 0:
#        print 'in filterResults'
#        print name
        resultList = listOfNames[:]
        base = name.split(" ")
        distances = {}
        for fullname in resultList:
            comp = fullname.split(" ")
            distances[fullname] = editdistance(comp, base)
        x = distances.values()
        x.sort()
        median = x[int(len(x)/2)]
        for fullname in resultList:
            if (distances[fullname] >= median+threshold):
                 listOfNames.remove(fullname)
        
def editdistance(words1, words2):
#    print "words1: " + str(words1)
#    print "words2: " + str(words2)
    if (words1 == words2): 
        return 0
    if (len(words1) == 0):
        return len(words2)
    elif (len(words2) == 0):
        return len(words1)
      
    costL = 1 + editdistance(words1[1:], words2[:])
    costR = 1 + editdistance(words1[:], words2[1:])
    costM = int(words1[0] != words2[0]) + editdistance(words1[1:], words2[1:])
    
    return min(costL, costR, costM)

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
def getGoogleProductData(baseName, upc, numResults=30):
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
        validUPC = False
        hasPrice = False
        for attribute in entry.item_attributes:
            if attribute.name == "upc" and attribute.text == upc:
                validUPC = True
            if attribute.name == "price":
                price = attribute.text
                hasPrice = True
        if validUPC and hasPrice:
            itemList.append([entry.title.text, entry.author[0].name.text, price])
            
    # filter item list
    itemNames = [x[0] for x in itemList]
    filterResults(itemNames, baseName, 1)
    
    # item := [product name, merchant, price]
    if len(itemList) > 0:
        minPriceItem = itemList[0]
        minPrice = parsePriceStr(minPriceItem[2])
        sum = 0.0
        for item in itemList:
            if item[0] in itemNames:
                itemPrice = parsePriceStr(item[2])
                item[2] = itemPrice
                sum += itemPrice
                if itemPrice < minPrice:
                    minPriceItem = item
                    minPrice = itemPrice
        return [minPriceItem, float(sum)/len(itemList)]
    return None


class localDataParser(HTMLParser):
    BASE_URL = "http://www.shoplocal.com/"
    SEARCH_QUERY = "/searchlocal.aspx?searchtext="
    html = ""
    ready = False
    ready2 = False
    titleClassReady = False
    titleReady = False
    dataready = False
    priceready = False
    startTag = ""
    products = []
    stores = []
    prices = []

    def __init__(self):
        HTMLParser.__init__(self)
#        print 'Parser Created'
            
    def handle_starttag(self, tag, attrs):
        self.startTag = self.get_starttag_text()
        if tag == 'div'and 'itemTitleDescription' in self.startTag:
                self.titleClassReady = True
        if tag == 'a' and self.titleClassReady == True:
                self.titleReady = True
        if tag == 'strong' and self.ready == False:
                self.ready = True
                self.ready2 = True
        if tag == 'a' and self.ready == True:
                self.dataready = True
        if tag == 'p' and self.ready2 == True:
                self.priceready = True

    def handle_data(self, data):
        if self.titleReady == True:
            self.products.append(data)
            self.titleReady = False
            self.titleClassReady = False
        if self.dataready == True and '<a ' in self.startTag and 'class' not in self.startTag:
            self.stores.append(data)
            self.dataready = False
            self.ready = False
        if self.priceready == True and 'span' in self.startTag and 'id' in self.startTag:
#            print data
            self.prices.append(str(data))
            self.ready2 = False
            self.priceready = False
  
    def getLocalProductData(self, product_name, location):
        n_product_name = product_name.replace(' ', '%20');
        avg_price = 0.0
        n_location = location.replace(' ', '+')
        try:
            req = urlopen(self.BASE_URL+n_location+self.SEARCH_QUERY+n_product_name)
            self.html = req.read()
            self.feed(self.html)
        except:
            pass
        minPrice = 100000.0
        sumPrice = 0.0
        minIndex = -1
        validNames = self.products[:]
        filterResults(validNames, product_name, 1)
        numProducts = 0
        for i in range(len(self.products)):
            if self.products[i] in validNames:
                nprice = self.prices[i]
                sumPrice += parsePriceStr(nprice)
                if (parsePriceStr(nprice) < parsePriceStr(minPrice)):
                    minIndex = i
                    minPrice = nprice
                    numProducts += 1
        if len(self.prices) != 0:
            avg_price = sumPrice / numProducts
        if minIndex == -1:
            return [avg_price, "", "", 0.0]
        return [avg_price, self.products[minIndex], self.stores[minIndex], parsePriceStr(self.prices[minIndex])]

#file = open("C:/text.txt",'w')
#ar = sys.argv
#file.write("KAKKKKKKKKKKKK")
#file.write("\n")
#file.write(str(len(ar)))
#file.write("\n")
#for i in ar:
#    file.write(str(i))
#    file.write("\n")
#file.close()
#    
# Testing
#testCases = ["", "2", "410000039144", "013803050844"]
#for upc in testCases:
#    name = getProductName(upc)
#    print "Product name from upc: " + str(name)
#    print "Cheapest item: " + str(getGoogleProductData(upc, 30))
    
# Testing2
#p = localDataParser()
#print p.getLocalProductData("Western Digital 250GB External", "San Francisco")
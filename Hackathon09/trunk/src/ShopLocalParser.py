import atom
import gdata.base
import gdata.service
from HTMLParser import HTMLParser
try:
    from xml.etree import ElementTree
except ImportError:
    from elementtree import ElementTree
from urllib2 import urlopen

class LocalResultsGrabber(HTMLParser):
    BASE_URL = "http://www.shoplocal.com/"
    SEARCH_QUERY = "/searchlocal.aspx?searchtext="
    product_name = ""
    location = ""
    html = ""
    ready = False
    ready2 = False
    titleClassReady = False
    titleReady = False
    dataready = False
    priceready = False
    startTag = ""
    results = []
    products = []
    stores = []
    prices = []
    numResults = -1
 
    def __init__(self, product_name, location, numResults = 1):
        HTMLParser.__init__(self)
        print 'Parser Created'
        self.product_name = product_name.replace(' ', '%20')
        self.location = location.replace(' ', '+')
        self.numResults = numResults
        self.processLocalResults()
            
    def setLocation(self, location):
        self.location = location.replace(' ', '+')     
  
    def handle_starttag(self, tag, attrs):
        self.startTag = self.get_starttag_text()
        if tag == 'div':
            if ('itemTitleDescription' in self.startTag):
                self.titleClassReady = True
        if tag == 'a':
            if (self.titleClassReady == True):
                self.titleReady = True
        if tag == 'strong':
            if (self.ready == False):
                self.ready = True
                self.ready2 = True
        if tag == 'a':
            if self.ready == True:
                self.dataready = True
        if tag == 'p':
            if self.ready2 == True:
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
        if self.priceready == True:
            if 'span' in self.startTag and 'id' in self.startTag:
                print data
                self.prices.append(str(data)[1:])
                self.ready2 = False
                self.priceready = False
  
    def processLocalResults(self):
        try:
            req = urlopen(self.BASE_URL+self.location+self.SEARCH_QUERY+self.product_name)
        except:
            print 'BAD URL retry'
            return
        self.html = req.read()
        try:
            self.feed(self.html)
        except:
            print ''
        print str(self.products) + ' length: ' + str(len(self.products))
        print str(self.stores) + ' length: ' + str(len(self.stores))
        print str(self.prices) + ' length: ' + str(len(self.prices))
        minPrice = 100000.0
        minIndex = -1
        for i in range(len(self.prices)):
            nprice = self.prices[i]
            if (float(nprice) < float(minPrice)):
                print 'blah'
                minIndex = i
                minPrice = nprice
        self.results = [self.products[minIndex], self.stores[minIndex], self.prices[minIndex]]
        
#        for i in range(len(self.stores)):
#            self.results[str(self.stores[i])] = str(self.prices[i])
#        newresults = {}
#        minKey = None
#        count = 0
#        self.numResults = min(self.numResults, len(self.results))
#        for i in range(self.numResults):            
#            for key in self.results.keys():
#                nprice = str(self.results.get(key))[1:]
#                print 'nprice: ' + nprice
#                if (float(nprice) < float(minPrice)):
#                    minKey = key
#                    minPrice = nprice
#            priceObj = self.results.get(minKey)
#            newresults[minKey] = str(priceObj)[1:]
#            self.results.pop(minKey)
#            minPrice = 100000.0
#            minKey = None
#        self.results = newresults
       
    def getResults(self):
        return self.results
      
grabber = LocalResultsGrabber("Western Digital 500GB passport", "Plano", 1)
print grabber.getResults()
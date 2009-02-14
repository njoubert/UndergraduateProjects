import re
import cgi
import gdata.base.service
import gdata.service
from HTMLParser import HTMLParser
try:
    from xml.etree import ElementTree
except ImportError:
    from elementtree import ElementTree
import atom
import gdata.base
from urllib2 import urlopen

class UPCGrabber(HTMLParser):
    BASE_URL = "http://www.shoplocal.com/"
    SEARCH_QUERY = "/searchlocal.aspx?searchtext="
    product_name = ""
    location = ""
    html = ""
    description = ""
    ready = False
    dataready = False
    ready2 = False
    priceready = False
    startTag = ""
  
    def __init__(self):
        HTMLParser.__init__(self)
        print 'Parser Created'
      
    def setProduct(self, product_name):
        self.product_name = product_name.replace(' ', '%20')
       
    def setLocation(self, location):
        self.location = location.replace(' ', '+')      
   
    def handle_starttag(self, tag, attrs):
        self.startTag = self.get_starttag_text()
        if tag == 'strong':
            if (self.ready == False):
                self.ready = True
                self.ready2 = True
#                print tag
        if tag == 'a':
            if self.ready == True:
                self.dataready = True
        if tag == 'p':
            if self.ready2 == True:
                self.priceready = True
   
    def processLocalResults(self):
        req = urlopen(self.BASE_URL+self.location+self.SEARCH_QUERY+self.product_name)
        self.html = req.read()
        try:
            self.feed(self.html)
        except:
            print ''
       
    def handle_data(self, data):
        if self.dataready == True:
            print data
            self.dataready = False
            self.ready = False
        if self.priceready == True:
            if 'span' in self.startTag and 'id' in self.startTag:
                print data
                self.ready2 = False
                self.priceready = False
#        print self.html
#        m = re.search(self.REGEX_DESCRIPTION, self.html)
#        if m != None:
#            startPos = m.start() + len(self.REGEX_DESCRIPTION)
#            print 'start: ' + str(startPos)
#            print 'end: ' + m.end()
#            self.html = self.html[startPos:]
#        n = re.search(self.REGEX_DESCRIPTION2, self.html)
#        if n != None:
#            endPos = n.start()
#            print 'end: ' + str(endPos)
#            self.description = self.html[:endPos]

grabber = UPCGrabber()
grabber.setProduct("Western Digital 250GB external")
grabber.setLocation("San Francisco")
print grabber.product_name
print grabber.location
grabber.processLocalResults()
#print grabber.description
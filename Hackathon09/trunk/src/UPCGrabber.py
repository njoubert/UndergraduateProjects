import re
import cgi
import gdata.base.service
import gdata.service
try:
    from xml.etree import ElementTree
except ImportError:
    from elementtree import ElementTree
import atom
import gdata.base
from urllib2 import urlopen

class UPCGrabber():
    BASE_URL = "http://www.upcdatabase.com/item/"
    upc = ""
    html = ""
    description = ""
    REGEX_DESCRIPTION = "(<tr><td>Description</td><td></td><td>)"
    REGEX_DESCRIPTION2 = "(</td>){1}"
   
    def __init__(self):
        print 'Parser Created'
       
    def setUPC(self, upc):
        self.upc = upc
       
    def processUPC(self):
        req = urlopen(self.BASE_URL+self.upc)
        self.html = req.read()
        m = re.search(self.REGEX_DESCRIPTION, self.html)
        if m != None:
            startPos = m.start() + len(self.REGEX_DESCRIPTION) - 2
            print 'start: ' + str(startPos)
            self.html = self.html[startPos:]
        n = re.search(self.REGEX_DESCRIPTION2, self.html)
        if n != None:
            endPos = n.start()
            print 'end: ' + str(endPos)
            self.description = self.html[:endPos]

grabber = UPCGrabber()
grabber.setUPC("013803050844")
grabber.processUPC()
base =  grabber.description.replace('&quot;','')
print base

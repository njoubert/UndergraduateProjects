import re
from HTMLParser import HTMLParser
from urllib2 import urlopen
    
class rateParser(HTMLParser):
    cityReady = False
    cityReadySend = False
    city = None
    
    def __init__(self, zipcode):
        HTMLParser.__init__(self)
        n_zipcode = zipcode.replace(' ', '+')
        cityReady = False
        cityReadySend = False
        req = urlopen("http://www.zipinfo.com/cgi-local/zipsrch.exe?zip=" + str(zipcode) + "&Go=Go")
        html = req.read()
        self.feed(html)

        
    def handle_starttag(self, tag, attrs):
        x = self.get_starttag_text()
        if (tag == 'th'):
            self.cityReady = True
        if (tag == 'td') and self.cityReady == True and 'align=center' in x:
            self.cityReadySend = True
                                
    def handle_data(self, data):
        if self.cityReadySend == True and self.city == None:
            self.city = data
            self.cityReady = False
            self.cityReadySend = False


def getCity(zipcode):
    r = rateParser(zipcode)
    return r.city

#print getCity("94018")

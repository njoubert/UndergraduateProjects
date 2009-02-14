import re
from HTMLParser import HTMLParser
from urllib2 import urlopen
    
class rateParser(HTMLParser):
    ratingReady = False
    rating = None
    
    def __init__(self, product_name):
        HTMLParser.__init__(self)
        n_product_name = product_name.replace(' ', '+')
        ratingReady = False
        req = urlopen("http://www.amazon.com/s/ref=nb_ss_gw?url=search-alias%3Daps&field-keywords="+n_product_name+"&x=0&y=0")
        html = req.read()
        self.feed(html)

        
    def handle_starttag(self, tag, attrs):
        if (tag == 'img'):
            x = self.get_starttag_text()
            if 'stars-' in x and self.ratingReady == False:
#                print x
                self.ratingReady = True
                start = x.index("alt=")
                self.rating = x[start+5:start+8]

def getRating(product_name):
    r = rateParser(product_name)
    return r.rating

#print getRating("Learning Java")

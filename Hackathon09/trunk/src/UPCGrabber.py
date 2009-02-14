import re
from urllib2 import urlopen
            
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


name = getProductName("013803050844").replace('&quot;','')
print name

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
base =  grabber.description
print base.replace('&quot;','')

#START OF GOOGLE

gb_client = gdata.base.service.GBaseService() 
gb_client.ClientLogin('hackathon09@gmail.com', 'hackathonyozio');
q = gdata.base.service.BaseQuery() 
q2 = gdata.base.service.BaseQuery() 

q.feed = '/base/feeds/snippets'
q['start-index'] = '1'
q['max-results'] = '30'

q.bq = '013803050844'

feed = gb_client.QuerySnippetsFeed(q.ToUri())

def editdistance(words1, words2):
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

#dists = []
itemList = []
for entry in feed.entry:
    i = 0
    for attribute in entry.item_attributes:
        if attribute.name == "upc":
            i+=1
        if attribute.name == "price":
            price = attribute.text
            i+=1
#            print 'editdist: ' + str()
#            print 'title: ' + str(entry.title.text)
#            print 'merchant: ' + str(entry.author)
#            print 'price: ' + str(item[3])
#            list1 = list(set(base.lower().split(' ')))
#            list2 = list(set(entry.title.text.lower().split(' ')))
#            editdist = editdistance(list1[:], list2[:])
#            dists.append(editdist)
    if i == 2:
#        itemList.append([editdist, entry.title.text, entry.author, attribute.text])
        itemList.append([entry.title.text, entry.author[0].name.text, price])

#dists.sort()
#median = dists[len(dists)/2]
#print "median: " + str(median) + '\n'
minPriceItem = itemList[0]
for item in itemList:
#    if int(item[0]) > median + 1:
#        distTitlePriceList.remove(item)
#    else:
    itemPrice = item[2]
    if itemPrice < minPriceItem[2]:
        minPriceItem = item
#    print 'editdist: ' + str(item[0])
    print '\ntitle: ' + str(item[0])
    print 'merchant: ' + str(item[1])
    print 'price: ' + str(item[2])

print "\ncheapest item: " + str(minPriceItem)
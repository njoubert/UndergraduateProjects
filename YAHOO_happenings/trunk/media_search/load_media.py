#! /usr/bin/env python

import amara
import httplib
import simplejson
from urllib2 import urlopen, HTTPError
from PHPUnserialize import PHPUnserialize
from pickle import dump, load
from os.path import isfile
from sys import argv, stdout, exit
from time import strptime
from datetime import datetime
from pytz import utc, timezone, country_timezones

safe_mode = True

AGG_HOST = 'prod1.yrb.scd.yahoo.com'
AGG_PORT = ''
AGG_PATH = '8rlv2l5An3TL2Qs/happenings'
AGG_FILE = 'getEventMedia.php'
AGG_QUERY = 'output=php&eventid=%s'
AGG_API = 'http://%s%s/%s/%s?%s' % (AGG_HOST, AGG_PORT, AGG_PATH, AGG_FILE, 
                                    AGG_QUERY)

TZ_API = 'http://ws.geonames.org/timezoneJSON?lat=%s&lng=%s'

CHAPI_HOST = 'cs2.berkeley.corp.yahoo.com'
CHAPI_PORT = 4080
ITEM_PATH = '/item/2.0/-/'

ACT_REF = """<items:reference 
  type="http://chapi.research.yahoo.com/class/2.0/-/Act" 
  href="%s"/>
"""
CRI_REF = """<items:reference 
  type="http://chapi.research.yahoo.com/class/2.0/-/ConcertRelatedItem" 
  href="%s"/>
"""

chapi = httplib.HTTPConnection(CHAPI_HOST, CHAPI_PORT)

def yesno(prompt, default = True):
    if safe_mode:
        return ('y' == raw_input('%s [y/n]? ' % prompt))
    else:
        return default

def translate_datetime(time_string, tzid):
    naive_time = datetime(*strptime(time_string, '%Y-%m-%d %H:%M:%S')[0:6])
    local_time = timezone(tzid.encode('ascii')).localize(naive_time)
    utc_time = local_time.astimezone(utc)
    w3c_time = utc_time.strftime('%Y-%m-%dT%H:%M:%S%z')
    return w3c_time[:-2] + ':' + w3c_time[-2:]

def flickr_urls(photo):
    contextURL = 'http://www.flickr.com/photos/%s/%s' % \
        (photo['owner'], photo['flickrid'])
    mediaURL = 'http://farm%s.static.flickr.com/%s/%s_%s.jpg' % \
        (int(photo['farm']), photo['server'], photo['flickrid'], photo['secret'])
    return contextURL, mediaURL

def PUT(classname, id, props):
    template = file('templates/%s.xml' % classname).read()
    xml = template % props
    headers = {}
    headers['Content-Type'] = 'application/y-items+xml'
    headers['Yahoo-Content-Classes'] = \
        '"http://chapi.research.yahoo.com/class/2.0/-/%s"' % classname
    headers['Content-Language'] = 'en'
    chapi.request('PUT', ITEM_PATH + id, xml, headers)
    return chapi.getresponse()

def die(response):
    exit('%s error: %s' % (response.status, response.read()))

# Main --------------------------------------------------------------------------

if len(argv) > 1:
    event_id = argv[1]
else:
    prompt = 'Upcoming ID for concert: '
    event_id = raw_input(prompt)
    while len(event_id) == 0:
        print 'Please enter a valid Upcoming event ID.'
        event_id = raw_input(prompt)

# Load event data -----------------------

dumpfile = 'concert-%s.pkl' % event_id
if isfile(dumpfile) and yesno('Found saved event data; use that'):
    data = load(file(dumpfile))
else:
    print 'Searching for media items (may take a while)...',
    stdout.flush()
    data = PHPUnserialize().unserialize(urlopen(AGG_API % event_id).read())
    print 'done.'
    dump(data, file(dumpfile, 'w'))

event = data['event']
photos = {}
videos = {}
if 'photos' in data['media']['flickr']:
    photos = data['media']['flickr']['photos']['photo']
if 'videos' in data['media']['youtube']:
    videos = data['media']['youtube']['videos']['video']
print '%s, %s, %s' % (event['name'], event['venue_name'], event['start_date'])
print '%s photos, %s videos' % (len(photos), len(videos))
if not yesno('Does this look correct'):
    exit()

# Check if we are updating or not -------

headers = {}
headers['Yahoo-Content-Workflow'] = 'ignored'
headers['Accept'] = 'application/y-items+xml'
chapi.request('GET', ITEM_PATH + ('concert/%s' % event_id), None, headers)
response = chapi.getresponse()

update_concert = False

if response.status == 200:
    if not yesno('Concert already exists, overwrite existing data'):
        exit()
    update_concert = True
elif response.status == 404:
    update_concert = False
else:
    die(response)

# Determine timezone --------------------

tzid=''
print 'Looking up timezone (may take a while)...',
stdout.flush()
if len(event['latitude']) > 0:
    tz = simplejson.loads(urlopen(TZ_API % (event['latitude'], 
                                            event['longitude'])).read())
    tzid = tz['timezoneId']
    print tzid
else:
    candidates = country_timezones(event['venue_country_code'])
    if len(candidates) == 1:
        tzid = candidates[0]
        print tzid
    else:
        print 'multiple possibilities:'
        index = 1
        for candidate in candidates:
            print '%s: %s' % (index, candidate)
            index +=1
        tzid = candidates[int(raw_input('Select correct timezone by number: '))]

# Determine start time ------------------

if len(event['start_time']) == 0:
    print 'Missing start time.'
    event['start_time'] = raw_input('Start time (24-hour HH:MM:SS): ')

start = translate_datetime(event['start_date'] + ' ' + event['start_time'], tzid)
print 'Concert start time: %s' % start 

# Add acts ------------------------------

acts = ''

if yesno('Add acts', False):
    act = 1
    while True:
        name = raw_input('Act #%s (leave empty to stop): ' % act)
        if len(name) == 0:
            break
        response = PUT('Act',
                       'concert/%s/act/%s' % (event_id, act),
                       (name))
        if not (response.status == 201 or response.status == 204):
            die(response)
        item_uri = response.getheader('yahoo-content-item')
        acts += (ACT_REF % item_uri)
        act += 1

# Add media items -----------------------
                       
relatedItems = ''

update_photos = True
if update_concert:
    update_photos = yesno('Update photo information', False)

for photo in photos.values():
    if update_photos:
        contextURL, mediaURL = flickr_urls(photo)
        response = PUT('FlickrImage', 
                       'photo/flickr/%s' % photo['flickrid'],
                       (photo['flickrid'],
                        photo['title'],
                        photo['owner'], 
                        photo['ownername'], 
                        translate_datetime(photo['datetaken'], tzid),
                        int(photo['farm']),
                        photo['secret'],
                        photo['server'],
                        contextURL,
                        mediaURL))
        if not (response.status == 201 or response.status == 204):
            die(response)
        item_uri = response.getheader('yahoo-content-item')
    else:
        item_uri = 'http://chapi.research.yahoo.com/item/2.0/-/photo/flickr/%s'\
            % photo['flickrid']
    relatedItems += (CRI_REF % item_uri)

if update_photos:
    print 'Updated %s photos.' % len(photos)

update_videos = True
if update_concert:
    update_videos = yesno('Update video information', False)

for video in videos.values():
    if update_videos:
        mediaURL = 'http://cache.googlevideo.com/get_video?video_id=%s'\
            % video['id']
        response = PUT('YouTubeVideo', 
                       'video/youtube/%s' % video['id'],
                       (video['id'],
                        video['title'],
                        video['description'],
                        video['author'],
                        int(video['length_seconds']) * 1000,
                        video['url'],
                        mediaURL,
                        video['thumbnail_url']))
        if not (response.status == 201 or response.status == 204):
            die(response)
        item_uri = response.getheader('yahoo-content-item')
    else:
        item_uri = 'http://chapi.research.yahoo.com/item/2.0/-/video/youtube/%s'\
            % video['id']
    relatedItems += (CRI_REF % item_uri)

if update_videos:
    print 'Updated %s videos.' % len(videos)

# Update concert ------------------------

response = PUT('Concert', 
               'concert/%s' % event_id, 
               (event['name'], 
                event['description'], 
                event['venue_name'], 
                translate_datetime(event['start_date'] + ' ' + 
                                   event['start_time'], 
                                   tzid),
                acts,
                relatedItems))

if (not update_concert) and response.status == 201:
    print 'Created new concert.'
elif update_concert and response.status == 204:
    print 'Updated existing concert.'
else:
    die(response)


print 
    






    







    



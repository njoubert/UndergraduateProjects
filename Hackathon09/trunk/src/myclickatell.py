#!/usr/bin/env python
# encoding: utf-8

"""
myclickatell.py

Created by Niels Joubert on 2009-02-14.
Copyright (c) 2009 __MyCompanyName__. All rights reserved.
"""

import urllib
import urllib2

class Clickatell:
    def __init__(self, username, password, app_id):
        self.username = username
        self.password = password
        self.app_id = app_id
        self.send_uri = "http://api.clickatell.com/http/sendmsg"
    
    
    def sendMsg(self, message):
        """
        Message is:
        msg = {
            'to' : '16508231662',
            'msg': 'Hi! You're awesome!'
            
        }
        """
        values = {'api_id' : self.app_id,
            'user' : self.username,
            'password' :  self.password,
            'to' : msg['to'],
            'text' : msg['msg']
        }
        
        data = urllib.urlencode(values)
        req = urllib2.Request(self.send_uri, data)
        response  = urllib2.urlopen(req)
        
        
        

def main():
    pass

if __name__ == '__main__':
    main()
    c = Clickatell("njoubert", "rsd887", "3076162")
    msg = {'to':'14056410773', 'msg':'BUY SHIT! BUY IT NOW!'}
    c.sendMsg(msg)
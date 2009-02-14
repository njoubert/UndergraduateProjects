#!/usr/bin/env python
# encoding: utf-8

"""
myclickatell.py

Created by Niels Joubert on 2009-02-14.
Copyright (c) 2009 __MyCompanyName__. All rights reserved.
"""

import urllib
import urllib2
import re

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
        
        Returns:
            on success:
                (True, 'ID')
            on fail:
                (False, 'error-code', 'reason')
        """
        values = {'api_id' : self.app_id,
            'user' : self.username,
            'password' :  self.password,
            'to' : message['to'],
            'text' : message['msg']
        }
        
        data = urllib.urlencode(values)
        req = urllib2.Request(self.send_uri, data)
        response  = urllib2.urlopen(req)
        the_page = response.read()
        
        m = re.search('(ID: (.*))|(ERR: (\d*), (.*))', the_page)
        if m == None:
            return (False, '0', 'REGEX DID NOT MATCH')
        else:
            if m.group(1) != None:
                return (True, m.group(2))
            else:
                return (False, m.group(4), m.group(5))
        return None
        

def main():
    pass

if __name__ == '__main__':
    main()
    c = Clickatell("njoubert", "rsd887", "3076162")
    msg = {'to':'16508231662', 'msg':'BUY SHIT! BUY IT NOW!'}
    print c.sendMsg(msg)
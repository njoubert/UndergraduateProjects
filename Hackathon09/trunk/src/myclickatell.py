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
import math

class Clickatell:

    def __init__(self, username, password, app_id):
        self.username = username
        self.password = password
        self.app_id = app_id
        self.send_uri = "http://api.clickatell.com/http/sendmsg"
        self.singleMsgLen = 160
    
    def sendMsg(self, cellnr, msg):
        """
            Returns a list of (True/False, ID, [error message]) tuples.
            The length of this list tells you how many messages you sent.
        """
    
        metaData = {'api_id' : self.app_id,
            'user' : self.username,
            'password' :  self.password,
            'to' : cellnr,
        }
        
        msgLen = len(msg)
        numMsgs = int(math.ceil(float(msgLen)/float(self.singleMsgLen)))
            
        result = []
        for i in range(numMsgs):
            text = msg[(i*self.singleMsgLen) : (i+1)*self.singleMsgLen]
            result.append(self.__sendSingleMsg(metaData, text))
        
        return result
        
    def getMaxMessageLen():
        return self.singleMsgLen
        
    def __sendSingleMsg(self, metaData, text):
        """        
        Returns:
            on success:
                (True, 'ID')
            on fail:
                (False, 'error-code', 'reason')
        """
        values = dict(metaData)
        values['text'] = text
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
        


"""
    EXAMPLE USAGE:
    
    c = Clickatell("njoubert", "rsd887", "3076162")
    cellnr = '16508231662'
    msg = 'Lorem ipsum dolor sit amet, consectetur adipiscing elit. Aliquam fermentum orci sit amet mi. Donec eget massa. Aenean aliquet, magna nullam. A A A A A A A A A A A'
    print len(msg)
    print c.sendMsg(cellnr, msg)
    
"""
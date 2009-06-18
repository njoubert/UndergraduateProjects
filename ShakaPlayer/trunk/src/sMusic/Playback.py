'''
Created on Jun 12, 2009

@author: njoubert
'''

import logging
import sCodec
import pyglet
import os

MEDIAHANDLERS = [DiskAudio]

def loadMediaFromDisk(path):
    root, ext = os.path.splitext(path)
    for h in MEDIAHANDLERS:
        if python

class Audio:
    ''' Represents a single audio file '''
    
    def findPlayer(self):
        pass
    
    def play(self):
        pass

class DiskAudio(Audio):
    
    def __init__(self, filepath):
        self.path = filepath
        self.source = pyglet.media.load(self.path)
            
    def play(self):
        player = pyglet.media.Player()
        player.queue(self.source)
        player.play()

        
        
        
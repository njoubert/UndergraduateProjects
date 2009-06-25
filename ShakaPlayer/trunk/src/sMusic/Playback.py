'''
Created on Jun 12, 2009

@author: njoubert
'''

import logging
import sCodec
import pyglet
import os

class Audio:
    ''' Represents a single audio file '''
    
    def canHandle(self, path):
        return False
    canHandle = classmethod(canHandle)
    
    def play(self):
        pass
    
class PygletAudio(Audio):
    
    def canHandle(self, path):
        try:
            pyglet.media.load(path)
            return True
        except:
            return False
    canHandle = classmethod(canHandle)
    
    def __init__(self, filepath):
        self.path = filepath
        self.source = pyglet.media.load(self.path)
            
    def play(self):
        player = pyglet.media.Player()
        player.queue(self.source)
        player.play()

        
        
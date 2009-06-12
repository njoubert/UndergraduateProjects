'''
Created on Jun 12, 2009

@author: njoubert
'''

import logging
import sCodec
import pyglet


class Audio:
    ''' Represents a single audio file '''
    
    def findPlayer(self):
        pass
    
    def play(self):
        pass

class DiskAudio(Audio):
    
    def loadPath(self, path):
        self.path = path
        self.source = pyglet.media.load(self.path)
        
    def play(self):
        player = pyglet.media.Player()
        player.queue(self.source)
        player.play()

class Player:
    '''
    classdocs
    '''

    def __init__(self):
        '''
        Constructor
        '''
        
'''
Created on Jun 12, 2009

@author: njoubert
'''
import os
import sys
import logging
import sMusic
import sMusic.Playback
import pyglet

def init():
    logging.basicConfig(level=logging.DEBUG)
    logging.debug("ShakaPlayer launched.")
    
def driverloop():
    
    
    
    sMusic.foo()
    clip1 = sMusic.Playback.DiskAudio()
    clip1.loadPath("../examples/08 - The Life.flac")
    clip1.play()
    clip2 = sMusic.Playback.DiskAudio()
    clip2.loadPath("../examples/08 - The Life.flac")
    clip2.play()
    pyglet.app.run()

if __name__ == '__main__':
    init()
    driverloop()

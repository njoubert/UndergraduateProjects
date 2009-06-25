'''
Created on Jun 12, 2009

@author: njoubert
'''
import os
import sys
import logging
import sMusic
import pyglet

def init():
    logging.basicConfig(level=logging.DEBUG)
    logging.debug("ShakaPlayer launched.")
    
def driverloop():
    clip1 = sMusic.loadMediaFromDisk("../examples/08 - The Life.flac")
    
    clip1.play()
    try:
        clip2 = sMusic.loadMediaFromDisk("../examples/mac prices.txt")
    except:
        print "No handler for a text file!"
    pyglet.app.run()

if __name__ == '__main__':
    init()
    driverloop()

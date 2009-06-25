import logging
import sCodec
import threading
import thread
import inspect
import Playback

def foo():
    logging.debug("sMusic foo")
    sCodec.foo()

''' Here we build the list of valid  objects: '''

def __isValidPlaybackClass(obj):
    if inspect.isclass(obj) and obj != Playback.Audio and \
     Playback.Audio in inspect.getmro(obj):
        return True
    return False

MEDIATYPES = []
for m in inspect.getmembers(Playback, __isValidPlaybackClass):
    MEDIATYPES.append(m[1])

def loadMediaFromDisk(path):
    logging.debug("Loading media from disk, from handlers " + str(MEDIATYPES))
    for h in MEDIATYPES:
        if h.canHandle(path):
            return h(path)
    raise NotImplementedError("Could not find supported type for given path!")
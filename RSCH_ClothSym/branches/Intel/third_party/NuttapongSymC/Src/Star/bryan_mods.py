#!/usr/bin/env python
# encoding: utf-8
"""
bryan_mods.py

Automatically apply small changes to the Star source so calling programs work.
"""

import sys
import os


def main():
    # add the defines around main.c so it won't be there
    infile = open('starmain.c', 'r')
    origFile = infile.read()
    infile.close()
    
    # check to see if we've already made the change
    if origFile.find('#ifndef NOMAIN') == 0:
        print "not changing main.c; it's already been fixed..."
    else:
        print "index it was found at is", origFile.find('#ifndef NOMAIN')
        outfile = open('starmain.c', 'w')
        outfile.write('#ifndef NOMAIN\n')
        outfile.write(origFile)
        outfile.write('#endif /* NOMAIN */\n')
        outfile.close()
    
    # since makeheaders seems to choke on the starreal else statement,
    # need to fix top.c to make sure starreal gets defined
    infile = open('startop.c', 'r')
    origFile = infile.readlines()
    infile.close()
    
    # check to see if we've already made the change
    if 1 == 1:
    #if ("typedef float starreal;\n" in origFile) == False:
        print "not changing top.c; it's already been fixed..."
    else:    
        # delete the troublesome if/then/else defines
        origFile.remove('#ifdef SINGLE\n')
        origFile.remove('typedef float starreal;\n')
        origFile.remove('#else /* not SINGLE */\n')
        origFile.remove('#endif /* not SINGLE */\n')
        
        outfile = open('top.c', 'w')
        outfile.write(''.join(origFile))
        outfile.close()

if __name__ == '__main__':
    main()


#!/usr/bin/env python
# encoding: utf-8
"""
makeGrid2.py

Created by Niels Joubert on 2008-09-23.
Copyright (c) 2008 __MyCompanyName__. All rights reserved.
"""

import sys
import getopt


help_message = '''
The help message goes here.
'''


class Usage(Exception):
	def __init__(self, msg):
		self.msg = msg

def frange(start, end=None, inc=None):
    if end == None:
        end = start + 0.0
        start = 0.0
    else: start += 0.0 # force it to be a float

    if inc == None:
        inc = 1.0

    count = int((end - start) / inc)
    if start + count * inc != end:
    # need to adjust the count.
    # AFAIKT, it always comes up one short.
        count += 1

    L = [None,] * count
    for i in xrange(count):
        L[i] = start + i * inc
    return L


def main(argv=None):
	if argv is None:
		argv = sys.argv
	try:
		try:
			opts, args = getopt.getopt(argv[1:], "w:h:s:", [])
		except getopt.error, msg:
			raise Usage(msg)
	
		width = 1.0
		height = 1.0
		step = 0.2
	
		# option processing
		for option, value in opts:
			if option == "-w":
				width = float(value)
			if option == "-h":
				height = float(value)
			if option == "-s":
				step = float(value)
			
	except Usage, err:
		print >> sys.stderr, sys.argv[0].split("/")[-1] + ": " + str(err.msg)
		print >> sys.stderr, "\t for help use --help"
		return 2
	
	#print "Making grid of width ", width, "height ", height, " with step size", step
	#for j in range(-height, height+1, step):
	#	for i in range(-width, width+1, step):
	#		print "v ", i, j, 0
	widthCount = int(2*(width +1) / step)
	heightCount = int(2*(height +1) / step)
	
	#an array, first indice is width, second is height
	vertices = [(0,0,0)] * (widthCount * heightCount)
	
	def getIndice(x, y):
		print x, y, widthCount, x+y*widthCount
		int(x + y*widthCount)
		
	widthI = 0
	heightI = 0
	for x in frange(-width, width, step):
		for y in frange(-height, height, step):
			vertices[getIndice(widthI,heightI)] = (x,y,0) 
	
	print "# Outputting", len(vertices), "VERTICES"
	for vertex in vertices:
		print "v ", vertex[0], vertex[1], vertex[2]
		
	print "#FACES"				

if __name__ == "__main__":
	sys.exit(main())

#!/usr/bin/env python
# encoding: utf-8
"""
untitled.py

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


def main(argv=None):
	if argv is None:
		argv = sys.argv
	try:
		try:
			opts, args = getopt.getopt(argv[1:], "w:h:s:", [])
		except getopt.error, msg:
			raise Usage(msg)
	
		# option processing
		for option, value in opts:
			if option == "-w":
				width = int(value)
			if option == "-h":
				height = int(value)
			if option == "-s":
				step = int(value)
			
	except Usage, err:
		print >> sys.stderr, sys.argv[0].split("/")[-1] + ": " + str(err.msg)
		print >> sys.stderr, "\t for help use --help"
		return 2
	
	#print "Making grid of width ", width, "height ", height, " with step size", step
	#for j in range(-height, height+1, step):
	#	for i in range(-width, width+1, step):
	#		print "v ", i, j, 0

	x = 1
	print "v ", -width,   height,   0
	print "v ", -width+1, height,   0
	for i in range(-width+step, width+1, step*2):
		for j in range(height-step, -height-1, -step*2):
			print "v ", i,   j+1,  0
			print "v ", i+1, j+1, 0
			print "f ", x+2, x+1, x
			print "f ", x+1, x+2, x+3 
			x = x + 4
			

if __name__ == "__main__":
	sys.exit(main())

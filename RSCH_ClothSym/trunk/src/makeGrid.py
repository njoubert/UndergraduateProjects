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

	pointsList = [] 	#List of points (x,y,z)
	pointsQueue = []  	#List of 		(indice, (x,y,z))
	faceList = [] 		#list of face 	(indice1, indice2, indice2)
	
	print "# Creating grid with h", height, " and w", width
	
	pointsList.append((-width,height,0))
	pointsQueue.append((0,(-width,height,0)))
	while not len(pointsQueue) == 0:
		currentPoint = pointsQueue.pop()
		pointDown = (currentPoint[1][0], currentPoint[1][1]-step,0)
		pointRight = (currentPoint[1][0]+step,currentPoint[1][1],0)
		pointCross = (currentPoint[1][0]+step,currentPoint[1][1]-step,0)
		if (pointDown[1] < -height or pointRight[0] > width):
			continue
		if (pointsList.count(pointDown) == 0):
			pointsList.append(pointDown)
			pointDown2 = (len(pointsList)-1, pointDown)
		else:
			pointDown2 = (pointsList.index(pointDown), pointDown)
			
		if (pointsList.count(pointRight) == 0):
			pointsList.append(pointRight)
			pointRight2 = (len(pointsList)-1, pointRight)
		else:
			pointRight2 = (pointsList.index(pointRight), pointRight)

		if (pointsList.count(pointCross) == 0):
			pointsList.append(pointCross)
			pointCross2 = (len(pointsList)-1, pointCross)
		else:
			pointCross2 = (pointsList.index(pointCross), pointCross)

		if (faceList.count((pointCross2[0], pointRight2[0], pointDown2[0])) == 0):
			faceList.append((pointCross2[0], pointRight2[0], pointDown2[0]))
		if (faceList.count((pointDown2[0], pointRight2[0], currentPoint[0])) == 0):
			faceList.append((pointDown2[0], pointRight2[0], currentPoint[0]))	
		pointsQueue.append(pointDown2)
		pointsQueue.append(pointRight2)
		pointsQueue.append(pointCross2)	
	
	print "#VERTICES"
	for vertex in pointsList:
		print "v ", vertex[0], vertex[1], vertex[2]
		
	print "#FACES"	
	for face in faceList:
		print "f ", face[0]+1, face[1]+1, face[2]+1
		
		
			

if __name__ == "__main__":
	sys.exit(main())

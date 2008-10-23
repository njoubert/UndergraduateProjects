#!/usr/bin/env python
# encoding: utf-8
"""
makeGrid2.py

Created by Niels Joubert on 2008-10-06.
Copyright (c) 2008 UC Berkeley. All rights reserved.
"""
import sys
import os
import math

def frange(limit1, limit2 = None, increment = 1.):
  """
  Range function that accepts floats (and integers).
  The returned value is an iterator.  Use list(frange) for a list.
  """

  if limit2 is None:
    limit2, limit1 = limit1, 0.
  else:
    limit1 = float(limit1)

  count = int(math.ceil(limit2 - limit1)/increment) + 1
  return (limit1 + n*increment for n in range(count))

class Box():
	p0 = 0
	p1 = 0
	p2 = 0
	p3 = 0
	c = -1
	
	def display(self):
		print self.p0, self.p1, self.p2, self.p3, self.c
		
	def display(self, vertices):
		if c == -1:
			print vertices[self.p0], vertices[self.p1], vertices[self.p2], vertices[self.p3]
		else:
			print vertices[self.p0], vertices[self.p1], vertices[self.p2], vertices[self.p3], vertices[self.c]


def main():
	print " # "
	print " # SYMMETRIC MESH"
	print " # "
	
	UL = (-1.0, -1.0)
	LR = (1.0, 1.0)
	step = 1.1
	
	print " # Creating mesh from UL", UL, "to LR", LR, "with major points at each", step
	xsteps = int(abs((LR[0] - UL[0]) / step)) #steps along x axis, along width
	ysteps = int(abs((LR[1] - UL[1]) / step)) #steps along y axis, along height
	
	print " # Mesh is", xsteps, "major points wide and", ysteps, "major points high"
	verticeList = [] #Vertice is of the form (x,y,z,index)
	boxList = [] 
	faceList = [] #Face is of form (v1, v2, v3)
	
	def getIndice(x, y):
		return int(x) + int(y*(xsteps+1))
	
	count = 0
	for y in frange(UL[1], LR[1], step):
		for x in frange(UL[0], LR[0], step):
			verticeList.append((x, y, 0, count))
			count += 1
			
	assert(count == len(verticeList))		
	
	print " # Created", len(verticeList), "major points."
	
	count = -1
	for y in range(0, ysteps):
		for x in range(0, xsteps):
			count += 1
			#print x, y, getIndice(x, y)
			if x == xsteps or y == ysteps:
				continue
			box = Box()
			box.p0 = getIndice(x, y)
			box.p1 = getIndice(x+1, y)
			box.p2 = getIndice(x, y+1)
			box.p3 = getIndice(x+1, y+1)
			boxList.append(box)
	
	for box in boxList:
		#calculate center point
		cX = ((verticeList[box.p1][0] + verticeList[box.p0][0])/2.0 + (verticeList[box.p2][0] + verticeList[box.p3][0])/2.0)/2.0
		cY = ((verticeList[box.p1][1] + verticeList[box.p0][1])/2.0 + (verticeList[box.p2][1] + verticeList[box.p3][1])/2.0)/2.0
		cZ = ((verticeList[box.p1][2] + verticeList[box.p0][2])/2.0 + (verticeList[box.p2][2] + verticeList[box.p3][2])/2.0)/2.0
		verticeList.append((cX, cY, cZ, len(verticeList)))
		#box.display(verticeList)
		#print verticeList[len(verticeList)-1]
		box.c = len(verticeList)-1
		faceList.append((box.c, box.p0, box.p1))
		faceList.append((box.c, box.p1, box.p3))
		faceList.append((box.c, box.p3, box.p2))
		faceList.append((box.c, box.p2, box.p0))
	
	print " # ", len(verticeList), "total vertices"
	print " # ", len(faceList), "total faces"	
		
	print ""	
	print " # VERTICES FOLLOW:"
	print ""
	for v in verticeList:
		print "v  ", v[0], v[1], v[2]
		
	print " #FACES FOLLOW:"
	print ""
	for f in faceList:
		print "f  ", f[0]+1, f[1]+1, f[2]+1
		 
if __name__ == '__main__':
	main()


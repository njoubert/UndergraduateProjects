#!/usr/bin/env python
# encoding: utf-8
"""
makeheaders.py

Modifies source files in Star's source to work with makeheaders 
(cordons off typedefs and defines from function definitions with #if EXPORT_INTERFACE)
"""

import sys
import getopt
import re
import commands


def main(argv=None):
    # define the list of files making up Star.c, and note the ones we want to grab things from
    files = [('startop', True), \
             ('interact', True), \
             ('misc', False), \
             ('arraypool', True), \
             ('proxipool', True), \
             ('zorder', False), \
             ('allocmap', True), \
             ('linkring', True), \
             ('link2d', True), \
             ('primitives', False), \
             ('tetcomplex', True), \
             ('input', True), \
             ('output', False), \
             ('bowyerwatson', False), \
             ('locate', True), \
             ('incremental', True), \
             ('constrain', False), \
             ('statistics', True), \
             ('starmain', False), \
             ]
    
    # insert special definitions
    catFiles = []
    for thisFile, useIt in files:
        if useIt:
            catFiles.append('%s_mod.c' % thisFile)
            insertDefines(thisFile)
        else:
            catFiles.append('%s.c' % thisFile)
    
    # concatenate files into Star_mod.c
    catCmd = "cat %s > Star_mod.c" % ' '.join(catFiles)
    status, output = commands.getstatusoutput(catCmd)
    
    # generate header file
    status, output = commands.getstatusoutput('./mkh.exe Star_mod.c:Star.h')
    if status != 0:
        print "makeheaders failed, output is", output
    
    # remove modified files
    status, output = commands.getstatusoutput('rm *_mod.c')
    
    # fix up broken if/then/else handling of starreal definition
    infile = open('Star.h', 'r')
    hfile = infile.readlines()
    infile.close()
    
    badStart = hfile.index('#if defined(SINGLE)\n')
    print "badStart is", badStart
    hfile.insert(badStart, 'typedef double starreal;\n')
    
    outfile = open('Star.h', 'w')
    outfile.write(''.join(hfile))
    outfile.close()

# insert defines that wrap around global typedefs, etc
def insertDefines(filename):
    infile = open(filename + '.c', 'r')
    outfile = open(filename + "_mod.c", 'w')
    
    #the string to start and stop the .h stuff
    startString = '#if INTERFACE\n'
    stopString = '#endif /* INTERFACE */\n'
    
    # a regular expression that tries to find the start of a function
    # definition, and hence the end of stuff that needs to be exported
    # also try to match global variable declarations
    reString = '([^(/\*)#]*[\w]*\()|(/\*\s*Global)'
    regexp = re.compile(reString)
    
    #read the input file
    orig = infile.readlines()
    infile.close()
    
    
    # look for the line where the function definitions start
    lineNum = 1
    found = False
    for thisLine in orig:
        if regexp.match(thisLine) != None:
            #print "I think I found a function definition in %s.c on line %d:" % (filename, lineNum)
            #print thisLine
            
            # append the end of the definition right before declaration
            orig[lineNum-2] = orig[lineNum-2] + '\n' + stopString + '\n'
            
            found = True
            break
        lineNum += 1
    if found == False:
        print "Didn't find any function declarations in", fileName
        
    # make a second pass looking for any later functions defined as
    # macros. I'm trying to identify these as any lines that start
    # with #define until the next line after that that doesn't end with
    # a continuation character (\)
    defines = []
    reDefine = '#define[\s]*[\w]*\('
    regexp = re.compile(reDefine)
    for thisLineNum in range(lineNum, len(orig)):
        # if the line starts with #define, we're on to something
        if regexp.match(orig[thisLineNum]) != None:
            #print "found a #define macro in %s.c on line %d:" % (filename, thisLineNum)
            #print orig[thisLineNum]
            
            # now find where the #define ends, which is the next line
            # that doesn't end with a continuation character
            found = False
            offset = 0
            while found == False:
                # check the last char for continuation
                if orig[thisLineNum+offset][-2] != '\\':
                    found = True
                    #print "found an end to the #define on line %d:" % (thisLineNum + offset)
                    #print orig[thisLineNum + offset]
                offset += 1
            
            # save a tuple of the starting and ending lines of this define
            defines.append((thisLineNum, thisLineNum+offset))
    
    # now that we've found all the macro functions, need to stick in the annotation
    # so they get pulled into the .h file.
    offset = 0
    for defStart, defStop in defines:
        orig.insert(defStart + offset, startString)
        offset += 1
        orig.insert(defStop + offset, stopString)
        offset += 1

    #write modified output file
    outlines = ''.join(orig)
    outfile.write(startString)
    outfile.write(outlines)
    outfile.close()

if __name__ == "__main__":
    sys.exit(main())

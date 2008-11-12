import random #getrandbits
import sys #argv
import math #ceil

def getrandnumstring(ndigits):
    result = ""

    # A number modded by 10000000000000000000L could return as many as 9 digits,
    # but on average it is safer to assume 18.
    nnumbers = long(math.ceil(float(ndigits) / 18.0))

    # generate numbers and store in string. getrandbits returns a long.
    for a in range(nnumbers):
        result = result + str(random.getrandbits(64) % 10000000000000000000L)

    #truncate extra numbers
    return result[:long(ndigits)]

if (len(sys.argv) < 2) :
    print "please enter more arguments"
    exit

sys.stdout.write(getrandnumstring(sys.argv[1]))

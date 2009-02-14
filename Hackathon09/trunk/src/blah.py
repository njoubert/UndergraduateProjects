import sys


file = open("C:/text.txt",'w')
file.write(sys.argv[1])
file.close()
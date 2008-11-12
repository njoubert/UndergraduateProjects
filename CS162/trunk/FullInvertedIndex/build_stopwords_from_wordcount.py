#!/usr/bin/python

from optparse import OptionParser
import os, re, sys, string, heapq

class PriorityQueue:
    """
    Implements a priority queue data structure.
    Sorts items by highest priority.
    """
  
    def  __init__(self):
        self.heap = []
    
    def push(self, item, priority):
        priority = -1*priority
        pair = (priority,item)
        heapq.heappush(self.heap,pair)

    def pop(self):
        (priority,item) = heapq.heappop(self.heap)
        priority = -1*priority
        return priority, item
  
    def isEmpty(self):
        return len(self.heap) == 0

def main():
    # Parse command line options
    opt_parser = OptionParser()
    opt_parser.add_option("--directory", "-d", dest="dir", default="",
               help="directory containing files from which you build stop words")
    (options, args) = opt_parser.parse_args()

    # Check args
    if (options.dir is ""):
        print "ERROR: You must specify a directory containing word count files."
        return 1
    elif (not os.path.exists(options.dir)):
        print "ERROR: You must specify a valid directory."
        return 1

    # Get the list of all word count files to parse
    count_files = []
    os.system("ls %s > files.txt" % options.dir)
    files = open("files.txt", 'r')
    file = files.readline()
    while (file != ""):
        count_files.append(os.path.join(options.dir, file.strip()))
        file = files.readline()
    files.close()
    os.system("rm files.txt")
    if len(count_files) == 0:
        print "ERROR: Directory must contain at least one file."
        return 1
    
    # Run through each input file to tally the count for each word
    all_words = {}
    for count_file in count_files:
        count_handle = open(count_file, 'r')
        count_entry = count_handle.readline()
        while (count_entry != ""):
            count_entry_split = count_entry.strip().split("\t")
            word, count = count_entry_split[0], int(count_entry_split[1])
            if word in all_words:
                all_words[word] += count
            else:
                all_words[word] = count
            count_entry = count_handle.readline()
        count_handle.close()
    
    # Put all the words into a priority queue
    pqueue = PriorityQueue()
    all_counts = []
    for word in all_words.keys():
        count = all_words[word]
        pqueue.push(word, count)
        all_counts.append(count)
    
    # Define how many stop words to output
    avg = sum(all_counts)/len(all_counts)
    sdsq = sum([(i-avg)**2 for i in all_counts])
    sd = (sdsq/(len(all_counts)-1 or 1))**.5
    upperLim = avg + 2*sd
    
    # Output the stop words
    out_file = "stopwords.txt"
    out_handle = open(out_file, 'w')
    while (True):
        count, word = pqueue.pop()
        if count > upperLim:
            out_handle.write(word+"\n")
        else:
            break
    out_handle.close()
    print "Complete.  Please check %s for a list of stop words." % out_file

if __name__ == "__main__":
    main()

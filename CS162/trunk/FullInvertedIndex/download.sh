#!/bin/bash

if [ $# -ne 0 ]
then
	echo "Usage - ./download.sh"
	exit 1
fi

echo "Setting environment variables..."
export HDFS=/user/$USER
export TMPSTORE=/tmp/$USER-storage
export HADOOP=hadoop-0.16.3
export PATH=/hadoop/$HADOOP/bin:$PATH

#Lame, we're just checking for the first file, but oh well!
hadoop dfs -stat input/0ws0110.txt &> /dev/null

if [ $? -eq 0 ]
then
	echo "Corpus file input already found in Hadoop DFS."
else
	echo "Could not locate Shakespeare input corpus in DFS"
	if [ -f corpus/0ws0110.txt ]
	then
		echo "Located corpus source files."
	else
		echo "Downloading corpus..."
		wget http://inst.eecs.berkeley.edu/~cs162/sp08/Nachos/Hadoop/Shakespeare.tar.gz
		rm -rf corpus &> /dev/null
		mkdir corpus &> /dev/null
		tar xf Shakespeare.tar.gz -C corpus/ &> /dev/null
		rm -f Shakespeare.tar.gz &> /dev/null
	fi
	
	echo "Cleaning up current input directory if it exists"
	hadoop dfs -rmr input &> /dev/null
	hadoop dfs -mkdir input &> /dev/null
	
	for x in `ls corpus/*.txt`
	do
		echo "Putting into INPUT dfs file $x"
		hadoop dfs -put $x input
	done 
	
	echo "Inserted corpus into Hadoop DFS."
fi

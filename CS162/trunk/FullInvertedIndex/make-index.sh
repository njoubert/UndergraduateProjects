#!/bin/bash

if [ $# -ne 1 ]
then
	echo "Usage - ./make-index.sh <temp folder>"
	echo "   This will create in index for whatever files is in DFS's input directory."
	echo "   Uses given folder as temporary storage."
	exit 1
fi

TEMPF=$1

echo "Setting environment variables..."
export HDFS=/user/$USER
export TMPSTORE=/tmp/$USER-storage
export HADOOP=hadoop-0.16.0
export PATH=/hadoop/$HADOOP/bin:$PATH
export HOME=/home/$USER

function buildStopwordsFromWordcount {
	
	hadoop dfs -get output/wordcount $TEMPF
	rm -rf $TEMPF/stopwords.txt
	python build_stopwords_from_wordcount.py --directory $TEMPF/wordcount/
	hadoop dfs -put stopwords.txt $HDFS/output
	rm -rf $TEMPF/wordcount
	rm -rf stopwords.txt
	
}


#Check that FullInvertedIndex.jar exists...

if [ -f "FullInvertedIndex.jar" ]
then
	echo "JAR file found."
	echo "Cleaning up output folder"
	hadoop dfs -rmr output &> /dev/null

	echo "Building Stopwords File"
	hadoop jar FullInvertedIndex.jar temp.WordCount $HDFS/input $HDFS/output/wordcount
	echo "Done - wordcount saved to output/wordcount"
	
	buildStopwordsFromWordcount
	
	echo "Building Full Inverted Index"
	hadoop jar FullInvertedIndex.jar temp.MakeIndexDriver $HDFS/input $HDFS/output/index $HDFS/output/stopwords.txt
	echo "Done - Index saved to output/index"
		
else
	echo "Could not locate FullInvertedIndex.jar. Please make sure this JAR file is in the working directory..."
	exit 1
fi

exit 0
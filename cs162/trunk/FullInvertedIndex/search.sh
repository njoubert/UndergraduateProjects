#!/bin/bash

echo "Setting environment variables..."
export HDFS=/user/$USER
export TMPSTORE=/tmp/$USER-storage
export HADOOP=hadoop-0.16.0
export PATH=/hadoop/$HADOOP/bin:$PATH
export HOME=/home/$USER

if [ $# -ne 1 ]
then
	echo "Usage - ./search.sh \"<query>\""
	echo "   This uses the index in output/index to construct output/result."
	echo "   It creates a query.txt temporary file in the current working directory."
	exit 1
fi

if [ ! -f "FullInvertedIndex.jar" ]
then
	echo "Could not locate FullInvertedIndex.jar. Please make sure this JAR file is in the working directory..."
	exit 1
fi

hadoop dfs -rm query.txt &> /dev/null
hadoop dfs -rmr output/result &>/dev/null

rm query.txt &> /dev/null
echo $1 > query.txt

hadoop dfs -put query.txt . &> /dev/null

hadoop jar FullInvertedIndex.jar temp.QueryDriver $HDFS/output/index $HDFS/output/result $HDFS/query.txt

echo 
echo "RESULTS:"
echo 

hadoop dfs -cat output/result/*

#hadoop dfs -get output/result ./result &> /dev/null
#for x in `ls result/*`
#do
#	cat $x
#done 
#rm -rf result &> /dev/null

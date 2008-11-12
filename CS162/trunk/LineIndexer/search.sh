#! /bin/bash

# Need to check for num args.  Make sure there are 2

echo "Setting environment variables..."
export HDFS=/user/$USER
export TMPSTORE=/tmp/$USER-storage
export HADOOP=hadoop-0.16.0
export PATH=/hadoop/$HADOOP/bin:$PATH

echo "Cleaning up possible previous query results..."
hadoop dfs -rm query.txt

echo "Writing query to query.txt, add ('put') query.txt to HDFS"

echo "Run querydriver"
hadoop jar FullInvertedIndex.jar temp.QueryDriver output query-output query.txt

echo "Pull down file contained in \"query-output\" directory from HDFS"

echo "Write contents of this file to stdout"
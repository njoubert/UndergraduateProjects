#! /bin/bash

# Need to check for num args.  Make sure there's only 1

echo "Setting environment variables..."
export HDFS=/user/$USER
export TMPSTORE=/tmp/$USER-storage
export HADOOP=hadoop-0.16.0
export PATH=/hadoop/$HADOOP/bin:$PATH

echo "Creating \"input\" directory on Hadoop DFS..."
hadoop dfs -mkdir $HDFS/input

echo "Adding the Shakespeare corpus to HDFS \"input\" directory..."
hadoop dfs -put /tmp/willywu-storage/corpus/0ws0110.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws0210.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws0310.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws0410.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws0610.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws0910.txt $HDFS/input

hadoop dfs -put /tmp/willywu-storage/corpus/0ws1010.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws1110.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws1210.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws1410.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws1510.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws1610.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws1710.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws1810.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws1910.txt $HDFS/input

hadoop dfs -put /tmp/willywu-storage/corpus/0ws2010.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws2110.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws2210.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws2310.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws2410.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws2510.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws2610.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws2810.txt $HDFS/input

hadoop dfs -put /tmp/willywu-storage/corpus/0ws3010.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws3110.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws3210.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws3310.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws3410.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws3510.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws3610.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws3710.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws3910.txt $HDFS/input

hadoop dfs -put /tmp/willywu-storage/corpus/0ws4010.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws4110.txt $HDFS/input
hadoop dfs -put /tmp/willywu-storage/corpus/0ws4210.txt $HDFS/input

echo "Running MakeIndexDriver on \"input\" directory..."
hadoop jar FullInvertedIndex.jar temp.MakeIndexDriver $HDFS/input $HDFS/output

echo "Index successfully created."

echo "Moving output from HDFS to local storage..."
hadoop dfs -get $HDFS/output/part-00000 $TMPSTORE/output/fullindex
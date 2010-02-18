#!/bin/bash
export JAVA_HOME=`which javac | xargs readlink -f | xargs dirname | xargs dirname`
/usr/class/cs149/hadoop-0.20.1/bin/hadoop jar ngrammer.jar cs149.ngram.Main 2 query1.txt file://$PWD/input file://$PWD/output



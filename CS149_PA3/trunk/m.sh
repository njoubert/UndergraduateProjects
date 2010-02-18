#!/bin/bash

rm -rf ngrammer.jar
export JAVA_HOME=`which javac | xargs readlink -f | xargs dirname | xargs dirname`
javac -classpath hadoop-0.20.1-core.jar ./cs149/ngram/*.java -verbose
jar -cf ngrammer.jar ./cs149/ngram/*.class
rm -rf output/

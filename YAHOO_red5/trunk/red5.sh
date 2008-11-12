#!/bin/bash

for JAVA in "/home/y/libexec/jre1.6.0/bin/java" "/System/Library/Frameworks/JavaVM.framework/Versions/1.6.0/Home/bin/java" "$JAVA_HOME/bin/java" "/usr/bin/java" "/usr/local/bin/java"
do
  if [ -x $JAVA ]
  then
    break
  fi
done

if [ ! -x $JAVA ]
then
  echo "Unable to locate java. Please set JAVA_HOME environment variable."
  exit
fi

YAHOO_CLASSPATH=:/home/y/libexec/jre1.6.0/lib:
# YAHOO_CLASSPATH=$CLASSPATH:$YAHOO_CLASSPATH
echo "Classpath in use: "
echo $YAHOO_CLASSPATH

exec $JAVA -Djava.security.manager -Djava.security.policy=conf/red5.policy -cp red5.jar:conf:$YAHOO_CLASSPATH org.red5.server.Standalone

#!/bin/bash

writefile=$1
writestr=$2

if [[ "$writefile" == "" || "$writestr" == "" ]]
then
  echo "No dir and No Str"
  exit 1
else
  if [[ ! -d $writefile ]]
  then
    mkdir $(dirname "$writefile")
  fi
  echo "$writestr" > "$writefile"
  if [ $? == 1 ]
  then
    echo "file could not creted"
    exit 1
  fi
fi


#!/bin/bash

writefile=$1
writestr=$2

if [[ "$writefile" == "" || "$writestr" == "" ]]
then
  echo "ERROR : either dir or serchstring is not provided!! Please provide inputs properly"
  exit 1
else
  if [[ ! -d $writefile ]]
  then
    mkdir $(dirname "$writefile")
  fi
  echo "$writestr" > "$writefile"
  if [ $? == 1 ]
  then
    echo "ERROR :file could not creted"
    exit 1
  fi
fi


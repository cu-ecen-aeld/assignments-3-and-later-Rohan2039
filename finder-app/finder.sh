#!/bin/sh
# Tester script for assignment 1 and assignment 2
# Author: Siddhant Jajoo

filesdir=$1
searchstring=$2

echo $filesdir
echo $searchstring

if [[ "$filesdir" == ""  ||  "$searchstring" == "" ]]
then
  echo "ERROR : either dir or serchstring is not provided!! Please provide inputs properly"
  exit 1
elif [ ! -d $filesdir ]
then
  echo " ERROR : No such file directory"
  exit 1
else
  X=$(find "$filesdir" -type f | wc -l)
  Y=$(grep -n $searchstring $filesdir/* | wc -l)
 printMsg="The number of files are $X and the number of matching lines are $Y"
 echo $printMsg
fi

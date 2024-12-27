#!/bin/bash

filesdir=No_dir
searchstring=No_Str
filesdir=$1
searchstring=$2

if [$filesdir ==No_dir] || [$searchstring ==No_Str]
then
  echo 1
elif [-d filesdir]
then
  echo 1
else
  X=$(ls -c)
  Y=$(grep -c searchstring *)
  printMsg="The number of files are $X and the number of matching lines are $Y"
  echo $printMsg
fi
  
  

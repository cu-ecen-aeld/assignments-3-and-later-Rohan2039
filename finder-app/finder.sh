
#!/bin/bash

filesdir=$1
searchstring=$2

if [[ "$filesdir" == ""  ||  "$searchstring" == "" ]]
then
  echo "No dir or No string"
  exit 1
elif [ ! -d $filesdir ]
then
  echo "No file directory"
  exit 1
else
  X=$(find "$filesdir" -type f | wc -l)
  Y=$(grep -n $searchstring $filesdir/* | wc -l)
 printMsg="The number of files are $X and the number of matching lines are $Y"
 echo $printMsg
fi








  

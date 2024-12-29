
#!/bin/bash

filesdir="No_dir"
searchstring="No_Str"
filesdir=$1
searchstring=$2

echo $filesdir
echo $searchstring
if [[ $filesdir == "No_dir"  ||  $searchstring == "No_Str" ]]
then
  echo "No dir or No string"
  exit 1
elif [ ! -d $filesdir ]
then
  echo "No file directory"
  exit 1
else
  X=$(find "$filesdir" -type f | wc -l)
  Y=$(grep -c $searchstring *)
 printMsg="The number of files are $X and the number of matching lines are $Y"
 echo $printMsg
fi








  

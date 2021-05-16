#!/bin/bash

echo Enter the name of the folder containing various core data: 
read folder
n=0
arg=""
newline="\n"
rowdelay=10
coldelay=2
for filename in $folder/*; do
    # echo $filename
    let "n += 1"
    echo "Statistics for file number $n :"
    ./a4.out $filename $rowdelay $coldelay
    temp=$arg
    arg="$temp $filename" 
done
#echo $arg
echo "Enter the number of clock cycles who wish to execute:"
read c
echo Output:-
./a5.out $n $c $arg $rowdelay $coldelay
mv output.txt $folder
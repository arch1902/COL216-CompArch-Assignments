#!/bin/bash

echo Enter the name of the folder you want to clean: 
read folder

for filename in $folder/*.txt; do
    # echo $filename
    rm $filename
done

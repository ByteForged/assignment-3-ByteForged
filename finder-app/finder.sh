#!/bin/sh

if [ $# -ne 2 ]; then
    echo "Must provide 2 arguments: filesDir and searchStr"
    exit 1
fi

filesDir=$1
searchStr=$2

if [ ! -d $filesDir ]; then
    echo "Error: Arguement filesDir is not a directory."
    exit 1
fi

# List all regular files
files=$(find $filesDir -type f)
# Count all regular files
fileCount=$(echo "$files" | wc -l)
# Find matches in all non-binary files for the search string
matchingLines=$(grep -I $searchStr $files | wc -l) # skip binary files

echo "The number of files are $fileCount and the number of matching lines are $matchingLines"
exit 0

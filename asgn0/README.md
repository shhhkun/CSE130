# Assignment #0: Split
Serjo Barron

sepbarro

Fall 2022

## [files]
 - split.c
 - Makefile
 - README.md

## [description]
This program can take as input an arbitrary number of files (including stdin) to receive text from to then replace any instances of the specified delimiter character with a newline. The resulting modified or not modified text depending on the delimiter will then be printed to stdout for the user to see.

## [design]
Listed below are a few notable design decisions make when creating this program:
 - Buffer size: 4096 bytes in order to reduce the amount of syscalls ran such as read or write

## [instructions]
To run this program first ensure that the files listed above are present in the same directory. Then run the command **make all** to produce a binary called **split**. Then take any file(s) of your choice and run split using the syntax below:

 > ./split <files> <delimiter>

You may also include "-" as a file for split to read from stdin instead of opening a specified file. 

Example: *files* are file1, file2, file3, -, and file4 with the *delimiter* being x

 > ./split file1 file2 file3 - file4 x


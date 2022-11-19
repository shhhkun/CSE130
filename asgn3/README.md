# Assignment #3: Audit Log
Serjo Barron
sepbarro

Fall 2022

## [files]
 - httpserver.c
 - bind.c
 - bind.h
 - Makefile
 - README.md

## [description]
This takes the httpserver previously written in Assignment #1, and implements an audit log to keep track of the processed requests sent to the server. This server at this point in particular is still single-threaded so it can handle at most 1 request at a time. The types of code phrases returned has also been drastically reduced compared to Assignment #1 and now only cover the codes: 200, 201, 404, and 500.

## [design]
Listed below are a few notable design decisions made when creating this assignment:
 - Reduced status codes:
     - as a result of the reduced status codes I took the choice to keep certain error handling (even though it may not be tested), and other error handling regarding file permissions to now fall under
the status code of 500.
 - Concurrency:
     - to ensure that the audit log keeps the right order as the requests are processed, after the httpserver has sent its response a logentry soon follows to ensure that the request was 100% processed before writing anything to the audit log

## [instructions]
First ensure that the files listed are in the same directory, then run the command **make** to build the executable "httpserver". Similar to Assignment #1 the server takes a port as an argument, but new (optional) arguments have been added. The first being the number of threads, which does nothing in this assignment as the server is still single-threaded but will play a role in the next assignment. The second is the option for audit log to be written to a specified file by the user. If no logfile is specified by the user, then entries will simply be printed to STDERR.

The following CLI options are:

> ./httpserver [-t threads (optional)] [-l logfile (optional)] []()<port[]()>

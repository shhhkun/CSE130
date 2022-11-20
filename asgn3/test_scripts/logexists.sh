#!/usr/bin/env bash

# This tests checks that the audit log has been created by the httpserver by checking whether the
# server is running or not running after giving the -l CLI option.

port=$(bash test_files/get_port.sh)
log="log.txt"

# Start up server.
./httpserver -l $log $port > /dev/null &
pid=$!

# if pid is running then the log was accepted and the server is waiting
if ps -p $pid > /dev/null
then
    kill -9 $pid
    wait $pid
    
    rm -f $log
    exit 0
fi

# else the server didnt accept the log and is not running
kill -9 $pid
wait $pid
rm -f $log
exit 1

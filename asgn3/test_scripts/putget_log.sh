#!/usr/bin/env bash

# This tests checks that the audit log has the expected response when running a PUT for 201 code
# with a GET after for 200 code

port=$(bash test_files/get_port.sh)
log="log.txt"

# Start up server.
./httpserver -l $log $port > /dev/null &
pid=$!

# Wait until we can connect.
while ! nc -zv localhost $port; do
    sleep 0.01
done

for i in {1..5}; do
    file="randomfile.txt"
    temp="temp.txt"

    for y in {1..i}; do
	    printf "PUT,/$file,201,2\nGET,/$file,200,1\n" >> $temp
    done

    printf "PUT /$file HTTP/1.1\r\nRequest-Id: 2\r\nContent-Length: 12\r\n\r\nApple Juice?" >> request2
    printf "GET /$file HTTP/1.1\r\nRequest-Id: 1\r\n\r\n" >> request1


    # run PUT and GET request
    cat request2 | nc localhost $port > /dev/null
    cat request1 | nc localhost $port > /dev/null

    # Check the diff.
    diff $log $temp
    if [[ $? -ne 0 ]]; then
        # Make sure the server is dead.
        kill -9 $pid
        wait $pid
        rm -f $log $temp $file request1 request2
        exit 1
    fi

    # Clean up.
    rm -f $file request1 request2
    
done

# Make sure the server is dead.
kill -9 $pid
wait $pid

# Clean up.
rm -f $log $temp $file request1 request2

exit 0

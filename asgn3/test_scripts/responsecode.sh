#!/usr/bin/env bash

# This tests checks that the audit log's attached status codes match up and are in the right order
# of the response status codes sent by the server.

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
    expected1=404
    expected2=201

    for y in {1..i}; do
	    printf "GET,/$file,404,1\nPUT,/$file,201,2\n" >> $temp
    done

    printf "GET /$file HTTP/1.1\r\nRequest-Id: 1\r\n\r\n" >> request1
    printf "PUT /$file HTTP/1.1\r\nRequest-Id: 2\r\nContent-Length: 12\r\n\r\nApple Juice?" >> request2


    # run GET and PUT request
    code1=$(cat request1 | nc localhost $port | head -n 1 | awk '{ print $2 }')
    code2=$(cat request2 | nc localhost $port | head -n 1 | awk '{ print $2 }')

    # Check the diff.
    diff $log $temp
    if [[ $? -ne 0 ]]; then
        # Make sure the server is dead.
        kill -9 $pid
        wait $pid
        rm -f $log $temp $file request1 request2
        exit 1
    fi

    # Check response code1
    if [[ $code1 -ne $expected1 ]]; then
	kill -9 $pid
	wait $pid
	rm -f $log $temp $file request1 request2
	exit 1
    fi

    # Check response code2
    if [[ $code2 -ne $expected2 ]]; then
	kill -9 $pid
	wait $pid
	rm -f $log $temp $ file request1 request2
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

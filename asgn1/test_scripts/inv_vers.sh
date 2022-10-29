#!/usr/bin/env bash

# This tests if httpserver will return '400' Bad Request upon giving an HTTP version
# that is not '1.1'

port=$(bash test_files/get_port.sh)

# Start up server.
./httpserver $port > /dev/null &
pid=$!

# Wait until we can connect.
while ! nc -zv localhost $port; do
    sleep 0.01
done

for i in {1..10}; do
    # Test input file.
    file="test_files/wonderful.txt"
    infile="temp.txt"
    size=$(wc -c $file | awk '{ print $1 }') # take filesize of file

    # Create the input file to overwrite.
    cp $file $infile

    # Expected status code.
    expected=400

    # HTTP vers 1.0
    printf "GET /$infile HTTP/1.0\r\n\r\n" > requestfile

    # The only thing that should be printed is the status code.
    actual=$(cat requestfile | nc localhost $port | head -n 1 | awk '{ print $2 }')

    # Check the status code.
    if [[ $actual -ne $expected ]]; then
        # Make sure the server is dead.
        kill -9 $pid
        wait $pid
        rm -f $infile requestfile
        exit 1
    fi

    # Clean up.
    rm -f $infile requestfile
done

# Make sure the server is dead.
kill -9 $pid
wait $pid

# Clean up.
rm -f $infile requestfile

exit 0

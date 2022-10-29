#!/usr/bin/env bash

# This tests if httpserver will return '501' Not Implemented when a method that is within range
# [a-z][A-Z] and valid length is used

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
  
    # Create the input file to overwrite.
    cp $file $infile

    # Expected status code.
    expected=501

    # Method: [a-z][A-Z] and 0 < len < 9
    printf "STARE /$infile HTTP/1.1\r\n\r\n" > requestfile

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

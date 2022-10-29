#!/usr/bin/env bash

# Tests httpserver on HEAD requests on files with no read permissions

# Get available port.
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
    file="test_files/cantreadme.txt"
    
    # Expected status code.
    expected=403

    # The only thing that is should be printed is the status code.
    actual=$(curl -s -w "%{http_code}" -o /dev/null localhost:$port/$file -I)

    # Check the status code.
    if [[ $actual -ne $expected ]]; then
        # Make sure the server is dead.
        kill -9 $pid
        wait $pid
        exit 1
    fi

done

# Make sure the server is dead.
kill -9 $pid
wait $pid

exit 0

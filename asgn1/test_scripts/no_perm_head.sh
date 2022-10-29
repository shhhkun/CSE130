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
    infile="cantreadme.txt"

    echo "secretttt" > $infile
    chmod -r $infile
    
    # Expected status code.
    expected=403

    # The only thing that is should be printed is the status code.
    actual=$(curl -s -w "%{http_code}" -o /dev/null localhost:$port/$infile -I)

    # Check the status code.
    if [[ $actual -ne $expected ]]; then
        # Make sure the server is dead.
        kill -9 $pid
        wait $pid
	rm -f $infile
        exit 1
    fi

    rm -f $infile
done

# Make sure the server is dead.
kill -9 $pid
wait $pid

rm -f $infile

exit 0

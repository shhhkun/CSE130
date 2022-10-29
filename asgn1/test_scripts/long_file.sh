#!/usr/bin/env bash

# This tests whether the httpserver will return '400' Bad Request if the filename pointed to by
# the file path is 'strlen(filename) > 19'

port=$(bash test_files/get_port.sh)

# Start up server.
./httpserver $port > /dev/null &
pid=$!

# Wait until we can connect.
while ! nc -zv localhost $port; do
    sleep 0.01
done

for i in {1..5}; do
    # Test input file.
    file="test_files/fooooooooooooooooooo.txt" # 24 char long file

    # Expected status code.
    expected=400

    # The only thing that should be printed is the status code.
    actual=$(curl -s -w "%{http_code}" -o /dev/null localhost:$port/$file)

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

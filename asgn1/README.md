# Assignment #1: httpserver (single-threaded)
Serjo Barron

sepbarro

Fall 2022

## [files]
 - httpserver.c
 - bind.c
 - Makefile
 - README.md

## [description]
This program implements a webserver with (simplified) HTTP 1.1 protocol. It takes in a valid port (of type uint16\_t) with a valid range of 1024 ... 65,535. After taking in a valid port specified by the user, the webserver will then be able to accept requests limited to GET, HEAD, and PUT. These requests must be formatted according to HTTP/1.1 and can be sent on terminal via curl or nc (netcat).

## [design]
Listed below are a few notable design decisions made when creating this program:
 - No regex:
     - there are a variety of functions to check range [a-z][A-Z][0-9] such as isalnum() which checks if a character is alphabetical or numeric
     - you can also simply compare the character to special characters such as . and _
 - Functions (modularity):
     - separated important/reusable tasks such as retrieving the code phrase, formatting responses
     - kept methods GET, PUT, HEAD as separate functions outside of main loop for ease of testing
 - Buffers:
     - kept buffers that have to deal/store things related to requests to a size of 2048 bytes (requests are MAX 2048 bytes)
     - read/write (mostly) in chunks of 4096 bytes
     - exception of request handling where we read 1 byte at a time which in the end makes it easier to parse the request and detect any potential ill-formated parts
 - Read/Write permission handling (for PUT):
     - in the case that a file exists do not change its read/write perms (assuming no errors were triggered up to this point)
     - in the case that a file does not exist, create it and assign read and write user permissions

## [instructions]
To run this program first ensure that the files listed above are present in the same directory. Then run the command **make all** to produce a binary called **httpserver**. To run httpserver use the syntax below:

> ./httpserver port

You may also add a "&" as another argument (following after port) to run the server in the background, to bring it back to the foreground use the command "fg".

#### Now to send valid requests using curl:

> curl http[]()://localhost:[]()<port[]()>/[]()<filepath[]()> -i

Above is an example of a GET request, to change it to a HEAD request simply change the "-i" to "-I". As for a PUT request:

> curl -X PUT http[]()://localhost:[]()<port[]()>/[]()<filepath[]()> -d "[]()<message body[]()>"

#### Now to send valid requests using netcat:

> printf "[]()<method[]()> /[]()<filepath[]()> HTTP/1.1\r\n[]()<[]()header-fields[]()>\r\n[]()<message body[]()>" | nc localhost []()<port[]()>

> header-fields: []()<key: value\r\n[]()>

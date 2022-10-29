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

## [instructions]
To run this program first ensure that the files listed above are present in the same directory. Then run the command **make all** to produce a binary called **httpserver**. To run httpserver use the syntax below:

> ./httpserver <span><port></span>

You may also add a "&" as another argument (following after port) to run the server in the background, to bring it back to the foreground use the command "fg".

#### Now to send valid requests using curl:

> curl <span>http://localhost:<port>/<filepath> -i</span>

Above is an example of a GET request, to change it to a HEAD request simply change the "-i" to "-I". As for a PUT request:

> curl <span>-X PUT http://localhost:<port>/<filepath> -d "<message body>"</span>

#### Now to send valid requests using netcat:

> printf <span>"<method> /<filepath> HTTP/1.1\r\n<Header-fields>\r\n<message body>" | nc localhost <port></span>

> Header-fields: <span><key: value\r\n></span>

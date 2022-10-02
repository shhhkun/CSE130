# Tests split using random delimiters on split itself
text="split"
./split $text $(cat /dev/random | head -c 1) > /dev/null

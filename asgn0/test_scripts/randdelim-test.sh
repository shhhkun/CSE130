# Tests split using random delimiters
text="test_files/Wallpaper.webp"
./split $text $(cat /dev/random | head -c 1) > /dev/null

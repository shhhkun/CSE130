# Tests split using invalid 1 character delimiters "* and ~"
text="/test_files/Wallpaper.webp"
./split $text * >> /dev/null
if [ $? = 0 ]
then
	exit 1
fi
./split $text ~ >> /dev/null
if [ $? = 0 ]
then
        exit 1
fi


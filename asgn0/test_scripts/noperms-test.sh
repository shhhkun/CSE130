# Tests split when attempting to open a no read permissions file
touch secret.txt
chmod -r secret.txt
texts="test_files/test0 secret.txt test_files/Wallpaper.webp"
diff <(./split $texts x) <(./rsplit $texts x) > /dev/null
rm secret.txt

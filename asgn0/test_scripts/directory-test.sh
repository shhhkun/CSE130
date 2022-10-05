# Tests split when attempting to open a filename that is instead a directory
text="test_files/fold"
diff <(./split $text o) <(./rsplit $text o) > /dev/null

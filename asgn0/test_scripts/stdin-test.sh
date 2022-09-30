# Tests if split can handle stdin input alongside regular files
text="test_files/test0 - test_files/test1"
text1="Ax ax ax AX xA aX xxA ax ax ax X"
echo $text1 | ./split $text x >> /dev/null


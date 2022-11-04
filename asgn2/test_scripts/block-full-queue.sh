# This test pushes has Thread 1 push an element onto a full queue causing an indefinite wait, but
# then has Thread 2 pop an element to allow the Thread 1's push to pass.

cleanup(){
	rm -r test_dir
}

QUEUE="queue.o queue.h"
TEST_FILES="test_files/block-full-queue.c test_files/Makefile"

mkdir test_dir

cp $QUEUE test_dir
cp  $TEST_FILES test_dir

make -C test_dir

./test_dir/queue-test

if [ $? != 0 ]
then
	cleanup
	exit 1
fi

cleanup

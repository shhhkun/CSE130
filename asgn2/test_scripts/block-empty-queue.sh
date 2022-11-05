# This test pushes has Thread 1 pop an element from an empty queue causing an indefinite wait,
# but has Thread 2 push an element allowing Thread 1's pop.

cleanup(){
	rm -r test_dir
}

QUEUE="queue.o queue.h"
TEST_FILES="test_files/block-empty-queue.c test_files/Makefile"

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

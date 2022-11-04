# This test creates a full queue, dequeues half of its elements and enqueues another half of elements
# to check that the queue is circular.

cleanup(){
	rm -r test_dir
}

QUEUE="queue.h queue.o"
TEST_FILES="test_files/circular-queue.c test_files/Makefile"

mkdir test_dir

cp $QUEUE test_dir
cp  $TEST_FILES test_dir

make -C test_dir

timeout 10 ./test_dir/queue-test

if [ $? != 0 ]
then
        cleanup
        exit 1
fi

cleanup

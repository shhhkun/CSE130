# Assignment #2: Bounded Buffer (multi-threaded)
Serjo Barron
sepbarro

Fall 2022

## [files]
 - queue.c
 - queue.h
 - Makefile
 - README.md

## [description]
This is a multi-threaded queue able to handle multiple threads at once while keeping concurrency (order). This in particular is not a main program, but instead resembles that of a included library or ADT (abstract data type). This ADT has operations to construct & delete a queue, as well as enqueue & dequeue on the queue.

## [design]
Listed below are a few notable design decisions made when creating this assignment:
 - Circular queue:
     - indexing is calculated using an incrementer % (modulus) the size of the queue to ensure that even if the tail of the queue is at the end of the array that does not guarantee the queue is full
 - Locks:
     - since there are non-atomic areas of code, mutex locks & unlocks are used to ensure that no more than 1 thread can enter that "critical region" at a time
 - Wait/Signal:
     - in the event that the queue is empty or full, the queue should be able to identify that and block enqueue or dequeue until the "block" condition is no more

## [instructions]
Since this is only an ADT running the command **make** with the given Makefile will only produce a **.o** file called queue.o. In order to use this file you must link (using -ld flag) and compile it to another **.c** file that has #include "queue.h".

Once you have produced a binary that includes queue.h then you may use the 4 functions defined in the ADT.

The 4 functions included in the queue are:

> queue\_new(size)
> queue\_delete(q)
> queue\_push(q, elem)
> queue\_pop(q, elem)

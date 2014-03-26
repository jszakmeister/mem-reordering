**************
mem-reordering
**************

An example showing how processors can re-order memory accesses.  Much of the
code is taken from:
http://preshing.com/20120515/memory-reordering-caught-in-the-act/

This is an implementation of the example in section 8.2.3.4 of the
:title:`Intel 64 and IA-32 Architectures Software Developer’s Manual Volume 3:
System Programming Guide`.  It detects when memory accesses have been reordered,
and also shows how the issue goes away with a memory fence.  On Linux, it will
also build a third binary that ties the threads to a single CPU so that you can
see that the problem does not exist when the threads are scheduled onto the same
cpu.

Note: on Mac OS X, we don't have fine-grained control over where the threads get
scheduled.  Mac's idea of affinity is actually L2 cache affinity, meaning that
the threads would get scheduled to run out of the same L2 cache, but could run
on different cores within the same physical processor.  In that case, the
re-ordering can still happen.

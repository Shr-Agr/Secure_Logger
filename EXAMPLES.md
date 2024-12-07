[Back](README.md)

Examples
========
Consider the following 4 invocations of `logappend` followed by an invocation of `logread`:

    $ ./logappend -T 1 -K secret -A -E Fred log1
    $ ./logappend -T 2 -K secret -A -G Jill log1
    $ ./logappend -T 3 -K secret -A -E Fred -R 1 log1
    $ ./logappend -T 4 -K secret -A -G Jill -R 1 log1

These commands have used the key *secret* to append 4 events to the log `log1`, recording the arrival of *Fred* and *Jill* in room *1* of the campus. If `logread` is then used to print the state of the campus, the following is printed: 

    $ ./logread -K secret -S log1
    Fred
    Jill
    1: Fred,Jill

If we continue using `log1` and record some movements, we can then use `logread` to get a list of the rooms entered by Fred.

    ./logappend -T 5 -K secret -L -E Fred -R 1 log1
    ./logappend -T 6 -K secret -A -E Fred -R 2 log1
    ./logappend -T 7 -K secret -L -E Fred -R 2 log1
    ./logappend -T 8 -K secret -A -E Fred -R 3 log1
    ./logappend -T 9 -K secret -L -E Fred -R 3 log1
    ./logappend -T 10 -K secret -A -E Fred -R 1 log1
    ./logread -K secret -R -E Fred log1
    1,2,3,1 

We can also use `logappend` in batch mode (on a fresh log `log2`):

    $ cat batch
    -K secret -T 0 -A -E John log2
    -K secret -T 1 -A -R 0 -E John log2
    -K secret -T 2 -A -G James log2
    -K secret -T 3 -A -R 0 -G James log2
    $ ./logappend -B batch
    invalid
    invalid
    $ echo $?
    0
    $ ./logread -K secret -S log2
    James
    0:James
    $ echo $?
    0

The first two lines of the batch file are invalid since 0 is an invalid time so the first line fails. 
Then the second line fails with `"invalid"` since John is not in the campus. 
Here is a successful run with the time arguments fixed: 

    $ cat batch
    -K secret -T 1 -A -E John log2
    -K secret -T 2 -A -R 0 -E John log2
    -K secret -T 3 -A -G James log2
    -K secret -T 4 -A -R 0 -G James log2
    $ ./logappend -B batch
    $ echo $?
    0
    $ ./logread -K secret -S log2
    John
    James
    0:James,John
    $ echo $?
    0
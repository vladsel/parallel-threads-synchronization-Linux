# parallel-threads-synchronization-Linux

(19.04.2021)
Means of interaction of parallel threads of the Linux operating system.

1. Write a program that implements the work of parallel streams according to the given by
variant of the scheme. Peculiarities of implementation of synchronization of parallel streams and mutual
the exclusion of streams when accessing shared resources is specified by options in tables | and
2. When writing a program, perform a full tracing of the operation of the program according to
using print operators, i.e. arrange the print operators in the program in such a way,
so that it is possible to follow all options for the execution of parallel threads and make sure of
the correctness of the program. It is recommended to write the trace protocol to a file (Ioye-
file).
3. All threads must be started in the main program.
4. Each flow must be organized in the form of an endless loop.
5 All actions set according to the options indicated in the table must be performed inside this endless loop.
6. Mutual location of synchronization operators and access to a shared resource,
if they are in the same thread is arbitrary.
7. Because synchronization using semaphores 5SV21, 5SV22 according to the task
is located inside infinite loops, then immediately after performing synchronization these
the semaphores must be reset to their original closed state.
8. You can complete the program in two ways:
. forced interruption by pressing a key combination
skin;
. by the Bgeac operator when the condition is fulfilled, which becomes true when the buffer
of the shared resource is completely filled and completely freed at least twice.
9. If the i5ser() function was used when implementing parallel streams, then provide a program launch mode with "disabled" i5iser() functions.
10. Debugging the written program.

Explanation to the table with task options

RI-Rb streams should be organized in the form of endless cycles, in the body of which should
to implement all the actions specified by the task variants.

SVI is the first common resource (sopatop gokoshgse) in the form of a buffer for data exchange between
flow-suppliers and flow-consumers RI-Rb. The way to implement the buffer and
the method of mutual exclusion of RI-Rb flows when accessing the SVI is defined in Table I
by options.

The type of access to the shared SVI resource (writing or reading) is determined, respectively, by the type
of each of the RI-Rb flows (supplier or consumer) and is shown in the diagram according to the variant
arrows

The method of implementing both the 8SVI semaphore and the MSKI mutex (blocking or non-blocking)
indicated in the task table by options.

SVO - the second shared resource (sotitope hesoigse) in the form of atomic variables.
Conditional terms:

"Using" - taking the values of atomic variables without changing them (for example,
- whoops fessi apa ada with zero) and using these values for local calculations;
"Modification" - changing the values of atomic variables without using the obtained values
for local calculations;

"Usage and modification" - changing the values of atomic variables and usage
values returned by functions for local calculations.

In the columns "Means of synchronization of parallel streams" of the task options table is indicated
thread synchronization tasks. To synchronize streams, depending on the task
synchronization, which is resolved (full or partial) by option, should be used
- for the task of full synchronization: either two binary semaphores (5 SV21-5 SV22), or two
signal (conditional) variables (8i221--8ir22), or barrier (VSE);

- for the task of incomplete synchronization: either one binary 5SVI semaphore, or one signal
(conditional) variable 51921.

In addition, for each of the semaphores 8СЕ2И and 8СВО2, the table shows the method of its
implementation (blocking or non-blocking), and for signal (conditional) variables - the type of signal
(multi-valued or single-valued).

DSD - Dynamic Data Structure.

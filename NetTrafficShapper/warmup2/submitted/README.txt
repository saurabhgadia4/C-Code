Documentation for Warmup Assignment 2
=====================================

+-------+
| BUILD |
+-------+

1. Grader can build the warmup2 program executable by just executing make command.
2. Submitted tar consists of Makefile, README.txt, cs402.h, driver.c, driver.h, my402list.h,
   my402queue.c, my402queue.h files.

+------+
| SKIP |
+------+
Each and every test case mentioned in grading guidelines works as expected.

+---------+
| GRADING |
+---------+

Basic running of the code : 100 out of 100 pts

Missing required section(s) in README file : (No section in readme file is incomplete.)

Cannot compile : (No compiling issues. Program works as expected.)

Compiler warnings : (No compiling warnings generated while compiling. Program works as expected.)

"make clean" : (works as expected.)

Segmentation faults : (There are no segmentation fault in program.)

Separate compilation : (Have used seperate compilation modules. I have my402queue.c module to be linked with main driver.c module.)

Using busy-wait : (There is no busy-wait problem in any of the mentioned test-case testing.)

Handling of commandline arguments:
    1) -n : (Handled properly. Program works as expected)
    2) -lambda : (Handled properly. Program works as expected. if 1/lambda > 10 sec then we take 1/lambda=10sec i.e 10000ms)
    3) -mu : (Handled properly. Program works as expected. if 1/lambda > 10 sec then we take 1/mu=10sec i.e 10000ms)
    4) -r : (Handled properly. Program works as expected. if 1/r > 10 sec then we take 1/lambda=10sec i.e 10000ms)
    5) -B : (Handled properly. Program works as expected.)
    6) -P : (Handled properly. Program works as expected.)

Trace output :
    1) regular packets: (Program generates exactly 7 lines of trace and contains correct information)
    2) dropped packets: (exactly 1 line of trace and contain correct information)
    3) removed packets: (Program generates output expected as per specification of removal of packets)
    4) token arrival (dropped or not dropped): (Program handles both the cases as mentioned in specs.)

Statistics output :
    1) inter-arrival time : (Works properly as per specifications)
    2) service time : (Works properly as per specifications)
    3) number of customers in Q1 : (Works properly as per specifications)
    4) number of customers in Q2 : (Works properly as per specifications)
    5) number of customers at a server : (Works properly as per specifications)
    6) time in system : (Works properly as per specifications)
    7) standard deviation for time in system : (Works properly as per specifications)
    8) drop probability : (Works properly as per specifications)

Output bad format : (Output format is generated as it is mentioned in specs. No issues.)

Output wrong precision for statistics (should be 6-8 significant digits) : (Output precision format is generated as it is mentioned in specs. No issues.)

Large service time test : (Programs handles every test-case under this parent test-case properly.)

Large inter-arrival time test : (Programs handles every test-case under this parent test-case properly.)

Tiny inter-arrival time test : (Programs handles every test-case under this parent test-case properly.)

Tiny service time test : (Programs handles every test-case under this parent test-case properly.)

Large total number of customers test : (Programs handles every test-case under this parent test-case properly.)

Large total number of customers with high arrival rate test : (Programs handles every test-case under 
this parent test-case properly.)

Dropped tokens test : (Programs handles every test-case under this parent test-case properly.)
Cannot handle <Cntrl+C> at all (ignored or no statistics) : (Program handles <ctrl+c> properly and 
works as per specifications)

Can handle <Cntrl+C> but statistics way off : (Program handles <ctrl+c> properly and works as per specifications. Statistic part is also displayed properly without any way off.)

Not using condition variables and do some kind of busy-wait : (Condition variable is used as mentioned in specifications.)

Synchronization check : (Synchronization between all 4 threads is done by using mutex as mentioned in 
specifications. No semaphore is used.)

Deadlocks : (Program is deadlock free.)

+------+
| BUGS |
+------+

The warmup2 program is bug free. There are no bugs generated according to grading guidelines specifications.

+-------+
| OTHER |
+-------+

Comments on design decisions:

 Instead of 'my402list' I have designed my own 'my402queue' data structure which does all required operations for this project in O(1) running time. Instead of each node having back and next pointer I have only next pointer for each node. back pointer is not required for this project. 

Comments on deviation from spec:

 Program is developed and tested as per specifications and grading guidelines mentioned by Prof.Bill Cheng.


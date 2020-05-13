Disk Benchmarking 

Folder Structure - 
	* MyDiskBenchmark folder contains src folder having source code file MyDiskBench.c and Makefile to build code. It also contains folders and files for their output runs.
	* The Iozone folder contains the iozoneRunTests.sh script to run the benchmark test. It also contains other folders and files with their output runs. 

Steps to run MyDiskBenchmark - 
	* First run MakeFile to build the source code.
	* To run MakeFile just enter the name of the file and hit enter.
	Command to run makefile:  . ./Makefile
	* After this, an executable file(green color) with name MyDiskBenchmark will be generated.  
	* Now, to take benchmark run MyDiskBench binary file with following arguments - 
	1. First Argument - Record Size in bytes
	2. Second Argument - Number of threads
	3. Thirds Argument - Access Pattern(WS, WR, RS, RR)
	* Example - ./.MyDiskBench 1048576 4 WS
        	In this run, Record size will be 1MB, total of 4 threads and WS(Write Sequential) access pattern.
	* Similarly you can change record size, number of threads and different access patterns for different benchmarks.
	* Make sure to give record size in bytes.
	* Output of the benchmark will be printed on the console like this - 
        	-  Write Sequential:
        	-  Record Size: 1048676 bytes
         	-  No of threads: 4
        	- File Size: 26843564560 bytes
        	- Execution time for writing sequentially is 400.16 secs and throughput is 25.589 MBps
	The file size printed in the output is the size of the file read or write by each thread.
	Execution time is the time taken by the threads to write sequentially and throughput is the disk benchmark speed in MBps.


Steps to run IOZone Benchmark - 
	* I have created a script with the name iozoneRunTests.sh which contains a command to run all the benchmark tests with different record size and concurrency and will store the results in output files.
	* Output files can be checked in the output folder for more detail.
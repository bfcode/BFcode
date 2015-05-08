Bloom-filter-test
=================

####1. produceTraffic: Produce traffic file which contains random elements without duplicates.

 * Compile:  g++ -O2 traffic.cpp -o traffic

 * Run:      ./traffic 64 100000

	Means produce a traffic file with 102,400,000 elements without duplicates, and each element is 20 characters in length.

 * Detailed: 

	* First parameter: Bit length of the element using the binary system. But the actual element stored in the traffic file is in decimalism, which is 20 characters in length.
So the parameter 64 means the biggest element in the traffic file is 2^64-1=18446744073709551615, which is 20 characters in length.

	* Second parameter: Size of the traffic file, which will multiply by 1024 in the program.
So the parameter N will actually produce N*1024 elements in the output file.

	* Default parameter: If you run as "./traffic" without any parameters, the code will use 64 as the first parameter and 1 as the second parameter.
Thus, it will produce a traffic file with 1,024 elements without duplicates, and the longest element is 20 characters in length.

####2. sbf_linux: Including all the tests described in the evaluation section of the paper. (Performance test on many-core is in folder sbf_linux_tile_muticore)

`To run the specific test, you should comment out the corresponding test code in main().`

* Note:            Need to change the path in main.cpp first. 

	`The const variable "fNamePrefix" should be your own folder path which includes the traffic file.`

* Compile:         make

* Run:             ./sbf rand_20b_100M.tr(the name of the traffic file) 20(length of the element in traffic file) 100(multiply by 1,000,000 `M as a unit` in the program, so will query 100,000,000 elements)

* Default parameter: If you run as "./sbf" without any parameters, the code will run as "./sbf rand_20b_100M.tr 20 100" as described above.
                        
####3. sbf_linux_tile_muticore: Insert the Telera TLR4-03680 Card into the Linux PC and test the sbf's performance.
		    
* Note:           Need to change the path in main.cpp first.

	`The const variable "fNamePrefix" should be your own folder path which contains the traffic file.`

	`The const variable "fNamePrefixOut" should be your own folder path which will store the result file.`

* Compile:        make

* Run:            Use the script "./do.sh" to to the test. You can modify it depends on your specific needs.

* Default parameter:  Use the script to run the test, no need to use the command line by yourself.   

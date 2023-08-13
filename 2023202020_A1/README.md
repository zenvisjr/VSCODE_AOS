#Assignment 1:

my code is taking A.txt file as input through command line and then the output is saved in Assignment1_1/1_A.txt which I create in the code by appending A.txt file to "Assignment1_1/1_" using string functions
appropriate permission is given to the output file 1_A.txt (R,W) and the directory Assignment1_1 (R,W,X)
3.1st I'm checking if the outout directory exist using stat(), if no then I'm creating a new directory using mkdir() otherwise I'm just overwriting the output file.
I have used buffer of fixed size of 5kb
I'm using a reverse function (made by me) to reverse my file content by taking chunks of data in a buffer from end of file and appending the result in output file repeteadly.
The code is showing the % of file reversed and at 100% the file is reversed completely and stored in Assignment1_1/1_A.txt.

##Execution: 
g++ Q1.cpp
./a.out A.txt

###Problem
gedit is adding a new line at the end of input file A.txt so after reversing the file a blank line is inserted above in output file 1_A.txt


#Assignment 2:
my code is taking A.txt file along with 2 index n, m as input through command line and then the output is saved in Assignment1_2/2_A.txt which I create in the code by appending A.txt file to "Assignment1_2/2_" using string functions
appropriate permission is given to the output file 2_A.txt (R,W) and the directory Assignment1_2 (R,W,X)
1st I'm checking if the outout directory exist using stat(), if no then I'm creating a new directory mkdir() otherwise I'm just overwriting the output file.
I have used buffer of fixed size of 5kb
same reverse function is used to first reverse the file till n and after m till end of file
I'm storing the content from n to m without reversing in the output file 2_A.txt
percentage code is implemented which shows the progress of the operation on A.txt

#Execution:
g++ Q2.cpp
./a.out A.txt n m

##PRoblem:
same problem of extra line, when I reverse the input file A.txt after second pointer till end of file (which has a extra line) then in output file 2_A.txt the reversed content goes to next line after index m because it detect a new line at end of input file A.txt
output file size is 1 less than the input file even though the file is getting reversed in specific order as mentioned


#Assignment 3:
I'm am taking 3 argument, 1st is output file 2_A.txt, 2nd ibput file A.txt and 3rd directory Assignment1_1
Im using 2 different buffer to check if the content of outfile file is reverse of the input file, and using a flag variable to keep a track while reversing
Depending upon the value of the flag Im printing if the file is reversed  or not 
Im using opendir system call to check if directory is created
Im using stat() to find all the permission on the output input, input file and directory

##Execution:
g++ Q3.cpp
./a.out 1_A.txt A.txt Assignment1_1





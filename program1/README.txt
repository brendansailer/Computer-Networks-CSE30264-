In this directory, please find udpserver.c and udpclient.c along with File1, File2, and File3.txt 

To compile my code, please type 'make' and the Makefile will make the two executables.

Run the server on student02: ./udpserver 41042
Run the client with the following commands to test it on student00:
- ./udpclient student02.cse.nd.edu 41042 "This is a test."
- ./udpclient student02.cse.nd.edu 41042 File1.txt
- ./udpclient student02.cse.nd.edu 41042 File2.txt
- ./udpclient student02.cse.nd.edu 41042 File3.txt

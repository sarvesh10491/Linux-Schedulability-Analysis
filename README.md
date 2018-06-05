Schedulability Analysis 

   Following project is used to program various schedulability testing approaches for EDF, RM, and DM schedule algorithms and comparative analysis of the schedulability of EDF, RM and DM algorithms using synthetic tasks sets.

Getting Started :

    These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. 
    See deployment for notes on how to deploy the project on a live system.

Prerequisites :

  Linux kernel (preferably linux 2.6.19 and above)
  GNU (preferably gcc 4.5.0 and above)

Installing :

Unzip & download below files in user directory on your machine running linux distribution.

   1)task2.c
   2)scheduling.h
   3)Makefile
   4)new_input.txt
   5)Report_2


Deployment :

   Open the terminal & go to directory where files in installing part have been downloaded. [cd <Directory name>] 
   
   Use below command to to compile :
   For Linux Host -
	make
   
   Once above completed successfully, then run the below command
   to execute the program code
   
   cd /home
   ./task2


Expected results :

It asks for input choice to select execution to be done with manual taskset or synthetically generated taskset.

Entering the choice sets program in execution with subsequent taskset. 
In part 1, it prints out analysis report in terminal. 
In part 2, it generates 4 files in parent directory for each case for 5000 tasksets which is then used for plot.

Built With :

  Linux 4.10.0-28-generic
  x86_64 GNU/Linux
  64 bit x86 machine

Authors :

Sarvesh Patil 
Vishwakumar Doshi

License :

This project is licensed under the ASU License


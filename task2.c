//################################################################################################
//
// Program     : Real-time task scheduling algorithm analysis in Linux
// Source file : task2.c
// Authors     : Sarvesh Patil & Vishwakumar Doshi
// Date        : 3 March 2018
//
//################################################################################################


#include <stdio.h>
#include <stdlib.h> 
#include <math.h>
#include <time.h>

#include "scheduling.h"


//###########
// Main
//###########
int main()
{
	int choice=0;
	printf("Which Test you wish to run?\n");
	printf("----------------------------\n");
	printf("Enter 0 for manual taskset.\n");
	printf("Enter 1 for synthetically generated tasksets.\n\n");
	printf("Your choice : ");
	scanf("%d",&choice);

	if(choice == 0)
	{
		file_ops();
		printf("\n###############################\n\n");
		printf("EDF Analysis\n================\n");
		edf_test();
		printf("\n###############################\n\n");
		printf("RM Analysis\n================\n");
		rm_test();
		printf("\n###############################\n\n");
		printf("DM Analysis\n================\n");
		dm_test();
	}
	else if(choice == 1)
	{
		Taskset_generator(10,0);
		Taskset_generator(25,0);
		Taskset_generator(10,1);
		Taskset_generator(25,1);
	}
	else
		printf("Incorrect choice.\n");

	return 0;
}

//#####################
//
// End of task2.c
//
//#####################
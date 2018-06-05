CC=gcc
APP = task2

all:
	$(CC) -Wall -o $(APP) $(APP).c -lm 
clean:
	
	rm -f *.o
	rm -f $(APP)
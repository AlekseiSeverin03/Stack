stack.o: stack.cpp
	gcc -Wall -c stack.cpp

main.o: main.cpp
	gcc -Wall -c main.cpp

file_operations.o: file_operations.cpp
	gcc -Wall -c file_operations.cpp

yes: main.o stack.o file_operations.o
	gcc main.o stack.o file_operations.o -o yes
	rm *.o

all: yes

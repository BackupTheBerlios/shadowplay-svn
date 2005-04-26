all: main.o

main.o : Shadow.o VideoInput.o main.cpp
	c++ -o shadowplay Shadow.o VideoInput.o main.cpp

Shadow.o : Shadow.cpp Shadow.h 
	c++ -c Shadow.cpp

VideoInput.o : VideoInput.cpp VideoInput.h
	c++ -c VideoInput.cpp

clean :
	rm *.o

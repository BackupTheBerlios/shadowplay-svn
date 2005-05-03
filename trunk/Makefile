all: main.o

main.o : Shadow.o VideoInput.o GameController.o KeyInput.o main.cpp
	c++ -ggdb `sdl-config --cflags` `sdl-config --libs` -lGL -o shadowplay Shadow.o VideoInput.o KeyInput.o GameController.o main.cpp

GameController.o : GameController.cpp GameController.h KeyInput.o VideoInput.o
	c++ -ggdb `sdl-config --cflags` -I/usr/include/GL -c GameController.cpp

KeyInput.o : KeyInput.cpp KeyInput.h GameController.h
	c++ -ggdb `sdl-config --cflags` -c KeyInput.cpp

VideoInput.o : VideoInput.cpp VideoInput.h Shadow.o
	c++ -ggdb `sdl-config --cflags` -c VideoInput.cpp

Shadow.o : Shadow.cpp Shadow.h 
	c++ -ggdb `sdl-config --cflags` -c Shadow.cpp

clean :
	rm *.o

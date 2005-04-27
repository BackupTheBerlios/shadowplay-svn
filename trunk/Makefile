all: main.o

main.o : Shadow.o VideoInput.o GameController.o KeyInput.o main.cpp
	c++ `sdl-config --libs` -lGL -o shadowplay Shadow.o VideoInput.o KeyInput.o GameController.o main.cpp

GameController.o : GameController.cpp GameController.h KeyInput.o VideoInput.o
	c++ `sdl-config --cflags` -I/usr/include/GL -c GameController.cpp

KeyInput.o : KeyInput.cpp KeyInput.h
	c++ `sdl-config --cflags` -c KeyInput.cpp

VideoInput.o : VideoInput.cpp VideoInput.h Shadow.o
	c++ -c VideoInput.cpp

Shadow.o : Shadow.cpp Shadow.h 
	c++ -c Shadow.cpp

clean :
	rm *.o

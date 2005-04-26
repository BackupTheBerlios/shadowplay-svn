all: main.o

main.o : Shadow.o VideoInput.o Controller.o GameController.o main.cpp
	c++ `sdl-config --libs` -o shadowplay Shadow.o VideoInput.o Controller.o GameController.o main.cpp

Shadow.o : Shadow.cpp Shadow.h 
	c++ -c Shadow.cpp

VideoInput.o : VideoInput.cpp VideoInput.h
	c++ -c VideoInput.cpp

Controller.o : Controller.cpp Controller.h
	c++ -c Controller.cpp

GameController.o : GameController.cpp GameController.h
	c++ `sdl-config --cflags` -c GameController.cpp

clean :
	rm *.o

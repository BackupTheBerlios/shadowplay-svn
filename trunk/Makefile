COMPILE=c++ -O1 -ggdb `sdl-config --cflags`

all: main.o

main.o : Shadow.o VideoInput.o GameController.o KeyInput.o main.cpp
	$(COMPILE) `sdl-config --libs` -lGL -o shadowplay Shadow.o VideoInput.o KeyInput.o GameController.o main.cpp

GameController.o : GameController.cpp GameController.h KeyInput.o VideoInput.o
	$(COMPILE) -I/usr/include/GL -c GameController.cpp

KeyInput.o : KeyInput.cpp KeyInput.h GameController.h
	$(COMPILE) -c KeyInput.cpp

VideoInput.o : VideoInput.cpp VideoInput.h Shadow.o
	$(COMPILE) -c VideoInput.cpp

Shadow.o : Shadow.cpp Shadow.h 
	$(COMPILE) -c Shadow.cpp

clean :
	rm *.o

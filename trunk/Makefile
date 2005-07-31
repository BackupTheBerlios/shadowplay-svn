#COMPILE=c++ -ggdb -O3 -mmmx -msse -m3dnow -finline-limit=2000 -frerun-loop-opt -march=athlon-xp `sdl-config --cflags`
COMPILE=c++ -O3 `sdl-config --cflags`

all: main.o Makefile

main.o : Shadow.o VideoInput.o GameController.o Sand.o Cover.o SquishMaze.o KeyInput.o main.cpp
	$(COMPILE) `sdl-config --libs` -L/usr/X11R6/lib/ -lGL -lGLU -o shadowplay Shadow.o VideoInput.o KeyInput.o GameController.o Sand.o Cover.o SquishMaze.o main.cpp

SquishMaze.o : SquishMaze.cpp SquishMaze.h GameController.h KeyInput.o VideoInput.o
	$(COMPILE) -I/usr/include/GL -c SquishMaze.cpp

Cover.o : Cover.cpp Cover.h GameController.h KeyInput.o VideoInput.o
	$(COMPILE) -I/usr/include/GL -c Cover.cpp

Sand.o : Sand.cpp Sand.h GameController.h KeyInput.o VideoInput.o
	$(COMPILE) -I/usr/include/GL -c Sand.cpp

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

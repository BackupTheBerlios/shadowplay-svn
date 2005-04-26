#include <cstdio>
#include <cstdlib>
#include <cerrno>

#include <unistd.h>

#include "Shadow.h"
#include "GameController.h"

#include "SDL.h"

#include <iostream>
using namespace std;


GameController::GameController(void)
{
	Initialize();
}

GameController::~GameController(void)
{
}

int GameController::Initialize(void)
{
    if( SDL_Init(SDL_INIT_VIDEO) < 0 ) { 
        cout << "Could not initialize SDL: " << SDL_GetError() << endl;
        return -1; 
    }

	atexit(SDL_Quit);

	SDL_Surface* screen;
	screen = SDL_SetVideoMode(800, 600, 16, SDL_HWSURFACE|SDL_ANYFORMAT);
	if( !screen ) {
		cout << "Couldn't create a surface: " << SDL_GetError() << endl;
		return -1;
	}	
	while (true)
	{
	}

}


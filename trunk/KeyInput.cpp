#include <cstdio>
#include <cstdlib>
#include <cerrno>

#include <unistd.h>

#include "GameController.h"
#include "KeyInput.h"

#include "SDL.h"

#include <iostream>
using namespace std;

KeyInput::KeyInput(GameController *gc)
{
	gamecontroller = gc;
}

KeyInput::~KeyInput(void)
{
}

int KeyInput::CheckKeys(void)
{
	SDL_Event event;

	while(SDL_PollEvent(&event)) {
		switch(event.type){
			case SDL_KEYDOWN:
			case SDL_KEYUP:
				break;
			case SDL_QUIT:
				cout << "KeyInput: Got a quit command\n";
				gamecontroller->StopPlaying();
				break;
		}
	}
}

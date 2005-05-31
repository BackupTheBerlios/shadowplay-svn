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

void KeyInput::StartKeyInput(void)
{
	playing = true;
	cout << "KeyInput: Trying to create keyinput thread\n";
	thread = SDL_CreateThread(KeyInputThread,
			static_cast<void *>(this));
	if (thread == NULL)
	{
		cout << "KeyInput: Unable to create thread: ";
		cout << SDL_GetError() << endl;
	}
}

void KeyInput::StopKeyInput(void)
{
	playing = false;
	SDL_WaitThread(thread, NULL);
}

	extern "C"
int KeyInputThread(void *ki)
{
	static_cast<KeyInput *>(ki)->KeyLoop();
}

void KeyInput::KeyLoop(void)
{
	nice(10);

	SDL_Event event;

	while (playing)
	{
		usleep(50);
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYDOWN:
					switch (event.key.keysym.sym)
					{
						case SDLK_EQUALS:
							gamecontroller->HandleKey(KEY_THRESH_UP);
							break;
						case SDLK_MINUS:
							gamecontroller->HandleKey(KEY_THRESH_DOWN);
							break;
						case SDLK_i:
							gamecontroller->HandleKey(KEY_VIDEO);
							break;
						case SDLK_ESCAPE:
							cout << "KeyInput: Got a quit command\n";
							gamecontroller->StopPlaying();
							break;
					}
					break;
				case SDL_QUIT:
					cout << "KeyInput: Got a quit command\n";
					gamecontroller->StopPlaying();
					break;
			}
		}
	}
}

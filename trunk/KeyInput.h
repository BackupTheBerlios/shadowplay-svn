#ifndef KEYINPUT_H
#define KEYINPUT_H

#include "GameController.h"
#include "SDL.h"
#include "SDL_thread.h"

class GameController;

extern "C" int KeyInputThread(void *);

class KeyInput
{
  public:
	KeyInput(GameController *gc);
	~KeyInput(void);

	void StartKeyInput(void);
	void StopKeyInput(void);

	void KeyLoop(void);

  private:
	GameController *gamecontroller;

	SDL_Thread *thread;

	bool playing;
};

#endif

#ifndef KEYINPUT_H
#define KEYINPUT_H

#include "GameController.h"

class GameController;

class KeyInput
{
  public:
	KeyInput(GameController *gc);
	~KeyInput(void);

	int CheckKeys(void);

  private:
	GameController *gamecontroller;

  protected:
};

#endif

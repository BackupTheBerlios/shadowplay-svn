#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "VideoInput.h"
#include "Shadow.h"
#include "KeyInput.h"

class GameController
{
  public:
	GameController(void);
	~GameController(void);

  private:
	int InitializeVideoOut(void);
	bool Draw(void);
	
	bool running;
	
	KeyInput *keyinput;
	Shadow *shadow;
	
	vidbuffertype *videobuffer;

  protected:
};

#endif

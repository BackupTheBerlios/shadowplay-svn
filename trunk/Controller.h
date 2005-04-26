#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "Shadow.h"
#include "GameController.h"

class Controller
{
  public:
	Controller(void);
	~Controller(void);

  private:
	Shadow *shadow;
	GameController *gamecontroller;

  protected:
};

#endif

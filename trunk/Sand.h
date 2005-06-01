#ifndef SAND_H
#define SAND_H

#include "GameController.h"
#include "VideoInput.h"
#include "Shadow.h"
#include "KeyInput.h"
#include "GL/gl.h"

class Sand : public GameController
{
  public:
	Sand(void);
	~Sand(void);

	bool Draw(void);

  private:
};

#endif

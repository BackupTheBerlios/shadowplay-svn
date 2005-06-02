#ifndef SAND_H
#define SAND_H

#include "GameController.h"
#include "VideoInput.h"
#include "Shadow.h"
#include "KeyInput.h"
#include "GL/gl.h"
#include "GL/glu.h"

struct sandtype
{
	float x, y;
	float vx, vy;
	float ax, ay;

	float r;
	float cr, cg, cb ;
};

class Sand : public GameController
{
  public:
	Sand(void);
	~Sand(void);

	bool Draw(void);

  private:
	sandtype *sand;

	int n;

	int tick, lastTick;
	float dt;
	GLUquadricObj *quadratic;
};

#endif

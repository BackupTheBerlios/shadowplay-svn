#ifndef SAND_H
#define SAND_H

#include <vector>

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

	float m, mi;
	float d;
};

class Sand : public GameController
{
  public:
	Sand(void);
	~Sand(void);

	bool Draw(void);

  private:
	sandtype *sand;
	vector<int> *location; // Subdivides the screen for faster collisions
	int locw, loch;

	int tick, lastTick;
	float dt;
	GLUquadricObj *quadratic;

};

#endif

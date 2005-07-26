#ifndef COVER_H
#define COVER_H

#include "GameController.h"
#include "VideoInput.h"
#include "Shadow.h"
#include "KeyInput.h"
#include "GL/gl.h"
#include "GL/glu.h"

struct circletype
{
	float x, y;
	float vx, vy;

	float r;
	float cr, cg, cb;

	bool covered;
	int player;
};

class Cover : public GameController
{
  public:
	Cover(void);
	~Cover(void);

	bool Draw(void);

  private:
	circletype *circle;
	float score;
	float scoreinc;

	int tick, lastTick;
	float dt;
	GLUquadricObj *quadratic;
};

#endif

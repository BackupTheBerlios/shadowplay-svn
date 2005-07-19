#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "VideoInput.h"
#include "Shadow.h"
#include "KeyInput.h"
#include "GL/gl.h"

struct windowtype
{
	float left, right, top, bottom, front, back;
};

class KeyInput;
class Shadow;

class GameController
{
  public:
	GameController(void);
	~GameController(void);

	virtual void StartPlaying(void);
	virtual void StopPlaying(void);

	virtual void HandleKey(int);

  private:

  protected:
	virtual int InitializeVideoOut(void);
	
	virtual bool Draw(void);

	bool running;

	bool showcube;
	float angle;

	windowtype window;
	
	KeyInput *keyinput;
	Shadow *shadow;
	
	vidbuffertype *videobuffer;
	vidbuffertype *shadowbuffer;

	int displaytype;

	int out_w;
	int out_h;

	int tex_w;
	int tex_h;
	GLfloat tex_param_s;
	GLfloat tex_param_t;
	GLfloat aspect_ratio;
	GLuint cube_list;
	GLuint texture;
	uint8_t *teximage;

    GLdouble *tex_mat;

	int frames;
	int fpsTicks;
	float fps;
	bool showfps;
};

#endif

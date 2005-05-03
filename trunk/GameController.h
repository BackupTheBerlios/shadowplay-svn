#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "VideoInput.h"
#include "Shadow.h"
#include "KeyInput.h"
#include "GL/gl.h"

class KeyInput;
class Shadow;

class GameController
{
  public:
	GameController(void);
	~GameController(void);

	void StartPlaying(void);
	void StopPlaying(void);

  private:
	int InitializeVideoOut(void);
	bool Draw(void);
	
	bool running;
	
	KeyInput *keyinput;
	Shadow *shadow;
	
	vidbuffertype *videobuffer;

	int out_w;
	int out_h;

	int tex_w;
	int tex_h;
	GLfloat tex_param_s;
	GLfloat tex_param_t;
	GLfloat aspect_ratio;
	GLuint cube_list;
	GLuint texture;
	uint8_t *ui_image_copy;

    GLdouble *tex_mat;

  protected:
};

#endif

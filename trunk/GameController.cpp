#include <cstdio>
#include <cstdlib>
#include <cerrno>

#include <unistd.h>

#include "Shadow.h"
#include "VideoInput.h"
#include "GameController.h"
#include "KeyInput.h"

#include "SDL.h"
#include "GL/gl.h"

#include <iostream>
using namespace std;


GameController::GameController(void)
{
	cout << "GameController: Creating the Shadow object\n";
	shadow = new Shadow(640, 480);

	videobuffer = shadow->GetBuffer();

	out_w = 640;
	out_h = 480;
	tex_w = 256;
	tex_h = 256;
	tex_param_s = 1.0f;
	tex_param_t = 1.0f;
	tex_mat = 0;

	cout << "GameController: Initializing SDL and openGL\n";
	InitializeVideoOut();

	cout << "GameController: Creating the KeyInput object\n";
	keyinput = new KeyInput(this);
}

GameController::~GameController(void)
{
	delete ui_image_copy;
	delete tex_mat;
	delete shadow;
}

void GameController::StartPlaying(void)
{
    running = true;
	shadow->StartPlaying();

	cout << "GameController: Entering the event loop\n";
	while (running)
	{
		Draw();
		keyinput->CheckKeys();
	}
	
	shadow->StopPlaying();
}

void GameController::StopPlaying(void)
{
		running = false;
}

int GameController::InitializeVideoOut(void)
{
	cout << "GameController: Setting up the output video texture\n";

    if (videobuffer->w > 256 && videobuffer->w <= 512)
      tex_w = 512;
    else if (videobuffer->w > 512 && videobuffer->w <= 1024)
      tex_w = 1024;
    else if (videobuffer->w > 1024)
      tex_w = 2048;

    if (videobuffer->h > 256 && videobuffer->h <= 512)
      tex_h = 512;
    else if (videobuffer->h > 512 && videobuffer->h <= 1024)
      tex_h = 1024;
    else if (videobuffer->h > 1024)
      tex_h = 2048;

	cout << "GameController: Input size: " << videobuffer->w << "x" << videobuffer->h << endl;
	cout << "GameController: Output size: " << out_w << "x" << out_h << endl;
	cout << "GameController: Texture size: " << tex_w << "x" << tex_h << endl;

    tex_param_s = static_cast<GLfloat>(videobuffer->w)  / static_cast<GLfloat>(tex_w);
    tex_param_t = static_cast<GLfloat>(videobuffer->h) / static_cast<GLfloat>(tex_h);

    ui_image_copy = new uint8_t[tex_w*tex_h*sizeof(uint8_t)];

	// Create a texture matrix that will make the video scale properly to the size of the texture
    tex_mat = new GLdouble[16];
    for (int i=0; i<16; ++i)
		tex_mat[i] = 0.0;
    tex_mat[0]  = tex_param_s;
    tex_mat[5]  = tex_param_t;
    tex_mat[10] = tex_mat[15] = 1.0;


	SDL_Surface* screen;

    if( SDL_Init(SDL_INIT_VIDEO) < 0 )
	{ 
        cout << "GameController: Could not initialize SDL: " << SDL_GetError() << endl;
        return -1; 
    }

	atexit(SDL_Quit);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	int flags = SDL_OPENGLBLIT;

	screen = SDL_SetVideoMode(out_w, out_h, 16, flags);
	if( !screen )
	{
		cout << "GameController: Couldn't create a surface: " << SDL_GetError() << endl;
		return -1;
	}	

	glClearColor(1.0f,1.0f,1.0f,1.0f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);	
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_CULL_FACE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glGenTextures(1, &texture);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-320.0f, 320.0f, 240.0f, -240.0f, -340.0f, 340.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	cube_list = glGenLists(1);
	glNewList(cube_list, GL_COMPILE_AND_EXECUTE);
	glBegin(GL_QUADS);
		// Front Face
		glNormal3f(0.0f, 0.0f, 0.5f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		// Back Face
		glNormal3f(0.0f, 0.0f,-0.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		// Top Face
		glNormal3f( 0.0f, 0.5f, 0.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		// Bottom Face
		glNormal3f( 0.0f,-0.5f, 0.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		// Right Face
		glNormal3f( 0.5f, 0.0f, 0.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		// Left Face
		glNormal3f(-0.5f, 0.0f, 0.0f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f, -1.0f);
	glEnd();
	glEndList();

	//Set texture transform matrix to alter (here only to scale) texture coordinates
	glMatrixMode(GL_TEXTURE);
	glLoadMatrixd(tex_mat);
	glMatrixMode(GL_MODELVIEW);
}

bool GameController::Draw(void)
{
    for (int j=0; j < videobuffer->h; ++j)
        memcpy(&ui_image_copy[j*tex_w], &videobuffer->buffer[j*videobuffer->w], videobuffer->w);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, tex_w, tex_h, 0,
                 GL_LUMINANCE, GL_UNSIGNED_BYTE, ui_image_copy);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glBegin(GL_QUADS);
		glTexCoord2f(1, 1);
		glVertex3f( 320.0f,  240.0f, -320.0f);
		glTexCoord2f(0, 1);
		glVertex3f(-320.0f,  240.0f, -320.0f);
		glTexCoord2f(0, 0);
		glVertex3f(-320.0f, -240.0f, -320.0f);
		glTexCoord2f(1, 0);
		glVertex3f( 320.0f, -240.0f, -320.0f);
	glEnd();

	SDL_GL_SwapBuffers();

	return true;
}


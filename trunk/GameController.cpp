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

	out_w = 800;
	out_h = 600;
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

void GameController::StartPlaying(void)
{
    running = true;

	cout << "GameController: Entering the event loop\n";
	while (running)
	{
		Draw();
		keyinput->CheckKeys();
	}
	
	shadow->StopPlaying();
}

GameController::~GameController(void)
{
	delete ui_image_copy;
	delete tex_mat;
	delete shadow;
}

int GameController::InitializeVideoOut(void)
{
	cout << "GameController: Setting up the output video texture\n";

    // adjust texture to video file (256 is standard (see declaration above))
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
    tex_param_t = static_cast<GLfloat>(videobuffer->w) / static_cast<GLfloat>(tex_h);

    ui_image_copy = new uint8_t[tex_w*tex_h*sizeof(uint8_t)];

    /* create texture transform matrix (avi file must not be 2^n by 2^m but a texture has to be) */
    tex_mat = new GLdouble[16];
    for (int i=0; i<16; ++i) tex_mat[i] = 0.0;
    tex_mat[0]  = tex_param_s;
    tex_mat[5]  = tex_param_t;
    tex_mat[10] = tex_mat[15] = 1.0;


	SDL_Surface* screen;

    if( SDL_Init(SDL_INIT_VIDEO) < 0 ) { 
        cout << "GameController: Could not initialize SDL: " << SDL_GetError() << endl;
        return -1; 
    }

	atexit(SDL_Quit);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);		//Use at least 5 bits of Red
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);	//Use at least 5 bits of Green
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);		//Use at least 5 bits of Blue
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);	//Use at least 16 bits for the depth buffer
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);	//Enable double buffering

	int flags = SDL_OPENGLBLIT;

	screen = SDL_SetVideoMode(800, 600, 16, flags);
	if( !screen ) {
		cout << "GameController: Couldn't create a surface: " << SDL_GetError() << endl;
		return -1;
	}	

	glViewport(0,0,800,600);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0.0f, 800, 600, 0.0f, -1.0f, 1.0f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.0f,0.0f,0.0f,0.5f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);	
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glDisable(GL_CULL_FACE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	glGenTextures(1, &texture);               // generate OpenGL texture object
	glEnable(GL_TEXTURE_2D);
	glBindTexture( GL_TEXTURE_2D, texture );  // use previously created texture object and set options
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);  // how to set texture coords
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);

	// display lists are much faster than drawing directly
	cube_list = glGenLists(1); // generate display list
	glNewList(cube_list, GL_COMPILE_AND_EXECUTE);  // fill display list
	glBegin(GL_QUADS);										// Begin Drawing A Cube
		// Front Face
		glNormal3f( 0.0f, 0.0f, 0.5f);
		glTexCoord2f(0.0f, 0.0f); glVertex3f(-1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 0.0f); glVertex3f( 1.0f, -1.0f,  1.0f);
		glTexCoord2f(1.0f, 1.0f); glVertex3f( 1.0f,  1.0f,  1.0f);
		glTexCoord2f(0.0f, 1.0f); glVertex3f(-1.0f,  1.0f,  1.0f);
		// Back Face
		glNormal3f( 0.0f, 0.0f,-0.5f);
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
	glEnd();												// Done Drawing Our Cube
	glEndList();

	// set texture transform matrix to alter (here only to scale) texture coordinates
	// (our video stream must not be 2^n by 2^m but our texture has to be)
	glMatrixMode(GL_TEXTURE);
	glLoadMatrixd(tex_mat);
	glMatrixMode(GL_MODELVIEW);
}

bool GameController::Draw(void)
{
	//Make certain everything is cleared from the screen before we draw to it
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();	     // Reset The Modelview Matrix
	glCallList(cube_list);   // Draw cube via display list

	//Flip the backbuffer to the primary
	SDL_GL_SwapBuffers();

	return true;
}

void GameController::StopPlaying(void)
{
	running = false;
}

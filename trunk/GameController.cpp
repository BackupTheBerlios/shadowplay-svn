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
	cout << "Initializing SDL and openGL\n";
	InitializeVideoOut();

	cout << "Creating the Shadow object\n";
	shadow = new Shadow();

	videobuffer = shadow->GetBuffer();

	cout << "Creating the KeyInput object\n";
	keyinput = new KeyInput();

	Draw();

    running = true;

	cout << "Entering the event loop\n";
	while (running)
	{
		keyinput->CheckKeys();
		running = keyinput->Running();
	}
}

GameController::~GameController(void)
{
}

int GameController::InitializeVideoOut(void)
{
	SDL_Surface* screen;

    if( SDL_Init(SDL_INIT_VIDEO) < 0 ) { 
        cout << "Could not initialize SDL: " << SDL_GetError() << endl;
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
		cout << "Couldn't create a surface: " << SDL_GetError() << endl;
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
}

bool GameController::Draw(void)
{
    //glBindTexture(GL_TEXTURE_2D, image);

    //Generate the texture
    //glTexImage2D(GL_TEXTURE_2D, 0, 3, temp->w, temp->h, 0, GL_BGR, GL_UNSIGNED_BYTE, temp->pixels);

    //Use linear filtering, very good
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    //Make certain everything is cleared from the screen before we draw to it
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Enable texturing
    //glEnable(GL_TEXTURE_2D);

    //Load the texture
    //glBindTexture(GL_TEXTURE_2D, image);

    glBegin(GL_QUADS);
        //Top-left vertex (corner)
        //glTexCoord2f(0,0);
        glVertex3f(0.0f, 0.0f, 0.0f);

        //Bottom-left vertex (corner)
        //glTexCoord2f(1,0);
        glVertex3f(600.0f, 0.0f, 0.0f);

        //Bottom-right vertex (corner)
        //glTexCoord2f(1,1);
        glVertex3f(600.0f, 800.0f, 0.0f);

        //Top-right vertex (corner)
        //glTexCoord2f(0,1);
        glVertex3f(800.0f, 0.0f, 0.0f);
    glEnd();

    //Disable texturing
    //glDisable(GL_TEXTURE_2D);

    //Flush the OpenGL pipeline
    glFlush();

    //Flip the backbuffer to the primary
    SDL_GL_SwapBuffers();

	return true;
}


#include <cstdio>
#include <cstdlib>
#include <cerrno>

#include <unistd.h>

#include "Shadow.h"
#include "GameController.h"

#include "SDL.h"
#include "GL/gl.h"

#include <iostream>
using namespace std;


GameController::GameController(void)
{
	Initialize();
}

GameController::~GameController(void)
{
}

int GameController::Initialize(void)
{
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

	SDL_Surface* screen;
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

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBegin(GL_QUADS);
		glVertex3f(100.0f, 100.0f, 0.0f);
		glVertex3f(228.0f, 100.0f, 0.0f);
		glVertex3f(228.0f, 228.0f, 0.0f);
		glVertex3f(100.0f, 228.0f, 0.0f);
	glEnd();

	SDL_Event event;
	bool running = true;

	while(running) {
		while(SDL_PollEvent(&event)) {
			switch(event.type){
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					break;
				case SDL_QUIT:
					running = false;
					break;
			}
		}
	}
}


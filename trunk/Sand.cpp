#include <iostream>
#include <math.h>

#include "Sand.h"

using namespace std;

Sand::Sand(void)
{
	srand(time(0));

	n = 100;
	sand = new sandtype [n];
	for (int i = 0; i < n; i++)
	{
		sand[i].x =rand()/(float)RAND_MAX*(RIGHT-LEFT)-(RIGHT-LEFT)/2;
		sand[i].y =rand()/(float)RAND_MAX*(TOP-BOTTOM)-(TOP-BOTTOM)/2;
		sand[i].vx = rand()/(float)RAND_MAX*50.0f-25.0f;
		sand[i].vy = rand()/(float)RAND_MAX*50.0f-25.0f;
		sand[i].ax = 0;
		sand[i].ay = -10;
		sand[i].r = 5;
		sand[i].cr = rand()/(float)RAND_MAX;
		sand[i].cg = rand()/(float)RAND_MAX;
		sand[i].cb = rand()/(float)RAND_MAX;
	}

	quadratic = gluNewQuadric();
	gluQuadricNormals(quadratic, GLU_SMOOTH);
	gluQuadricTexture(quadratic, GL_TRUE);
}

Sand::~Sand(void)
{
}

bool Sand::Draw(void)
{
	tick = SDL_GetTicks();
	dt = (float)(tick-lastTick)/100;
	lastTick = tick;

	if (displaytype == 0)
		for (int j=0; j < videobuffer->h; ++j)
			memcpy(&teximage[j*tex_w], &videobuffer->buffer[j*videobuffer->w], videobuffer->w);
	else
		for (int j=0; j < shadowbuffer->h; ++j)
			memcpy(&teximage[j*tex_w], &shadowbuffer->buffer[j*shadowbuffer->w], shadowbuffer->w);


	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glDisable(GL_TEXTURE_2D);
	for (int i = 0; i < n; i++)
	{
		sand[i].x += sand[i].vx*dt + 0.5f*sand[i].ax*dt*dt;
		sand[i].y += sand[i].vy*dt + 0.5f*sand[i].ay*dt*dt;
		sand[i].vx += sand[i].ax*dt;
		sand[i].vy += sand[i].ay*dt;

		if (sand[i].x > RIGHT || sand[i].x < LEFT ||
			sand[i].y > TOP + 30 || sand[i].y < BOTTOM)
		{
			sand[i].x = rand()/(float)RAND_MAX*RIGHT-RIGHT/2.0f;
			sand[i].y = TOP + 10;
			sand[i].vx = rand()/(float)RAND_MAX*10.0f-5.0f;
			sand[i].vy = rand()/(float)RAND_MAX*5.0f;
			sand[i].ax = 0;
			sand[i].ay = -10;
			sand[i].r = 5;
			sand[i].cr = rand()/(float)RAND_MAX;
			sand[i].cg = rand()/(float)RAND_MAX;
			sand[i].cb = rand()/(float)RAND_MAX;
		}

		glLoadIdentity();

		glTranslatef(sand[i].x, sand[i].y, 1.0f);

		glColor3f(sand[i].cr, sand[i].cg, sand[i].cb);

		gluDisk(quadratic, 0.0f, sand[i].r, 8, 2);
		/*
		   glBegin(GL_QUADS);
		   glVertex3f(sand[i].r, sand[i].r, 0.0f);
		   glVertex3f(-sand[i].r, sand[i].r, 0.0f);
		   glVertex3f(-sand[i].r, -sand[i].r, 0.0f);
		   glVertex3f(sand[i].r, -sand[i].r, 0.0f);
		   glEnd();
		 */
	}

	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, tex_w, tex_h, 0,
			GL_LUMINANCE, GL_UNSIGNED_BYTE, teximage);

	glLoadIdentity();

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(RIGHT, TOP, BACK);
		glTexCoord2f(1, 0);
		glVertex3f(LEFT, TOP, BACK);
		glTexCoord2f(1, 1);
		glVertex3f(LEFT, BOTTOM, BACK);
		glTexCoord2f(0, 1);
		glVertex3f(RIGHT, BOTTOM, BACK);
	glEnd();

	SDL_GL_SwapBuffers();

	return true;
}


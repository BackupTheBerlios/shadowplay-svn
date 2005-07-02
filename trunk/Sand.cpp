#include <iostream>
#include <math.h>

#include "Sand.h"

#define N 300
#define R 3

using namespace std;

Sand::Sand(void)
{
	srand(time(0));

	sand = new sandtype [N];
	for (int i = 0; i < N; i++)
	{
		sand[i].x =rand()/(float)RAND_MAX*(RIGHT-LEFT)-(RIGHT-LEFT)/2;
		sand[i].y =rand()/(float)RAND_MAX*(TOP-BOTTOM)-(TOP-BOTTOM)/2;
		sand[i].vx = rand()/(float)RAND_MAX*50.0f-25.0f;
		sand[i].vy = rand()/(float)RAND_MAX*50.0f-25.0f;
		sand[i].ax = 0;
		sand[i].ay = -10;

		sand[i].r = R;
		sand[i].m = 10;
		sand[i].d = 0.95f;

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

inline bool Sand::Draw(void)
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

	int j, posx, posy;
	float dx, dy, len, ux, uy;
	for (int i = 0; i < N; i++)
	{
		sandtype &s = sand[i];
	
		posx = shadowbuffer->w -
			(int)((s.x-LEFT)/(RIGHT-LEFT)*shadowbuffer->w) - 1;
		posy = shadowbuffer->h -
			(int)((s.y-BOTTOM)/(TOP-BOTTOM)*shadowbuffer->h) - 1;

		// Check if it's on the screen
		if (posx < shadowbuffer->w && posy < shadowbuffer->h &&
				posx >= 0 && posy >= 0)
		{
			// Check if it's in a shadow
			if (shadowbuffer->buffer[posx+posy*shadowbuffer->w] > 0)
			{
				s.x += s.vx*dt + 0.5f*s.ax*dt*dt;
				s.y += s.vy*dt + 0.5f*s.ay*dt*dt;
				s.vx += s.ax*dt;
				s.vy += s.ay*dt;
			}
			else
			{
				s.vx = 0;
				s.vy = 0;
			}
		}
		else
		{
			s.x += s.vx*dt + 0.5f*s.ax*dt*dt;
			s.y += s.vy*dt + 0.5f*s.ay*dt*dt;
			s.vx += s.ax*dt;
			s.vy += s.ay*dt;
		}
			

		if (s.x >= RIGHT - s.r ||
				s.x <= LEFT + s.r)
			s.vx = -1.0f*s.vx*s.d;

		if (s.y < BOTTOM - s.r ||
				s.y > TOP + 4*s.r)
		{
			s.x = rand()/(float)RAND_MAX*RIGHT-RIGHT/2.0f;
			s.y = TOP + R;
			s.vx = rand()/(float)RAND_MAX*80.0f-40.0f;
			s.vy = rand()/(float)RAND_MAX*-20.0f;
		}

		// Loop through for collision between sand
		for (j = 0; j < N; j++)
		{
			if (powf(s.x-sand[j].x, 2) + powf(s.y-sand[j].y, 2) <= 
					powf(s.r + sand[j].r, 2))
			{
				dx = s.x - sand[j].x;
				dy = s.y - sand[j].y;
				if (s.vx*dx+s.vy*dy - sand[j].vx*dx+sand[j].vy*dy > 0)
				{
					len = sqrt(pow(dx,2)+pow(dy,2));
					ux = dx/len;
					uy = dy/len;
				}
			}
		}

		glLoadIdentity();

		glTranslatef(s.x, s.y, 1.0f);

		glColor3f(s.cr, s.cg, s.cb);

		gluDisk(quadratic, 0.0f, s.r, 10, 1);
		/*
		   glBegin(GL_QUADS);
		   glVertex3f(s.r, s.r, 0.0f);
		   glVertex3f(-s.r, s.r, 0.0f);
		   glVertex3f(-s.r, -s.r, 0.0f);
		   glVertex3f(s.r, -s.r, 0.0f);
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


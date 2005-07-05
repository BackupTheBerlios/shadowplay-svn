#include <iostream>
#include <math.h>

#include "Sand.h"

#define N 50
#define R 20

using namespace std;

Sand::Sand()
{
	srand(time(0));

	sand = new sandtype [N];
	for (int i = 0; i < N; i++)
	{
		sand[i].x = rand()/(float)RAND_MAX*(RIGHT-LEFT)-(RIGHT-LEFT)/2;
		sand[i].y = rand()/(float)RAND_MAX*(TOP-BOTTOM)-(TOP-BOTTOM)/2;
		sand[i].vx = rand()/(float)RAND_MAX*50.0f-25.0f;
		sand[i].vy = rand()/(float)RAND_MAX*50.0f-25.0f;
		sand[i].ax = 0;
		sand[i].ay = -10;

		sand[i].r = R;
		sand[i].m = 10;
		sand[i].mi = 1.0f/sand[i].m;
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

	int j, x, y, posxl, posxr, posyl, posyr, check;
	float dx, dy, distance, ax, ay, l2, va1, vb1, va2, vb2, vaP1, vaP2;

	int i = 0;
	do
	{
		sandtype &s = sand[i];
	
		posxl = shadowbuffer->w - (int)((s.x+s.r-LEFT)/(RIGHT-LEFT)*shadowbuffer->w) - 1;
		posxr = shadowbuffer->w - (int)((s.x-s.r-LEFT)/(RIGHT-LEFT)*shadowbuffer->w) - 1;
		posyl = shadowbuffer->h - (int)((s.y+s.r-BOTTOM)/(TOP-BOTTOM)*shadowbuffer->h) - 1;
		posyr = shadowbuffer->h - (int)((s.y-s.r-BOTTOM)/(TOP-BOTTOM)*shadowbuffer->h) - 1;

		// Check if it's on the screen
		if (posxr < shadowbuffer->w && posyr < shadowbuffer->h && posxl >= 0 && posyl >= 0)
		{
			// Check if it's in a shadow
			check = 0;
			for (x = posxl; x <= posxr; x++)
				for (y = posyl; y <= posyr; y++)
				{
					if (shadowbuffer->buffer[x+y*shadowbuffer->w] < 100)
						check++;
				}

			if (check < 4)
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
			

		if (s.x >= RIGHT - s.r || s.x <= LEFT + s.r)
			s.vx = -1.0f*s.vx*s.d;

		if (s.y < BOTTOM - s.r || s.y > TOP + 4*s.r)
		{
			s.x = rand()/(float)RAND_MAX*RIGHT-RIGHT/2.0f;
			s.y = TOP + R;
			s.vx = rand()/(float)RAND_MAX*80.0f-40.0f;
			s.vy = rand()/(float)RAND_MAX*-20.0f;
		}

		// Loop through for collision between sand
		j = 0;
		do
		{
			sandtype &s2 = sand[j];

			dx = s2.x-s.x;
			dy = s2.y-s.y;

			l2 = dx*dx + dy*dy;
			if (l2 > powf(s.r + s2.r, 2) || l2 <= 0)
				goto Next;

			distance = sqrt(l2);

			ax = dx/distance;
			ay = dy/distance;

			// Projection of the velocities in these axes
			va1 = s.vx*ax + s.vy*ay;
			va2 = s2.vx*ax + s2.vy*ay;

			if (va2 - va1 >= 0)
				goto Next;

			vb1 = s.vy*ax - s.vx*ay;
			vb2 = s2.vy*ax - s2.vx*ay;

			// New velocities in these axes (after collision)
			vaP1 = va1 + (1+s.d)*(va2-va1)/(1+s.m*s2.mi);
			vaP2 = va2 + (1+s.d)*(va1-va2)/(1+s2.m*s.mi);

			// Undo the projections
			s.vx = vaP1*ax - vb1*ay;
			s.vy = vaP1*ay + vb1*ax;
			s2.vx = vaP2*ax - vb2*ay;
			s2.vy = vaP2*ay + vb2*ax;

			s2.x = s.x + (s.r+s2.r)*ax;
			s2.y = s.y + (s.r+s2.r)*ay;

Next:
			j++;
		} while (j < N);

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

		i++;
	} while (i < N);


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


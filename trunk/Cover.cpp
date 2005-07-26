#include <iostream>
#include <math.h>

#include "Cover.h"

#define N 4
#define R 10

using namespace std;

Cover::Cover()
{
	srand(time(0));

	circle = new circletype [N];
	for (int i = 0; i < N; i++)
	{
		circle[i].x = rand()/(float)RAND_MAX*(window.right-window.left)+window.left;
		circle[i].y = rand()/(float)RAND_MAX*(window.top-window.bottom)+window.bottom;
		float a = rand()/(float)RAND_MAX*3.14159*2;
		circle[i].vx = 5*sin(a);
		circle[i].vy = 5*cos(a);

		circle[i].r = R;

		circle[i].covered = false;

		if (i < N/2)
			circle[i].player = 0;
		else
			circle[i].player = 1;

		if (circle[i].player == 0)
		{
			circle[i].cr = 1.0f;
			circle[i].cg = 0;
			circle[i].cb = 0;
		}
		else
		{
			circle[i].cr = 0;
			circle[i].cg = 0;
			circle[i].cb = 1.0f;
		}
	}

	score = 0;
	scoreinc = .02;

	quadratic = gluNewQuadric();
	gluQuadricNormals(quadratic, GLU_SMOOTH);
	gluQuadricTexture(quadratic, GL_TRUE);
}

Cover::~Cover(void)
{
}

inline bool Cover::Draw(void)
{
	float ax, ay, va1, va2, vb1, vb2, d;
	int i, j, k, c, p1, p2;

	tick = SDL_GetTicks();
	dt = (float)(tick-lastTick)/100;
	lastTick = tick;

	SDL_Delay(10);

	for (i = 0; i < N; i++)
	{
		circle[i].x += circle[i].vx*dt;
		circle[i].y += circle[i].vy*dt;
		
		// bounce it off the walls
		if (circle[i].x >= window.right - circle[i].r)
		{
			circle[i].x = window.right - circle[i].r;
			circle[i].vx = -1.0f*fabs(circle[i].vx);
		}
		if (circle[i].x <= window.left + circle[i].r)
		{
			circle[i].x = window.left + circle[i].r;
			circle[i].vx = fabs(circle[i].vx);
		}
		if (circle[i].y >= window.top - circle[i].r)
		{
			circle[i].y = window.top - circle[i].r;
			circle[i].vy = -1.0f*fabs(circle[i].vy);
		}
		if (circle[i].y <= window.bottom + circle[i].r)
		{
			circle[i].y = window.bottom + circle[i].r;
			circle[i].vy = fabs(circle[i].vy);
		}

		// bounce off each other
		for (j = 0; j < N; j++)
		{
			ax = circle[j].x-circle[i].x;
			ay = circle[j].y-circle[i].y;
			d = hypotf(ax, ay);
			if (d < circle[i].r + circle[j].r && d > 0.00001)
			{
				ax /= d;
				ay /= d;

				// Projection of the velocities in these axes
				va1 = circle[i].vx*ax + circle[i].vy*ay;
				va2 = circle[j].vx*ax + circle[j].vy*ay;

				if (va2 - va1 < 0)
				{
					vb1 = circle[i].vy*ax - circle[i].vx*ay;
					vb2 = circle[j].vy*ax - circle[j].vx*ay;

					// Undo the projections
					circle[i].vx = va2*ax - vb1*ay;
					circle[i].vy = va2*ay + vb1*ax;
					circle[j].vx = va1*ax - vb2*ay;
					circle[j].vy = va1*ay + vb2*ax;
				}

				circle[i].x -= (circle[i].r+circle[j].r-d)*ax*.5f;
				circle[i].y -= (circle[i].r+circle[j].r-d)*ay*.5f;

				circle[j].x += (circle[i].r+circle[j].r-d)*ax*.5f;
				circle[j].y += (circle[i].r+circle[j].r-d)*ay*.5f;
			}
		}

		c = 0;
		for (j = (int)circle[i].x-(int)circle[i].r; j <= (int)circle[i].x+(int)circle[i].r; j++)
			for (k = (int)circle[i].y-(int)circle[i].r; k <= (int)circle[i].y+(int)circle[i].r; k++)
				if (j >= 0 && k >= 0 && j < shadowbuffer->w && k < shadowbuffer->h &&
						hypot(j-circle[i].x, k-circle[i].y) <= circle[i].r &&
						shadowbuffer->buffer[(shadowbuffer->w-j-1)+(shadowbuffer->h-k-1)*shadowbuffer->w] < 100)
				{
					c++;
				}

		if (c > .5*(circle[i].r*circle[i].r*4))
			circle[i].covered = true;
		else
			circle[i].covered = false;
	}

	// find out who's covering more
	p1 = 0;
	p2 = 0;
	for (i = 0; i < N; i++)
	{
		if (circle[i].player == 0 && circle[i].covered)
			p1++;
		if (circle[i].player == 1 && circle[i].covered)
			p2++;

	}
	score += scoreinc*(p2-p1);

	if (score > videobuffer->w/2 || score < videobuffer->w/-2)
		score = 0;
	

	// Copy the image to a texture
	if (displaytype == 0)
		for (int j=0; j < videobuffer->h; ++j)
			memcpy(&teximage[j*tex_w], &videobuffer->buffer[j*videobuffer->w], videobuffer->w);
	else
		for (int j=0; j < shadowbuffer->h; ++j)
			memcpy(&teximage[j*tex_w], &shadowbuffer->buffer[j*shadowbuffer->w], shadowbuffer->w);


	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);

	// draw loop
	for (i = 0; i < N; i++)
	{
		glLoadIdentity();

		glTranslatef(circle[i].x, circle[i].y, 1.0f);

		if (circle[i].covered)
			glColor3f(circle[i].cr+.5, circle[i].cg+.5, circle[i].cb+.5);
		else
			glColor3f(circle[i].cr, circle[i].cg, circle[i].cb);

		gluDisk(quadratic, 0.0f, circle[i].r, 20, 1);
		/*
		   glBegin(GL_QUADS);
		   glVertex3f(s.r, s.r, 0.0f);
		   glVertex3f(-s.r, s.r, 0.0f);
		   glVertex3f(-s.r, -s.r, 0.0f);
		   glVertex3f(s.r, -s.r, 0.0f);
		   glEnd();
		 */
	}

	if (score < 0)
		glColor3f(1.0f, 0.0f, 0.0f);
	else
		glColor3f(0.0f, 0.0f, 1.0f);

	glLoadIdentity();

	glBegin(GL_QUADS);
		glVertex3f(window.right/2, window.top-5, window.front);
		glVertex3f(window.right/2+score, window.top-5, window.front);
		glVertex3f(window.right/2+score, window.top-15, window.front);
		glVertex3f(window.right/2, window.top-15, window.front);
	glEnd();
	

	glEnable(GL_TEXTURE_2D);
	glColor3f(1.0f, 1.0f, 1.0f);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, tex_w, tex_h, 0,
			GL_LUMINANCE, GL_UNSIGNED_BYTE, teximage);

	glLoadIdentity();

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(window.right, window.top, window.back);
		glTexCoord2f(1, 0);
		glVertex3f(window.left, window.top, window.back);
		glTexCoord2f(1, 1);
		glVertex3f(window.left, window.bottom, window.back);
		glTexCoord2f(0, 1);
		glVertex3f(window.right, window.bottom, window.back);
	glEnd();

	SDL_GL_SwapBuffers();

	return true;
}


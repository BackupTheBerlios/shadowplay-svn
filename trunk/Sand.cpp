#include <iostream>
#include <vector>
#include <math.h>

#include "Sand.h"

#define N 1000
#define R 1

using namespace std;

Sand::Sand()
{
	srand(time(0));

	sand = new sandtype [N];
	for (int i = 0; i < N; i++)
	{
		sand[i].x = rand()/(float)RAND_MAX*(window.right-window.left)+window.left;
		sand[i].y = rand()/(float)RAND_MAX*(window.top-window.bottom)+window.bottom;
		sand[i].vx = rand()/(float)RAND_MAX*30.0f-15.0f;
		sand[i].vy = rand()/(float)RAND_MAX*30.0f-15.0f;
		sand[i].ax = 0;
		sand[i].ay = -5;

		sand[i].r = R+.5f;
		sand[i].m = 10;
		sand[i].mi = 1.0f/sand[i].m;
		sand[i].d = 0.95f;

		sand[i].cr = rand()/(float)RAND_MAX;
		sand[i].cg = rand()/(float)RAND_MAX;
		sand[i].cb = rand()/(float)RAND_MAX;
	}

	locw = (int)((window.right-window.left)/R)+1;
	loch = (int)((window.top-window.bottom)/R)+1;
	location = new vector<int> [locw*loch];

	quadratic = gluNewQuadric();
	gluQuadricNormals(quadratic, GLU_SMOOTH);
	gluQuadricTexture(quadratic, GL_TRUE);
}

Sand::~Sand(void)
{
}

inline bool Sand::Draw(void)
{
	static float dx, dy, distance, ax, ay, va1, vb1, va2, vb2, vaP1, vaP2;
	static int i, j, x, y, posxl, posxr, posyl, posyr, posx, posy, check;

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

	// clear out the location arrays
	for (int i = 0; i < locw*loch; i++)
		location[i].clear();

	// movement loop
	i = 0;
	do
	{
		sandtype &s = sand[i];
	
		posxl = shadowbuffer->w - (int)(s.x+s.r) - 1;
		posxr = shadowbuffer->w - (int)(s.x-s.r) - 1;
		posyl = shadowbuffer->h - (int)(s.y+s.r) - 1;
		posyr = shadowbuffer->h - (int)(s.y-s.r) - 1;

		posx = shadowbuffer->w - (int)(s.x) - 1;
		posy = shadowbuffer->h - (int)(s.y) - 1;

		// will be used to find the normal of the shadow
		ax = 0;
		ay = 0;

		// Check if it's in a shadow
		check = 0;
		for (x = posxl; x <= posxr; x++)
			for (y = posyl; y <= posyr; y++)
				if (x >= 0 && y >= 0 && x < shadowbuffer->w && y < shadowbuffer->h &&
						shadowbuffer->buffer[x+y*shadowbuffer->w] < 100 &&
						hypotf(x-posx, y-posy) <= s.r)
				{
					check++;
					ax -= x - posx;
					ay -= y - posy;
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

			j = (int)(posxr-posx);
			do
			{
				for (x = posx-j; x < posx+j; x++)
					for (y = posy-j; y < posy+j; y++)
					{
						if (x >= 0 && y >= 0 && x < shadowbuffer->w && y < shadowbuffer->h &&
								shadowbuffer->buffer[x+y*shadowbuffer->w] < 100)
						{
							check++;
							ax -= x - posx;
							ay -= y - posy;
						}
					}
				j++;
			} while (check < 10 && j <= R*2);

			distance = hypotf(ax, ay);
			if (distance > 0.0001)
			{
				ax /= distance;
				ay /= distance;

				// Projection of the velocity in these axes
				va1 = s.vx*ax + s.vy*ay;

				if (va1 > 0)
				{
					vb1 = s.vy*ax - s.vx*ay;

					// New velocity in these axes (after collision)
					vaP1 = va1 - (1+s.d)*va1;

					// Undo the projections
					s.vx = vaP1*ax - vb1*ay;
					s.vy = vaP1*ay + vb1*ax;
				}
			}
			else
			{
				s.vy = 2;
			}

			s.x += s.vx*dt*check/(R*R);
			s.y += s.vy*dt*check/(R*R);
		}


		// bounce it off the walls
		if (s.x >= window.right - s.r)
			s.vx = -1.0f*fabs(s.vx*s.d);
		if (s.x <= window.left + s.r)
			s.vx = fabs(s.vx*s.d);

		// reset if off the screen
		if (s.y < window.bottom - s.r || s.y > window.top + 4*s.r)
		{
			s.x = rand()/(float)RAND_MAX*window.right/4+window.right*3/8;
			s.y = window.top + R;
			s.vx = rand()/(float)RAND_MAX*10.0f-5.0f;
			s.vy = rand()/(float)RAND_MAX*-2.0f;
		}

		x = (int)(s.x/R);
		y = (int)(s.y/R);
		if (x >= 0 && x < locw && y >= 0 && y < loch)
			location[x+y*loch].push_back(i);

		i++;
	} while (i < N);


	// collision loop
	i = 0;
	do
	{
		sandtype &s = sand[i];

		// loop through the locations adjacent to the one that the particle
		// is in as well as it's own spot
		for (x = (int)(s.x/R)-1; x <= (int)(s.x/R)+1; x++)
			for (y = (int)(s.y/R)-1; y <= (int)(s.y/R)+1; y++)
			{
				if (x >= 0 && y >= 0 && x < locw && y < loch)
				{
					//cout << x << ":" << y << " " << location[x+y*loch].size() << " " << x+y*loch << endl;
					for (j = 0; j < location[x+y*loch].size(); j++)
					{
						check = location[x+y*loch].at(j);

						sandtype &s2 = sand[check];

						ax = s2.x-s.x;
						ay = s2.y-s.y;
						distance = hypotf(ax, ay);
						if (distance <= s.r + s2.r && distance > 0.00001)
						{
							ax /= distance;
							ay /= distance;

							// Projection of the velocities in these axes
							va1 = s.vx*ax + s.vy*ay;
							va2 = s2.vx*ax + s2.vy*ay;

							if (va2 - va1 < 0)
							{

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
							}
						}
					}
					location[x+y*loch].clear();
				}
			}

		i++;
	} while (i < N);

	// draw loop
	i = 0;
	do
	{
		sandtype &s = sand[i];
		//s = sandtemp[i];

		glLoadIdentity();

		glTranslatef(s.x, s.y, 1.0f);

		glColor3f(s.cr, s.cg, s.cb);

		gluDisk(quadratic, 0.0f, s.r, 20, 1);
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


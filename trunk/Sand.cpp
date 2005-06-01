#include <iostream>

#include "Sand.h"

using namespace std;

Sand::Sand(void)
{
	cout << "Sand: Constructing the sand object\n";
}

Sand::~Sand(void)
{
}

bool Sand::Draw(void)
{
	if (displaytype == 1)
		for (int j=0; j < videobuffer->h; ++j)
			memcpy(&teximage[j*tex_w+tex_w-videobuffer->w], &videobuffer->buffer[j*videobuffer->w], videobuffer->w);
	else
		for (int j=0; j < shadowbuffer->h; ++j)
			memcpy(&teximage[j*tex_w+tex_w-shadowbuffer->w], &shadowbuffer->buffer[j*shadowbuffer->w], shadowbuffer->w);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE, tex_w, tex_h, 0,
			GL_LUMINANCE, GL_UNSIGNED_BYTE, teximage);

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();

	glBegin(GL_QUADS);
		glTexCoord2f(1, 1);
		glVertex3f( 220.0f,  140.0f, -320.0f);
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


#include <cstdio>
#include <cstdlib>
#include <cerrno>

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include "VideoInput.h"
#include "Shadow.h"
#include "SDL.h"
#include "SDL_thread.h"

#include <iostream>
using namespace std;


Shadow::Shadow(int in_w, int in_h)
{
	videoinput = new VideoInput(in_w, in_h);
	
	videobuffer = videoinput->GetBuffer();

	thread = NULL;
	playing = false;
	threshold = 75;

	shadowbuffer = new vidbuffertype;
	shadowbuffer->buffer = new uint8_t [videobuffer->w*videobuffer->h];
	shadowbuffer->w = videobuffer->w;
	shadowbuffer->h = videobuffer->h;
	shadowbuffer->bufferlen = videobuffer->w*videobuffer->h;
}

Shadow::~Shadow(void)
{
	videoinput->StopPlaying();
	delete videoinput;
}

int Shadow::IncThreshold(int inc)
{
	threshold += inc;

	if (threshold <= 0)
		threshold = 1;
	if (threshold >= 255)
		threshold = 255-1;

	return threshold;
}

vidbuffertype *Shadow::GetBuffer(void)
{
	return videobuffer;
}

vidbuffertype *Shadow::GetShadow(void)
{
	return shadowbuffer;
}

void Shadow::StartPlaying(void)
{
	videoinput->StartPlaying();

	playing = true;
	cout << "Shadow: Creating shadow thread\n";
	thread = SDL_CreateThread(ShadowThread,
			static_cast<void *>(this));
	if (thread == NULL)
	{
		cout << "Shadow: Unable to create thread: ";
		cout << SDL_GetError() << endl;
	}
}

void Shadow::StopPlaying(void)
{
	videoinput->StopPlaying();

	playing = false;
	SDL_WaitThread(thread, NULL);
}

	extern "C"
int ShadowThread(void *s)
{
	static_cast<Shadow *>(s)->MainLoop();
}


void Shadow::MainLoop(void)
{
	const uint8_t *b = videobuffer->buffer;
	uint8_t *s = shadowbuffer->buffer;

	const int w = videobuffer->w;
	const int h = videobuffer->h;

	bool *bt = new bool [w*h];

	for (int i=0; i < w*h; i++)
		s[i] = 255;

	int i, j, l;

	while (playing)
	{
		for (i = 7; i < w - 7; i++)
		{
			for (j = 5; j < h - 5; j++)
			{
				l = i+j*w;				
				if (b[l] + b[l+1] + b[l-1] +
						b[l-w] + b[l-w-1] + b[l-w+1] +
						b[l+w] + b[l+w-1] + b[l+w+1] >
						threshold * 8)
				{
					bt[l] = false;
				}
				else
				{
					bt[l] = true;
				}
			}
		}
		for (i = 7; i < w - 7; i++)
		{
			for (j = 5; j < h - 5; j++)
			{
				l = i+j*w;				
				if (bt[l] && bt[l+1] + bt[l-1] +
						bt[l-w] + bt[l-w-1] + bt[l-w+1] +
						bt[l+w] + bt[l+w-1] + bt[l+w+1] > 4)
					s[l] = 0;
				else
					s[l] = 255;
				if (!bt[l] && bt[l+1] + bt[l-1] +
						bt[l-w] + bt[l-w-1] + bt[l-w+1] +
						bt[l+w] + bt[l+w-1] + bt[l+w+1] < 4)

					s[l] = 255;
				else
					s[l] = 0;
			}
		}
		SDL_Delay(25);
	}
}

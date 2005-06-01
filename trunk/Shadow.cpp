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
		threshold = 254;

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
	nice(15);
	
	uint8_t *b = videobuffer->buffer;
	int w = videobuffer->w;
	int h = videobuffer->h;

	for (int i=0; i < w*h; i++)
		shadowbuffer->buffer[i] = 255;

	while (playing)
	{
		for (int i=7; i < w - 7; i++)
		{
			for (int j=5; j < h - 5; j++)
			{
				if (b[i+j*w] + b[i+j*w+1] + b[i+j*w-1] +
						b[i+(j-1)*w] + b[i+(j+1)*w] 
						> threshold * 5.0f)
					shadowbuffer->buffer[i+j*w] = 255;
				else
					shadowbuffer->buffer[i+j*w] = 0;
			}
		}
	}
}

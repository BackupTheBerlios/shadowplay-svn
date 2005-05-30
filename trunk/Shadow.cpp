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
	cout << "Shadow: Creating the VideoInput object\n";
	videoinput = new VideoInput(in_w, in_h);
	
	videobuffer = videoinput->GetBuffer();

	thread = NULL;
	playing = false;

	shadowbuffer = new shadowtype;
	shadowbuffer->buffer = new bool [videobuffer->w*videobuffer->h];
	shadowbuffer->w = videobuffer->w;
	shadowbuffer->h = videobuffer->h;
	shadowbuffer->bufferlen = videobuffer->w*videobuffer->h;
}

Shadow::~Shadow(void)
{
	cout << "Shadow: Stopping videoinput and then deleting it\n";
	videoinput->StopPlaying();
	delete videoinput;
}

vidbuffertype *Shadow::GetBuffer(void)
{
	return videobuffer;
}

shadowtype *Shadow::GetShadow(void)
{
	return shadowbuffer;
}

void Shadow::StartPlaying(void)
{
	videoinput->StartPlaying();

	playing = true;
	cout << "Shadow: Trying to create shadow thread\n";
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
	while (playing)
	{
	}
}

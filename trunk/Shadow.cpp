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
}

Shadow::~Shadow(void)
{
	cout << "Shadow: Stopping videoinput and then deleting it\n";
	videoinput->StopPlaying();
	delete videoinput;
}

void Shadow::StopPlaying(void)
{
	cout << "Shadow: Stopping videout\n";
	videoinput->StopPlaying();
}

vidbuffertype *Shadow::GetBuffer(void)
{
	return videobuffer;
}

void Shadow::StartPlaying(void)
{
	videoinput->StartPlaying();
}

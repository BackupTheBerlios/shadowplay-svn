#ifndef SHADOW_H
#define SHADOW_H

#include "VideoInput.h"
#include "SDL.h"
#include "SDL_thread.h"

struct shadowtype
{
	bool *buffer;
	int bufferlen;
	int w, h;
};

class VideoInput;

extern "C" int ShadowThread(void *);

class Shadow
{
  public:
	Shadow(int in_w, int in_h);
	~Shadow(void);

	vidbuffertype *GetBuffer(void);
	shadowtype *GetShadow(void);
	void StartPlaying(void);
	void StopPlaying(void);

	void MainLoop(void);

  private:
	VideoInput *videoinput;

	vidbuffertype *videobuffer;
	shadowtype *shadowbuffer;

	SDL_Thread *thread;
	
	bool playing;
};

#endif

#ifndef SHADOW_H
#define SHADOW_H

#include "VideoInput.h"
#include "SDL.h"
#include "SDL_thread.h"

class VideoInput;

extern "C" int ShadowThread(void *);

class Shadow
{
  public:
	Shadow(int in_w, int in_h);
	~Shadow(void);

	vidbuffertype *GetBuffer(void);
	vidbuffertype *GetShadow(void);
	void StartPlaying(void);
	void StopPlaying(void);

	void MainLoop(void);

	int IncThreshold(int);
	void Invert(void);

  private:
	VideoInput *videoinput;

	vidbuffertype *videobuffer;
	vidbuffertype *shadowbuffer;

	SDL_Thread *thread;
	
	bool playing;
	int threshold;

	uint8_t one, two;
};

#endif

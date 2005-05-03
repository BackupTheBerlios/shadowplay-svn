#ifndef SHADOW_H
#define SHADOW_H

#include "VideoInput.h"
#include "SDL.h"
#include "SDL_thread.h"

class VideoInput;

class Shadow
{
  public:
	Shadow(int in_w, int in_h);
	~Shadow(void);

	vidbuffertype *GetBuffer(void);
	void StartPlaying(void);
	void StopPlaying(void);

  private:
	VideoInput *videoinput;

	vidbuffertype *videobuffer;
	SDL_Thread *thread;

  protected:
};

#endif

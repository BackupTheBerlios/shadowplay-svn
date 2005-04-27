#ifndef SHADOW_H
#define SHADOW_H

#include "VideoInput.h"

class VideoInput;

class Shadow
{
  public:
	Shadow(void);
	~Shadow(void);

	vidbuffertype *GetBuffer(void);

  private:
	VideoInput *videoinput;

	vidbuffertype *videobuffer;

  protected:
};

#endif

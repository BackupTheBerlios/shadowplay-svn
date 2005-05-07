#ifndef VIDEOINPUT
#define VIDEOINPUT

#include <vector>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

#include "SDL.h"
#include "SDL_thread.h"

using namespace std;

struct vidbuffertype
{
	int bufferlen;
	int w;
	int h;
	unsigned char *buffer;
};

extern "C" int CaptureThread(void *);

class VideoInput
{
	public:
		VideoInput(int in_w, int in_h);
		~VideoInput();

		void StartPlaying(void);
		void StopPlaying(void);
		vidbuffertype *GetBuffer(void);

		void CaptureLoop(void);

	protected:

	private:
		void Initialize(void);
		bool Open(void);

		void BufferIt(unsigned char *buf);

		bool playing;
		bool errored;

		int fd;
		int w, h;
		int inputchannel;

		int usebttv;
		float video_aspect;

		vidbuffertype *videobuffer;

		int act_video_buffer;

		int video_buffer_count;
		long video_buffer_size;

		int w_out;
		int h_out;

		int channelfd;

		SDL_Thread *thread;
};

#endif

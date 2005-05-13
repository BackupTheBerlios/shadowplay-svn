#ifndef VIDEOINPUT
#define VIDEOINPUT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <vector>

#include <asm/types.h>
#include <linux/videodev2.h>

#include "SDL.h"
#include "SDL_thread.h"
#include <pthread.h>



#define CLEAR(x) memset(&(x), 0, sizeof(x))

typedef enum
{
	IO_METHOD_READ,
	IO_METHOD_MMAP,
} io_method;

struct vidbuffertype
{
	uint8_t *buffer;
	int bufferlen;
	int w, h;
};

using namespace std;

extern "C" int CaptureThread(void *);

class VideoInput
{
	public:
		VideoInput(int in_w, int in_h);
		~VideoInput(void);

		void StartPlaying(void);
		void StopPlaying(void);
		vidbuffertype *GetBuffer(void);

		void MainLoop(void);

	protected:

	private:
		void ErrnoError(const char *s);
		int Xioctl(int request, void *arg);
		void ProcessImage(const void *p);
		int ReadFrame(void);
		void StopCapturing(void);
		void StartCapturing(void);
		void UninitDevice(void);
		void InitRead(unsigned int buffer_size);
		void InitMmap(void);
		void InitDevice(void);
		void OpenDevice(void);
		void CloseDevice(void);

		bool playing;
		bool errored;

		int fd;
		int w, h;

		SDL_Thread *thread;

		char *dev_name;
		io_method io;
		vidbuffertype *buffers;
		vidbuffertype *videobuffer;
		unsigned int n_buffers;
};

#endif

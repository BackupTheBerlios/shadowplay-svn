#ifndef VIDEOINPUT
#define VIDEOINPUT

#include <vector>
#include <sys/time.h>
#include <time.h>
#include <pthread.h>

using namespace std;

typedef struct vidbuffertype
{
    unsigned char *buffer;
    int bufferlen;
} vidbuffertyp;

class VideoInput
{
 public:
    VideoInput();
   ~VideoInput();

	void StartCapture(void);
	vidbuffertype *GetBuffer(void);

 protected:
    
 private:
    void Initialize(void);

    bool Open(void);
    void BufferIt(unsigned char *buf, int len = -1);
    
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
};

#endif

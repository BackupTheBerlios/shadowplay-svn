#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <cmath>

#include <iostream>

using namespace std;

#include "VideoInput.h"

#include "videodev.h"

VideoInput::VideoInput()
{
    fd = -1;
    channelfd = -1;
    inputchannel = 1;

    w = 352;
    h = 240;
    video_aspect = 1.33333;

    video_buffer_size = 0;

    errored = false;
	playing = false;

	Initialize();
}

VideoInput::~VideoInput(void)
{
    if (fd >= 0)
        close(fd);

	delete videobuffer;
}

vidbuffertype *VideoInput::GetBuffer(void)
{
	return videobuffer;
}

void VideoInput::Initialize(void)
{
    if (!video_buffer_size)
    {
        video_buffer_size = w_out * h_out * 3 / 2;
    }

    videobuffer = new vidbuffertype;
    videobuffer->buffer = new unsigned char[video_buffer_size];
    videobuffer->bufferlen = 0;
}

bool VideoInput::Open(void)
{
	char videodevice [] = "/dev/video0";

    if (channelfd>0)
        return true;

    int retries = 0;
    fd = open(videodevice, O_RDWR);
    while (fd < 0)
    {
        usleep(30000);
        fd = open(videodevice, O_RDWR);
        if (retries++ > 5)
        {
            cout << "VideoInput: Can't open video device: " << videodevice << endl;
            perror("open video:");
            errored = true;
            return false;
        }
    }

    channelfd = fd;
    return true;
}

void VideoInput::StartCapture(void)
{
    if (getuid() == 0)
        nice(-10);

    if (!Open())
    {
        errored = true;
        return;
    }

    struct video_capability vc;
    struct video_mmap mm;
    struct video_mbuf vm;
    struct video_channel vchan;
    struct video_audio va;
    struct video_tuner vt;

    memset(&mm, 0, sizeof(mm));
    memset(&vm, 0, sizeof(vm));
    memset(&vchan, 0, sizeof(vchan));
    memset(&va, 0, sizeof(va));
    memset(&vt, 0, sizeof(vt));
    memset(&vc, 0, sizeof(vc));

    if (ioctl(fd, VIDIOCGCAP, &vc) < 0)
    {
        perror("VIDIOCGCAP:");
        errored = true;
        return;
    }

    int channelinput = 66;

    vchan.channel = channelinput;

    if (ioctl(fd, VIDIOCGCHAN, &vchan) < 0)
        perror("VIDIOCGCHAN");

    if (ioctl(fd, VIDIOCGMBUF, &vm) < 0)
    {
        perror("VIDOCGMBUF:");
        errored = true;
        return;
    }

    if (vm.frames < 2)
    {
        cout << "VideoInput: Need a minimum of 2 capture buffers\n";
        errored = true;
        return;
    }

    int frame;

    unsigned char *buf = (unsigned char *)mmap(0, vm.size, 
                                               PROT_READ|PROT_WRITE, 
                                               MAP_SHARED, 
                                               fd, 0);
    if (buf <= 0)
    {
        perror("mmap");
        errored = true;
        return;
    }

    mm.height = h;
    mm.width  = w;
    mm.format = VIDEO_PALETTE_YUV420P;  

    mm.frame  = 0;
    if (ioctl(fd, VIDIOCMCAPTURE, &mm)<0) 
        perror("VIDIOCMCAPTUREi0");
    mm.frame  = 1;
    if (ioctl(fd, VIDIOCMCAPTURE, &mm)<0) 
        perror("VIDIOCMCAPTUREi1");
    
    playing = true;

    int syncerrors = 0;

    while (playing) 
    {
        frame = 0;
        mm.frame = 0;
        if (ioctl(fd, VIDIOCSYNC, &frame)<0) 
        {
            syncerrors++;
            if (syncerrors == 10)
				cout << "VideoInput: Multiple bttv errors, further messages supressed\n";
            else if (syncerrors < 10)
                perror("VIDIOCSYNC");
        }
        else 
        {
            BufferIt(buf+vm.offsets[0], video_buffer_size);
            //memset(buf+vm.offsets[0], 0, video_buffer_size);
        }

        if (ioctl(fd, VIDIOCMCAPTURE, &mm)<0) 
            perror("VIDIOCMCAPTURE0");

        frame = 1;
        mm.frame = 1;
        if (ioctl(fd, VIDIOCSYNC, &frame)<0) 
        {
            syncerrors++;
            if (syncerrors == 10)
				cout << "VideoInput: Multiple bttv errors, further messages supressed\n";
            else if (syncerrors < 10)
                perror("VIDIOCSYNC");
        }
        else 
        {
            BufferIt(buf+vm.offsets[1], video_buffer_size);
            //memset(buf+vm.offsets[1], 0, video_buffer_size);
        }
        if (ioctl(fd, VIDIOCMCAPTURE, &mm)<0) 
            perror("VIDIOCMCAPTURE1");
    }

    munmap(buf, vm.size);

    close(fd);
}

void VideoInput::BufferIt(unsigned char *buf, int len)
{
    memcpy(videobuffer->buffer, buf, len);
    videobuffer->bufferlen = len;

    return;
}


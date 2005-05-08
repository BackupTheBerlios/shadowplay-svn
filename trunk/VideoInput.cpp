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
#include <sys/mman.h>
#include <sys/ioctl.h>

#include <asm/types.h>
#include <linux/videodev2.h>

#include "SDL.h"
#include "SDL_thread.h"

#include "VideoInput.h"

#define CLEAR(x) memset(&(x), 0, sizeof(x))

void VideoInput::VideoInput(int in_w, int in_h)
{
	w = in_w;
	h = in_h;

	dev_name = "/dev/video0";

	io	= IO_METHOD_MMAP;
	fd = -1;
	buffers = NULL;
	n_buffers = 0;

	errored = false;
	playing = false;

	OpenDevice();
	InitDevice();
	StartCapturing();
}

void VideoInput::~VideoInput(void)
{
	StopCapturing();
	UninitDevice();
	CloseDevice();
}

vidbuffertype *VideoInput::GetBuffer(void)
{
	return buffers[1];
}

void VideoInput::StopPlaying(void)
{
	playing = false;
	SDL_WaitThread(thread, NULL);
}

void VideoInput::StartPlaying(void)
{
	thread = SDL_CreateThread(CaptureThread,
			static_cast<void *>(this));
	if (thread == NULL)
	{
		cout << "VideoInput: Unable to create thread: ";
		cout << SDL_GetError() << endl;
	}
}

	extern "C"
int CaptureThread(void *vi)
{
	static_cast<VideoInput *>(vi)->MainLoop();
}

void VideoInput::ErrnoError(const char *s)
{
	cout << "VideoInput: " << s << " error " << errno;
	cout << " " << strerror(errno) << endl;
	errored = true;
}

int VideoInput::Xioctl(int request, void *arg)
{
	int r;

	do
		r = ioctl(fd, request, arg);
	while (-1 == r && EINTR == errno);

	return r;
}

void VideoInput::ProcessImage(const void *p)
{
	fputc('.', stdout);
	fflush(stdout);
}

int VideoInput::ReadFrame(void)
{
	struct v4l2_buffer buf;
	unsigned int i;

	switch (io)
	{
		case IO_METHOD_READ:
			if (-1 == read(fd, buffers[0].start, buffers[0].length))
				ErrnoError("read");

			ProcessImage(buffers[0].start);
			break;

		case IO_METHOD_MMAP:
			CLEAR(buf);

			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;

			if (-1 == Xioctl(VIDIOC_DQBUF, &buf))
				ErrnoError("VIDIOC_DQBUF");

			assert(buf.index < n_buffers);

			ProcessImage(buffers[buf.index].start);

			if (-1 == Xioctl(VIDIOC_QBUF, &buf))
				ErrnoError("VIDIOC_QBUF");

			break;
	}

	return errored;
}

void VideoInput::MainLoop(void)
{
	while (playing && !errored)
	{
		for (;;)
		{
			fd_set fds;
			struct timeval tv;
			int r;

			FD_ZERO(&fds);
			FD_SET(fd, &fds);

			// Timeout
			tv.tv_sec = 2;
			tv.tv_usec = 0;

			r = select(fd + 1, &fds, NULL, NULL, &tv);

			if (-1 == r) {
				if (EINTR == errno)
					continue;

				ErrnoError("select");
			}

			if (0 == r) {
				cout << "VideoInput: select timeout\n";
				errored = true;
			}

			if (ReadFrame())
				break;

			// EAGAIN - continue select loop
		}
	}
}

void VideoInput::StopCapturing(void)
{
	enum v4l2_buf_type type;

	switch (io)
	{
		case IO_METHOD_READ:
			// Nothing to do
			break;

		case IO_METHOD_MMAP:
			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

			if (-1 == Xioctl(VIDIOC_STREAMOFF, &type))
				errno_exit ("VIDIOC_STREAMOFF");

			break;
	}
}

void VideoInput::StartCapturing(void)
{
	unsigned int i;
	enum v4l2_buf_type type;

	switch (io)
	{
		case IO_METHOD_READ:
			// Nothing to do
			break;

		case IO_METHOD_MMAP:
			for (i = 0; i < n_buffers; ++i)
			{
				struct v4l2_buffer buf;

				CLEAR (buf);

				buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
				buf.memory = V4L2_MEMORY_MMAP;
				buf.index  = i;

				if (-1 == Xioctl(VIDIOC_QBUF, &buf))
					ErrnoError("VIDIOC_QBUF");
			}

			type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

			if (-1 == Xioctl(VIDIOC_STREAMON, &type))
				ErrnoError("VIDIOC_STREAMON");

			break;
	}
}

void VideoInput::UninitDevice(void)
{
	unsigned int i;

	switch (io)
	{
		case IO_METHOD_READ:
			free(buffers[0].start);
			break;

		case IO_METHOD_MMAP:
			for (i = 0; i < n_buffers; ++i)
				if (-1 == munmap (buffers[i].start, buffers[i].length))
					ErrnoError("munmap");
			break;
	}

	free(buffers);
}

void VideoInput::InitRead(unsigned int buffer_size)
{
	buffers = calloc(1, sizeof(*buffers));

	if (!buffers)
	{
		cout << "VideoInput: Out of memory (1)\n";
		errored = true;
	}

	buffers[0].length = buffer_size;
	buffers[0].start = malloc(buffer_size);

	if (!buffers[0].start)
	{
		cout << "VideoInput: Out of memory (2)\n";
		errored = true;
	}
}

void VideoInput::InitMmap(void)
{
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count  = 4;
	req.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	if (-1 == Xioctl(VIDIOC_REQBUFS, &req))
	{
		if (EINVAL == errno)
		{
			cout << "VideoInput: " << dev_name;
			cout << " does not support memory mapping\n";
			errored = true;
		} else {
			ErrnoErrored("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2)
	{
		cout << "VideoInput: Insufficient buffer memory on "
			cout << dev_name << endl;;
		errored = true;
	}

	buffers = calloc (req.count, sizeof (*buffers));

	if (!buffers)
	{
		cout << "VideoInput: Out of memory (3)\n";
		errored = true;
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
	{
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index  = n_buffers;

		if (-1 == Xioctl(VIDIOC_QUERYBUF, &buf))
			ErrnoError("VIDIOC_QUERYBUF");

		buffers[n_buffers].length = buf.length;
		buffers[n_buffers].start = mmap(NULL, buf.length,
				PROT_READ|PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

		if (MAP_FAILED == buffers[n_buffers].start)
			ErrnoError("mmap");
	}
}

void VideoInput::InitDevice(void)
{
	struct v4l2_capability cap;
	struct v4l2_cropcap cropcap;
	struct v4l2_crop crop;
	struct v4l2_format fmt;
	unsigned int min;

	if (-1 == Xioctl(VIDIOC_QUERYCAP, &cap))
	{
		if (EINVAL == errno)
		{
			cout << "VideoInput: " << dev_name;
			cout << " is not a V4L2 device\n";
			errored = true;
		} else {
			ErrnoError("VIDIOC_QUERYCAP");
		}
	}

	if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
	{
		cout << "VideoInput: " << dev_name;
		cout << " is not a capture device\n";
		errored = true;
	}

	switch (io)
	{
		case IO_METHOD_READ:
			if (!(cap.capabilities & V4L2_CAP_READWRITE))
			{
				cout << "VideoInput: " << dev_name;
				cout << " does not support read i/o\n";
				errored = true;
			}

			break;

		case IO_METHOD_MMAP:
			if (!(cap.capabilities & V4L2_CAP_STREAMING)) {
				cout << "VideoInput: " << dev_name;
				cout << " does not support streaming i/o\n";
				errored = true;
			}

			break;
	}

	// Select video input, video standard and tune here

	cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == Xioctl(VIDIOC_CROPCAP, &cropcap))
		cout << "VideoInput: Cropcap failure\n";

	crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	crop.c = cropcap.defrect; // reset to default

	if (-1 == Xioctl(VIDIOC_S_CROP, &crop))
		cout << "VideoInput: Cropping not supported\n";

	CLEAR(fmt);

	fmt.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width       = w; 
	fmt.fmt.pix.height      = h;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	fmt.fmt.pix.field       = V4L2_FIELD_INTERLACED;

	if (-1 == Xioctl(VIDIOC_S_FMT, &fmt))
		ErrnoError("VIDIOC_S_FMT");

	// Note VIDIOC_S_FMT may change width and height

	// Buggy driver paranoia
	min = fmt.fmt.pix.width * 2;
	if (fmt.fmt.pix.bytesperline < min)
		fmt.fmt.pix.bytesperline = min;
	min = fmt.fmt.pix.bytesperline * fmt.fmt.pix.height;
	if (fmt.fmt.pix.sizeimage < min)
		fmt.fmt.pix.sizeimage = min;

	switch (io)
	{
		case IO_METHOD_READ:
			InitRead(fmt.fmt.pix.sizeimage);
			break;

		case IO_METHOD_MMAP:
			InitMmap();
			break;
	}
}

void VideoInput::CloseDevice(void)
{
	if (-1 == close(fd))
		ErrnoError("close");

	fd = -1;
}

void VideoInput::OpenDevice(void)
{
	struct stat st; 

	if (-1 == stat(dev_name, &st))
		ErrnoError("Cannot identify '"+dev_name+"'");

	if (!S_ISCHR (st.st_mode))
	{
		cout << "VideoInput: " << dev_name << " is not a device\n";
		errored = true;
	}

	fd = open(dev_name, O_RDWR|O_NONBLOCK, 0);

	if (-1 == fd)
		ErrnoError("Could not open '"+dev_name+"'");
}


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

#include <iostream>
using namespace std;


Shadow::Shadow(void)
{
	cout << "Creating the VideoInput object\n";
	videoinput = new VideoInput();
}

Shadow::~Shadow(void)
{
}
